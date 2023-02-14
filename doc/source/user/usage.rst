.. _user_executables_usage:

*****************
Executables Usage
*****************

This project generates different executables:

  - ``fmdt-detect`` (and ``fmdt-detect-rt*`` if ``-DFMDT_AFF3CT_RUNTIME`` is set
    to ``ON``): meteors detection chain,
  - ``fmdt-visu``: visualization of the detected meteors,
  - ``fmdt-check``: validation of the detected meteors with the field truth,
  - ``fmdt-maxred``: max reduction of grayscale pixels on a video.

The next sections describe *how to use* the generated executables.

.. toctree::
   :maxdepth: 1
   :caption: Contents

   usage/detect
   usage/visu
   usage/check
   usage/maxred
