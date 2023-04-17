#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri Feb  3 09:35:01 2023

@author: vaubaill
"""
import logging


# logging
log = logging.getLogger(__name__)
log.setLevel(logging.DEBUG)
log_fmt = '%(levelname)s %(filename)s %(lineno)d (%(funcName)s) : %(message)s '
sdlr = logging.StreamHandler()
sdlr.setFormatter(logging.Formatter(fmt=log_fmt))
log.addHandler(sdlr)
