#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Jan 19 11:32:43 2023

@author: A. Pautova, IMCCE, 2023
"""

import os
import glob
import argparse
import logging
import numpy as np
import astropy.units as u

### LOGGING ###


# creates a logger
logger = logging.getLogger(__name__)

# set the logger level
logger.setLevel(logging.DEBUG)

# define file handler and set formatter
file_handler = logging.FileHandler('logfile.log')
formatter = logging.Formatter('%(asctime)s : %(levelname)s : %(name)s : %(message)s')
file_handler.setFormatter(formatter)

# logging
log = logging.getLogger(__name__)
log.setLevel(logging.DEBUG)
log_fmt = '%(levelname)s %(filename)s %(lineno)d (%(funcName)s) : %(message)s '
sdlr = logging.StreamHandler()
sdlr.setFormatter(logging.Formatter(fmt=log_fmt))
log.addHandler(sdlr)

def img2img(pth_in,pth_out,sub_root,img_in,img_out,):
    """ Convert the images to the correct pattern (change format and rename)

    Parameters
    ----------
    pth_in : string
        path to the folder containing the images.
    pth_out : string
        path to output folder.
    sub_root : string
        subfolder root name.
    img_in : string
        format of input image.
    img_out : string
        format of output image.
        
    Returns
    -------
    None.

    """  
    
    # save current directory and change directory
    curdir = os.getcwd()
    logger.info('Relocating to '+pth_in)
    try:
        os.chdir(pth_in)
    except:
        logger.error('Sorry I am lost...')
    # list of sub_files 
    sub_list = glob.glob(sub_root+'*')
    logger.info('sub_list: '+str(sub_list))
    # loop processing each sub_file
    for sub in sub_list : 
        os.chdir(pth_in+sub)
        logger.info('Relocating to '+pth_in+sub)
        # list of images
        img_list = glob.glob(sub+'*')
        # loop processing each image
        for img in img_list : 
            # rename the img
            img_und = img.split('_')[-1]
            img_bis = img_und.zfill(10)
            new_img = img.replace(img_und,img_bis)
            cmd = 'mv '+img+' '+new_img
            logger.info('cmd to rename '+cmd)
            try:
                os.system(cmd)
            except:
                logger.error('image '+img+'has not been renamed')
            # build convert command
            cmd = 'convert '+new_img+' '+new_img.replace(img_in,img_out)
            logger.info('cmd to convert '+cmd)
            try:
                os.system(cmd)
            except:
                logger.error('image '+new_img+'has not been converted')
            # mv output files
            cmd = 'mv *'+img_out+' '+pth_out+sub+'_processed'
            logger.info('cmd to mv : '+cmd)
            try:
                os.system(cmd)
            except:
                logger.error('folder '+sub+' has not been moved')
    # go back to initial directory
    os.chdir(curdir)
    logger.info('Got back to initial directory.')


if __name__ == '__main__':
    
    # parse arguments
    parser = argparse.ArgumentParser(description='img2img.py arguments.')
    parser.add_argument('-ii', default='.tiff', help='Image file extension. Default is: .tiff')
    parser.add_argument('-io', default='.pgm', help='Output image file extension. Default is: .pgm')
    parser.add_argument('-pi', default='/Volumes/Expansion/TAH2022/Basler_2040/', help='Path to images. Default is: /Volumes/Expansion/TAH2022/Basler_2040/')
    parser.add_argument('-po', default='/Volumes/Expansion/TAH2022/Basler_2040/Images_PGM/', help='Output path of processed images. Default is: /Volumes/Expansion/TAH2022/Basler_2040/Images_PGM/')
    parser.add_argument('-sr', default='Basler', help='Subfolder root name. Default is: Basler')
    args = parser.parse_args()
    
    # 
    ii = args.ii
    io = args.io
    pi = args.pi
    po = args.po
    sr = args.sr
  
    
    # launch 
    img2img(pi,po,sr,ii,io)

    
else:
    logger.debug('successfully done')
              
        
    





