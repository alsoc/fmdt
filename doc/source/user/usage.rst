.. _user_executables_usage:

*****************
Executables Usage
*****************

This project generates different executables:
  - ``fmdt-detect`` (and ``fmdt-detect-rt*`` if ``-DFMDT_AFF3CT_RUNTIME`` is set
    to ``ON``): meteors detection chain.
  - ``fmdt-visu``: visualization of the detected meteors.
  - ``fmdt-check``: validation of the detected meteors with the field truth.
  - ``fmdt-maxred``: max reduction of grayscale pixels on a video.

The next sections describe *how to use* the generated executables.

Detection Executable Parameters
"""""""""""""""""""""""""""""""

The meteors detection chain is located here: ``./exe/fmdt-detect``

The list of available arguments:

| **Argument**       | **Deprecated**     | **Type** | **Default** | **Req** | **Description** |
| :---               | :---               | :---     | :---        | :---    | :--- |
| `--vid-in-path`    | `--in-video`       | str      | None        | Yes     | Input video path (supports also a path to a sequence of images `path/basename_%05d.jpg`). |
| `--vid-in-start`   | `--fra-start`      | int      | 0           | No      | First frame id (included) to start the detection in the video sequence. |
| `--vid-in-stop`    | `--fra-end`        | int      | 0           | No      | Last frame id (included) to stop the detection in the video sequence. If set to 0, read entire video. |
| `--vid-in-skip`    | `--fra-skip`       | int      | 0           | No      | Number of frames to skip. |
| `--vid-in-buff`    | `--video-buff`     | bool     | -           | No      | Bufferize all the video in global memory before executing the chain. |
| `--vid-in-loop`    | `--video-loop`     | int      | 1           | No      | Number of times the video is read in loop. |
| `--vid-in-threads` | `--ffmpeg-threads` | int      | 0           | No      | Select the number of threads to use to decode video input (in `ffmpeg`). If set to 0, `ffmpeg` chooses the number of threads automatically. |
| `--ccl-hyst-lo`    | `--light-min`      | int      | 55          | No      | Minimum light intensity for hysteresis threshold (grayscale [0;255]). |
| `--ccl-hyst-hi`    | `--light-max`      | int      | 80          | No      | Maximum light intensity for hysteresis threshold (grayscale [0;255]). |
| `--ccl-fra-path`   | `--out-frames`     | str      | None        | No      | Path of the files for CC debug (`path/cc_%05d.png`). |
| `--ccl-fra-id`     | `--show-id`        | bool     | -           | No      | Show the RoI/CC ids on the output frames (to combine with `--ccl-fra-path` parameter). Requires to link with OpenCV library (`-DFMDT_OPENCV_LINK` CMake option). |
| `--mrp-s-min`      | `--surface-min`    | int      | 3           | No      | Minimum surface of the CCs in pixels. |
| `--mrp-s-max`      | `--surface-max`    | int      | 1000        | No      | Maximum surface of the CCs in pixels. |
| `--knn-k`          | `-k`               | int      | 3           | No      | Maximum number of neighbors considered in k-nearest neighbor matching (k-NN algorithm). |
| `--knn-d`          | `--max-dist`       | int      | 10          | No      | Maximum distance in pixels between two images (k-NN algorithm). |
| `--knn-s`          | `--min-ratio-s`    | int      | 0.125       | No      | Minimum surface ratio to match two CCs in k-NN (0 matches alls, 1 matches nothing). This parameter is also used for extrapolation in the tracking. |
| `--trk-ext-d`      | `--r-extrapol`     | int      | 10          | No      | Search radius in pixels for CC extrapolation (piece-wise tracking). |
| `--trk-ext-o`      | `--extrapol-order` | int      | 3           | No      | Maximum number of frames to extrapolate for lost objects (linear extrapolation). |
| `--trk-angle`      | `--angle-max`      | float    | 20.0        | No      | Tracking max angle between two meteors at t-1 and t (in degree). |
| `--trk-star-min`   | `--fra-star-min`   | int      | 15          | No      | Minimum number of frames required to track a star. |
| `--trk-meteor-min` | `--fra-meteor-min` | int      | 3           | No      | Minimum number of frames required to track a meteor. |
| `--trk-meteor-max` | `--fra-meteor-max` | int      | 100         | No      | Maximum number of frames required to track a meteor. |
| `--trk-ddev`       | `--diff-dev`       | float    | 4.0         | No      | Multiplication factor of the standard deviation (CC error has to be higher than `ddev` x `stddev` to be considered in movement). |
| `--trk-all`        | `--track-all`      | bool     | -           | No      | By default the program only tracks `meteor` object type. If `--track-all` is set, all object types are tracked (`meteor`, `star` or `noise`). |
| `--trk-bb-path`    | `--out-bb`         | str      | None        | No      | Path to the bounding boxes file required by `fmdt-visu` to draw detection rectangles. |
| `--trk-mag-path`   | `--out-mag`        | str      | None        | No      | Path to the output file containing magnitudes of the tracked objects. |
| `--log-path`       | `--out-stats`      | str      | None        | No      | Path of the output statistics, only required for debugging purpose. |

