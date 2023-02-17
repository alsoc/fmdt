#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri Feb  3 09:22:51 2023

@author: vaubaill
"""
import argparse
from fmdt_log import log

import fmdt_read
from video_startstop2frame import vid2meteorframe


def video_track2frame(video_file,track_file,out_pth=None,fmt='%5d.png'):
    """Extract meteor frames from video file, given FMDT output track file.

    Parameters
    ----------
    video_file : string
        Input video file name.
    track_file : string
        Input FMDT track file name.
    out_pth : string, optional
        Output frame directory. The default is None, meaning that images are
        saved in the same directory as the input video directory.
    fmt : string, optional
        Ouput file name format. Default is: '%5d.pgm'.

    Returns
    -------
    None.

    """
    # read FMDT track file
    log.info('Reading track file '+track_file)
    trk_data = fmdt_read.read_track(track_file)
    
    # select meteor track data
    met_trk = trk_data[trk_data['type']=='meteor']
    # for each meteor: extract frames
    for met in met_trk:
        met_id = met['id']
        frame_srt = met['f_b']
        frame_end = met['f_e']
        log.info('Extract meteor id= '+str(met_id)+' from '+video_file)
        vid2meteorframe(video_file,frame_srt,frame_end,met_id=met_id,\
                        fmt=fmt,out_dir=out_pth)
    log.info('Done.')
    return


if __name__ == '__main__':
    
    # parse arguments
    parser = argparse.ArgumentParser(description='Extract meteor frames from video and FMDT track file.')
    parser.add_argument('-v',help='Input video file.')
    parser.add_argument('-t',help='Input FMDT track file.')
    parser.add_argument('-o',help='Output file directory. Default is None, meaning that images are saved in the same directory as the input video directory.',default=None)
    parser.add_argument('-f',help='Output image format. Default is %5d.pgm',default='%5d.png')
    args = parser.parse_args()
    
    # set video file and output root name
    vid_file = args.v
    trk_file = args.t
    out_dir = args.o
    fmt = args.f
    
    # launch 
    video_track2frame(vid_file,trk_file,out_dir=out_dir,fmt=fmt)
    
else:
    log.debug('successfully imported')
