.. _user_executables_usage:

*****************
Executables Usage
*****************

This project generates the following **command line** executables:

  - ``fmdt-detect``,
  - ``fmdt-visu``,
  - ``fmdt-check``,
  - ``fmdt-maxred``,
  - ``fmdt-ellipse``.

``fmdt-detect`` is an optimized and efficient C/C++ code for meteors detection.
It produces only text outputs. The main results are the detected tracks and they
can be read on the standard output (in the terminal). If the CMake
``-DFMDT_AFF3CT_RUNTIME=ON`` option is used to compile the project, then
additional detection binaries are produced:

- ``fmdt-detect-rt-seq``: this version comes with new performance measurement
  tools. However, this is a sequential version and the efficiency should be
  similar with the standard ``fmdt-detect`` executable,
- ``fmdt-detect-rt-pip``: this version is multi-threaded. Thus, the throughput
  in term of |FPS| is much higher than the standard ``fmdt-detect`` executable
  (depending on the CPU target).

Both ``fmdt-detect-rt-seq`` and ``fmdt-detect-rt-pip`` have the same level of
features than the standard ``fmdt-detect`` executable.

``fmdt-visu`` mainly uses the ``fmdt-detect`` text outputs to generate
highlighted video sequences. It can be combined with ground truth to
distinguish good detected tracks (*true positive*) and bad detected tracks
(*false positive*).

``fmdt-check`` compares detected tracks (``fmdt-detect``) with a given ground
truth. The results are shown on the standard output.

``fmdt-maxred`` performs a max-reduction from a video sequence into an image.
The produced image is in grayscale mode.

``fmdt-ellipse`` is a new executable designed to detect meteors (like
``fmdt-detect``). Its design is based on a max-reduction + a classification
of the meteors with ellipsoid features. At this time this tool is not fully
documented, it is still at the research level.

The next sections describe the command line parameters of these tools.

.. toctree::
   :maxdepth: 1
   :caption: Contents

   usage/detect
   usage/visu
   usage/check
   usage/maxred
