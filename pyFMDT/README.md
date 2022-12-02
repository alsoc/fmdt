# pyFMDT

Suite of tools to exploit the [FMDT software](https://github.com/alsoc/fmdt) output

## What is pyFMDT?

pyFMDT was created to exploit the results provided by the [Fast Meteor Detection Tool](https://github.com/alsoc/fmdt).
It performs astrophotometry computations and outputs the position and magnitude of each detected meteor as a function of time.

## List of tools

- fmdt_read.py: read FMDT output files
- fmdt-reduce.py: reduce meteor detection: performs astrophotometry and outputs position and magnitude as a function of time.
- tah22-putposition.py : tuned for the 2022 tau-Herculids mission: put airplane GPS position in RMS configuration files.

## Dependencies

pyFMDT makes extensive use of [astropy](https://www.astropy.org/) tools, including [astroquery](https://astroquery.readthedocs.io/en/latest/).
In particular it queries [astrometry.net](https://nova.astrometry.net/) server as well as [Vizier services](https://astroquery.readthedocs.io/en/latest/vizier/vizier.html). The photometry fit is performed using the [sklearn.stats package](https://scikit-learn.org/stable/modules/generated/sklearn.linear_model.RANSACRegressor.html)

## How to use pyFMDT.
The later documentation assumes you have successfully processed your video thanks to [FMDT](https://github.com/alsoc/fmdt).
pyFMDT sues a configuration file to provide all desired options.
pyFMDT is launched on a Terminal using the syntax:
	python3 fmdt-reduce.py -c config_file
with:
- config_file: the pyFMDT [configuration file](#header-configuration-file).

## Configuration file.

The configuration file allows the user to provide all necessary information and options. Here is an example:

	[USER]
	# data directory: this is where the FMDT data are located
	data_dir = /Users/vaubaill/Desktop/W7_COLOUR_TEST/
	# log level: choice is: DEBUG, INFO, WARNING, ERROR.
	log_level = INFO
	# astrometry.net API key: your API key. see: http://astrometry.net/doc/net/api.html
	api_key = XXXXXXXX
	# output file root name: used to identify all pyFMDT files. Default is ''.
	root = Gray_

	[CAMERA]
	# sensor width and height in [pixels]
	image_width = 3840
	image_height = 2160
	# FOV size in [deg]
	fov_width = 27.0
	# estimtated limiting magnitude
	LM = 6.0
	# camera frame per second, in [Hz]
	fps = 25.0
	# approximate center of field of view: right Ascension in HH:MM:SS and Declination in DEG
	RA = 11:56:01
	DEC = -27:44:49
	# video time start
	time_start = 2022-05-31T05:00:00.000
	# Photometric band. choice is 'V' (visible), 'R' (red), 'G' (green), 'B' (Blue)
	band = V

	[PROCESS]
	# Flux average over n frames (default is 0). This is used to make photometry calibration more robust.
	avgflx = 5
	# star match tolerance factor, in [pixel]. Star match is performed at the pixel resolution level, unless this factor is greater than 1. Default is 1.0.
	match_tol_fct = 1.5
	# option: read existing astrometry data: useful for debug purpose only. Default is False.
	read_astrometry = True
	
	[FMDT]
	# Object tracking file name
	track_file = ${USER:data_dir}/out_detect_tracks_w7_CT.txt
	# box boundaries file name
	bb_file = ${USER:data_dir}/out_detect_bb_w7_CT.txt
	# magnitude file name
	mag_file = ${USER:data_dir}/mag_w7_CT.txt

## What does pyFMDT do, and how does it do it?

pyFMDT first reads FMDT output files, thanks to the fmdt_read package.
It searches for 'meteor' labeled objects in the FMDT track file.
For each meteor, it loops over each frame and performs an astrophotometry reduction.
For this, the location of each star detected in the frame is extracted from the FMDT bb-type file.
Similarly, the flux (in ADU) of each star in the frame is extracted from the mag-type file provided by FMDT.
The astrometry calibration is performed using [Astrometry.net server](https://nova.astrometry.net).
The output is an [astropy WCS object](https://docs.astropy.org/en/stable/wcs/index.html), used to convert the meteor and star image location (X,Y) into sky coordinates (RA,DEC).
Then the Vizier service is queried to retrieve all known stars in the field of view.
The currently used star catalog is [Calibrated griz magnitudes of Tycho stars (Ofek, 2008 ; aka J/PASP/120/1128)](https://vizier.cds.unistra.fr/viz-bin/VizieR-3?-source=J/PASP/120/1128), with a limiting magnitude of 12 in the V-band.
The output is a catalog (list) of known stars, including their visual magnitude Vmag.
The later is matched to the stars detected in the image. 
The star match is performed at the pixel level by default. However, it may be extended to the value set by the match_tol_fct keyword of the configuration file (see the PROCESS section).
A fit between the flux (in [ADU]) and V-magnitude of known stars is performed, using the [Ransac algorithm (from the sklearn.stats package)](https://scikit-learn.org/stable/modules/generated/sklearn.linear_model.RANSACRegressor.html).
This fit is used to compute the magnitude of the meteor in the chosen band, for each frame.
The operation is repeated for each frame, and for each meteor.

## pyFMDT output files

pyFMDT produces several files, located in the output directory specified in the configuration file (in USER section): where FMDT files are located.
The root name of all files are also specified by the root keyword of the configuration file.

Typical files are:
- root_header_frame.txt : astrometry.net generated Fits header text file. $root is specified in the configuration file, and $frame is the video frame number.
- root_star_catalog_frame.dat : star catalog for frame $frame, text file.
- root_star_matched_frame.dat: matched star catalog for frame $frame, text file.
- root_photfit_frame.png : photometry fit: ADU vs magnitude.
- root_meteor_frame.dat : meteor data: Time, frame, X, Y, RA, DEC, Flux, Mag.
- root_meteor_frame_LC.png : meteor light curve graph: Mag vs frame number.

## How to use pyFMDT output files?

root_meteor_frame.dat may be used for 3D-trajectory and orbit calculation provided the same meteor was observed with another camera from another location.
