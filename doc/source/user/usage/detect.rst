.. _user_executables_usage_detect:

Detection Parameters
""""""""""""""""""""

The meteors detection chain is located here: ``./bin/fmdt-detect``.

The following table summarizes the available parameters:

+----------------------+---------+----------------------------------------------------+
| Argument             | Type    | Details                                            |
+======================+=========+====================================================+
| ``--vid-in-path``    | STRING  | See :numref:`detect_vid-in-path`.                  |
+----------------------+---------+----------------------------------------------------+
| ``--vid-in-start``   | INTEGER | See :numref:`detect_vid-in-start`.                 |
+----------------------+---------+----------------------------------------------------+
| ``--vid-in-stop``    | INTEGER | See :numref:`detect_vid-in-stop`.                  |
+----------------------+---------+----------------------------------------------------+
| ``--vid-in-skip``    | INTEGER | See :numref:`detect_vid-in-skip`.                  |
+----------------------+---------+----------------------------------------------------+
| ``--vid-in-buff``    | BOOLEAN | See :numref:`detect_vid-in-buff`.                  |
+----------------------+---------+----------------------------------------------------+
| ``--vid-in-loop``    | INTEGER | See :numref:`detect_vid-in-loop`.                  |
+----------------------+---------+----------------------------------------------------+
| ``--vid-in-threads`` | INTEGER | See :numref:`detect_vid-in-threads`.               |
+----------------------+---------+----------------------------------------------------+
| ``--vid-in-dec``     | STRING  | See :numref:`detect_vid-in-dec`.                   |
+----------------------+---------+----------------------------------------------------+
| ``--ccl-impl``       | STRING  | See :numref:`detect_ccl-impl`.                     |
+----------------------+---------+----------------------------------------------------+
| ``--ccl-hyst-lo``    | INTEGER | See :numref:`detect_ccl-hyst-lo`.                  |
+----------------------+---------+----------------------------------------------------+
| ``--ccl-hyst-hi``    | INTEGER | See :numref:`detect_ccl-hyst-hi`.                  |
+----------------------+---------+----------------------------------------------------+
| ``--ccl-fra-path``   | STRING  | See :numref:`detect_ccl-fra-path`.                 |
+----------------------+---------+----------------------------------------------------+
| ``--ccl-fra-id``     | BOOLEAN | See :numref:`detect_ccl-fra-id`.                   |
+----------------------+---------+----------------------------------------------------+
| ``--cca-mag``        | BOOLEAN | See :numref:`detect_cca-mag`.                      |
+----------------------+---------+----------------------------------------------------+
| ``--cca-ell``        | BOOLEAN | See :numref:`detect_cca-ell`.                      |
+----------------------+---------+----------------------------------------------------+
| ``--cca-roi-max1``   | INTEGER | See :numref:`detect_cca-roi-max1`.                 |
+----------------------+---------+----------------------------------------------------+
| ``--cca-roi-max2``   | INTEGER | See :numref:`detect_cca-roi-max2`.                 |
+----------------------+---------+----------------------------------------------------+
| ``--mrp-s-min``      | INTEGER | See :numref:`detect_mrp-s-min`.                    |
+----------------------+---------+----------------------------------------------------+
| ``--mrp-s-max``      | INTEGER | See :numref:`detect_mrp-s-max`.                    |
+----------------------+---------+----------------------------------------------------+
| ``--knn-k``          | INTEGER | See :numref:`detect_knn-k`.                        |
+----------------------+---------+----------------------------------------------------+
| ``--knn-d``          | INTEGER | See :numref:`detect_knn-d`.                        |
+----------------------+---------+----------------------------------------------------+
| ``--knn-s``          | FLOAT   | See :numref:`detect_knn-s`.                        |
+----------------------+---------+----------------------------------------------------+
| ``--trk-ext-d``      | INTEGER | See :numref:`detect_trk-ext-d`.                    |
+----------------------+---------+----------------------------------------------------+
| ``--trk-ext-o``      | INTEGER | See :numref:`detect_trk-ext-o`.                    |
+----------------------+---------+----------------------------------------------------+
| ``--trk-angle``      | FLOAT   | See :numref:`detect_trk-angle`.                    |
+----------------------+---------+----------------------------------------------------+
| ``--trk-star-min``   | INTEGER | See :numref:`detect_trk-star-min`.                 |
+----------------------+---------+----------------------------------------------------+
| ``--trk-meteor-min`` | INTEGER | See :numref:`detect_trk-meteor-min`.               |
+----------------------+---------+----------------------------------------------------+
| ``--trk-meteor-max`` | INTEGER | See :numref:`detect_trk-meteor-max`.               |
+----------------------+---------+----------------------------------------------------+
| ``--trk-ddev``       | FLOAT   | See :numref:`detect_trk-ddev`.                     |
+----------------------+---------+----------------------------------------------------+
| ``--trk-ell-min``    | FLOAT   | See :numref:`detect_trk-ell-min`.                  |
+----------------------+---------+----------------------------------------------------+
| ``--trk-all``        | BOOLEAN | See :numref:`detect_trk-all`.                      |
+----------------------+---------+----------------------------------------------------+
| ``--trk-roi-path``   | STRING  | See :numref:`detect_trk-roi-path`.                 |
+----------------------+---------+----------------------------------------------------+
| ``--log-path``       | STRING  | See :numref:`detect_log-path`.                     |
+----------------------+---------+----------------------------------------------------+
| ``--vid-out-path``   | STRING  | See :numref:`detect_vid-out-path`.                 |
+----------------------+---------+----------------------------------------------------+
| ``--vid-out-play``   | BOOLEAN | See :numref:`detect_vid-out-play`.                 |
+----------------------+---------+----------------------------------------------------+
| ``--vid-out-id``     | BOOLEAN | See :numref:`detect_vid-out-id`.                   |
+----------------------+---------+----------------------------------------------------+

