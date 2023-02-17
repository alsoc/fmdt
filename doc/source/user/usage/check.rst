.. _user_executables_usage_check:

Check Parameters
""""""""""""""""

The meteors checking program is located here: ``./exe/fmdt-check``.

.. The list of available arguments:

.. | **Argument** | **Deprecated** | **Type** | **Default** | **Req** | **Description** |
.. | :---         | :---           | :---     | :---        | :---    | :--- |
.. | `--trk-path` | `--in-tracks`  | str      | None        | Yes     | The track file corresponding to the input video (generated from `fmdt-detect`). |
.. | `--gt-path`  | `--in-gt`      | str      | None        | Yes     | File containing the ground truth. |

.. **Note**: to run `fmdt-check`, it is required to run `fmdt-detect` before. This
.. will generate the required `tracks.txt` file.

.. Input/output text formats are detailed in the
.. [Input and Output Text Formats](#input-and-output-text-formats) section.


The following table summarizes the available parameters:

+----------------------+---------+----------------------------------------------------+
| Argument             | Type    | Details                                            |
+======================+=========+====================================================+
| ``--trk-path``       | STRING  | See :numref:`check_trk-path`.                      |
+----------------------+---------+----------------------------------------------------+
| ``--gt-path``        | STRING  | See :numref:`check_gt-path`.                       |
+----------------------+---------+----------------------------------------------------+


.. _check_stdout:

Standard Output
---------------

The first part of ``fmdt-check`` *stdout* is a table where each entry
corresponds  to an object of the |GT|:

.. code-block:: bash

	# ---------------||---------------||-----------------||--------
	#    GT Object   ||      Hits     ||    GT Frames    || Tracks
	# ---------------||---------------||-----------------||--------
	# -----|---------||--------|------||--------|--------||--------
	#   Id |    Type || Detect |  GT  ||  Start |  Stop  ||      #
	# -----|---------||--------|------||--------|--------||--------
	 {tid} | {otype} ||   {dh} | {gh} || {staf} | {stof} ||   {nt}


* ``{tid}``: a positive integer value representing a unique identifier of ground
  truth track/object,
* ``{otype}``: a string of the object type, can be: ``meteor``, ``star`` or
  ``noise``,
* ``{dh}``: a positive integer value of the number of frames when the object is
  detected (from the tracks, ``--trk-path``),
* ``{gh}``: a positive integer value of the number of frame when the object is
  present (from the ground truth, ``--gt-path``),
* ``{staf}``: a positive integer value of the frame start (from the ground
  truth, ``--gt-path``),
* ``{stof}``: a positive integer value of the frame stop (from the ground truth,
  ``--gt-path``),
* ``{nt}``: a positive integer value of the number of tracks that match the
  ground truth object.

In a second part, ``fmdt-check`` *stdout* gives some statistics in the following
format (``{pi}`` stands for *positive integer* and ``{pf}`` for *positive
float*):

.. code-block:: bash

	Statistics:
	  - Number of GT objs = ['meteor': {pi}, 'star': {pi}, 'noise': {pi}, 'all': {pi}]
	  - Number of tracks  = ['meteor': {pi}, 'star': {pi}, 'noise': {pi}, 'all': {pi}]
	  - True positives    = ['meteor': {pi}, 'star': {pi}, 'noise': {pi}, 'all': {pi}]
	  - False positives   = ['meteor': {pi}, 'star': {pi}, 'noise': {pi}, 'all': {pi}]
	  - True negative     = ['meteor': {pi}, 'star': {pi}, 'noise': {pi}, 'all': {pi}]
	  - False negative    = ['meteor': {pi}, 'star': {pi}, 'noise': {pi}, 'all': {pi}]
	  - Tracking rate     = ['meteor': {pf}, 'star': {pf}, 'noise': {pf}, 'all': {pf}]

* ``Number of GT objs``: the number of objects from the ground truth,
* ``Number of tracks``: the number of objects from the tracks (``fmdt-detect``
  output),
* ``True positives``: number of detected objects that are in the ground truth
  (with the same type),
* ``False positives``: number of detected objects that are not in the ground
  truth (or that have a different type).
* ``True negative``: number of detected objects that are different from the
  current type of object. For instance, if we focus on ``meteor`` object type,
  the number of false negatives is the sum of all the objects in the tracks that
  are ``star`` or ``noise``,
* ``False negative``: number of non-detected objects (present in the ground
  truth and not present in the tracks),
* ``Tracking rate``: the sum of detected hits on the sum of the ground truth
  hits. Range is between ``1`` (perfect tracking) and ``0``
  (nothing is tracked). When there are more hits in a track than in the ground
  truth, the detected hits are the ground truth hits minus the extra hits of the
  track.

For each line, the ``meteor``, ``star`` and ``noise`` object types are
considered. ``all`` stands for all types, sometime ``all`` can be mean-less.

.. _check_trk-path:

``--trk-path``
--------------

   :Deprecated: ``--in-tracks``
   :Type: STRING
   :Default: [empty]
   :Examples: ``--trk-path tracks.txt``

The tracks file corresponding to the input video (generated from
``fmdt-detect``). See :numref:`detect_stdout` for the description of the
expected text input format.

.. _check_gt-path:

``--gt-path``
-------------

   :Deprecated: ``--in-gt``
   :Type: STRING
   :Default: [empty]
   :Examples: ``--gt-path gt.txt``

File containing the ground truth.
See :numref:`visu_gt-path` for the description of the expected text input
format.
