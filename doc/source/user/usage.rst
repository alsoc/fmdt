.. _user_executables_usage:

*****************
Executables Usage
*****************

This project generates the following **command line** executable binaries:

  - ``fmdt-detect`` (and its variants),
  - ``fmdt-ellipse``,
  - ``fmdt-log-parser``,
  - ``fmdt-visu``,
  - ``fmdt-check``,
  - ``fmdt-reduce``.

**fmdt-detect**

``fmdt-detect`` is an efficient C/C++ code for meteors detection for ground,
plane or balloon observations. It produces text and video outputs. The main
results are the detected tracks that can be read from the standard output.

If the CMake ``-DFMDT_SPU_RUNTIME=ON`` option is used to compile the project,
then additional detection binaries are produced:

- ``fmdt-detect-rt*-seq``: this version comes with new performance measurement
  tools. However, this is a sequential version and the efficiency should be
  similar to the standard ``fmdt-detect*`` executable,
- ``fmdt-detect-rt*-pip``: this version is multi-threaded. Thus, the throughput
  in term of |FPS| is higher than the standard ``fmdt-detect*`` executable.

Both ``fmdt-detect-rt-seq`` and ``fmdt-detect-rt-pip`` have the same level of
features than the standard ``fmdt-detect`` executable. The ``*-rt-*`` binaries
are based on the `StreamPU runtime <https://github.com/aff3ct/streampu>`_
:cite:`Cassagne2023`.

``fmdt-detect*-no-fail`` is a variant of ``fmdt-detect`` that is more robust to
errors. Indeed, when the number of CCs on a frame is higher than expected,
``fmdt-detect`` will stop because it lacks allocated memory (can be tuned with
:ref:`detect_cca-roi-max1` and :ref:`detect_cca-roi-max2` parameters). Instead,
``fmdt-detect*-no-fail`` will detect that the current frame cannot be processed
(too much CCs) and it will skip the labeling. Still, the tracking will be
processed so the objects can be extrapolated for later frames.

``fmdt-detect*-opt`` is a variant that is feature equivalent to ``fmdt-detect``
or ``fmdt-detect*-no-fail`` (depending on the executable binary name). ``opt``
suffix means "optimized" version: these versions are the fastest in term of
latency and throughput. They are well suited for heavy computations like
real-time or big data processing.

**fmdt-ellipse**

``fmdt-ellipse`` is an executable designed to detect meteors (like
``fmdt-detect``). Its design is based on a max-reduction plus a classification
of the meteors with ellipsoid features. At this time this tool is not fully
documented, it is still at the research level.

**fmdt-log-parser**

``fmdt-log-parser`` is a Python script used to convert ``fmdt-detect`` log
output into text files used by ``fmdt-visu`` and ``fmdt-check``.

**fmdt-visu**

``fmdt-visu`` mainly uses the ``fmdt-detect`` text outputs (after conversion
with ``fmdt-log-parser``) to generate highlighted video sequences. It can be
combined with ground truth to distinguish good detected tracks (*true positive*)
and bad detected tracks (*false positive*).

**fmdt-check**

``fmdt-check`` compares detected tracks (``fmdt-detect``) with a given ground
truth. The results are shown on the standard output.

**fmdt-reduce**

``fmdt-reduce`` performs a reduction from a video sequence into an image.
The produced image is in grayscale mode.

The next sections describe the command line parameters of these tools.

.. toctree::
   :maxdepth: 1
   :caption: Contents

   usage/detect
   usage/log
   usage/visu
   usage/check
   usage/reduce
