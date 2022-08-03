# Meteors Detection Project

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
 * `-DTAH_DEBUG`      [default=`OFF`] {possible:`ON`,`OFF`}: build the project using debugging code.

## Short User Documentation

This project generates 4 different executables:
  - `meteor-detect`: meteors detection chain.
  - `meteor-visu`: visualization of the detected meteors.
  - `meteor-check`: validation of the detected meteors with the field truth.
  - `meteor-maxred`: max reduction of grayscale pixels on a video.

The next sub-sections describe *how to use* the generated executables.

### Detection with `meteor-detect`

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


### Visualization with `meteor-visu`

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

### Checking with `meteor-check`

The meteors checking program is located here: `./exe/meteor-check`.

The list of available arguments:

| **Argument**     | **Type** | **Default** | **Req** | **Description** |
| :---             | :---     | :---        | :---    | :--- |
| `--input-tracks` | str      |  None       | Yes     | The track file corresponding to the input video (generated from `meteor-detect`). |
| `--validation`   | str      |  None       | Yes     | File containing the ground truth. |

**Note**: to run `meteor-check`, it is required to run `meteor-detect` before. This will generate the required `tracks.txt` file.

### Generating a max-reduction image with `meteor-maxred`

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
