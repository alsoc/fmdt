#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Extract meteor frames from video.

Created on Fri Jan 20 11:12:02 2023

@author: vaubaill
"""

import os
import logging
import argparse


# logging
log = logging.getLogger(__name__)
log.setLevel(logging.DEBUG)
log_fmt = '%(levelname)s %(filename)s %(lineno)d (%(funcName)s) : %(message)s '
sdlr = logging.StreamHandler()
sdlr.setFormatter(logging.Formatter(fmt=log_fmt))
log.addHandler(sdlr)


def vid2meteorframe(vid_file,frame_srt,frame_end,root='frame_meteor_',fmt='%5d.pgm'):
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
        Output file root name. Default is 'frame_meteor_'.
    fmt : string, optional
        Ouput file name format. Default is: '%5d.pgm'.
    
    Returns
    -------
    None.

    """
    # ex ffmpeg -i test_w3_0500_tracked.mp4 -vf select='between(n\,149\,157)' -vsync 0 frames_meteor_%5d.pgm
    # build ffmpeg command
    cmd = 'ffmpeg -i ' + vid_file + ' -vf select=\'between(n\\,149\\,157)\' -vsync 0 '+root+fmt
    log.info('cmd: '+cmd)
    os.system(cmd)
    log.info('done')
    return



if __name__ == '__main__':
    
    # parse arguments
    parser = argparse.ArgumentParser(description='fmdt-reduce arguments.')
    parser.add_argument('-i',help='input video file.')
    parser.add_argument('-s',help='Start frame number.')
    parser.add_argument('-e',help='Output frame file name root. Default: frame_meteor_%5d.')
    parser.add_argument('-r',help='Output frame file name root. Default: frame_meteor_%5d.',default='frame_meteor_')
    parser.add_argument('-f',default='_meteor_%5d.pgm',help='Output image format. Default is %5d.pgm')

    args = parser.parse_args()
    
    # set video file and output root name
    vid_file = args.i
    frame_srt = int(args.s)
    frame_end = int(args.e)
    root = args.r
    fmt = args.f
    
    # launch 
    vid2meteorframe(vid_file,frame_srt,frame_end,root=root,fmt=fmt)
    
else:
    log.debug('successfully imported')
