.. _user_executables_usage_log:

Log Parser Parameters
"""""""""""""""""""""

The log parser is located here: ``./bin/fmdt-log-parser``.

The following table summarizes the available parameters:

+----------------------+---------+----------------------------------------------------+
| Argument             | Type    | Details                                            |
+======================+=========+====================================================+
| ``--log-path``       | STRING  | See :numref:`log_log-path`.                        |
+----------------------+---------+----------------------------------------------------+
| ``--trk-roi-path``   | STRING  | See :numref:`log_trk-roi-path`.                    |
+----------------------+---------+----------------------------------------------------+
| ``--log-flt``        | STRING  | See :numref:`log_log-flt`.                         |
+----------------------+---------+----------------------------------------------------+
| ``--fra-path``       | STRING  | See :numref:`log_fra-path`.                        |
+----------------------+---------+----------------------------------------------------+
| ``--ftr-name``       | STRING  | See :numref:`log_ftr-name`.                        |
+----------------------+---------+----------------------------------------------------+
| ``--ftr-path``       | STRING  | See :numref:`log_ftr-path`.                        |
+----------------------+---------+----------------------------------------------------+
| ``--trk-path``       | STRING  | See :numref:`log_trk-path`.                        |
+----------------------+---------+----------------------------------------------------+
| ``--trk-json-path``  | STRING  | See :numref:`log_trk-json-path`.                   |
+----------------------+---------+----------------------------------------------------+
| ``--trk-bb-path``    | STRING  | See :numref:`log_trk-bb-path`.                     |
+----------------------+---------+----------------------------------------------------+

.. _log_log-path:

``--log-path``
--------------

   :Type: STRING
   :Default: [empty]
   :Example: ``--log-path detect_logs/``

Path of the input logs. These logs should be firstly generated by
``fmdt-detect``. This path is **mandatory** and can be a file or a directory.
The contents of these logs are fully detailed in :numref:`detect_log-path`.

.. _log_trk-roi-path:

``--trk-roi-path``
------------------

   :Type: STRING
   :Default: [empty]
   :Example: ``--trk-roi-path trk2roi.txt``

Path to the input file containing lists of the |RoI| ids per tracked object.
This is mandatory if you want to generate the |BBs| (see the
:ref:`log_trk-bb-path` parameter) or to extract a specific feature (see the
:ref:`log_ftr-path` and the :ref:`log_ftr-name` parameters).
The contents of this file is detailed in :numref:`detect_trk-roi-path`.

.. _log_log-flt:

``--log-flt``
-------------

   :Type: STRING
   :Default: "^[0-9]{5}\.txt"
   :Example: ``--log-flt .*``

This is a regular expression to select the files to parse as ``fmdt-detect``
logs. It allows to skip files that are not related to the logs.

.. _log_fra-path:

``--fra-path``
--------------

   :Type: STRING
   :Default: [empty]
   :Example: ``--fra-path frames.json``

Path to store the frames in a Json format. Each frame contains ``fmdt-detect``
log tables (``RoIs``, ``Assocs``, ``Motion``, ``Tracks``).
It is required to fill the :ref:`log_log-path` parameter.

.. _log_ftr-name:

``--ftr-name``
--------------

   :Type: STRING
   :Default: [empty]
   :Example: ``--ftr-name mag``

This option allows to tell which specific *feature* you want to extract.
It is required to fill both the :ref:`log_log-path` and the
:ref:`log_trk-roi-path` parameters.

.. _log_ftr-path:

``--ftr-path``
--------------

   :Type: STRING
   :Default: [empty]
   :Example: ``--ftr-path mag.txt``

The path to store the extracted feature. It is required to fill both the
:ref:`log_log-path` and the :ref:`log_trk-roi-path` parameters.

The output file will contain the features per tracked object. Each line
corresponds to a track/object and here is the corresponding line format:

.. code-block:: bash

   {tid} {otype} {ftr1} {ftr2} {...} {ftrn}

``{ftr1}`` is the first feature value of the track/object of ``{tid}`` id.
``{ftr2}`` is the second feature value (in the second frame where the object
has been tracked). And so on, until the last feature value ``{ftrn}``. Note
that sometime the feature value can be ``0``, it means that the object has
been extrapolated on this frame, thus the feature cannot be returned.

.. _log_trk-path:

``--trk-path``
--------------

   :Type: STRING
   :Default: [empty]
   :Example: ``--trk-path tracks.txt``

Path to the output file containing the list of the final tracks. The contents of
this file is detailed in :numref:`detect_stdout`.
This option requires to fill the :ref:`log_log-path` input file.

.. _log_trk-json-path:

``--trk-json-path``
-------------------

   :Type: STRING
   :Default: [empty]
   :Example: ``--trk-json-path tracks.json``

Path to the output file containing a dictionary of the final tracks in Json
format. This is very similar to the :ref:`log_trk-path` parameter but the
data format differs.

.. _log_trk-bb-path:

``--trk-bb-path``
-----------------

   :Deprecated: ``--out-bb``
   :Type: STRING
   :Default: [empty]
   :Example: ``--trk-bb-path bb.txt``

Path to the output bounding boxes file required by ``fmdt-visu`` to draw
detection rectangles. Each bounding box defines the area of an object, frame by
frame. This option requires to fill both the :ref:`log_log-path` and the
:ref:`log_trk-roi-path` parameters.

Here is the corresponding line format:

.. code-block:: bash

	{frame_id} {x_radius} {y_radius} {center_x} {center_y} {track_id} {is_extrapolated}

Each line corresponds to a frame and to an object, each value is separated by a
space character.