.. _detect_stdout:

Standard Output
---------------

``fmdt-detect`` outputs a list of tracks. The tracks represent the detected
objects in the video sequence. Here is the template of the output text:

.. code-block:: bash

	# -------||---------------------------||---------------------------||---------
	#  Track ||           Begin           ||            End            ||  Object
	# -------||---------------------------||---------------------------||---------
	# -------||---------|--------|--------||---------|--------|--------||---------
	#     Id || Frame # |      x |      y || Frame # |      x |      y ||    Type
	# -------||---------|--------|--------||---------|--------|--------||---------
	   {tid} ||  {fbeg} | {xbeg} | {ybeg} ||  {fend} | {xend} | {yend} || {otype}

* ``{tid}``: a positive integer (start from ``1``) value representing a unique
  track identifier,
* ``{fbeg}``: a positive integer value representing the first frame in the video
  sequence when the track is detected,
* ``{xbeg}``: a positive real value of the x-axis coordinate (beginning of the
  track),
* ``{ybeg}``: a positive real value of the y-axis coordinate (beginning of the
  track),
* ``{fend}``: a positive integer value representing the last frame in the video
  sequence when the track is detected,
* ``{xend}``: a positive real value of the x-axis coordinate (end of the track),
* ``{yend}``: a positive real value of the y-axis coordinate (end of the track),
* ``{otype}``: a string of the object type, can be: ``meteor``, ``star`` or
  ``noise``.

.. _detect_vid-in-path:

``--vid-in-path``
-----------------

   :Deprecated: ``--in-video``
   :Type: STRING
   :Default: [empty]
   :Example: ``--vid-in-path ~/Videos/meteors.mp4``

Input video path (supports also a path to a sequence of images
``path/basename_%05d.jpg``).

.. _detect_vid-in-start:

``--vid-in-start``
------------------

   :Deprecated: ``--fra-start``
   :Type: INTEGER
   :Default: ``0``
   :Example: ``--vid-in-start 12``

First frame id (included) to start the detection in the video sequence.

.. _detect_vid-in-stop:

``--vid-in-stop``
-----------------

   :Deprecated: ``--fra-end``
   :Type: INTEGER
   :Default: ``0``
   :Example: ``--vid-in-stop 42``

Last frame id (included) to stop the detection in the video sequence. If set to
``0``, read entire video.

.. _detect_vid-in-skip:

``--vid-in-skip``
-----------------

   :Deprecated: ``--fra-skip``
   :Type: INTEGER
   :Default: ``0``
   :Example: ``--vid-in-skip 1``

