#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Reorganize tiff images and create short videos for further processing.


Created on Tue Dec  6 10:33:20 2022

@author: vaubaill
"""

import os
import glob
import shutil
import numpy as np
import astropy.units as u


# User input settings
# camera frame per second
fps = 10 / u.s
# image format
img_fmt = '.tiff'
# path to process
# pth = '/media/sf_Volumes/LaCie/DATA_OBS/2022-TAH/TAH2022/Basler1920/'
#pth = '/Volumes/LaCie/DATA_OBS/2022-TAH/TAH2022/Basler_2040/'
pth = '/Volumes/Expansion/TAH2022/Basler_2040/'
#pth = '/media/sf_Volumes/LaCie/DATA_OBS/2022-TAH/TAH2022/Basler_2040/'
#pth = '/media/sf_Volumes/Expansion/TAH2022/Basler_2040/'
# sub_dir pattern to process:
subpattern = 'Basler_*_*'

# User output settings
# directory file listing root name
filist_root = 'filelist.txt'
# max duration of videos to create
max_dur = 5 *u.min
# temporary directory
tmp_dir = os.getenv('HOME')+'/tmp/'
# video extension
vid_ext = '.mkv'

# Main program
# save current directory and change directory
curdir = os.getcwd()
print('Relocating to '+pth)
os.chdir(pth)

# list of sub-directories
sublist = glob.glob(subpattern+ os.path.sep)
print('sublist: '+str(sublist))

for subpth in sublist:
    print('======= Processing subpth: '+subpth)
    print(os.getcwd())
    # read directory file listing: see make_listdir.py
    list_file = subpth.replace('/','')+'-list.txt'
    print('Now reading file listing: '+list_file)
    if not os.path.exists(list_file):
        raise IOError(list_file+' does NOT exist: run make_list.py first.')
    with open(list_file) as f:
        lines = f.readlines()
    
    nlines = len(lines)
    print('There are '+str(nlines)+' files to process')
    # total number of images per video
    nvid =  int(max_dur.to('s') * fps)
    print('There will be '+str(nvid)+' img in output video')
    # set star / stop index
    idx_start = np.arange(0,nlines,nvid)
    idx_stop = idx_start+nvid
    # count the number of file in last video sequence
    nlast = idx_start[-1]-nlines
    print(idx_start)
    print(idx_stop)    
    print(nlast)
    # if there is only 1 output file
    if len(idx_start)==1:
        idx_stop = np.array([nlines])
    else:
        # change if output video last less than 10 sec
        if nlast<(fps.value*60):
            idx_start = idx_start[:-1]
            idx_stop = idx_stop[:-1]
            idx_stop[-1] = nlines
    print(idx_start)
    print(idx_stop)    
    
    
    for idstart,idstop in zip(idx_start,idx_stop):
        print('Processing files from index '+str(idstart))
        # make video output file name
        vid_file = subpth.replace('/','')+'-'+str(idstart).zfill(5)+vid_ext
        print('Video will be saved in: '+vid_file)
        # clean previous work: delete temprorary directory if needed
        if os.path.isdir(tmp_dir):
            print('Cleaning: remove all data from '+tmp_dir)
            shutil.rmtree(tmp_dir)
        print('Making new temporary directory: '+tmp_dir)
        os.mkdir(tmp_dir)
        # go to temporary directory
        os.chdir(tmp_dir)
        for file2link,i in zip(lines[idstart:idstop],np.arange(idstop-idstart)):
            src = pth+subpth+'/'+file2link.replace('\n','')
            dst = './img-'+str(i).zfill(5)+'.tiff'
            print('symlink '+src+' into '+dst)
            os.symlink(src, dst)
        # launch video creation command
        cmd = 'ffmpeg -i img-%05d.tiff -framerate 20 -c:v libx264 -crf 0 -pix_fmt yuv420p -preset veryslow '+vid_file
        print('cmd: '+cmd)
        os.system(cmd)
        # move video to subdir
        dst = pth+'/Videos/'+vid_file
        print('Now moving video to '+vid_file,dst)
        shutil.move(vid_file,dst)
        # go back to pth directory
        os.chdir(pth)
    
        
# go back to initial directory
os.chdir(curdir)









