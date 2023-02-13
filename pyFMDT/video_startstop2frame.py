#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Extract meteor frames from video.

Created on Fri Jan 20 11:12:02 2023

@author: vaubaill
"""

import os
import glob
import argparse

from fmdt_log import log


def make_img_name(vid_file,frame,fmt='%5d.png',met_id=None):
    """Make frame image file name from video file name and frame number.
    
    Parameters
    ----------
    vid_file : string
        Video file name.
    frame: int
        Frame number.
    fmt : string, optional
        Ouput file name format. Default is: '%5d.png'.
    met_id : int, optional
        Object id. Default is None.
    
    Returns
    -------
    img_name : string
        Frame image file name.
    """
    (root,vid_ext) = os.path.splitext(vid_file)
    [f_fmt,f_ext] = fmt.split('.')
    [f_fmt,f_ext] = fmt.split('.')
    # extract number of digits in name extension
    dig = int(f_fmt.replace('%','').replace('d',''))
    frmstr = '_'+str(frame).zfill(dig)
    # add meteor id if needed
    if met_id:
        idstr = '_'+str(met_id).zfill(dig)+'_'
    else:
        idstr = '_'
    img_name = root + idstr + frmstr
    img_name = img_name + '.' + f_ext
    return img_name


def vid2meteorframe(vid_file,frame_srt,frame_end,root='meteor_',\
                    out_dir=None,fmt='%5d.png',met_id=None):
    """Extract specific frames from video.

    Parameters
    ----------
    vid_file : string
        Video file name.
    frame_srt : int
        Frame start number.
    frame_end : int
        Frame end number.
    root : string, optional.
        Output file root name. Default is 'meteor_'.
    out_dir : string, optional
        Full output file directory. Default is None, meaning that images are saved in the
        same directory as vid_file.
    fmt : string, optional
        Ouput file name format. Default is: '%5d.png'.
    
    Returns
    -------
    None.

    """
    # change extention if needed
    if (frame_end>99999) and ('%5d.' in fmt):
        common,ext = fmt.split('.')
        fmt = '%6d.'+ext
    # build ffmpeg command
    cmd = 'ffmpeg -i ' + vid_file + ' -vf select=\'between(n\\,'+str(frame_srt)+'\\,'+str(frame_end)+')\' -vsync 0 '+root+fmt
    log.info('cmd: '+cmd)
    os.system(cmd)
    # rename output file so that it has the frame number
    pattern = root+fmt
    pattern = pattern.replace('%5d','*')
    list_file = glob.glob(pattern)
    list_file.sort()
    log.debug('list_file: '+str(list_file))
    for (f,num) in zip(list_file,range(frame_srt,frame_end+1)):
        fo = make_img_name(vid_file,num,fmt=fmt,met_id=met_id)
        if out_dir:
            fo = out_dir + os.path.basename(fo)
        log.debug('renaming: '+f+' into: '+fo)
        os.rename(f, fo)
    log.info('done')
    return

if __name__ == '__main__':
    
    # parse arguments
    parser = argparse.ArgumentParser(description='Extract frames from video by providing start/stop frame numbers.')
    parser.add_argument('-i',help='input video file.')
    parser.add_argument('-s',help='Start frame number.')
    parser.add_argument('-e',help='Stop frame number.')
    parser.add_argument('-o',help='Output file directory. Default is None, meaning that images are saved in the same directory as the input video directory.',default=None)
    parser.add_argument('-r',help='Output frame file name root. Default: meteor_%5d.',default='meteor_')
    parser.add_argument('-f',help='Output image format. Default is %5d.png',default='%5d.png')
    parser.add_argument('-m',help='Meteor id. Default: None.',default=None)

    args = parser.parse_args()
    
    # set video file and output root name
    vid_file = args.i
    frame_srt = int(args.s)
    frame_end = int(args.e)
    root = args.r
    fmt = args.f
    met_id = args.m
    out_dir = args.o
    
    # launch 
    vid2meteorframe(vid_file,frame_srt,frame_end,root=root,fmt=fmt,met_id=met_id,out_dir=out_dir)
    
else:
    log.debug('successfully imported')
