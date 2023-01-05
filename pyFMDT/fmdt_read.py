#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Mon Nov 22 12:40:42 2022

@author: vaubaill
"""
# Read FMDT software output data files.

import os
import glob
import shutil
import numpy as np
from astropy.time import Time
from astropy.table import QTable
import astropy.units as u
import pandas as pd


def read_fmdt_outputs(track_file,bb_file,mag_file):
    """Read FMDT output data.

    Parameters
    ----------
    track_file : string
        FMDT tracking output data file name.
    bb_file : string
        FMDT bb output data file name.
    mag_file : string
        FMDT magnitude output data file name.

    Returns
    -------
    trk_data : astropy.table.QTable object.
        FMDT tracking output data.
    bb_data : astropy.table.QTable object.
        FMDT bb output data.
    mag_data : astropy.table.QTable object.
        FMDT magnitude output data.

    """
    trk_data = read_track(track_file)
    bb_data = read_bb(bb_file)
    mag_data = read_mag(mag_file)
    return trk_data,bb_data,mag_data

def read_track(track_file):
    """Read FMDT output track data.

    Parameters
    ----------
    track_file : string
        FMDT tracking output data file name.


    Returns
    -------
    trk_data : astropy.table.QTable object.
        FMDT tracking output data.

    """
    col_names = ['id','f_b','x_b','y_b','f_e','x_e','y_e','type']
    trk_data = QTable().read(track_file,format='ascii.fixed_width',data_start=0,names=col_names)
    return trk_data

def read_bb(bb_file):
    """Rerad FMDT output track data.

    Parameters
    ----------
    bb_file : string
        FMDT bb output data file name.


    Returns
    -------
    bb_data : astropy.table.QTable object.
        FMDT bb output data.

    """
    #col_names = ['id','Xr','Yr','Xc','Yc','trk']
    col_names = ['frame','Xr','Yr','Xc','Yc','id','is_extrapolated']
    bb_data = QTable().read(bb_file,format='ascii.fast_no_header',names=col_names)
    return bb_data

def read_mag(mag_file):
    """Read magnitude data.

    Parameters
    ----------
    mag_file : string
        FMDT magnitude output data file name.
    
    Returns
    -------
    mag_data : dict
        FMDT magnitude output data.
        id : FMDT object id
            type : string
                FMDT object type: star, meteor, noise.
            data : numpy.array
                FMDT pixel count for each frame.
    
    """
    mag_data = {}
    mag_lines = read_mag_lines(mag_file)
    for line in mag_lines:
        data = line.split()
        objid = int(data[0])
        mag_data[objid] = {'type': data[1], 'data': np.array(data[2:],dtype='int')}
    return mag_data

def read_mag_lines(mag_file):
    """Read magnitude data.

    Parameters
    ----------
    mag_file : string
        FMDT magnitude output data file name.
    objid : int
        FMDT object id for which pixel counts are to be retrieved.

    Returns
    -------
    mag_data : astropy.table.QTable object.
        FMDT magnitude output data.

    """
    with open(mag_file,'r') as f:
        lines = f.readlines()
    return lines

def get_obj_mag(mag_lines,objid):
    """Read magnitude data.

     Parameters
    ----------
    mag_lines : string
        Lines in the FMDT magnitude data file.
    objid : int
        FMDT object id for which pixel counts are to be retrieved.

    Returns
    -------
    mag_data : astropy.table.QTable object.
        FMDT magnitude output data.

    """
    for line in mag_lines:
        if line.lstrip().startswith(str(objid)):
            mag_data = line.split()
    return mag_data
    
