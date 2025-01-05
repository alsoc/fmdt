.. _user_executables_usage_reduce:

Reduction Parameters
""""""""""""""""""""

The reduction program is located here: ``./bin/fmdt-reduce``.

The following table summarizes the available parameters:

+----------------------+---------+----------------------------------------------------+
| Argument             | Type    | Details                                            |
+======================+=========+====================================================+
| ``--vid-in-path``    | STRING  | See :numref:`reduce_vid-in-path`.                  |
+----------------------+---------+----------------------------------------------------+
| ``--vid-in-start``   | INTEGER | See :numref:`reduce_vid-in-start`.                 |
+----------------------+---------+----------------------------------------------------+
| ``--vid-in-stop``    | INTEGER | See :numref:`reduce_vid-in-stop`.                  |
+----------------------+---------+----------------------------------------------------+
| ``--vid-in-threads`` | INTEGER | See :numref:`reduce_vid-in-threads`.               |
+----------------------+---------+----------------------------------------------------+
| ``--red-op``         | STRING  | See :numref:`reduce_red-op`.                       |
+----------------------+---------+----------------------------------------------------+
| ``--trk-path``       | STRING  | See :numref:`reduce_trk-path`.                     |
+----------------------+---------+----------------------------------------------------+
| ``--trk-id``         | BOOLEAN | See :numref:`reduce_trk-id`.                       |
+----------------------+---------+----------------------------------------------------+
| ``--trk-nat-num``    | BOOLEAN | See :numref:`reduce_trk-nat-num`.                  |
+----------------------+---------+----------------------------------------------------+
| ``--trk-only-meteor``| BOOLEAN | See :numref:`reduce_trk-only-meteor`.              |
+----------------------+---------+----------------------------------------------------+
| ``--gt-path``        | STRING  | See :numref:`reduce_gt-path`.                      |
+----------------------+---------+----------------------------------------------------+
| ``--fra-out-path``   | STRING  | See :numref:`reduce_fra-out-path`.                 |
+----------------------+---------+----------------------------------------------------+
| ``--stats``          | BOOLEAN | See :numref:`reduce_stats`.                        |
+----------------------+---------+----------------------------------------------------+
| ``--stats-histo``    | BOOLEAN | See :numref:`reduce_stats-histo`.                  |
+----------------------+---------+----------------------------------------------------+

.. _reduce_vid-in-path:

``--vid-in-path``
-----------------

   :Deprecated: ``--in-video``
   :Type: STRING
   :Default: [empty]
   :Example: ``--vid-in-path ~/Videos/meteors.mp4``

Input video path (supports also a path to a sequence of images
``path/basename_%05d.jpg``).

.. _reduce_vid-in-start:

``--vid-in-start``
------------------

   :Deprecated: ``--fra-start``
   :Type: INTEGER
   :Default: ``0``
   :Example: ``--vid-in-start 12``

First frame id (included) to start the detection in the video sequence.

.. _reduce_vid-in-stop:

``--vid-in-stop``
-----------------

   :Deprecated: ``--fra-end``
   :Type: INTEGER
   :Default: ``0``
   :Example: ``--vid-in-stop 42``

Last frame id (included) to stop the detection in the video sequence. If set to
``0``, read entire video.

.. _reduce_vid-in-threads:

``--vid-in-threads``
--------------------

   :Deprecated: ``--ffmpeg-threads``
   :Type: INTEGER
   :Default: ``0``
   :Example: ``--vid-in-threads 1``

Select the number of threads to use to decode video input (in ``ffmpeg``). If
set to ``0``, ``ffmpeg`` chooses the number of threads automatically.

.. _reduce_red-op:

``--red-op``
------------

   :Type: STRING
   :Default: ``NONE``
   :Example: ``--red-op MAX``

Set the reduction operation. Supported values are:

- ``NONE``: Does not perform any reduction
- ``MAX``: Performs :math:`\max(i)` per pixel :math:`i`
- ``MIN``: Performs :math:`\min(i)` per pixel :math:`i`
- ``MAX-MIN``: Performs :math:`\max(i)-\min(i)` per pixel :math:`i`