Number of frames to skip.

.. _detect_vid-in-buff:

``--vid-in-buff``
-----------------

   :Deprecated: ``--video-buff``
   :Type: BOOLEAN
   :Example: ``--vid-in-buff``

Bufferize all the video in global memory before executing the chain.

.. _detect_vid-in-loop:

``--vid-in-loop``
-----------------

   :Deprecated: ``--video-loop``
   :Type: INTEGER
   :Default: ``1``
   :Example: ``--vid-in-loop 10``

Number of times the video is read in loop.

.. _detect_vid-in-threads:

``--vid-in-threads``
--------------------

   :Deprecated: ``--ffmpeg-threads``
   :Type: INTEGER
   :Default: ``0``
   :Example: ``--vid-in-threads 1``

Select the number of threads to use to decode video input (in ``ffmpeg``). If
set to ``0``, ``ffmpeg`` chooses the number of threads automatically.

.. _detect_vid-in-dec:

``--vid-in-dec``
----------------

   :Type: STRING
   :Default: ``FFMPEG-IO``
   :Example: ``--vid-in-dec VCODECS-IO``

Select the input video decoder interface. ``FFMPEG-IO`` is based on the cmd line
``ffmpeg`` executable to exchange decoded frames over a system pipe.
``VCODECS-IO`` directly call the ``libav``.

.. note:: ``VCODECS-IO`` works only if |FMDT| has been compiled with the CMake
          ``-DFMDT_USE_VCIO=ON`` option (see :numref:`user_installation_cmake`).

.. _detect_ccl-impl:

``--ccl-impl``
--------------

   :Type: STRING
   :Default: ``LSLH``
   :Example: ``--ccl-impl LSLH``

Choose the LSL implementation. Can be ``LSLH`` or ``LSLM``.

``LSLH`` is the implementation discribed in :cite:`Lacassagne2009` and ``LSLM``
is the implementation discribed in :cite:`Lemaitre2020`.

.. note:: ``LSLM`` is only available if |FMDT| has been compiled with the CMake
          ``-DFMDT_LSL_LINK=ON`` option (see :numref:`user_installation_cmake`).

.. _detect_ccl-hyst-lo:

``--ccl-hyst-lo``
-----------------

   :Deprecated: ``--light-min``
   :Type: INTEGER
   :Default: ``55``
   :Example: ``--ccl-hyst-lo 100``

Minimum light intensity for hysteresis threshold (grayscale :math:`[0;255]`).

.. _detect_ccl-hyst-hi:

``--ccl-hyst-hi``
-----------------

   :Deprecated: ``--light-max``
   :Type: INTEGER
   :Default: ``80``
   :Example: ``--ccl-hyst-hi 140``

Maximum light intensity for hysteresis threshold (grayscale :math:`[0;255]`).

.. _detect_ccl-fra-path:

``--ccl-fra-path``
------------------

   :Deprecated: ``--out-frames``
   :Type: STRING
   :Default: [empty]
   :Example: ``--ccl-fra-path ccl_fra/%05d.png``

Path of the files for |CC| debug (``path/cc_%05d.png``).

.. _detect_ccl-fra-id:

``--ccl-fra-id``
----------------

   :Deprecated: ``--show-id``
   :Type: BOOLEAN
   :Example: ``--ccl-fra-id``

Show the |RoI|/|CC| ids on the output frames (to combine with ``--ccl-fra-path``
parameter). Requires to link with OpenCV library (``-DFMDT_OPENCV_LINK`` CMake
option, see :numref:`user_installation_cmake`).

.. _detect_cca-mag:

``--cca-mag``
-------------

   :Type: BOOLEAN
   :Default: [empty]
   :Example: ``--cca-mag``

Enable the computation of two news features in the |CCA|: the magnitude and the
counter of saturated pixels (to be combined with the :ref:`detect_log-path`
option).

.. _detect_cca-ell:

``--cca-ell``
-------------

   :Type: BOOLEAN
   :Default: [empty]
   :Example: ``--cca-ell``

Enable the computation of two news features in the |CCA|: ``a`` the semi-major
axis of an ellipse and ``b`` the semi-minor axis of an ellipse. This option
has to be combined with the :ref:`detect_log-path` option.

