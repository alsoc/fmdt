#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Mon Nov 21 14:17:42 2022

@author: vaubaill
"""
# modify RMS .config file and put correct plane position and heading.

import os
import glob
import logging
import argparse
import numpy as np
import matplotlib.pyplot as plt
import astropy.units as u
from astropy.io import fits
from astropy.wcs import WCS
from astropy.time import Time,TimeDelta
from astropy.table import QTable
from astropy.coordinates import SkyCoord,match_coordinates_sky
from astroquery.astrometry_net import AstrometryNet
from astroquery.vizier import Vizier
from sklearn.linear_model import RANSACRegressor
from configparser import ConfigParser, ExtendedInterpolation
from astropy.stats import sigma_clipped_stats
from photutils.detection import DAOStarFinder
from PIL import Image 

import fmdt_read
from fmdt_log import log
from video_startstop2frame import make_img_name
from imgseq_track2frame import make_seqimg_name

# global variable init
config = None

def read_config(config_file):
    """

    Parameters
    ----------
    config_file : String
        pyFMDT configuration file.

    Returns
    -------
    config : configparser.ConfigParser object.
        FMDT configuration.
    img_sz : list of 2 astropy.units.Quantity objects
        image size: [width,height] in [pix].
    fov_sz : list of 2 astropy.units.Quantity objects
        Size of field of view: [width,height] in [deg].
    img_scale : astropy.units.Quantity objects
        Image scale in [deg/pix].
    fps : astropy.units.Quantity objects
        Camera frame per second, in [1/s]
    time_start : astropy.time.Time object.
        Video sequence start time.
    skycoo_c : astropy.coordinates.SkyCoord object.
        Sky coordinates of center of field of view.
    LimMag : astropy.units.Quantity objects
        Camera estimated limiting magnitude.
    band : string, optional.
        Band of data acquisition. Choice is: 'V', 'B', 'R'.
        Default is 'V'.
    avgflx : int
        Number of frames to average to compute the star image flux.
        Deafult is 0, i.e. no averaging.
    match_tol_fct : float
        Star matching tolerance factor, in [pixel]. Star matching is performed
        at a maximum distance of match_tol_fct pixels. Default is 1.0.
    read_astrometry : boolean
        If True, astrometry results are red instead of computed. Useful for
        debug purpose to speed up the whole process. False is not absent from
        the configuration file.
    read_photometry : boolean
        fI True, photometry results are red instead of computed. Useful for
        debug purpose to speed up the whole process. False is not absent from
        the configuration file.
    opth : string
        Output directory.
    fpth : string
        Meteor frame directory.
    fmt : string
        Meteor frame format.
    root : string
        Output root file name.
    
    """
    # read configuration file
    config = ConfigParser(interpolation=ExtendedInterpolation())
    config.read(config_file)
    # change log level
    logdict = {'DEBUG' : logging.DEBUG,
               'INFO' : logging.INFO,
               'WARNING' : logging.WARNING,
               'ERROR' : logging.ERROR}
    log.setLevel(logdict[config['USER']['log_level']])
    log.propagate = True
    opth = config['USER']['out_dir']+'/'
    root = config['USER']['root']
    fpth = config['USER']['frame_dir']
    if not fpth.endswith('/'):
        fpth=fpth+'/'
    fmt = config['USER']['frame_fmt']
    
    # read CAMERA section
    img_width = int(config['CAMERA']['image_width'])*u.pix
    img_height = int(config['CAMERA']['image_height'])*u.pix
    fov_width = float(config['CAMERA']['fov_width'])*u.deg
    fov_height = float(config['CAMERA']['fov_height'])*u.deg
    # estimate scale
    img_scale = fov_width / img_width
    fps = float(config['CAMERA']['fps']) / u.s
    time_start = Time(config['CAMERA']['time_start'],scale='utc')
    # center of FOV: Right Ascension and Declination
    skycoo_c = SkyCoord(ra=config['CAMERA']['RA'],
                        dec=config['CAMERA']['DEC'],
                        unit=(u.hourangle, u.deg),frame='icrs')
    img_sz = [img_width,img_height]
    fov_sz = [fov_width, fov_height]
    LimMag = float(config['CAMERA']['LM']) * u.mag
    band = config['CAMERA']['band']
    
    # create output directory if needed
    if not os.path.exists(opth):
        log.info('Make output directory '+opth)
        os.mkdir(opth)
    
    # read PROCESS section
    try:
        avgflx = int(config['PROCESS']['avgflx'])
    except KeyError:
        avgflx = 0
    # set read_res parameter: useful to read results if the simulation has already been performed.
    try:
        read_astrometry = config['PROCESS']['read_astrometry'].lower() in ['true', '1', 't', 'y', 'yes']
    except:
        read_astrometry = False
    try:
        read_photometry = config['PROCESS']['read_photometry'].lower() in ['true', '1', 't', 'y', 'yes']
    except:
        read_photometry = False
    match_tol_fct = float(config['PROCESS']['match_tol_fct'])
    return (config,img_sz,fov_sz,img_scale,fps,time_start,skycoo_c,LimMag,band,\
            avgflx,match_tol_fct,read_astrometry,read_photometry,opth,fpth,fmt,root)


def frame2filename(pth_frame,frame,fmt='%5d.png',exclude='_visu'):
    """Retrieve frame image file from frame number.

    Parameters
    ----------
    pth_frame : string
        Frame directory name.
    frame : int
        Frame number.
    fmt : string, optional
        File extenstion. Default is '%5d.png'.
    exclude : string, optional
        File name excluding pattern. Default is '_visu'.
    
    Returns
    -------
    frame_file : string
        Frame file full path name.
    
    """
    # get list of images
    # extract root, extension and number of digits in name extension
    [f_fmt,f_ext] = fmt.split('.')
    dig = int(f_fmt.replace('%','').replace('d',''))
    frmstr = '_'+str(frame).zfill(dig)
    pattern = pth_frame+'*'+frmstr+'.'+f_ext
    log.debug('Get list of images of pattern: '+pattern+' excluding: '+exclude)
    list_img = glob.glob(pattern)
    list_img = [i for i in list_img if exclude not in i]
    # chec for multiple file
    if len(list_img)>1:
        msg = '*** FATAL ERROR: more than one file matches '+pattern+' : '+str(list_img)
        log.error(msg)
        raise IOError(msg)
    frame_file = list_img[0]
    log.debug('frame file: '+frame_file)
    return frame_file


def launch_astrometry(astrometry_data,img_width,img_height,img_scale,radec_center,\
                      api_key,frame,opth='./',root='',uf=1.1,lf=0.9,\
                      read=False,config=None,send_frame=False):
    """Launch astrometry reduction using astrometry.net service.

    Parameters
    ----------
    astrometry_data : astropy.table.QTable object.
        Source position and flux. Colum names must be: ['X','Y','Flux'].
    img_width : int
        Abscissa image number of pixels.
    img_height : int
        Ordinates image number of pixels.
    img_scale : astropy.units.Quantity object.
        Image scale, in 'deg/pix'.
    radec_center : astropy.coordinates.SkyCoord object.
        Image center sky coordinates.
    api_key : string
        Astrometry.net API key.
    frame : int
        Meteor frame number. 
    opth : string, optional
        Output directory name. Default is './'.
    root : string, optional
        Output file root name. Default is ''.    
    uf : float, optional
        Scale upper limit factor. Default is 1.1.
    lf: float, optional
        Scale lower limit factor. Default is 0.9.
    read : boolean, optional.
        If True, data are red from file instead of computed. Useful
        for debug purpose only and/or to speed up the process. Default is False.
    config : configparser.ConfigParser object, optional
        Program configuration used to retrieve image file name from frame number.
        Default is None.
    send_frame : boolean, optional
        If True, the whole frame is sent to astrometry.net server to solve for
        astrometry. Default is False.
    
    Returns
    -------
    wcs_obj : astropy.wcs.WCS object.
        World Coordinate System object.
        It contains all the astrometric transformation info.
    radec_center_c : astropy.coordinates.SkyCoord object.
        Updated image center sky coordinates.
    fov_sz : list of 2 astropy.units.Quantity objects.
        Size of field of view: width and height, in [deg].
    """
    # make header file name
    hdrfile = opth+root+'header_'+str(frame)+'.txt'
    if read:
        wcs_header = fits.Header.fromfile(hdrfile,sep='\n',endcard=False,padding=False)
    else: 
        ast = AstrometryNet()
        ast.api_key = api_key
        wcs_header = None
        # send the whole frame
        try:
            send_frame = config['PROCESS']['send_frame'].lower() in ['true', '1', 't', 'y', 'yes']
        except:
            send_frame = False
        if send_frame:
            frame_file = frame2filename(config['USER']['frame_dir'],frame)
            log.info('Sending frame '+frame_file)
            wcs_header = ast.solve_from_image(frame_file,force_image_upload=True)
        else:
            wcs_header = ast.solve_from_source_list(astrometry_data['X'],
                                                astrometry_data['Y'],
                                                img_width.value,
                                                img_height.value,
                                                solve_timeout=120,
                                                scale_units='degwidth',
                                                scale_type='ul',
                                                scale_upper=img_width.value*img_scale.value*uf,
                                                scale_lower=img_width.value*img_scale.value*lf,
                                                # center_ra=radec_center.ra.to('deg').value,
                                                # center_dec=radec_center.dec.to('deg').value,
                                                # radius=10.0,
                                                # positional_error=10,
                                                parity=2)
        log.info('*** Astrometry.net went through.')
        log.debug('header: '+str(wcs_header))
        try:
            radec_center_c = SkyCoord(ra=wcs_header['CRVAL1']*u.deg,
                                      dec=wcs_header['CRVAL2']*u.deg, frame="icrs")
            log.info('Astromety success!!! center: '+radec_center.to_string())
        # if there is not enough stars astrometry.net failed at solving the astrometry
        except KeyError:
            # send frame to astrometry.net
            log.warning('Astrometry calibration failed!')
            frame_file = frame2filename(config['USER']['frame_dir'],frame)
            log.warning('Trying again by sending frame '+frame_file)
            wcs_header = ast.solve_from_image(frame_file,force_image_upload=True)
            send_frame = True
            try:
                radec_center_c = SkyCoord(ra=wcs_header['CRVAL1']*u.deg,
                                          dec=wcs_header['CRVAL2']*u.deg, frame="icrs")
                log.info('Astromety success!!! center: '+radec_center.to_string())
            except:
                raise ValueError('Astrometry.net failed even by uploading the image.')
        except:
            msg = 'Astrometry.net failed.'
            log.error(msg)
            log.error(str(wcs_header))
            raise ValueError(msg)
        # save header
        wcs_header.totextfile(hdrfile, endcard=True, overwrite=True)
        log.info('Frame header file saved in '+hdrfile)
    # update image center sky coordinates
    radec_center_c = SkyCoord(ra=wcs_header['CRVAL1']*u.deg,
                              dec=wcs_header['CRVAL2']*u.deg, frame="icrs")    
    log.debug('Update center of fov:'+radec_center.to_string())
    
    # update fov
    fov_width = np.abs(float(wcs_header['CD1_1'])) * u.deg/u.pix * img_width
    fov_height = np.abs(float(wcs_header['CD2_2'])) * u.deg/u.pix * img_height
    fov_sz = [fov_width,fov_height]
    log.debug('Updated fov: '+str(fov_width)+' '+str(fov_height))    
    
    # create wcs object
    wcs_obj = WCS(wcs_header)
    log.debug(wcs_obj)
    return (wcs_obj,radec_center_c,fov_sz,send_frame)

def make_astrometry_data_from_bb(trk_data,bb_frame,mag_data,frame,opth='./',root='',avgflx=0,read=False):
    """Make astrometry data Table.

    Parameters
    ----------
    trk_data : astropy.table.QTable object.
        FMDT tracking output data, restricted to the considered meteor.
    bb_frame : astropy.table.QTable object.
        FMDT bb output data, restricted to one frame.
    mag_data : astropy.table.QTable object.
        FMDT magnitude output data.
    frame : int
        Meteor frame number. 
    opth : string, optional
        Output directory name. Default is './'.
    root : string, optional
        Output file root name. Default is ''.    
    avgflx : int, optional
        Average the star flux over plus and minus avgflx frames.
        Default is 0 (=no average).
    read : boolean, optional.
        If True, astrometry data are red from file instead of computed. Useful
        for debug purpose only and/or to speed up the process. Default is False.
    
    Returns
    -------
    astrometry_data : astropy.table.QTable object.
        Astrometry data: X, Y, Flux.

    """
    # make output file name
    outfile = opth+root+'_star_astrometry_'+str(frame)+'.dat'
    log.debug(str(read))
    if read:
        log.info('Reading astrometry data from '+outfile)
        astrometry_data = QTable.read(outfile,format='ascii.fixed_width_two_line')
    else:
        astrometry_data = QTable(names=['X','Y','Flux'])
        # loop over the stars
        for bb in bb_frame:
            log.debug('*** Considering object # '+str(bb['id']))
            log.debug('bb: '+str(bb))
            trk_obj = trk_data[trk_data['id']==bb['id']]
            obj_type = trk_obj['type']
            log.debug('trk_data for obj '+str(bb['id'])+' : '+str(trk_obj))
            if obj_type=='noise':
                log.info('=== Skip noise')
                continue
            if obj_type=='meteor':
                log.info('=== Skip meteor')
                continue
            # retrieve first and last frame where the star was detected
            frames = np.arange(trk_obj['f_b'],trk_obj['f_e']+1,1)
            frame_loc = np.where(frames==frame)[0][0]
            log.debug('frames: '+str(frames))
            log.debug('frame_loc: '+str(frame_loc))
            # retrieve star flux
            flux = mag_data[bb['id']]['data'][frame_loc]
            if avgflx>1:
                f_b = np.max([1,frame_loc-avgflx])-1 # because python index starts at 0
                f_e = np.min([len(frames),frame_loc+avgflx])
                log.debug('Frame average from: '+str(f_b)+' to '+str(f_e))
                log.debug('flux values: '+str(mag_data[bb['id']]['data'][f_b:f_e]))
                flux = np.median(mag_data[bb['id']]['data'][f_b:f_e])
                log.debug('flux averaged over '+str(f_e-f_b+1)+' frames: '+str(flux))
                log.debug('mag_data[id]: '+str(mag_data[bb['id']]))
            else:
                flux = mag_data[bb['id']]['data'][frame_loc]
                log.debug('flux (no average): '+str(flux))
            log.debug('flux of star # '+str(bb['id'])+' at frame '+str(frames[frame_loc])+' : '+str(flux))
            astrometry_data.add_row([bb['Xc'],bb['Yc'],flux])
        # rearrange data for astrometry.net service
        astrometry_data.sort('Flux')
        astrometry_data.reverse()
        log.debug('astrometry_data '+str(astrometry_data))
        log.info('There are '+str(len(astrometry_data))+' astrometry data')
        astrometry_data.write(outfile,format='ascii.fixed_width_two_line',overwrite=True)
        log.info('astrometry data saved in '+outfile)
    return astrometry_data


def make_astrometry_data_from_frame(pth_frame,met_id,frame,num_min=80,
                                    fmt='%5d.png',opth='./',root=''):
    """Make astrometry data from frame file.

    Parameters
    ----------
    pth_frame : string
        Frame files directory.
    met_id : int
        FMDT object id.
    frame : int
        Frame number.
    num_min : int, optional
        Minimum number of stars to detect in frame to perform astrometry.
        Default is 80.
    fmt : string, optional
        Image format. Default is '%5d.png'.
    opth : string, optional
        Output path. The default is './'.
    root : string, optional
        Output file name root. The default is ''.

    Returns
    -------
    astrometry_data : astropy.table.QTable object.
        Astrometry data: X, Y, Flux.

    """
    # make output file name
    outfile = opth+root+'_star_astrometry_'+str(frame)+'.dat'
    # create astrometry Table object
    astrometry_data = QTable(names=['X','Y','Flux'])
    # retrieve file name
    frame_file = frame2filename(pth_frame,frame,fmt=fmt)
    log.info('Extracting sources from '+frame_file)
    # extract sources with photutils
    data=np.array(Image.open(frame_file))
    mean, median, std = sigma_clipped_stats(data, sigma=3.0)
    #daofind = DAOStarFinder(threshold=5.0*std)
    fct_list = [5.0,4.5,4.0,3.5,3.0]
    ndata = 0
    for fct in fct_list:
        daofind = DAOStarFinder(fwhm=3.0, threshold=fct*std)
        astrometry_data = daofind(data - median)
        ndata = len(astrometry_data)
        if ndata>num_min:
            break
    if ndata<num_min:
        msg = '*** FATAL ERROR: impossible to detect more than '+str(num_min)+' stars in '+frame_file
        log.error(msg)
        raise ValueError(msg)
    # format astrometry_data
    astrometry_data.rename_column('xcentroid','X')
    astrometry_data.rename_column('ycentroid','Y')
    astrometry_data.rename_column('flux','Flux')
    astrometry_data.remove_columns(['id','sharpness','roundness1','roundness2',
                                    'npix','sky','peak','mag'])
    # recompute flux, since. according to DAOStarFinder documentation:
    # flux is "peak density in the convolved image divided by the detection threshold.
    astrometry_data['Flux'] = fct*std * astrometry_data['Flux']
    # rearrange data for astrometry.net service
    astrometry_data.sort('Flux')
    astrometry_data.reverse()
    log.debug('astrometry_data '+str(astrometry_data))
    log.info('There are '+str(len(astrometry_data))+' astrometry data')
    astrometry_data.write(outfile,format='ascii.fixed_width_two_line',overwrite=True)
    log.info('astrometry data saved in '+outfile)
    return astrometry_data


def match_stars(astrometry_data,wcs_obj,radec_center,fov_sz,maxsep,frame=1,LimMag=6*u.mag,opth='./',root='',cat='J/PASP/120/1128/catalog',nmatch_min=10):
    """Match image stars with catalog stars.
    
    Output data are saved in opth directory. File names start with root.

    Parameters
    ----------
    astrometry_data : astropy.table.Qtable object.
        Astrometry data QTable.
    wcs_obj : astropy.wcs.WCS object.
        World coordinate system.
    radec_center : astropy.coordinate.SkyCoord object.
        Sky coordinates of center of field of view.
    fov_sz : list of 2 astropy.units.Quantity objects.
        Field of view: width and height.
    maxsep : astropy.units.Quantity object
        Maximum angular separation for star matching.
    frame : int, optional
        Frame number. Default is 1.
    LimMag : astropy.units.Quantity object, optional.
        Camera limiting magnitude. Default is 6.0*u.mag.
    opth : string, optional
        Output directory name. Default is './'.
    root : string, optional
        Output file root name. Default is ''.  
    cat : string, optional
        Catalog Vizier name. Default is 'II/355/ugriz-2', corresponding to 
        A catalog of Sloan magnitudes for the brightest stars - version 2.
        Catalog limiting magnitude=7, 3969 stars with photometry.
        'J/PASP/120/1128': Calibrated griz magnitudes of Tycho stars (Ofek, 2008)
        Catalog of griz magnitudes for Tycho-2 stars with BT<13mag and VT<12mag (1560980 rows)
    nmatch_min : int, optional
        Mnimal number of matched stars in order to perfor photmetry calibration.
        Default is 10.
    
    See also
    --------
   Vizier catalog query: 
       'II/355/ugriz-2': A catalog of Sloan magnitudes for the brightest stars - version 2.
       https://vizier.cds.unistra.fr/viz-bin/VizieR-3?-source=J/PASP/120/1128
    
    Returns
    -------
    matched_star : astropy.table.Qtable object.
        Catalog star coordinates and V-magnitude.

    """
    [fov_width,fov_height] = fov_sz
    # transform star coorindates into sky coordinates
    radec_star = wcs_obj.pixel_to_world(astrometry_data['X'],
                                        astrometry_data['Y'])
    log.debug('star RADEC: '+str(radec_star))
    log.debug('LimMag: '+str(LimMag))
    # query star catalog
    v = Vizier(columns=['_RAJ2000', '_DEJ2000','+VTmag', 'BTmag','rmag','gmag'],
               column_filters={"VTmag":"<"+str(LimMag.value)},
               row_limit=-1)
    # query Bright star catalog
    log.debug('Vizier query at: '+radec_center.to_string()+' fov: '+str(fov_width)+' '+str(fov_height))
    data_cat = v.query_region(radec_center, 
                              width=fov_width,
                              height=fov_height,
                              frame='fk5',
                              catalog=cat)[0]
    # remove masked data
    if data_cat.mask[0]['rmag']:
        data_cat = data_cat[1:]
    log.debug('data_cat: '+str(data_cat))
    cat_file = opth+root+'star_catalog_'+str(frame)+'.dat'
    data_cat.write(cat_file,format='ascii.fixed_width_two_line',overwrite=True)
    log.info('catalog data saved in '+cat_file)
    radec_cat = SkyCoord(data_cat['_RAJ2000'],data_cat['_DEJ2000'])
    # match star catalog to source catalog.
    idx, d2d, d3d = match_coordinates_sky(radec_star,radec_cat)
    log.debug('idx: '+str(idx))
    # add magnitude data in star data
    astrometry_data.add_column(radec_star.ra,name='RAimg')
    astrometry_data.add_column(radec_star.dec,name='DECimg')
    astrometry_data.add_column(data_cat['_RAJ2000'][idx],name='RAcat')
    astrometry_data.add_column(data_cat['_DEJ2000'][idx],name='DEcat')
    astrometry_data.add_column(data_cat['VTmag'][idx])
    astrometry_data.add_column(data_cat['BTmag'][idx])
    astrometry_data.add_column(data_cat['rmag'][idx])
    astrometry_data.add_column(data_cat['gmag'][idx])
    astrometry_data.sort('VTmag')
    #log.debug('d2d: '+str(d2d))
    log.info('Star matching criterion: '+str(maxsep))
    mask = d2d < maxsep
    matched_star = astrometry_data[mask]
    nmatch = len(matched_star)
    if nmatch<nmatch_min:
        msg = '*** FATAL ERROR: only  '+str(nmatch)+' matched stars, i.e. less than '+str(nmatch_min)
        log.error(msg)
        raise ValueError(msg)
    matched_file = opth+root+'star_matched_'+str(frame)+'.dat'
    matched_star.write(matched_file,format='ascii.fixed_width_two_line',overwrite=True,
                       formats={'RAimg':'9.5f','DECimg':'9.5f'})
    log.info('matched data saved in '+matched_file)
    return matched_star


def make_photometry_fit(matched_data,met_flux,band='V',frame=1,opth='./',root=''):
    """Make photoetry fit and compute meteor magnitude.
    
    Data and plot are saved in opth directory.
    

    Parameters
    ----------
    matched_data : astropy.table.QTable
        Matched star data, including magnitude data.
    met_flux : float
        Meteor flux in [ADU].
    band : string, optional
        Wavelength band. Choice is: 'V': visible, 'B': blue, 'R': red.
        Default is 'V'.
    frame : int, optional
        Frame number. Default is 1.
    opth : string, optional
        Output directory name. Default is './'.
    root : string, optional
        Output file root name. Default is ''.
    
    See also
    --------
    Band definition depends on requested star catalog. In fmdt-reduce we use:
       'II/355/ugriz-2': A catalog of Sloan magnitudes for the brightest stars - version 2.
       https://vizier.cds.unistra.fr/viz-bin/VizieR-3?-source=J/PASP/120/1128


    Returns
    -------
    met_mag : astropy.units.QTable
        Meteor magnitude.

    """
    # build band name vs star catalog column name
    b2c = {'V':'VTmag',
           'G':'gmag',
           'B':'BTmag',
           'R':'rmag'}
    mag_col = b2c[band]
    
    # linear regression with Ransac
    ransac = RANSACRegressor(random_state=42)
    X = np.array(matched_data['Flux'].value).reshape(-1, 1)
    Y = np.array(matched_data[mag_col].value)
    ransac.fit(X,Y)
    inlier_mask = ransac.inlier_mask_
    outlier_mask = np.logical_not(inlier_mask)
    # compute meteor magnitude
    met_mag = ransac.predict(np.array(met_flux).reshape(-1, 1))
    slope = ransac.estimator_.coef_[0]
    intcp = ransac.estimator_.intercept_    
    log.debug('slope: '+str(slope))
    log.debug('intcp: '+str(intcp))
    
    # create fitted data set
    x_fit = np.linspace(0,np.max(matched_data['Flux'].value),num=100)
    y_fit = ransac.predict(np.array(x_fit).reshape(-1, 1))
        
    # plot fit
    fig=plt.figure()
    #plt.plot(matched_data['Flux'].value,matched_data['VTmag'].value,'bo')
    plt.plot(x_fit,y_fit,'r-')
    # Create scatter plot for inlier and outlier datset
    plt.scatter(X[inlier_mask], Y[inlier_mask],
            c='limegreen', #edgecolor='white',
            marker='o', label='Inliers')
    plt.scatter(X[outlier_mask], Y[outlier_mask],
             c='steelblue', #edgecolor='white',
             marker='x', label='Outliers')
    plt.suptitle('Flux vs Mag',color='indianred', fontsize=16)
#    plt.title(str(len(matched_data['Flux']))+' data used. x='+"{:6.2e}".format(slope)+' [+- '+"{:6.2e}".format(slope_err)+'] *X + '+"{:4.2f}".format(intcp)+' [+- '+"{:4.2f}".format(intsct_err)+']')
    plt.title(str(len(matched_data['Flux']))+' data used. Mag='+"{:6.2e}".format(slope)+'*Flux+ '+"{:4.2f}".format(intcp), fontsize=10)
    plt.xlabel('Flux [ADU]')
    plt.ylabel('Mag')
    plt.legend(loc='upper right')
    outfile = opth+root+'photfit_'+str(frame)+'.png'
    plt.savefig(outfile,dpi=300)
    plt.close(fig)
    log.info('plot saved in '+outfile)
    log.info('met mag: '+str(met_mag))
    return met_mag


def save_n_plot_meteor(met_data,met_id,opth='./',root='',LimMag=6.0*u.mag):
    """Save and plot meteor data.

    Parameters
    ----------
    met_data : astropy.table.QTable object.
        Meteor data: time, frame, X, Y, RA, DEC, Flux, Mag.
    met_id : int
        FMDT track meteor id.
    opth : string, optional
        Output directory name. Default is './'.
    root : string, optional
        Output file root name. Default is ''.
    LimMag : astropy.units.Quantity object.
        Camera limiting magnitude. Default is 6.0*u.mag.
    
    Returns
    -------
    None.

    """
    met_file = opth+'/'+root+'meteor_'+str(met_id)+'.dat'
    met_data.write(met_file,format='ascii.fixed_width_two_line',
                   formats={'Time':'%23s','RA':'8.4f','DEC':'8.4f','Mag':'6.2f'},
                   overwrite=True)
    log.info('Meteor data saved in '+met_file)
    # plot meteor light curve
    # time_rel = met_data['Time'][0]-met_data['Time']
    # time_rel.info.unit = u.day
    # log.debug('time_rel: '+str(time_rel))
    # time_rel = time_rel.to('s')
    fig=plt.figure()
    #plt.plot(time_rel.to('s').value,met_data['Mag'].value,'bo')
    plt.plot(met_data['frame'],met_data['Mag'].value,'bo')
    plt.title(str(met_id)+' Meteor light curve',color='indianred')
    plt.xlabel('Frame #')
    plt.ylabel('Mag')
    plt.ylim(bottom=None, top=LimMag.value),
    plt.legend(loc='lower right')
    outfile = opth+root+'meteor_'+str(met_id)+'_LC.png'
    plt.savefig(outfile,dpi=300)
    plt.close(fig)
    log.info('plot saved in '+outfile)
    return

def fmdt_reduce(config_file):
    """Launch FMDT reduce process.

    Parameters
    ----------
    config_file : string
        configuration file.

    Returns
    -------
    None.

    """
    # read configuration file
    (config,
     [img_width,img_height],
     [fov_width,fov_height],
     img_scale,
     fps,time_start,
     skycoo_c,
     LimMag,band,
     avgflx,
     match_tol_fct,
     read_astrometry,
     read_photometry,
     opth,
     fpth,
     fmt,
     root) = read_config(config_file)
    # read FMDT output files
    trk_data,bb_data,mag_data = fmdt_read.read_fmdt_outputs(config['FMDT']['track_file'],
                                                            config['FMDT']['bb_file'],
                                                            config['FMDT']['mag_file'])
    # select meteor track data
    met_trk = trk_data[trk_data['type']=='meteor']
    log.info('There are '+str(len(met_trk))+' meteors detected in '+config['FMDT']['track_file'])
    # set send_frame
    send_frame = False
    # for each meteor: perform astrometry
    for met in met_trk:
        met_id = met['id']
        log.info('*** Processing meteor # '+str(met_id))
        # make meteor output QTable
        met_data = QTable(names=['Time','frame','X','Y','RA','DEC','Flux','Mag'],
                          dtype=['object','int','float','float','float','float','float','float'])
        # loop over each meteor frame
        for frame in np.arange(met['f_b'],met['f_e']+1,1):
            log.debug('Considering frame #: '+str(frame))
            # compute time of frame
            frame_time = time_start + TimeDelta(frame/fps)
            # select object in current frame
            bb_obj = bb_data[bb_data['frame']==frame]
            log.debug('There are '+str(len(bb_obj))+' objects in frame '+str(frame))
            # retrieve meteor bb data
            bb_met = bb_obj[bb_obj['id']==met_id]
            # retrieve flux for all stars in the image
            astrometry_data = make_astrometry_data_from_bb(trk_data,
                                                           bb_obj,
                                                           mag_data,
                                                           frame,
                                                           avgflx=avgflx,
                                                           opth=opth,
                                                           root=root,
                                                           read=read_astrometry)
            # check if number of start is high enough for astrometry
            star_num = len(astrometry_data)
            num_min = int(int(config['PROCESS']['min_star_nb']))
            if (star_num<num_min):
                log.warning('Number of detected stars '+str(star_num)+' lower than required '+str(num_min))
                astrometry_data = make_astrometry_data_from_frame(fpth,
                                                                  met_id,
                                                                  frame,
                                                                  num_min=num_min,
                                                                  fmt=fmt,
                                                                  opth=opth,
                                                                  root=root)
            
            # perform astrometry with astrometry.net
            (wcs_obj,
             radec_center,
             [fov_width,fov_height],
             send_frame) = launch_astrometry(astrometry_data, 
                                            img_width, 
                                            img_height, 
                                            img_scale, 
                                            skycoo_c, 
                                            config['USER']['api_key'],
                                            frame=frame,
                                            opth=opth,root=root,
                                            read=read_astrometry,
                                            config=config,
                                            send_frame=send_frame)
            # compute meteor sky coordinates
            radec_met = wcs_obj.pixel_to_world(bb_met['Xc'][0],
                                               bb_met['Yc'][0])
            # match detected stars with catalog stars
            matched_data = match_stars(astrometry_data,
                                       wcs_obj,
                                       radec_center,
                                       [fov_width,fov_height],
                                       maxsep=img_scale*u.pix*match_tol_fct,
                                       LimMag=LimMag,
                                       frame=frame,
                                       opth=opth,
                                       root=root)
            # linear fit star magnitude with star flux and get meteor magnitude
            met_flux = mag_data[met_id]['data'][frame-met['f_b']]
            met_mag = make_photometry_fit(matched_data,
                                          met_flux,
                                          band=band,
                                          frame=frame,
                                          opth=opth,
                                          root=root)
            # add data to meteor table
            met_data.add_row([frame_time,
                              frame,
                              bb_met['Xc'][0],
                              bb_met['Yc'][0],
                              radec_met.ra.to('deg'),
                              radec_met.dec.to('deg'),
                              met_flux,met_mag])
            # log
            log.debug('time: '+frame_time.isot+' frame: '+str(frame))
            log.debug('bb_met[Xc]:'+str(bb_met['Xc'][0])+' bb_met[Yc]:'+str(bb_met['Yc'][0]))
            log.debug('radec_met:'+str(radec_met))
            log.debug('met_flux:'+str(met_flux)+' met_mag:'+str(met_mag))
        # save and plot meteor data
        save_n_plot_meteor(met_data,met_id,LimMag=LimMag,opth=opth,root=root)
        
    log.info('*** All done.',)
    return




if __name__ == '__main__':
    
    # parse arguments
    parser = argparse.ArgumentParser(description='fmdt-reduce arguments.')
    parser.add_argument('-c',default='./conf/config.in',help='configuration file. Default is: ./conf/config.in')
    args = parser.parse_args()
    
    # set configuration file
    config_file = args.c
    
    # launch 
    fmdt_reduce(config_file)
    
else:
    log.debug('successfully imported')

