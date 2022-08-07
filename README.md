# FMDT -- *Fast Meteor Detection Toolbox*

## Contents

[1. Dependencies](#dependencies)  
[2. Compilation with CMake](#compilation-with-cmake)  
[3. User Documentation](#user-documentation)  
[3.1. Detection Executable](#detection-executable)  
[3.2. Visualization Executable](#visualization-executable)  
[3.3. Checking Executable](#checking-executable)  
[3.4. Max-reduction Executable](#max-reduction-executable)  
[3.5. Examples of use](#examples-of-use)  
[3.6. Input and Output Text Formats](#input-and-output-text-formats)  

## Dependencies

This project use `ffmpeg-io` and `nrc2` projects as submodules:

```bash
git submodule update --init --recursive
```

## Compilation with CMake

```bash
mkdir build
cd build
cmake ..
make -j4
```

Example of optimization flags:
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_FLAGS="-Wall -funroll-loops -fstrict-aliasing -march=native"
```

**Tips**: on Apple Silicon M1 CPUs and with Apple Clang, use `-mcpu=apple-m1` instead of `-march=native`.

The `CMake` file comes with several options:
 * `-DTAH_DETECT_EXE` [default=`ON` ] {possible:`ON`,`OFF`}: compile the detection chain executable.
 * `-DTAH_VISU_EXE`   [default=`ON` ] {possible:`ON`,`OFF`}: compile the visual tracking executable.
 * `-DTAH_CHECK_EXE`  [default=`ON` ] {possible:`ON`,`OFF`}: compile the check executable.
 * `-DTAH_MAXRED_EXE` [default=`ON` ] {possible:`ON`,`OFF`}: compile the max reduction executable.
 * `-DTAH_DEBUG`      [default=`OFF`] {possible:`ON`,`OFF`}: build the project using debugging prints: these additional prints will be output on `stderr` and prefixed by `(DBG)`.

## User Documentation

This project generates 4 different executables:
  - `meteor-detect`: meteors detection chain.
  - `meteor-visu`: visualization of the detected meteors.
  - `meteor-check`: validation of the detected meteors with the field truth.
  - `meteor-maxred`: max reduction of grayscale pixels on a video.

The next sub-sections describe *how to use* the generated executables.

### Detection Executable

The meteors detection chain is located here: `./exe/meteor-detect`.

The list of available arguments:

| **Argument**      | **Type** | **Default** | **Req** | **Description** |
| :---              | :---     | :---        | :---    | :--- |
| `--input-video`   | str      | None        | Yes     | Input video path where we want to detect meteors. |
| `--output-bb`     | str      | None        | No      | Path to the bounding boxes file required by `meteor-visu` to draw detection rectangles. |
| `--output-frames` | str      | None        | No      | Path of the output frames for debug (PPM format). |
| `--output-stats`  | str      | None        | No      | TODO. |
| `--start-frame`   | int      | 0           | No      | First frame id to start the detection in the video sequence. |
| `--end-frame`     | int      | 200000      | No      | Last frame id to stop the detection in the video sequence. |
| `--skip-frames`   | int      | 0           | No      | Number of frames to skip. |
| `--light-min`     | int      | 55          | No      | Minimum light intensity hysteresis threshold (grayscale [0;255]). |
| `--light-max`     | int      | 80          | No      | Maximum light intensity hysteresis threshold (grayscale [0;255]). |
| `--surface-min`   | int      | 3           | No      | Minimum surface of the CCs in pixel. |
| `--surface-max`   | int      | 1000        | No      | Maximum surface of the CCs in pixel. |
| `-k`              | int      | 3           | No      | Number of neighbors in the k-nearest neighbor matching (KPPV algorithm). |
| `--r-extrapol`    | int      | 5           | No      | Search radius for CC extrapolation (piece-wise tracking). |
| `--d-line`        | int      | 25          | No      | Approximation factor of the rectilinear trajectory of meteors. |
| `--diff-deviaton` | float    | 4.0         | No      | Multiplication factor of the standard deviation (CC error has to be higher than `diff deviation` x `standard deviation` to be considered in movement). |
| `--track-all`     | bool     | -           | No      | By default the program only tracks `meteor` object type. If `--track-all` is set, all object types are tracked (`meteor`, `star` or `noise`). |
| `--frame-star`    | int      | 3           | No      | Minimum number of frames required to track a star. |

Output text formats are detailed in the [Input and Output Text Formats](#input-and-output-text-formats) section.

### Visualization Executable

The meteors visualization program is located here: `./exe/meteor-visu`.

The list of available arguments:

| **Argument**      | **Type** | **Default**    | **Req** | **Description** |
| :---              | :---     | :---           | :---    | :--- |
| `--input-video`   | str      | None           | Yes     | Input video path. |
| `--input-tracks`  | str      | None           | Yes     | The track file corresponding to the input video (generated from `meteor-detect`). |
| `--input-bb`      | str      | None           | Yes     | The bounding boxes file corresponding to the input video (generated from `meteor-detect`). |
| `--output-video`  | str      | "out_visu.mp4" | No      | Path of the output video (MPEG-4 format) with meteor tracking colored rectangles. If `--validation` is set then the bounding rectangles are red if *false positive* and green if *true positive*. If `--validation` is NOT set then the bounding rectangles are levels of green depending on the detection confidence. |
| `--output-frames` | str      | None           | No      | Path of the output frames for debug (PPM format). |
| `--validation`    | str      | None           | No      | File containing the ground truth. |

**Note**: to run `meteor-visu`, it is required to run `meteor-detect` before and on the same input video. This will generate the required `tracks.txt` and `bounding_box.txt` files.

Input text formats are detailed in the [Input and Output Text Formats](#input-and-output-text-formats) section.

### Checking Executable

The meteors checking program is located here: `./exe/meteor-check`.

The list of available arguments:

| **Argument**     | **Type** | **Default** | **Req** | **Description** |
| :---             | :---     | :---        | :---    | :--- |
| `--input-tracks` | str      |  None       | Yes     | The track file corresponding to the input video (generated from `meteor-detect`). |
| `--validation`   | str      |  None       | Yes     | File containing the ground truth. |

**Note**: to run `meteor-check`, it is required to run `meteor-detect` before. This will generate the required `tracks.txt` file.

Input/output text formats are detailed in the [Input and Output Text Formats](#input-and-output-text-formats) section.

### Max-reduction Executable

The max-reduction generation program is located here: `./exe/meteor-maxred`.

The list of available arguments:

| **Argument**     | **Type** | **Default** | **Req** | **Description** |
| :---             | :---     | :---        | :---    | :--- |
| `--input-video`  | str      |  None       | Yes     | Input video path. |
| `--output-frame` | str      |  None       | Yes     | Path of the output frame (PGM format). |
| `--start-frame`  | int      | 0           | No      | First frame id to start the max-reduction in the video sequence. |
| `--end-frame`    | int      | 200000      | No      | Last frame id to stop the max-reduction in the video sequence. |

### Examples of use

Download a video sequence containing meteors here: https://lip6.fr/adrien.cassagne/data/tauh/in/2022_05_31_tauh_34_meteors.mp4.

#### Step 1: Meteors detection

```shell
./exe/meteor-detect --input-video ./2022_05_31_tauh_34_meteors.mp4
```

Write tracks and bounding boxes into text files for `meteor-visu` and `meteor-check`:

```shell
./exe/meteor-detect --input-video ./2022_05_31_tauh_34_meteors.mp4 --output-bb ./out_detect_bb.txt > ./out_detect_tracks.txt
```

#### Step 2: Visualization

Visualization **WITHOUT** ground truth:

```shell
./exe/meteor-visu --input-video ./2022_05_31_tauh_34_meteors.mp4 --input-tracks ./out_detect_tracks.txt --input-bb ./out_detect_bb.txt
```

Visualization **WITH** ground truth:

```shell
./exe/meteor-visu --input-video ./2022_05_31_tauh_34_meteors.mp4 --input-tracks ./out_detect_tracks.txt --input-bb ./out_detect_bb.txt --validation ../validation/2022_05_31_tauh_34_meteors.txt
```

**Note**: by default, the resulting video will be written in the `./out_visu.mp4` file (this behavior can be overloaded with the `--output-video` argument).

#### Step 3: Offline checking

Use `meteor-check` with the following arguments:

```shell
./exe/meteor-check --input-tracks ./out_detect_tracks.txt --validation ../validation/2022_05_31_tauh_34_meteors.txt
```

#### Step 4: Max reduction

Use `meteor-maxred` with the following arguments:

```shell
./exe/meteor-maxred --input-video ./2022_05_31_tauh_34_meteors.mp4 --output-frame out_maxred.pgm
```

### Input and Output Text Formats

This section details the various text formats used by the toolchain.
For each text format, the `#` character can be used for comments (at the beginning of a new line).

#### Tracks: `stdout` of `meteor-detect` / `--input-tracks` in `meteor-visu` and `meteor-check`

The tracks represent the detected objects in the video sequence.

```
# -------||---------------------------||---------------------------||---------
#  Track ||           Begin           ||            End            ||  Object
# -------||---------------------------||---------------------------||---------
# -------||---------|--------|--------||---------|--------|--------||---------
#     Id || Frame # |      x |      y || Frame # |      x |      y ||    Type
# -------||---------|--------|--------||---------|--------|--------||---------
    {id} ||  {fbeg} | {xbeg} | {ybeg} ||  {fend} | {xend} | {yend} || {otype}
```

* `{id}`: a positive integer value representing a unique track identifier.
* `{fbeg}`: a positive integer value representing the first frame in the video sequence when the track is detected.
* `{xbeg}`: a positive real value of the x-axis coordinate (beginning of the track).
* `{ybeg}`: a positive real value of the y-axis coordinate (beginning of the track).
* `{fend}`: a positive integer value representing the last frame in the video sequence when the track is detected.
* `{xend}`: a positive real value of the x-axis coordinate (end of the track).
* `{yend}`: a positive real value of the y-axis coordinate (end of the track).
* `{otype}`: a string of the object type, can be: `meteor`, `star` or `noise`.

#### Bounding Boxes: `--output-bb` in `meteor-detect` / `--input-bb` in `meteor-visu`

The bounding boxes can be output by `meteor-detect` (with the `--output-bb` argument) and are required by `meteor-visu`.
Each bounding box defines the area of an object, frame by frame.

Here is the corresponding line format:
```
{frame_id} {x_radius} {y_radius} {center_x} {center_y}
```
Each line corresponds to a frame and to an object, each value is separated by a space character.

#### Ground Truth: `--validation` in `meteor-visu` & `meteor-check`

Ground truth file gives objects positions over time. Here is the expected text format of a line:

```
{object_type} {begin_frame} {begin_x} {begin_y} {end_frame} {end_x} {end_y}
```

`{object_type}` can be `meteor`, `star` or `noise`.
`{begin_frame}`, `{begin_x}`, `{begin_y}`, `{end_frame}`, `{end_x}`, `{end_y}` are positive integers.
Each line corresponds to an object and each value is separated by a space character.

#### Check Report: `stdout` in `meteor-check`

The first part of `meteor-check` `stdout` is a table where each entry corresponds to an object of the ground truth (GT):

```
# ---------------||---------------||-----------------||--------
#    GT Object   ||      Hits     ||    GT Frames    || Tracks
# ---------------||---------------||-----------------||--------
# -----|---------||--------|------||--------|--------||--------
#   Id |    Type || Detect |  GT  ||  Start |  Stop  ||      #
# -----|---------||--------|------||--------|--------||--------
 {oid} | {otype} ||   {dh} | {gh} || {staf} | {stof} ||   {nt}
```

* `{oid}`: a positive integer value representing a unique identifier of ground truth object.
* `{otype}`: a string of the object type, can be: `meteor`, `star` or `noise`.
* `{dh}`: a positive integer value of the number of frames when the object is detected (from the tracks, `--input-tracks`).
* `{gh}`: a positive integer value of the number of frame when the object is present (from the ground truth, `--validation`).
* `{staf}`: a positive integer value of the frame start (from the ground truth, `--validation`).
* `{stof}`: a positive integer value of the frame stop (from the ground truth, `--validation`).
* `{nt}`: a positive integer value of the number of tracks that match the ground truth object.

In a second part, `meteor-check` `stdout` gives some statistics in the following format (`{pi}` stands for *positive integer* and `{pf}` for *positive float*):

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
* `Number of tracks`: the number of objects from the tracks (`meteor-detect` output).
* `True positives`: number of detected objects that are in the ground truth (with the same type).
* `False positives`: number of detected objects that are not in the ground truth (or that have a different type).
* `True negative`: number of detected objects that are different from the current type of object. For instance, if we focus on `meteor` object type, the number of false negatives is the sum of all the objects in the tracks that are `star` or `noise`.
* `False negative`: number of non-detected objects (present in the ground truth and not present in the tracks).
* `Tracking rate`: the sum of detected hits on the sum of the ground truth hits. Range is between 1 (perfect tracking) and 0 (nothing is tracked). When there are more hits in a track than in the ground truth, the detected hits are the ground truth hits minus the extra hits of the track.

For each line, the `meteor`, `star` and `noise` object types are considered.
`all` stands for all types, sometime `all` can be mean-less.