.. _detect_cca-roi-max1:

``--cca-roi-max1``
------------------

   :Type: INTEGER
   :Default: ``65535``
   :Example: ``--cca-roi-max1 10000``

Maximum number of RoIs before hysteresis threshold. Allow to manage the memory
footprint of the program. The Smaller the maximum number of RoIs, the smaller
the memory footprint.

.. _detect_cca-roi-max2:

``--cca-roi-max2``
------------------

   :Type: INTEGER
   :Default: ``400``
   :Example: ``--cca-roi-max2 200``

Maximum number of RoIs after hysteresis threshold. Allow to manage the memory
footprint of the program. The Smaller the maximum number of RoIs, the smaller
the memory footprint.

.. _detect_mrp-s-min:

``--mrp-s-min``
---------------

   :Deprecated: ``--surface-min``
   :Type: INTEGER
   :Default: ``3``
   :Example: ``--mrp-s-min 5``

Minimum surface of the |CCs| in pixels.

.. _detect_mrp-s-max:

``--mrp-s-max``
---------------

   :Deprecated: ``--surface-max``
   :Type: INTEGER
   :Default: ``1000``
   :Example: ``--mrp-s-max 50``

Maximum surface of the |CCs| in pixels.

.. _detect_knn-k:

``--knn-k``
-----------

   :Deprecated: ``-k``
   :Type: INTEGER
   :Default: ``3``
   :Example: ``--knn-k 5``

Maximum number of neighbors considered in the |k-NN| algorithm.

.. _detect_knn-d:

``--knn-d``
-----------

   :Deprecated: ``--max-dist``
   :Type: INTEGER
   :Default: ``10``
   :Example: ``--knn-d 25``

Maximum distance in pixels between two images (|k-NN| algorithm).

.. _detect_knn-s:

``--knn-s``
-----------

   :Deprecated: ``--min-ratio-s``
   :Type: FLOAT
   :Default: ``0.125``
   :Example: ``--knn-s 0.0``

Minimum surface ratio to match two |CCs| in |k-NN| (``0`` matches alls, ``1``
matches nothing). This parameter is also used for extrapolation in the tracking.

.. _detect_trk-ext-d:

``--trk-ext-d``
---------------

   :Deprecated: ``--r-extrapol``
   :Type: INTEGER
   :Default: ``5``
   :Example: ``--trk-ext-d 25``

Search radius in pixels for |CC| extrapolation (piece-wise tracking).

.. _detect_trk-ext-o:

``--trk-ext-o``
---------------

   :Deprecated: ``--extrapol-orde``
   :Type: INTEGER
   :Default: ``3``
   :Example: ``--trk-ext-o 1``

Maximum number of frames to extrapolate for lost objects (linear extrapolation).

.. _detect_trk-angle:

``--trk-angle``
---------------

   :Deprecated: ``--angle-max``
   :Type: FLOAT
   :Default: ``20.0``
   :Example: ``--trk-angle 35.0``

Tracking max angle between two meteors at :math:`t-1` and :math:`t` (in degree).
This is a classification criterion.

.. _detect_trk-star-min:

``--trk-star-min``
------------------

   :Deprecated: ``--fra-star-min``
   :Type: INTEGER
   :Default: ``15``
   :Example: ``--trk-star-min 5``

Minimum number of frames required to track a star. This is a classification
criterion.

.. _detect_trk-meteor-min:

``--trk-meteor-min``
--------------------

   :Deprecated: ``--fra-meteor-min``
   :Type: INTEGER
   :Default: ``3``
   :Example: ``--trk-meteor-min 5``

Minimum number of frames required to track a meteor. This is a classification
criterion.

.. _detect_trk-meteor-max:

``--trk-meteor-max``
--------------------

   :Deprecated: ``--fra-meteor-max``
   :Type: INTEGER
   :Default: ``100``
   :Example: ``--trk-meteor-max 50``

Maximum number of frames required to track a meteor. This is a classification
criterion.

.. _detect_trk-ddev:

``--trk-ddev``
--------------

   :Deprecated: ``--diff-dev``
   :Type: FLOAT
   :Default: ``4.0``
   :Example: ``--trk-ddev 5.5``