Output text formats are detailed in the
[Input and Output Text Formats](#input-and-output-text-formats) section.

Visualization Executable Parameters
"""""""""""""""""""""""""""""""""""

The meteors visualization program is located here: `./exe/fmdt-visu`.

The list of available arguments:

| **Argument**        | **Deprecated**     | **Type** | **Default**    | **Req** | **Description** |
| :---                | :---               | :---     | :---           | :---    | :--- |
| `--vid-in-path`     | `--in-video`       | str      | None           | Yes     | Input video path (supports also a path to a sequence of images `path/basename_%05d.png`). |
| `--vid-in-start`    | `--fra-start`      | int      | 0              | No      | First frame id (included) to start the visualization in the video sequence. |
| `--vid-in-stop`     | `--fra-end`        | int      | 0              | No      | Last frame id (included) to stop the visualization in the video sequence. If set to 0, read entire video. |
| `--vid-in-threads`  | `--ffmpeg-threads` | int      | 0              | No      | Select the number of threads to use to decode video input (in `ffmpeg`). If set to 0, `ffmpeg` chooses the number of threads automatically. |
| `--trk-path`        | `--in-tracks`      | str      | None           | Yes     | The tracks file corresponding to the input video (generated from `fmdt-detect`). |
| `--trk-bb-path`     | `--in-bb`          | str      | None           | Yes     | The bounding boxes file corresponding to the input video (generated from `fmdt-detect`). |
| `--trk-id`          | `--show-id`        | bool     | -              | No      | Show the object ids on the output video and frames. Requires to link with OpenCV library (`-DFMDT_OPENCV_LINK` CMake option). |
| `--trk-nat-num`     | `--nat-num`        | bool     | -              | No      | Natural numbering of the object ids, work only if `--trk-id` is set. |
| `--trk-only-meteor` | `--only-meteor`    | bool     | -              | No      | Show only meteors. |
| `--gt-path`         | `--in-gt`          | str      | None           | No      | File containing the ground truth. |
| `--vid-out-path`    | `--out-video`      | str      | None           | No      | Path of the output video (supports also a path to a sequence of images `path/basename_%05d.jpg`) with meteor tracking colored rectangles. If `--gt-path` is set then the bounding rectangles are red if *false positive* meteor and green if *true positive* meteor. |

**Note**: to run `fmdt-visu`, it is required to run `fmdt-detect` before and on
the same input video. This will generate the required `tracks.txt` and
`bounding_box.txt` files.

Input text formats are detailed in the
[Input and Output Text Formats](#input-and-output-text-formats) section.

Checking Executable Parameters
""""""""""""""""""""""""""""""

The meteors checking program is located here: `./exe/fmdt-check`.

The list of available arguments:

| **Argument** | **Deprecated** | **Type** | **Default** | **Req** | **Description** |
| :---         | :---           | :---     | :---        | :---    | :--- |
| `--trk-path` | `--in-tracks`  | str      | None        | Yes     | The track file corresponding to the input video (generated from `fmdt-detect`). |
| `--gt-path`  | `--in-gt`      | str      | None        | Yes     | File containing the ground truth. |

**Note**: to run `fmdt-check`, it is required to run `fmdt-detect` before. This
will generate the required `tracks.txt` file.

Input/output text formats are detailed in the
[Input and Output Text Formats](#input-and-output-text-formats) section.

Max-reduction Executable Parameters
"""""""""""""""""""""""""""""""""""

The max-reduction generation program is located here: `./exe/fmdt-maxred`.

The list of available arguments:

| **Argument**        | **Deprecated**     | **Type** | **Default** | **Req** | **Description** |
| :---                | :---               | :---     | :---        | :---    | :--- |
| `--vid-in-path`     | `--in-video`       | str      | None        | Yes     | Input video path (supports also a path to a sequence of images `path/basename_%05d.png`). |
| `--vid-in-start`    | `--fra-start`      | int      | 0           | No      | First frame id (included) to start the max-reduction in the video sequence. |
| `--vid-in-stop`     | `--fra-end`        | int      | 0           | No      | Last frame id (included) to stop the max-reduction in the video sequence. If set to 0, read entire video. |
| `--vid-in-threads`  | `--ffmpeg-threads` | int      | 0           | No      | Select the number of threads to use to decode video input (in `ffmpeg`). If set to 0, `ffmpeg` chooses the number of threads automatically. |
| `--trk-path`        | `--in-tracks`      | str      | None        | No      | The tracks file corresponding to the input video (generated from `fmdt-detect`). |
| `--trk-id`          | `--show-id`        | bool     | -           | No      | Show the object ids on the output video and frames, works only if `--trk-path` is set. Requires to link with OpenCV library (`-DFMDT_OPENCV_LINK` CMake option). |
| `--trk-nat-num`     | `--nat-num`        | bool     | -           | No      | Natural numbering of the object ids, works only if `--trk-id` is set. |
| `--trk-only-meteor` | `--only-meteor`    | bool     | -           | No      | Show only meteors. |
| `--gt-path`         | `--in-gt`          | str      | None        | No      | File containing the ground truth. |
| `--fra-out-path`    | `--out-frame`      | str      | None        | Yes     | Path of the output frame. |

Input and Output Text Formats
"""""""""""""""""""""""""""""

This section details the various text formats used by the toolchain.
For each text format, the `#` character can be used for comments (at the
beginning of a new line).

Tracks: `stdout` of `fmdt-detect` / `--trk-path` in `fmdt-visu` and `fmdt-check`
--------------------------------------------------------------------------------

The tracks represent the detected objects in the video sequence.

```
# -------||---------------------------||---------------------------||---------
#  Track ||           Begin           ||            End            ||  Object
# -------||---------------------------||---------------------------||---------
# -------||---------|--------|--------||---------|--------|--------||---------
#     Id || Frame # |      x |      y || Frame # |      x |      y ||    Type
# -------||---------|--------|--------||---------|--------|--------||---------
   {tid} ||  {fbeg} | {xbeg} | {ybeg} ||  {fend} | {xend} | {yend} || {otype}
```

* `{tid}`: a positive integer (start to 1) value representing a unique track
  identifier.
* `{fbeg}`: a positive integer value representing the first frame in the video
  sequence when the track is detected.
* `{xbeg}`: a positive real value of the x-axis coordinate (beginning of the
  track).
* `{ybeg}`: a positive real value of the y-axis coordinate (beginning of the
  track).
* `{fend}`: a positive integer value representing the last frame in the video
  sequence when the track is detected.
* `{xend}`: a positive real value of the x-axis coordinate (end of the track).
* `{yend}`: a positive real value of the y-axis coordinate (end of the track).
* `{otype}`: a string of the object type, can be: `meteor`, `star` or `noise`.

Bounding Boxes: `--trk-bb-path` in `fmdt-detect` and `fmdt-visu`
----------------------------------------------------------------

The bounding boxes can be output by `fmdt-detect` (with the `--trk-bb-path`
argument) and are required by `fmdt-visu`. Each bounding box defines the area of
an object, frame by frame.

Here is the corresponding line format:
```
{frame_id} {x_radius} {y_radius} {center_x} {center_y} {track_id} {is_extrapolated}
```
Each line corresponds to a frame and to an object, each value is separated by a
space character.

#### Magnitudes: `--trk-mag-path` in `fmdt-detect`

The magnitudes can be output by `fmdt-detect` (with the `--trk-mag-path`
argument) and can be used for astrophotometry. For instance they are used as
input in [pyFMDT](pyFMDT/README.md).

Each line corresponds to a track/object and here is the corresponding line
format:
```
{tid} {otype} {mag1} {mag2} {...} {magn}
```

`mag1` is the first magnitude value of the track/object of `tid` id. `mag2` is
the second magnitude value (in the second frame where the object has been
tracked). And so on, until the last magnitude value `magn`. Note that sometime
the magnitude value can be `0`, it means that the object has been extrapolated
on this frame, thus the magnitude cannot be computed.

Ground Truth: `--gt-path` in `fmdt-visu`, `fmdt-check` & `fmdt-maxred`
----------------------------------------------------------------------

Ground truth file gives objects positions over time. Here is the expected text
format of a line:

```
{otype} {fbeg} {xbeg} {ybeg} {fend} {xend} {yend}
```

`{otype}` can be `meteor`, `star` or `noise`.
`{fbeg}` and `{fend}` stand for `frame begin` and `frame end`.
`{xbeg}` and `{ybeg}` stand for `x` and `y` coordinates of the `frame begin`.
`{xend}` and `{yend}` stand for `x` and `y` coordinates of the `frame end`.
`{fbeg}`, `{xbeg}`, `{ybeg}`, `{fend}`, `{xend}`, `{yend}` are positive
integers. Each line corresponds to an object and each value is separated by a
space character.

Check Report: `stdout` in `fmdt-check`
--------------------------------------

The first part of `fmdt-check` `stdout` is a table where each entry corresponds
to an object of the ground truth (GT):

```
# ---------------||---------------||-----------------||--------
#    GT Object   ||      Hits     ||    GT Frames    || Tracks
# ---------------||---------------||-----------------||--------
# -----|---------||--------|------||--------|--------||--------
#   Id |    Type || Detect |  GT  ||  Start |  Stop  ||      #
# -----|---------||--------|------||--------|--------||--------
 {tid} | {otype} ||   {dh} | {gh} || {staf} | {stof} ||   {nt}
```

* `{tid}`: a positive integer value representing a unique identifier of ground
  truth track/object.
* `{otype}`: a string of the object type, can be: `meteor`, `star` or `noise`.
* `{dh}`: a positive integer value of the number of frames when the object is
  detected (from the tracks, `--trk-path`).
* `{gh}`: a positive integer value of the number of frame when the object is
  present (from the ground truth, `--gt-path`).
* `{staf}`: a positive integer value of the frame start (from the ground truth,
  `--gt-path`).
* `{stof}`: a positive integer value of the frame stop (from the ground truth,
  `--gt-path`).
* `{nt}`: a positive integer value of the number of tracks that match the ground
  truth object.

In a second part, `fmdt-check` `stdout` gives some statistics in the following
format (`{pi}` stands for *positive integer* and `{pf}` for *positive float*):

```
Statistics:
  - Number of GT objs = ['meteor': {pi}, 'star': {pi}, 'noise': {pi}, 'all': {pi}]
  - Number of tracks  = ['meteor': {pi}, 'star': {pi}, 'noise': {pi}, 'all': {pi}]
  - True positives    = ['meteor': {pi}, 'star': {pi}, 'noise': {pi}, 'all': {pi}]
  - False positives   = ['meteor': {pi}, 'star': {pi}, 'noise': {pi}, 'all': {pi}]
  - True negative     = ['meteor': {pi}, 'star': {pi}, 'noise': {pi}, 'all': {pi}]
  - False negative    = ['meteor': {pi}, 'star': {pi}, 'noise': {pi}, 'all': {pi}]
  - Tracking rate     = ['meteor': {pf}, 'star': {pf}, 'noise': {pf}, 'all': {pf}]
```

* `Number of GT objs`: the number of objects from the ground truth.
* `Number of tracks`: the number of objects from the tracks (`fmdt-detect`
  output).
* `True positives`: number of detected objects that are in the ground truth
  (with the same type).
* `False positives`: number of detected objects that are not in the ground truth
  (or that have a different type).
* `True negative`: number of detected objects that are different from the
  current type of object. For instance, if we focus on `meteor` object type, the
  number of false negatives is the sum of all the objects in the tracks that are
  `star` or `noise`.
* `False negative`: number of non-detected objects (present in the ground truth
  and not present in the tracks).
* `Tracking rate`: the sum of detected hits on the sum of the ground truth hits.
  Range is between 1 (perfect tracking) and 0 (nothing is tracked). When there
  are more hits in a track than in the ground truth, the detected hits are the
  ground truth hits minus the extra hits of the track.

For each line, the `meteor`, `star` and `noise` object types are considered.
`all` stands for all types, sometime `all` can be mean-less.

Logs / Statistics: `--log-path` in `fmdt-detect`
------------------------------------------------

**This section targets advanced users, some knowledge about the implemented
algorithms may be required!! You have been warned ;-).**

`fmdt-detect` comes with the `--log-path` option to help to understand what is
happening during the execution.
This option enables to log internal statistics of the different algorithms used
to detect meteors.

The folder contains multiple files, one per frame. For instance, the file name
for the frame n°12 is: `00012.txt`. Each file contains 5 different tables:
  - Table 1: list of Regions Of Interest (RoIs) at `t - 1` (result of the
    CCL/CCA + hysteresis algorithm at `t - 1`)
  - Table 2: list of Regions Of Interest (RoIs) at `t` (result of the CCL/CCA +
    hysteresis algorithm at `t`)
  - Table 3: list of associations between `t - 1` RoIs and `t` RoIs (result of
    the k-NN algorithm) + errors/velocities after motion estimation
  - Table 4: motion estimation statistics between `t - 1` and `t` frame
  - Table 5: list of tracks since the beginning of the execution (final output
    of the detection chain)

Note that the first log file (usally named `00000.txt`) only contains the
table 2. This is normal because algorithms stating from k-NN require two
consecutive frames to work.

Table 1 and table 2: Regions of Interest (RoIs)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

```
# ------||----------------||---------------------------||---------------------------||-------------------||-----------
#   RoI ||      Track     ||        Bounding Box       ||   Surface (S in pixels)   ||      Center       || Magnitude
# ------||----------------||---------------------------||---------------------------||-------------------||-----------
# ------||------|---------||------|------|------|------||-----|----------|----------||---------|---------||-----------
#    ID ||   ID |    Type || xmin | xmax | ymin | ymax ||   S |       Sx |       Sy ||       x |       y ||        --
# ------||------|---------||------|------|------|------||-----|----------|----------||---------|---------||-----------
  {rid} || {tid}| {otype} ||{xmin}|{xmax}|{ymin}|{ymax}|| {S} |     {Sx} |     {Sy} ||    {cx} |    {cy} ||      {mag}
```

Each line corresponds to one region of interest (RoI) :
  - `{rid}`: unique identifier for the current RoI (start to 1)
  - `{tid}`: unique identifier of the corresponding track (start to 1), can be
    empty if no track is associated to the current RoI
  - `{otype}`: type of the track object (`meteor`, `noise` or `star`), only if
    there is a track corresponding to this RoI
  - `{xmin}`: minimum x position of the bounding box
  - `{xmax}`: maximum x position of the bounding box
  - `{ymin}`: minimum y position of the bounding box
  - `{ymax}`: maximum y position of the bounding box
  - `{S}`: surface (area) of the RoI in pixels
  - `{Sx}`: sum of x properties
  - `{Sy}`: sum of y properties
  - `{cx}`: x center of mass
  - `{cy}`: y center of mass
  - `{mag}`: magnitude of the current RoI (accumulated brightness of the RoI)

Table 3: List of associations between RoIs
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

```
# --------------------||---------------||------------------------||-----------
#         RoI ID      ||    Distance   ||  Error (or velocity)   ||   Motion
# --------------------||---------------||------------------------||-----------
# ----------|---------||--------|------||-------|-------|--------||-----------
#       t-1 |       t || pixels | rank ||    dx |    dy |      e || is moving
# ----------|---------||--------|------||-------|-------|--------||-----------
  {rid_t-1} | {rid_t} || {dist} |  {k} ||  {dx} |  {dy} |    {e} ||      {mov}
```

Each line corresponds to an association between one RoI at `t - 1` and at `t`:
  - `{rid_t-1}`: id of the RoI in the table 1 (in the `t - 1` frame)
  - `{rid_t}` : id of the RoI in the table 2 (in the `t` frame)
  - `{dist}`: distance in pixels between the two RoIs
  - `{rank}`: rank in the k-NN algorithm, if 1: it means that this is the
    closest RoI asso., if 2: it means that this is the second closest RoI asso.,
    etc.
  - `{dx}`: x distance between the estimated position (after motion estimation)
    and the real position (in frame `t - 1`)
  - `{dy}`: y distance between the estimated position (after motion estimation)
    and the real position (in frame `t - 1`)
  - `{e}`: euclidean distance between the estimated position and the real
    position
  - `{mov}`: `yes` if the RoI is moving, `no` otherwise. The criteria to detect
    the motion of an RoI is: abs(`{e}` - `{mean_err1}`) > `{std_dev1}`

If `{mov}` = `yes` then, `{dx}`,`{dy}` is the velocity vector and `{e}` is the
velocity norm in pixel.

**Note that `{dx}`, `{dy}`, `{e}` and `{mov}` are computed after the second
motion estimation.**

Table 4: Motion Estimation Statistics
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

```
# ------------------------------------------------------||------------------------------------------------------
#   First motion estimation (with all associated RoIs)  ||    Second motion estimation (exclude moving RoIs)
# ------------------------------------------------------||------------------------------------------------------
# ----------|----------|----------|----------|----------||----------|----------|----------|----------|----------
#     theta |       tx |       ty | mean err |  std dev ||    theta |       tx |       ty | mean err |  std dev
# ----------|----------|----------|----------|----------||----------|----------|----------|----------|----------
   {theta1} |    {tx1} |    {ty1} |{mean_er1}|{std_dev1}|| {theta2} |    {tx2} |    {ty2} |{mean_er2}|{std_dev2}
```

There is only one line in this table. It represents the motion estimation
between frame `t - 1` and frame `t`:
  - `{theta}`: the estimated rotation angle between frame `t` and frame
    `t - 1`
  - `{tx}` and `{ty}`: the estimated translation vector from frame `t` to frame
    `t - 1`
  - `{mean_er}`: the mean error of the associated RoIs
  - `{std_dev}`: the standard deviation of the associated RoI errors

The first estimation considers all the associated RoIs while the second
estimation excludes the associated RoIs in movement. To be considered in
movement, an RoI has to verify the following condition:
abs(`{e}` - `{mean_er1}`) > `{std_dev1}`, with `{e}` the error of the current
RoI.

Table 5: List of Tracks
^^^^^^^^^^^^^^^^^^^^^^^

```
# -------||---------------------------||---------------------------||---------||-------------------
#  Track ||           Begin           ||            End            ||  Object || Reason of changed
# -------||---------------------------||---------------------------||---------||    state (from
# -------||---------|--------|--------||---------|--------|--------||---------||  meteor to noise
#     Id || Frame # |      x |      y || Frame # |      x |      y ||    Type ||    object only)
# -------||---------|--------|--------||---------|--------|--------||---------||-------------------
   {tid} ||  {fbeg} | {xbeg} | {ybeg} ||  {fend} | {xend} | {yend} || {otype} ||          {reason}
```

Most of the columns of this table have been described in a previous section,
here we focus only on extra columns:
  - `{reason}`: reason of the classification from `meteor` to `noise`
