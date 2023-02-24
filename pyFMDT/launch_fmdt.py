#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Launch FMDT for a serie of videos in a given directory.

Created on Wed Dec  7 16:41:06 2022

@author: vaubaill
"""

import os
import glob
import argparse
from configparser import ConfigParser, ExtendedInterpolation
import subprocess
#from PIL import Image,ImageStat

from fmdt_log import log
import fmdt_reduce
from video_fmdt2frame import video_track2frame
from imgseq_track2frame import imgseq_track2frame


# get $HOME environment variable
home = os.getenv('HOME')

def check_file(filename):
    """Check existence of a file.
    Usefull especially for the several configuration files.
    Raise IOError if file does not exist.
    
    Parameters
    ----------
    filename : string
        Full path name of the file to check.
    
    Returns
    -------
    None.
    
    
    """
    if not os.path.exists(filename):
        msg = '*** fatal error: file '+filename+' does not exist.'
        log.error(msg)
        raise IOError(msg)
    log.debug(filename+' ok')
    return


def submitt_fmdt(cmd,crash=False,nosub=False):
    """Submitt FMDT command.
    
    Parameters
    ----------
    cmd : string
        FMDT command to be submitted.
    crash : Boolean, optional
        If True, the program raises a SystemError if FMDT does not work. Default is False.
    nosub : Boolean, optional
        If True the command is not run. Useful for debug purpose. Default is False.
    
    Returns
    ------
    None.
    """
    wngmsg = 'Continuing with next set of data (video or images)'
    try:
        log.debug('Submit cmd: '+cmd)
        if nosub:
            log.warning('Command not submitted on purpose!')
        else:
            os.system(cmd)
    except subprocess.CalledProcessError:
        msg = 'Problem with the FMDT command: '+cmd
        log.error(msg)
        raise SystemError(msg) if crash else log.warning(wngmsg)
    except:
        msg = 'FMDT cmd unable to go through: '+cmd
        log.error(msg)
        raise SystemError(msg) if crash else log.warning(wngmsg)
    return

def launch_fmdt(pth_in,pth_out,fil_pat,ext,out_root,config_template,\
                imgseq=False,nofmdt=False,noXtract=False,
                fmdt_params={'exe':home+'/fmdt/build/bin/fmdt-detect-rt-pip',
                             'visu':home+'/fmdt/build/bin/fmdt-visu',
                             'light_min':68,
                             'light_max':79,
                             'surface_min':6,
                             'fra_star_min':10,
                             'max_dist':25}):
    """Check and create directory of usefuls files before launching fmdt.

    Parameters
    ----------
    pth_in : string
        Path of all video files, or path to image series main folder.
    pth_out : string
        Output path.
    fil_pat : string
        Video or Images file pattern. Ex: "Basler" or "_processed."
    ext : string
        Video or Image file extension.
    out_root : string
        FMDT output file root name.
    config_template : string
        fmdt_reduce configuration file template.
    imgseq : Boolean, optional
        If True, image sequence rather than video files are searched for.
        Default is False.
    fmdt_params : Dict
        dictionnary of FMDT parameters.
    nofmdt : Boolean, optional
        If True the FMDT command is not run. Useful for debug purpose. Default is False.
    noXtract : Boolean, optional
        If True the frame extraction is not performed. Useful for debug purpose. Default is False.
    
    Returns
    -------
    None.

    """    
    # debug
    log.debug('pth_in : '+pth_in)
    log.debug('pth_out : '+pth_out)
    log.debug('fil_pat : '+fil_pat)
    log.debug('ext : '+ext)
    log.debug('out_root : '+out_root)
    log.debug('config_template : '+config_template)
    log.debug('imgseq : '+str(imgseq))
    log.debug('fmdt_params : '+str(fmdt_params))
        
    # go to video directory
    curdir = os.getcwd()
    log.info('Change directory: '+pth_in)
    os.chdir(pth_in)
    
    # check files and directories
    check_file(fmdt_params['exe'])
    check_file(fmdt_params['visu'])
    check_file(pth_in)
    check_file(config_template)
    
    # cosmetic tasks
    if not pth_in.endswith('/'):
        pth_in = pth_in + '/'
    if not pth_out.endswith('/'):
        pth_out = pth_out + '/'
    if not os.path.isdir(pth_out):
        log.error('Make output directory '+pth_out)
        os.mkdir(pth_out)
    
    # detedermine if data are in video or image serie format
    if imgseq:
        # process Image sequence
        pattern = '*'+fil_pat+'*' #+isp +'*'+ext
        imgpth_list = glob.glob(pattern)
        log.info('Number of images found of pattern: '+pattern+' : '+str(len(imgpth_list)))
        # check if images were found
        if not len(imgpth_list):
            msg = '*** FATAL ERROR: no image file found of pattern '+pattern
            log.error(msg)
            raise IOError(msg)
        # loop processing each sub_file
        for subpth in imgpth_list :
            log.debug('====== Now processing image path '+subpth)
            process_images(subpth.replace(fil_pat,''),config_template,fmdt_params,\
                           pth_in=pth_in+subpth,pth_out=pth_out,out_root=out_root,
                           nofmdt=nofmdt,noXtract=noXtract)
    else:
        # make video pattern and search for video files
        pattern = fil_pat+'*'+ext
        list_vid = glob.glob(pattern)
        log.info('Video files found: '+ str(list_vid))
        # check if videos were found
        if not len(list_vid):
            msg = '*** FATAL ERROR: no video file found of pattern '+pattern
            log.error(msg)
            raise IOError(msg)
        # loop over all found video files
        for vid_file in list_vid:
            log.debug('====== Now processing video '+vid_file)
            process_video(vid_file,config_template,fmdt_params,pth_out=pth_out,
                          nofmdt=nofmdt,noXtract=noXtract)
    # go back to initial directory
    os.chdir(curdir)
    log.info('Done.')
    return 

def process_video(vid_file,config_template,fmdt_params,pth_in='./',pth_out='./', 
                  out_root='./',trk='_track.dat',
                  bb='_bb.dat',mag='_mag.dat',cfg='_config.in',visu='_visu',
                  crash=False,nofmdt=False,noXtrat=False):
    """ Tune fmdt options based on an image.

    Parameters
    ----------
    vid_file : string
        name of the video file.
    config_template : string
        fmdt-reduce configuration file template.
    fmdt_params : Dict
        dictionnary of FMDT parameters.
    pth_in :string, optional
        Input path. Default is './'.
    pth_out :string, optional
        Output path. Default is './'.
    out_root : string, optional
        FMDT output file root name. Default is './'.
    trk : string, optional
        FMDT track output file name extension. Default is '_track.dat'.
    bb : string, optional
        FMDT box output file name extension. Default is '_bb.dat'.
    mag : string, optional
        FMDT magnitude output file name extension. Default is '_mag.dat'.
    cfg : string, optional
        fmdt_reduce configuration file name extension. Default is '_config.in'.
    visu : string, optional
        FMDT output video file extension name. Default is '_visu'.
    crash : Boolean, optional
        If True and the FMDT command does not go through, raise an Error. Default is False.
    nofmdt : Boolean, optional
        If True the FMDT command is not run. Useful for debug purpose. Default is False.
    noXtract : Boolean, optional
        If True the frame extraction is not performed. Useful for debug purpose. Default is False.
    
    Returns
    -------
    None.

    """    
    # make output files names
    log.debug('vid_file: '+vid_file)
    [bznm,vex] = os.path.basename(vid_file).split('.')
    common = pth_out + out_root + bznm
    # prevent empty string
    if not len(common):
        msg = '*** FATAL ERROR: input video file name is empty: '+vid_file
        log.error(msg)
        raise ValueError(msg)
    # meteor frame output directory
    pth_frame = pth_out+'/meteor_frame/'
    if not os.path.exists(pth_frame):
        os.mkdir(pth_frame)
    
    # make output file names
    track_file = common + trk
    bb_file = common + bb
    mag_file = common + mag
    conf_file = common + cfg
    if pth_out in vid_file:
        visu_file = vid_file.replace(vex,visu+'.'+vex)
    else:
        visu_file = common + visu+'.'+vex
    
    # build fmdt-detect command
    cmd = fmdt_params['exe'] +' --in-video '+ pth_in+vid_file+ \
        ' --out-bb ' + bb_file + \
        ' --out-mag '+ mag_file + \
        ' --track-all ' + \
        ' --light-min '+str(fmdt_params['light_min']) + \
        ' --light-max '+str(fmdt_params['light_max']) + \
        ' --extrapol-order 0 ' + \
        ' --max-dist '+str(fmdt_params['max_dist']) + \
        ' --surface-min '+str(fmdt_params['surface_min']) + \
        '  --fra-star-min '+str(fmdt_params['fra_star_min']) + \
        ' > '+track_file
    log.info('* Launch FMDT detect cmd: '+cmd)
    submitt_fmdt(cmd,crash=crash,nosub=nofmdt)
    
    # build fmdt-visu command
    cmd  = fmdt_params['visu'] + ' --in-video '+ pth_in+vid_file + \
        ' --in-tracks ' +track_file + \
        ' --in-bb '+ bb_file + \
        ' --out-video ' + visu_file + \
        ' --show-id --nat-num'
    log.info('* Launch FMDT visu cmd: '+cmd)
    submitt_fmdt(cmd,crash=crash,nosub=nofmdt)
    
    if not noXtract:
        # extract frames from original video for astrometry purpose
        video_track2frame(pth_in+vid_file,track_file,out_pth=pth_frame,fmt='%5d.png')
        # extract frames from visu video for visualisation purpose
        video_track2frame(visu_file,track_file,out_pth=pth_frame,fmt='%5d.png')
    
    # create a fmdt_reduce configuration file
    make_config_fmdt_reduce(config_template,conf_file,pth_in,pth_out,pth_frame,
                            bznm,track_file,bb_file,mag_file)
    log.debug('fmdt_reduce conf_file: '+conf_file)
    # launch fmdt-reduce
    log.info('Now launching fmdt_reduce with files: '+conf_file+' '+track_file+' '+bb_file+' '+mag_file)
    fmdt_reduce.fmdt_reduce(conf_file)
    return

def process_images(root,config_template,fmdt_params,ext='%05d.pgm',
                   pth_in='./',pth_out='./',out_root='./res_',
                   trk='_track.dat',bb='_bb.dat',mag='_mag.dat',
                   cfg='_config.in',crash=False,nofmdt=False,noXtract=False):
    """Tune fmdt options based on a serie of images.
    
    Parameters
    ----------
    root : string
        Root of each image file name. Ex: 'Image_'.
    config_template : string
        fmdt-reduce configuration file template.
    fmdt_params : Dict
        dictionnary of FMDT parameters.
    ext : string, optional
        Image file extension. Default is '%05d.pgm', see FMDT program.
    pth_in :string, optional
        Input path. Default is './'.
    pth_out :string, optional
        Output path. Default is './'.
    out_root : string, optional
        FMDT output file root name. Default is './res-'.
    trk : string, optional
        FMDT track output file name extension. Default is '_track.dat'
    bb : string, optional
        FMDT box output file name extension. Default is '_bb.dat'
    mag : string, optional
        FMDT magnitude output file name extension. Default is '_mag.dat'
    cfg : string, optional
        fmdt_reduce configuration file name extension. Default is '_config.in'
    crash : Boolean, optional
        If True and the FMDT command does not go through, raise an Error. Default is False.
    nofmdt : Boolean, optional
        If True the FMDT command is not run. Useful for debug purpose. Default is False.
    noXtract : Boolean, optional
        If True the frame extraction is not performed. Useful for debug purpose. Default is False.
    
    Returns
    -------
    None.

    """
    # debug
    log.debug('root : '+root)
    log.debug('config_template : '+config_template)
    log.debug('ext : '+ext)
    log.debug('pth_in : '+pth_in)
    log.debug('pth_out : '+pth_out)
    log.debug('out_root : '+out_root)
    log.debug('trk : '+trk)
    log.debug('bb : '+bb)
    log.debug('mag : '+mag)
    log.debug('cfg : '+cfg)
    log.debug('crash : '+str(crash))
    # make output files names
    common = pth_out + out_root + root
    log.debug('common : '+common)
    track_file = common + trk
    bb_file = common + bb
    mag_file = common + mag
    conf_file = common + cfg
    if pth_out in root:
        visu_file = root+'_visu.mp4'
    else:
        visu_file = common + '_visu.mp4'  
    # meteor frame output directory
    pth_frame = pth_out+'/meteor_frame/'
    if not os.path.exists(pth_frame):
        os.mkdir(pth_frame)
    
    # build fmdt-detect command
    cmd =  fmdt_params['exe'] +' --in-video ' + \
        pth_in +'/' + root + ext + \
        ' --out-bb ' + bb_file + \
        ' --out-mag '+ mag_file + \
        ' --track-all ' + \
        ' --light-min '+str(fmdt_params['light_min']) + \
        ' --light-max '+str(fmdt_params['light_max']) + \
        ' --extrapol-order 0 ' + \
        ' --max-dist '+str(fmdt_params['max_dist']) + \
        ' --surface-min '+str(fmdt_params['surface_min']) + \
        '  --fra-star-min '+str(fmdt_params['fra_star_min']) + \
        ' > '+track_file
    log.info('* Launch detect cmd: '+cmd)
    submitt_fmdt(cmd,crash=crash,nosub=nofmdt)
    
    # build fmdt-visu command
    cmd  = fmdt_params['visu'] + ' --in-video '+ \
        pth_in +'/' + root + ext + \
        ' --in-tracks ' +track_file + \
        ' --in-bb '+ bb_file + \
        ' --out-video ' + \
        visu_file + \
        ' --show-id --nat-num'
    log.info('* Launch visu cmd: '+cmd)
    submitt_fmdt(cmd,crash=crash,nosub=nofmdt)
    
    if not noXtract:
        # extract frames from original video for astrometry purpose
        imgseq_track2frame(pth_in,track_file,'pgm',out_pth=pth_frame,fmt_o='%5d.png')
        # extract frames from visu video for visualisation purpose
        video_track2frame(visu_file,track_file,out_pth=pth_frame,fmt='%5d.png')    
    
    # create fmdt_reduce config file
    make_config_fmdt_reduce(config_template,conf_file,pth_in,pth_out,pth_frame,
                            root,\
                            track_file,bb_file,mag_file,imgseq=True)
    # launch fmdt-reduce
    log.info('Now launching fmdt_reduce with files: '+conf_file+' '+track_file+' '+bb_file+' '+mag_file)
    fmdt_reduce.fmdt_reduce(conf_file)
    return

def make_config_fmdt_reduce(config_template,conf_file,pth_in,pth_out,pth_frame,\
                            root,\
                            track_file,bb_file,mag_file,\
                            imgseq=False,fmt_ori='%5d.tiff'):
    """Creates a pyfmdt_reduce configuration file.

    Parameters
    ----------
    config_template : string
        fmdt_reduce template configuration file.
    config_file : string
        fmdt_reduce output configuration file.
    pth_in : string
        Data directory.
    pth_out : string
        Output data directory.
    pth_frame : string
        Meteor frame file directory.
    root : string
        Data file root name.
    track_file : string, optional
        FMDT track output file name.
    bb_file : string, optional
        FMDT box output file name.
    mag_file : string, optional
        FMDT magnitude output file name.
    imgseq : Boolean, optional
        If True, image sequence section is created. Default is False.
    fmt_ori : string, optional
        Original image format. Useful is imgseq is True. Default is '.tiff'.
    
    Returns
    -------
    None.

    """  
    # debug
    log.debug('config_template: '+config_template)
    log.debug('conf_file: '+conf_file)
    log.debug('pth_in: '+pth_in)
    log.debug('track_file: '+track_file)
    log.debug('bb_file: '+bb_file)
    log.debug('mag_file: '+mag_file)
    # read template config file
    check_file(config_template)
    config = ConfigParser(interpolation=ExtendedInterpolation())
    config.read(config_template)
    config['USER']['data_dir'] = pth_in
    config['USER']['out_dir'] = pth_out
    config['USER']['frame_dir'] = pth_frame
    config['USER']['root'] = root
    config['FMDT']['track_file'] = track_file
    config['FMDT']['bb_file'] = bb_file
    config['FMDT']['mag_file'] = mag_file
    # set the image sequence section
    if imgseq:
        config['USER']['type'] = 'img_seq'
        config['IMGSEQ']['pth_ori'] = pth_in.replace('_processed','')
        config['IMGSEQ']['fmt_ori'] = fmt_ori
    else:
        config['USER']['type'] = 'video'
    # write changes back to file
    with open(conf_file,'w') as fo:
        config.write(fo)
    log.info('fmdt_reduce configuration file saved in '+conf_file)
    return 

if __name__ == '__main__':
    
    # parse arguments
    parser = argparse.ArgumentParser(description='launch_fmdt.py arguments.')
    parser.add_argument('-i', default='./', help='Input data path. Default is: ./')
    parser.add_argument('-o', default='./', help='Output data path. Default is: ./')
    parser.add_argument('-p', default='', help='Video or Images sub-directory file pattern. Example 1: p="Basler" if all video files starts with "Basler_". Example 2: for an image sequence stored in /my/path/, p="pgm" if pgm files are stored in /my/path_pgm/. Default is empty string.')
    parser.add_argument('-x', default='.mkv', help='Input file extension. Ex: .mkv  for video, or .pgm for image sequence. Default is .mkv')
    parser.add_argument('-img', action='store_true', help='If set, image sequence is considered, rather than video. Default is False.')
    parser.add_argument('-r', default='', help='FMDT output file root name. Default is empty string.')
    parser.add_argument('-c', default=home+'/fmdt/pyfmdt/conf/config_template.in', help='fmdt_reduce configuration file template full path name. Default is: $home/fmdt/pyfmdt/conf/config_template.in')
    parser.add_argument('-fx', default=home+'/fmdt/build/bin/fmdt-detect-rt-pip', help='FMDT-detect full path executable file. Default is: $home/fmdt/build/bin/fmdt-detect-rt-pip')
    parser.add_argument('-fv', default=home+'/fmdt/build/bin/fmdt-visu', help='FMDT-visu full path executable. Default is: $home/fmdt/build/bin/fmdt-visu')
    parser.add_argument('-fli', default=68, help='FMDT-detect light_min parameter. Default is 68.')
    parser.add_argument('-fla', default=79, help='FMDT-detect light_max parameter. Default is 79.')
    parser.add_argument('-fsm', default=6, help='FMDT-detect surface_min parameter. Default is 6.')
    parser.add_argument('-ffs', default=10, help='FMDT-detect fra_star_min parameter. Default is 10.')
    parser.add_argument('-fmd', default=25, help='FMDT-detect max_dist parameter. Default is 25.')
    parser.add_argument('-nofmdt', action='store_true', help='If set, FMDT is not launched. Useful for debug purpose.')
    parser.add_argument('-noXtract', action='store_true', help='If set, frame extraction is not performed. Useful for debug purpose.')
    args = parser.parse_args()
    
    # store fmdt parameters
    fmdt_params={'exe':args.fx,
                 'visu':args.fv,
                 'light_min':args.fli,
                 'light_max':args.fla,
                 'surface_min':args.fsm,
                 'fra_star_min':args.ffs,
                 'max_dist':args.fmd}
    
    # store arguments
    pth_in = args.i
    pth_out = args.o
    fil_pat = args.p
    ext = args.x
    out_root = args.r
    config_template = args.c
    imgseq = args.img
    nofmdt = args.nofmdt
    noXtract = args.noXtract
        
    # launch FMDT
    launch_fmdt(pth_in,pth_out,fil_pat,ext,out_root,config_template,
                fmdt_params=fmdt_params,imgseq=imgseq,nofmdt=nofmdt,
                noXtract=noXtract)
else:
    log.debug('successfully imported')