Multiplication factor of the standard deviation (|CC| error has to be higher
than :math:`ddev \times stddev` to be considered in movement). This is a
classification criterion.

.. _detect_trk-ell-min:

``--trk-ell-min``
-----------------

   :Type: FLOAT
   :Default: ``0.0``
   :Example: ``--cca-ell --trk-ell-min 3.0``

Minimum ellipse ratio to be considered as a meteor. This is a classification
criterion. If the value is ``0`` then this parameter has no effect. Moreover,
this parameter requires the :ref:`detect_cca-ell` parameter to work. If the
latest is not set, then this parameter is ignored.

.. _detect_trk-all:

``--trk-all``
-------------

   :Deprecated: ``--track-all``
   :Type: BOOLEAN
   :Example: ``--trk-all``

By default the program only tracks ``meteor`` object type. If ``--trk-all`` is
set, all object types are tracked (``meteor``, ``star`` or ``noise``).

This parameter is used in the :func:`_tracking_perform` function.

.. _detect_trk-roi-path:

``--trk-roi-path``
------------------

   :Type: STRING
   :Default: [empty]
   :Example: ``--trk-roi-path trk2roi.txt``

Path to the output file containing lists of the |RoI| ids of the tracked
objects. Each line corresponds to a track/object and here is the corresponding
line format:

.. code-block:: bash

   {tid} {otype} {rid1} {rid2} {...} {ridn}

``{rid1}`` is the first |RoI| id of the track/object of ``{tid}`` id.
``{rid2}`` is the second |RoI| id (in the second frame where the object
has been tracked). And so on, until the last |RoI| id ``{ridn}``. Note
that sometime the |RoI| id can be ``0``, it means that the object has been
extrapolated on this frame, thus there is no |RoI| id for this frame.

.. _detect_log-path:

``--log-path``
--------------

   :Deprecated: ``--out-stats``
   :Type: STRING
   :Default: [empty]
   :Example: ``--log-path detect_logs/``

Path of the output statistics, only required for debugging purpose.

.. warning:: This section targets advanced users, some knowledge about the
             implemented algorithms may be required!! You have been warned
             ;-).

``fmdt-detect`` comes with the ``--log-path`` option to help to understand what
is happening during the execution. This option enables to log internal
statistics of the different algorithms used to detect meteors.

The folder contains multiple files, one per frame. For instance, the file name
for the frame n°12 is: ``00012.txt``. Each file contains 5 different tables:

- Table 1: list of |RoIs| at :math:`t - 1` (result of the |CCL|/|CCA| +
  hysteresis algorithm at :math:`t - 1`),
- Table 2: list of |RoIs| at :math:`t` (result of the |CCL|/|CCA| +
  hysteresis algorithm at :math:`t`),
- Table 3: list of associations between :math:`t - 1` |RoIs| and :math:`t`
  |RoIs| (result of the |k-NN| algorithm) + errors/velocities after motion
  estimation,
- Table 4: motion estimation statistics between :math:`t - 1` and :math:`t`
  frame,
- Table 5: list of tracks since the beginning of the execution (final output
  of the detection chain).

.. note:: The first log file (usally named ``00000.txt``) only contains the
  table 2. This is normal because algorithms starting from |k-NN| require two
  consecutive frames to work.

Table 1 and table 2: |RoIs|
^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: bash

	# ------||----------------||---------------------------||-------------------------------------------||-------------||-----------||------------||-------------------
	#   RoI ||      Track     ||        Bounding Box       ||           Surface (S in pixels)           ||   Center    || Magnitude || Saturation ||      Ellipse
	# ------||----------------||---------------------------||-------------------------------------------||-------------||-----------||------------||-------------------
	# ------||------|---------||------|------|------|------||-----|------|------|-------|-------|-------||------|------||-----------||------------||-----|-----|-------
	#    ID ||   ID |    Type || xmin | xmax | ymin | ymax ||   S |   Sx |   Sy |   Sx2 |   Sy2 |   Sxy ||    x |    y ||        -- ||    Counter ||   a |   b | ratio
	# ------||------|---------||------|------|------|------||-----|------|------|-------|-------|-------||------|------||-----------||------------||-----|-----|-------
	  {rid} || {tid}| {otype} ||{xmin}|{xmax}|{ymin}|{ymax}|| {S} | {Sx} | {Sy} | {Sx2} | {Sy2} | {Sxy} || {cx} | {cy} ||     {mag} ||      {sat} || {a} | {b} |   {r}

