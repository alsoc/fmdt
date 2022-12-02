#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Mon Nov 21 14:17:42 2022

@author: vaubaill
"""
# modify RMS .config file and put correct plane position and heading.

import os
import glob
import shutil
import spiceypy as sp
from astropy.time import Time
from astropy.coordinates import EarthLocation
import astropy.units as u

# USERS input:
#SPICE kernels trajectory directory
spice_dir = '/astrodata/kernels/'
# airplane trajectory directory
plane_dir = '/home/vaubaill/PROJECTS/PODET/PODET-MET/MALBEC/malbec/DATA/20220531/PHENOM300/'
# detection directory
data_dir = '/Users/vaubaill/PROJECTS/PODET/PODET-MET/OBS-Campaign/2022-TAH/Data/DataPH0001/PH0001/RMS_data/CapturedFiles/'

# load SPICE kernels
sp.furnsh(spice_dir+'standard.ker')
sp.furnsh(plane_dir+'PHENOM300.spk')
sp.furnsh(plane_dir+'PHENOM300.ik')
sp.furnsh(plane_dir+'PHENOM300.tf')
sp.furnsh(plane_dir+'PHENOM300_fake.tsc')
sp.furnsh(plane_dir+'PHENOM300_CAMORI_FRAME.tk')
sp.furnsh(plane_dir+'PHENOM300.ck')
# retrieve plane id and name
plane_id = sp.spkobj(plane_dir+'PHENOM300.spk')[0]
plane_name = sp.bodc2n(plane_id)

# change directory
cwd = os.getcwd()
os.chdir(data_dir)

# loop over all RMS config files
list_conf = glob.glob('*/.config')
for conf in list_conf:
    print('*** Processinf config file '+conf)
    # make a copy of the config file
    dst = conf + '_original'
    print('Original file saved in '+dst)
    shutil.copyfile(conf, dst)
    if not os.path.isfile(dst):
        raise IOError('Original configuration file not copied!!!')
    # compute UT time
    time_lst = os.path.dirname(conf).split('/')[-1].split('_')
    time_str = time_lst[1][:4]+'-'+time_lst[1][4:6]+'-'+time_lst[1][6:8]+'T'
    time_str = time_str + time_lst[2][:2]+':'+time_lst[2][2:4]+':'+time_lst[2][4:6]
    time_str = time_str + '.'+time_lst[2][:3]
    time = Time(time_str,scale='utc')
    et = sp.utc2et(time_str)
    (eph, lt) = sp.spkezr(plane_name,et,'ITRF93','NONE','EARTH')
    eloc = EarthLocation.from_geocentric(eph[0],eph[1],eph[2],unit=u.km)
    # change config file
    with open(conf,'r') as f:
        lines = f.readlines()
    conf_test = conf+'_tst' 
    with open(conf_test,'w') as f:
        for line in lines:
            if line.startswith('latitude'):
                line = 'latitude: '+str(eloc.lat.to('deg').value)+' ; WGS84 +N  (degrees ) \n'
            if line.startswith('longitude'):
                line = 'longitude: '+str(eloc.lon.to('deg').value)+' ; WGS84 +E  (degrees ) \n'
            if line.startswith('elevation'):
                line = 'elevation: '+str(eloc.height.to('m').value)+'  ; mean sea level EGM96 geoidal datum, not WGS84 ellipsoidal (meters) \n'
            f.write(line)
    print('Configuration file saved in '+conf_test)
    # copy new config file into .config
    shutil.copyfile(conf_test, conf)

    
os.chdir(cwd)
