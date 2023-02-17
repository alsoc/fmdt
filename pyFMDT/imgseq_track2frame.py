#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri Feb  3 10:41:30 2023

@author: vaubaill
"""
import os
import glob
import numpy as np
from PIL import Image

import argparse
from fmdt_log import log
import fmdt_read

def convert_img(file_i,file_o,fmt='PNG'):
    """
    

    Parameters
    ----------
    file_i : TYPE
        DESCRIPTION.
    fmt : TYPE
        DESCRIPTION.

    Returns
    -------
    None.

    """
    # open input image file
    im = Image.open(file_i)
    # remove dot from format
    fmt = fmt.replace('.','')
    im.save(file_o, fmt.upper(), quality=100)
    return

def make_seqimg_name(frame_file,frame,fmt='%5d.png',met_id=None,pth_out=None):
    """Make output file name from image sequence and frame number.

    Parameters
    ----------
    frame_file : string
        Input image file name.
    frame : int
        Frame number.
    fmt : string, optional
        Output format. The default is '%5d.png'.
    met_id : int, optional
        Meteor id. The default is None. If set, the output file name includes it.
    pth_out : string, optional
        Output path. The default is None, meaning that the input file diretory 
        name will be considered.

    Returns
    -------
    file_out : string
        Output file name.

    """
    if pth_out:
        path_out = pth_out
    else:
        path_out = os.path.dirname(frame_file) + '/'
    # extract root, extension and number of digits in name extension
    (root,img_ext) = os.path.splitext(os.path.basename(frame_file))
    [f_fmt,f_ext] = fmt.split('.')
    dig = int(f_fmt.replace('%','').replace('d',''))
    frmstr = '_'+str(frame).zfill(dig)
    # remove tailing frame number from frame file name
    if (root.endswith(frmstr)):
        root = root.replace(frmstr,'')
    # add meteor id if needed
    if met_id:
        idstr = '_'+str(met_id).zfill(dig)+'_'
    else:
        idstr = '_'
    file_out = path_out + root + idstr + frmstr + '.' + f_ext
    return file_out


def imgseq_track2frame(pth_in,track_file,fmt_i,out_pth=None,fmt_o='%5d.png'):
    """Extract meteor frames from video file, given FMDT output track file.

    Parameters
    ----------
    pth_in : string
        Input image sequence directory.
    track_file : string
        Input FMDT track file name.
    fmt_i : string
        Input image file format. Ex: '.tiff'
    out_pth : string, optional
        Output frame directory. The default is None, meaning that images are
        saved in the same directory as the input image sequence directory.
    fmt_o : string, optional
        Ouput file name format. Default is: '%5d.png'.

    Returns
    -------
    None.

    """
    # list image sequence
    pattern = pth_in + '/*' + fmt_i
    list_img = glob.glob(pattern)
    num_img = len(list_img)
    log.info('There are '+str(num_img)+ ' image files of pattern '+pattern+' found')
    if not num_img:
        msg = '*** FATAL ERROR: no image of pattern '+pattern+' found'
        log.error(msg)
        raise IOError(msg)
    list_img.sort()
    
    # read FMDT track file
    log.info('Reading track file '+track_file)
    trk_data = fmdt_read.read_track(track_file)
    
    # select meteor track data
    met_trk = trk_data[trk_data['type']=='meteor']
    # for each meteor: extract frames
    for met in met_trk:
        met_id = met['id']
        log.info('Extract meteor id= '+str(met_id)+' from '+pth_in)
        # loop over each meteor frame
        for frame in np.arange(met['f_b'],met['f_e']+1,1):
            frame_file = list_img[frame] # Note in track file frame number stars at 0
            file_o = make_seqimg_name(frame_file,frame,fmt=fmt_o,met_id=met_id,pth_out=out_pth)
            convert_img(frame_file,file_o,fmt='PNG')
    log.info('Done.')
    return


if __name__ == '__main__':
    
    # parse arguments
    parser = argparse.ArgumentParser(description='Extract meteor frames from video and FMDT track file.')
    parser.add_argument('-i',help='Input image sequence path.')
    parser.add_argument('-t',help='Input FMDT track file.')
    parser.add_argument('-o',help='Output file directory. Default is None, meaning that images are saved in the same directory as the input video directory.',default=None)
    parser.add_argument('-fi',help='Input image format. Default is .tiff',default='.tiff')
    parser.add_argument('-fo',help='Output image format. Default is %5d.pgm',default='%5d.png')
    args = parser.parse_args()
    
    # set video file and output root name
    pth_i = args.i
    trk_file = args.t
    out_dir = args.o
    fmt_i = args.fi
    fmt_o = args.fo
    
    # launch 
    imgseq_track2frame(pth_i,trk_file,fmt_i,out_dir=out_dir,fmt_o=fmt_o)
    
else:
    log.debug('successfully imported')
