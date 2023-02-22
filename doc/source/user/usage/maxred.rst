.. _user_executables_usage_maxred:

Max-reduction Parameters
""""""""""""""""""""""""

The max-reduction generation program is located here: ``./exe/fmdt-maxred``.

The following table summarizes the available parameters:

+----------------------+---------+----------------------------------------------------+
| Argument             | Type    | Details                                            |
+======================+=========+====================================================+
| ``--vid-in-path``    | STRING  | See :numref:`maxred_vid-in-path`.                  |
+----------------------+---------+----------------------------------------------------+
| ``--vid-in-start``   | INTEGER | See :numref:`maxred_vid-in-start`.                 |
+----------------------+---------+----------------------------------------------------+
| ``--vid-in-stop``    | INTEGER | See :numref:`maxred_vid-in-stop`.                  |
+----------------------+---------+----------------------------------------------------+
| ``--vid-in-threads`` | INTEGER | See :numref:`maxred_vid-in-threads`.               |
+----------------------+---------+----------------------------------------------------+
| ``--trk-path``       | STRING  | See :numref:`maxred_trk-path`.                     |
+----------------------+---------+----------------------------------------------------+
| ``--trk-id``         | BOOLEAN | See :numref:`maxred_trk-id`.                       |
+----------------------+---------+----------------------------------------------------+
| ``--trk-nat-num``    | BOOLEAN | See :numref:`maxred_trk-nat-num`.                  |
+----------------------+---------+----------------------------------------------------+
| ``--trk-only-meteor``| BOOLEAN | See :numref:`maxred_trk-only-meteor`.              |
+----------------------+---------+----------------------------------------------------+
| ``--gt-path``        | STRING  | See :numref:`maxred_gt-path`.                      |
+----------------------+---------+----------------------------------------------------+
| ``--fra-out-path``   | STRING  | See :numref:`maxred_fra-out-path`.                 |
+----------------------+---------+----------------------------------------------------+

.. _maxred_vid-in-path:

``--vid-in-path``
-----------------

   :Deprecated: ``--in-video``
   :Type: STRING
   :Default: [empty]
   :Example: ``--vid-in-path ~/Videos/meteors.mp4``

Input video path (supports also a path to a sequence of images
``path/basename_%05d.jpg``).

.. _maxred_vid-in-start:

``--vid-in-start``
------------------

   :Deprecated: ``--fra-start``
   :Type: INTEGER
   :Default: ``0``
   :Example: ``--vid-in-start 12``

First frame id (included) to start the detection in the video sequence.

.. _maxred_vid-in-stop:

``--vid-in-stop``
-----------------

   :Deprecated: ``--fra-end``
   :Type: INTEGER
   :Default: ``0``
   :Example: ``--vid-in-stop 42``

Last frame id (included) to stop the detection in the video sequence. If set to
``0``, read entire video.

.. _maxred_vid-in-threads:

``--vid-in-threads``
--------------------

   :Deprecated: ``--ffmpeg-threads``
   :Type: INTEGER
   :Default: ``0``
   :Example: ``--vid-in-threads 1``

Select the number of threads to use to decode video input (in ``ffmpeg``). If
set to ``0``, ``ffmpeg`` chooses the number of threads automatically.

.. _maxred_trk-path:

``--trk-path``
--------------

   :Deprecated: ``--in-tracks``
   :Type: STRING
   :Default: [empty]
   :Example: ``--trk-path tracks.txt``

The tracks file corresponding to the input video (generated from
``fmdt-detect``). See :numref:`detect_stdout` for the description of the
expected text input format.

.. _maxred_trk-id:

``--trk-id``
------------

   :Deprecated: ``--show-id``
   :Type: BOOLEAN
   :Example: ``--trk-id``

Show the object ids on the output video and frames. Requires to link with OpenCV
library (``-DFMDT_OPENCV_LINK`` CMake option, see
:numref:`user_installation_cmake`).

.. _maxred_trk-nat-num:

``--trk-nat-num``
-----------------

   :Deprecated: ``--show-id``
   :Type: BOOLEAN
   :Example: ``--trk-nat-num``

Natural numbering of the object ids, work only if ``--trk-id`` is set.

.. _maxred_trk-only-meteor:

``--trk-only-meteor``
---------------------

   :Deprecated: ``--only-meteor``
   :Type: BOOLEAN
   :Example: ``--trk-only-meteor``

Show only meteors.

.. _maxred_gt-path:

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

The tracks file corresponding to the input video (generated from
``fmdt-detect``). See :numref:`detect_stdout` for the description of the
expected text input format.

.. _maxred_fra-out-path:

``--fra-out-path``
------------------

   :Deprecated: ``--out-frame``
   :Type: STRING
   :Default: [empty]
   :Example: ``--fra-out-path maxred.png``

Path of the output frame.