.. _reduce_trk-path:

``--trk-path``
--------------

   :Deprecated: ``--in-tracks``
   :Type: STRING
   :Default: [empty]
   :Example: ``--trk-path tracks.txt``

The tracks file corresponding to the input video (generated from
``fmdt-detect``). See :numref:`detect_stdout` for the description of the
expected text input format.

.. _reduce_trk-id:

``--trk-id``
------------

   :Deprecated: ``--show-id``
   :Type: BOOLEAN
   :Example: ``--trk-id``

Show the object ids on the output video and frames. Requires to link with OpenCV
library (``-DFMDT_OPENCV_LINK`` CMake option, see
:numref:`user_installation_cmake`).

.. _reduce_trk-nat-num:

``--trk-nat-num``
-----------------

   :Deprecated: ``--show-id``
   :Type: BOOLEAN
   :Example: ``--trk-nat-num``

Natural numbering of the object ids, work only if ``--trk-id`` is set.

.. _reduce_trk-only-meteor:

``--trk-only-meteor``
---------------------

   :Deprecated: ``--only-meteor``
   :Type: BOOLEAN
   :Example: ``--trk-only-meteor``

Show only meteors.

.. _reduce_gt-path:

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

.. _reduce_fra-out-path:

``--fra-out-path``
------------------

   :Deprecated: ``--out-frame``
   :Type: STRING
   :Default: [empty]
   :Example: ``--fra-out-path red.png``

Path of the output frame.

.. _reduce_stats:

``--stats``
-----------

   :Type: BOOLEAN
   :Example: ``--stats``

Display some statistics about the pixels in the video sequence.

.. code-block:: bash

	# Pixel Statistics:
	# -----------------
	#  * frames_cnt   = {fra_cnt}
	#  * pix_min      = {px_min}
	#  * pix_max      = {px_max}
	#  * pix_sum      = {px_sum}
	#  * pix_cnt      = {px_cnt}
	#  * pix_mean     = {px_mean}
	#  * pix_variance = {px_var}
	#  * pix_std_dev  = {px_sd}

- ``{fra_cnt}``: The number of parsed frames in the video sequence
- ``{px_min}``: The minimum pixel intensity in the video sequence
  (:math:`\min_{px}`)
- ``{px_max}``: The maximum pixel intensity in the video sequence
  (:math:`\max_{px}`)
- ``{px_sum}``: The sum of pixel intensities in the video sequence
- ``{px_cnt}``: The total number of pixels in the whole video sequence
- ``{px_mean}``: The mean pixel intensity in the whole video sequence
  (:math:`\bar{px}`)
- ``{px_var}``: The pixel intensity variance :math:`Var_{px}` (computed
  according to the Welford's online algorithm)
- ``{px_sd}``: The pixel intensity standard deviation
  (:math:`\sigma_{px} = \sqrt{Var_{px}}`)

.. _reduce_stats-histo:

``--stats-histo``
-----------------

   :Type: BOOLEAN
   :Example: ``--stats-histo``

Print pixels histogram on the standard output as follow:

.. code-block:: bash

	0 9
	1 3
	2 2
	3 5
	4 5
	5 5
	6 5
	7 8
	8 3
	9 11
	...
	254 52
	255 853

Each line corresponds to one pixel intensity.
The fist value of the line is the considered pixel intensity and the second
one is its occurrences.

.. note:: ``--stats-histo`` automatically sets the :ref:`reduce_stats`
          parameter.

.. note:: Can easily be printed in the console with the help of ``gnuplot``:

          .. code-block:: bash

          	./bin/fmdt-reduce --vid-in-path [...] --stats-histo | \
          	    gnuplot -e \
          	    "set terminal dumb size 100, 30; set autoscale; set xrange [0:255]; set title 'pixel histogram';
          	    set ylabel 'count'; set xlabel 'pixel value'; plot '-'  with lines notitle";

