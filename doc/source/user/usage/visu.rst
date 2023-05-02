.. _user_executables_usage_visu:

Visualization Parameters
""""""""""""""""""""""""

The meteors visualization program is located here: ``./bin/fmdt-visu``.

The following table summarizes the available parameters:

+----------------------+---------+----------------------------------------------------+
| Argument             | Type    | Details                                            |
+======================+=========+====================================================+
| ``--vid-in-path``    | STRING  | See :numref:`visu_vid-in-path`.                    |
+----------------------+---------+----------------------------------------------------+
| ``--vid-in-start``   | INTEGER | See :numref:`visu_vid-in-start`.                   |
+----------------------+---------+----------------------------------------------------+
| ``--vid-in-stop``    | INTEGER | See :numref:`visu_vid-in-stop`.                    |
+----------------------+---------+----------------------------------------------------+
| ``--vid-in-threads`` | INTEGER | See :numref:`visu_vid-in-threads`.                 |
+----------------------+---------+----------------------------------------------------+
| ``--trk-path``       | STRING  | See :numref:`visu_trk-path`.                       |
+----------------------+---------+----------------------------------------------------+
| ``--trk-bb-path``    | STRING  | See :numref:`visu_trk-bb-path`.                    |
+----------------------+---------+----------------------------------------------------+
| ``--trk-id``         | BOOLEAN | See :numref:`visu_trk-id`.                         |
+----------------------+---------+----------------------------------------------------+
| ``--trk-nat-num``    | BOOLEAN | See :numref:`visu_trk-nat-num`.                    |
+----------------------+---------+----------------------------------------------------+
| ``--trk-only-meteor``| BOOLEAN | See :numref:`visu_trk-only-meteor`.                |
+----------------------+---------+----------------------------------------------------+
| ``--gt-path``        | STRING  | See :numref:`visu_gt-path`.                        |
+----------------------+---------+----------------------------------------------------+
| ``--vid-out-path``   | STRING  | See :numref:`visu_vid-out-path`.                   |
+----------------------+---------+----------------------------------------------------+

.. _visu_vid-in-path:

``--vid-in-path``
-----------------

   :Deprecated: ``--in-video``
   :Type: STRING
   :Default: [empty]
   :Example: ``--vid-in-path ~/Videos/meteors.mp4``

Input video path (supports also a path to a sequence of images
``path/basename_%05d.jpg``).

.. _visu_vid-in-start:

``--vid-in-start``
------------------

   :Deprecated: ``--fra-start``
   :Type: INTEGER
   :Default: ``0``
   :Example: ``--vid-in-start 12``

First frame id (included) to start the detection in the video sequence.

.. _visu_vid-in-stop:

``--vid-in-stop``
-----------------

   :Deprecated: ``--fra-end``
   :Type: INTEGER
   :Default: ``0``
   :Example: ``--vid-in-stop 42``

Last frame id (included) to stop the detection in the video sequence. If set to
``0``, read entire video.

.. _visu_vid-in-threads:

``--vid-in-threads``
--------------------

   :Deprecated: ``--ffmpeg-threads``
   :Type: INTEGER
   :Default: ``0``
   :Example: ``--vid-in-threads 1``

Select the number of threads to use to decode video input (in ``ffmpeg``). If
set to ``0``, ``ffmpeg`` chooses the number of threads automatically.

.. _visu_trk-path:

``--trk-path``
--------------

   :Deprecated: ``--in-tracks``
   :Type: STRING
   :Default: [empty]
   :Example: ``--trk-path tracks.txt``

The tracks file corresponding to the input video (generated from
``fmdt-detect``). See :numref:`detect_stdout` for the description of the
expected text input format.

.. _visu_trk-bb-path:

``--trk-bb-path``
-----------------

   :Deprecated: ``--in-bb``
   :Type: STRING
   :Default: [empty]
   :Example: ``--trk-bb-path bb.txt``

The bounding boxes file corresponding to the input video (generated from
``fmdt-detect``). See :numref:`log_trk-bb-path` for the description of the
expected text output format.

.. _visu_trk-id:

``--trk-id``
------------

   :Deprecated: ``--show-id``
   :Type: BOOLEAN
   :Example: ``--trk-id``

Show the object ids on the output video and frames. Requires to link with OpenCV
library (``-DFMDT_OPENCV_LINK`` CMake option, see
:numref:`user_installation_cmake`).

.. _visu_trk-nat-num:

``--trk-nat-num``
-----------------

   :Deprecated: ``--show-id``
   :Type: BOOLEAN
   :Example: ``--trk-nat-num``

Natural numbering of the object ids, work only if ``--trk-id`` is set.

.. _visu_trk-only-meteor:

``--trk-only-meteor``
---------------------

   :Deprecated: ``--only-meteor``
   :Type: BOOLEAN
   :Example: ``--trk-only-meteor``

Show only meteors.

.. _visu_gt-path:

``--gt-path``
-------------

   :Deprecated: ``--in-gt``
   :Type: STRING
   :Default: [empty]
   :Example: ``--gt-path gt.txt``

File containing the ground truth.
Ground truth file gives objects positions over time. Here is the expected text
format of a line:

.. code-block:: bash

	{otype} {fbeg} {xbeg} {ybeg} {fend} {xend} {yend}

``{otype}`` can be ``meteor``, ``star`` or ``noise``.
``{fbeg}`` and ``{fend}`` stand for *frame begin* and *frame end*.
``{xbeg}`` and ``{ybeg}`` stand for :math:`x` and :math:`y` coordinates of the
*frame begin*.
``{xend}`` and ``{yend}`` stand for :math:`x` and :math:`y` coordinates of the
*frame end*.
``{fbeg}``, ``{xbeg}``, ``{ybeg}``, ``{fend}``, ``{xend}``, ``{yend}`` are
positive integers. Each line corresponds to an object and each value is
separated by a space character.

.. _visu_vid-out-path:

``--vid-out-path``
------------------

   :Deprecated: ``--out-video``
   :Type: STRING
   :Default: [empty]
   :Example: ``--vid-out-path sky.mp4``

Path of the output video (supports also a path to a sequence of images
``path/basename_%05d.jpg``) with meteor tracking colored rectangles (|BBs|).
If ``--gt-path`` is set then the bounding rectangles are red if *false positive*
meteor and green if *true positive* meteor.
