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
 * `-DTAH_DETECT_EXE` [default=`ON`] {possible:`ON`,`OFF`}: compile the detection chain executable.
 * `-DTAH_VISU_EXE`   [default=`ON`] {possible:`ON`,`OFF`}: compile the visual tracking executable.
 * `-DTAH_CHECK_EXE`  [default=`ON`] {possible:`ON`,`OFF`}: compile the check executable.

## Short User Documentation

This project generates 3 different executables:
  - `meteor-detect`: meteors detection chain.
  - `meteor-visu`: visualization of the detected meteors.
  - `meteor-check`: validation of the detected meteors with the field truth.

The next sub-sections describe *how to use* the generated executables.

### Detection with `meteor-detect`

The meteors detection chain is located here: `./exe/meteor-detect`.

The list of available arguments:

| **Argument**     | **Type** | **Default** | **Required** | **Description** |
| :---             | :---     | :---        | :---         | :--- |
| `-input_video`   | str      | None        | Yes          | Input video path where we want to detect meteors. |
| `-output_frames` | str      | None        | No           | Path of the output frames for debug. |
| `-output_tracks` | str      | "./debug"   | No           | Path of the output tracks (the list of the detected meteors are in `tracks.txt` and additional statistics are saved in `bounding_box.txt`). |
| `-start_frame`   | int      | 0           | No           | First frame id to start the detection in the video sequence. |
| `-end_frame`     | int      | 200000      | No           | Last frame id to stop the detection in the video sequence. |
| `-skip_frames`   | int      | 0           | No           | Number of frames to skip. |
| `-light_min`     | int      | 55          | No           | Minimum light intensity hysteresis threshold. |
| `-light_max`     | int      | 80          | No           | Maximum light intensity hysteresis threshold. |
| `-surface_min`   | int      | 3           | No           | Minimum surface of the CCs in pixel. |
| `-surface_max`   | int      | 1000        | No           | Maximum surface of the CCs in pixel. |
| `-k`             | int      | 3           | No           | Number of neighbors in the k-nearest neighbor matching (KPPV algorithm). |
| `-r_extrapol`    | int      | 5           | No           | Search radius for CC extrapolation (piece-wise tracking). |
| `-d_line`        | int      | 25          | No           | Approximation factor of the rectilinear trajectory of meteors. |
| `-diff_deviaton` | float    | 4.0         | No           | Multiplication factor of the standard deviation (CC error has to be higher than `diff deviation` x `standard deviation` to be considered in movement). |


### Visualization with `meteor-visu`

The meteors visualization program is located here: `./exe/meteor-visu`.

The list of available arguments:

| **Argument**    | **Type** | **Default** | **Required** | **Description** |
| :---            | :---     | :---        | :---         | :--- |
| `-input_tracks` | str      | None        | Yes          | The `tracks.txt` file corresponding to the input video (generated from `meteor-detect`). |
| `-input_video`  | str      | None        | Yes          | Input video path. |
| `-output_video` | str      | "../"       | No           | Path of the output video with meteor tracking colored rectangles. If `-validation` is set then the bounding rectangles are red if *false positive* and green if *true positive*. If `-validation` is NOT set then the bounding rectangles are levels of green depending on the detection confidence. |
| `-validation`   | str      | None        | No           | File containing the ground truth. |

Note: to run `./exe/meteor-visu`, it is required to run `./exe/meteor-detect` before and on the same input video. This will generate the required `tracks.txt` and `bouding_box.txt` files.

### Checking with `meteor-check`

The meteors checking program is located here: `./exe/meteor-check`.

The list of available arguments:

| **Argument**    | **Type** | **Default** | **Required** | **Description** |
| :---            | :---     | :---        | :---         | :--- |
| `-input_tracks` | str      |  None       | Yes          | The `tracks.txt` file corresponding to the input video (generated from `meteor-detect`). |
| `-validation`   | str      |  None       | Yes          | File containing the ground truth. |
| `-output`       | str      |  "../"      | No           | Path of the folder containing the result of the checking (`validation.txt` file). |

Note: to run `./exe/meteor-check`, it is required to run `./exe/meteor-detect` before. This will generate the required `tracks.txt` file.

### Examples of use

Download a video sequence containing meteors here: https://lip6.fr/adrien.cassagne/data/tauh/in/2022_05_31_tauh_34_meteors.mp4.

#### Step 1: Meteors detection

```shell
./exe/meteor-detect -input_video ../2022_05_31_tauh_34_meteors.mp4
```

Enable debug frames output:

```shell
./exe/meteor-detect -input_video ../2022_05_31_tauh_34_meteors.mp4 -output_frames ./frames
```

#### Step 2: Visualization

Visualization **WITHOUT** ground truth:

```shell
./exe/meteor-visu -input_video ../2022_05_31_tauh_34_meteors.mp4 -input_tracks ./debug/assoconflicts/SB_55_SH_80/2022_05_31_tauh_34_meteors/tracks.txt -output_video ../.
```

Visualization **WITH** ground truth:

```shell
./exe/meteor-visu -input_video ../2022_05_31_tauh_34_meteors.mp4 -input_tracks ./debug/assoconflicts/SB_55_SH_80/2022_05_31_tauh_34_meteors/tracks.txt -output_video ../ -validation ../validation/2022_05_31_tauh_34_meteors.txt
```

#### Step 3: Offline checking

Use `meteor-check` with the following arguments:

```shell
./exe/meteor-check -input_tracks ./debug/assoconflicts/SB_55_SH_80/2022_05_31_tauh_34_meteors.txt/tracks.txt -validation ../validation/2022_05_31_tauh_34_meteors.txt
```