Each line corresponds to one |RoI|:

- ``{rid}``: unique identifier for the current |RoI| (start from 1),
- ``{tid}``: unique identifier of the corresponding track (start from 1), can
  be, empty if no track is associated to the current |RoI|,
- ``{otype}``: type of the track object (``meteor``, ``noise`` or ``star``),
  only if there is a track corresponding to this RoI,
- ``{xmin}``: minimum :math:`x` position of the bounding box,
- ``{xmax}``: maximum :math:`x` position of the bounding box,
- ``{ymin}``: minimum :math:`y` position of the bounding box,
- ``{ymax}``: maximum :math:`y` position of the bounding box,
- ``{S}``: surface (area) of the |RoI| in pixels,
- ``{Sx}``: sum of :math:`x` properties,
- ``{Sy}``: sum of :math:`y` properties,
- ``{Sx2}``: sum of :math:`x^2` properties,
- ``{Sy2}``: sum of :math:`y^2` properties,
- ``{Sxy}``: sum of :math:`x \times y` properties,
- ``{cx}``: :math:`x` center of mass,
- ``{cy}``: :math:`y` center of mass,
- ``{mag}``: magnitude of the current |RoI| (accumulated brightness of the
  |RoI|),
- ``{sat}``: number of pixels that are saturated in the current |RoI| (a pixel
  :math:`x` is saturated when its intensity :math:`i_x = 255`),
- ``{a}``: semi-major axis (ellipse),
- ``{b}``: semi-minor axis (ellipse),
- ``{r}``: ratio :math:`a / b`.

``{mag}`` and ``{sat}`` features are not enabled by default (and the ``-``
character is printed in the corresponding columns). To enable theses features
you need to use the :ref:`detect_cca-mag` command line parameter. For more
information about those features you can refer to the
:func:`_features_compute_magnitude` function.

``{a}``, ``{b}`` and ``{r}`` features are are not enabled by default (and the
``-`` character is printed in the corresponding columns). To enable theses
features you need to use the :ref:`detect_cca-ell` command line parameter. For
more information about those features you can refer to the
:func:`_features_compute_ellipse` function.

Table 3: List of associations between |RoIs|
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: bash

	# --------------------||---------------||------------------------||-----------
	#         RoI ID      ||    Distance   ||  Error (or velocity)   ||   Motion
	# --------------------||---------------||------------------------||-----------
	# ----------|---------||--------|------||-------|-------|--------||-----------
	#       t-1 |       t || pixels | rank ||    dx |    dy |      e || is moving
	# ----------|---------||--------|------||-------|-------|--------||-----------
	  {rid_t-1} | {rid_t} || {dist} |  {k} ||  {dx} |  {dy} |    {e} ||      {mov}

Each line corresponds to an association between one RoI at :math:`t - 1` and at
:math:`t`:

- ``{rid_t-1}``: id of the |RoI| in the table 1 (in the :math:`t - 1` frame),
- ``{rid_t}`` : id of the |RoI| in the table 2 (in the :math:`t` frame),
- ``{dist}``: distance in pixels between the two |RoIs|,
- ``{rank}``: rank in the |k-NN| algorithm, if 1: it means that this is the
  closest |RoI| association, if 2: it means that this is the second closest
  |RoI| association, etc.,
- ``{dx}``: :math:`x`
  distance between the estimated position (after motion estimation) and the
  real position (in frame :math:`t - 1`),
- ``{dy}``: :math:`y` distance between the estimated position (after motion
  estimation) and the real position (in frame :math:`t - 1`),
- ``{e}``: euclidean distance between the estimated position and the real
  position,
- ``{mov}``: ``yes`` if the |RoI| is moving, ``no`` otherwise. The criteria to
  detect the motion of an |RoI| is:
  :math:`|e - \bar{e^1_t}| > \sigma^1_t`, with :math:`e` the error of the
  current |RoI|, :math:`\bar{e^1_t}` the mean error after the first motion
  estimation and :math:`\sigma^1_t` the standard deviation after the first
  motion estimation.

If ``{mov}`` = ``yes`` then, ``{dx}``, ``{dy}`` is the velocity vector and
``{e}`` is the velocity norm in pixel.

.. note:: ``{dx}``, ``{dy}``, ``{e}`` and ``{mov}`` are computed after the
          second motion estimation.

Table 4: Motion Estimation Statistics
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: bash

	# ------------------------------------------------------||------------------------------------------------------
	#   First motion estimation (with all associated RoIs)  ||    Second motion estimation (exclude moving RoIs)
	# ------------------------------------------------------||------------------------------------------------------
	# ----------|----------|----------|----------|----------||----------|----------|----------|----------|----------
	#     theta |       tx |       ty | mean err |  std dev ||    theta |       tx |       ty | mean err |  std dev
	# ----------|----------|----------|----------|----------||----------|----------|----------|----------|----------
	   {theta1} |    {tx1} |    {ty1} |{mean_er1}|{std_dev1}|| {theta2} |    {tx2} |    {ty2} |{mean_er2}|{std_dev2}

There is only one line in this table. It represents the motion estimation
between frame :math:`t - 1` and frame :math:`t`:

- ``{theta}``: the estimated rotation angle between frame :math:`t` and frame
  :math:`t - 1`,
- ``{tx}`` and ``{ty}``: the estimated translation vector from frame :math:`t`
  to frame :math:`t - 1`,
- ``{mean_er}``: the mean error of the associated |RoIs|,
- ``{std_dev}``: the standard deviation of the associated |RoI| errors.

The first estimation considers all the associated |RoIs| while the second
estimation excludes the associated |RoIs| in movement. To be considered in
movement, an |RoI| has to verify the following condition:
:math:`|e - \bar{e^1_t}| > \sigma^1_t`, with :math:`e` the error of
the current |RoI|, :math:`\bar{e^1_t}` the mean error after the first motion
estimation and :math:`\sigma^1_t` the standard deviation after the first motion
estimation.

Table 5: List of Tracks
^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: bash

	# -----------------||---------------------------||---------------------------||---------||-------------------
	#       Track      ||           Begin           ||            End            ||  Object || Reason of changed
	# -----------------||---------------------------||---------------------------||---------||    state (from
	# -------|---------||---------|--------|--------||---------|--------|--------||---------||  meteor to noise
	#     Id |  State  || Frame # |      x |      y || Frame # |      x |      y ||    Type ||    object only)
	# -------|---------||---------|--------|--------||---------|--------|--------||---------||-------------------
	   {tid} | {state} ||  {fbeg} | {xbeg} | {ybeg} ||  {fend} | {xend} | {yend} || {otype} ||          {reason}

Most of the columns of this table have been described in the
:ref:`detect_stdout` section, here we focus only on extra columns:

- ``{state}``: current state of the track (in a finite state machine): can be
  ``updated``, ``lost`` or ``finished``. First, when a track is created, its
  state is ``updated``. Then, ``updated`` can become ``lost``. ``lost`` can
  become either ``finished`` or ``updated``. ``finished`` is a final state.
- ``{reason}``: reason of the classification from ``meteor`` to ``noise``.

.. _detect_vid-out-path:

``--vid-out-path``
------------------

   :Type: STRING
   :Default: [empty]
   :Example: ``--vid-out-path ~/Videos/out_video.mp4``

Output video path with the bounding boxes around the detected tracks (supports
also a path to a sequence of images ``path/basename_%05d.jpg``).

.. _detect_vid-out-play:

``--vid-out-play``
------------------

   :Type: BOOLEAN
   :Example: ``--vid-out-play``

Open an SDL window to show the output video of the detected tracks (shows the
bounding boxes around the detected tracks)

.. _detect_vid-out-id:

``--vid-out-id``
----------------

   :Type: BOOLEAN
   :Example: ``--vid-out-id``

Work only if :ref:`detect_vid-out-path` or :ref:`detect_vid-out-play` is set.
Display the track ids corresponding to the bounding boxes. Requires to link with
OpenCV library (``-DFMDT_OPENCV_LINK`` CMake option,
see :numref:`user_installation_cmake`).
