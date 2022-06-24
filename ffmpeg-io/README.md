# ffmpeg-io

ffmpeg-io is a light-weight C library for video io.
It leverages all the features from ffmpeg:

- real-time acquisition (webcam or screen)
- real-time display (through ffplay)
- video read
- video write
- on-the-fly conversion
- on-the-fly rescale
- ...

For the library to work, the ffmpeg programs must be in the `PATH`.
The 3 following programs are used:

- `ffmpeg` (or `avconv`)
- `ffprobe` (or `avprobe`)
- `ffplay` (or `avplay`)

## Structures

### `ffmpeg_handle`

This is the main struct you will interact with.
It is used for readers, writers and players.

```c
typedef struct ffmpeg_handle {
  FILE* pipe;
  ffmpeg_descriptor input, output;
  ffmpeg_error error;
} ffmpeg_handle;

void ffmpeg_init(ffmpeg_handle*);
void ffmpeg_compatible_writer(ffmpeg_handle* writer, const ffmpeg_handle* reader);

const char* ffmpeg_error2str(ffmpeg_error);
```

For readers, `input` is the description of the file to be read, and `output` the description of what your program wants.
For writers and players, `input` is the description of what your program sends.
For writers, `output` is the description of the file to be written.

If an error occurs at any point, it can be retrieved through the `error` member and can be converted into a string with the `ffmpeg_error2str` function.
All functions on handles (except close) are disabled once an error has occured.

For readers and writers, if the `input` and `output` descriptors are different, the necessary conversions will be performed.

The function `ffmpeg_compatible_writer` is used to make the writer handle having the same input format as the output of the reader handle.

### `ffmpeg_descriptor`

This struct describes all the information of a video stream:

- size (width and height)
- framerate
- pixel format
- codec
- file format

```c
typedef struct ffmpeg_descriptor {
  unsigned width, height;
  ffmpeg_ratio framerate;
  ffmpeg_pixfmt pixfmt;
  ffmpeg_codec codec;
  ffmpeg_fileformat fileformat;
} ffmpeg_descriptor;

typedef struct ffmpeg_ratio {
  unsigned num, den;
} ffmpeg_ratio;

void ffmpeg_init(ffmpeg_handle*);
int ffmpeg_valid_descriptor(const ffmpeg_descriptor*, ffmpeg_error* /* = NULL */);
void ffmpeg_merge_descriptor(ffmpeg_descriptor*, const ffmpeg_descriptor*)

const char* ffmpeg_pixfmt2str(const ffmpeg_pixfmt*);
ffmpeg_pixfmt ffmpeg_str2pixfmt(const char*);

const char* ffmpeg_codec2str(const ffmpeg_codec*);
ffmpeg_codec ffmpeg_str2codec(const char*);

const char* ffmpeg_fileformat2str(const ffmpeg_fileformat*);
ffmpeg_fileformat ffmpeg_str2fileformat(const char*);

size_t ffmpeg_pixel_bitsize(ffmpeg_pixfmt);
size_t ffmpeg_pixel_size(ffmpeg_pixfmt);
size_t ffmpeg_pixel_nchannel(ffmpeg_pixfmt);
```

The members `pixfmt`, `codec` and `fileformat` should be considered opaque and used only through `ffmpeg_*2str` and `ffmpeg_str2*` functions.
For instance, one could set the pixel format of a reader handle with `reader.output.pixfmt = ffmpeg_str2pixfmt("gray")`.

In most cases, it is not necessary to modify `codec` and `fileformat` members.

### `ffmpeg_options`

This structure provides more options when starting a reader, writer or player.
Few notable options:

- `ffmpeg_path`: override the path to the `ffmpeg` program
- `ffprobe_path`: override the path to the `ffprobe` program
- `ffplay_path`: override the path to the `ffplay` program
- `debug`: prints in stderr the actual and complete commands used

```c
typedef struct ffmpeg_options {
  const char* window_title;
  const char* ffmpeg_path;
  const char* ffprobe_path;
  const char* ffplay_path;
  unsigned infinite_buffer:1;
  unsigned debug:1;
  unsigned force_input_framerate:1;
  unsigned lossless:1;
  unsigned keep_aspect:1;
} ffmpeg_options;

void ffmpeg_options_init(ffmpeg_options*);
```

## Readers

A reader is an object that is able to read a video stream from a video file, a sequence of images, a webcam...

### Usage

```c
// ffmpeg-io/reader.h

// void ffmpeg_init(ffmpeg_handle*);
int ffmpeg_probe(ffmpeg_handle*, const char* filename, const ffmpeg_options* /* = NULL */);
int ffmpeg_start_reader(ffmpeg_handle*, const char* filename, const ffmpeg_options* /* = NULL */);
int ffmpeg_read_raw(ffmpeg_handle*, size_t size, size_t nmemb, void*);
int ffmpeg_read1d(ffmpeg_handle*, uint8_t* data, size_t pitch);
int ffmpeg_read2d(ffmpeg_handle*, uint8_t** data);
int ffmpeg_stop_reader(ffmpeg_handle*);
```

All the functions (except `ffmpeg_read_raw`) return `1` on success and `0` on error.
The error shall be retrieved through the `error` member of the handle.

The usual way to use it is to first probe the file to be read with `ffmpeg_probe`.
This will properly set the input reader descriptor.

You can then start the reader with `ffmpeg_start_reader`.
If a value is missing from the `output` descriptor, the value from the `input` will be used and thus they don't need to be set explicitely.
The codec and fileformat of the `output` should most likely not be modified in order to read raw values of pixels.
You can modify the output descriptor to better fit your needs before calling `ffmpeg_start_reader`.
In particular you should most likely set `pixfmt` to the format you want.

Once the reader is started, the missing values from the `output` descriptors are filled in.

You can read frames into an allocated buffer of yours thanks to `ffmpeg_read1d` and `ffmpeg_read2d`.
`ffmpeg_read1d` is used when you 2d buffer is linearized, while `ffmpeg_read2d` should be used for iliffe vectors.
Once the end of the video is reached, read functions will fail with `ffmpeg_eof_error`.

At the end, you must stop the reader with `ffmpeg_stop_reader`.

### Special options

- `ffmpeg_options.keep_aspect`: when rescaling, if the target resolution has not the same aspect ratio as the input resolution, cropping is adding on top to keep the same aspect ratio.
- `ffmpeg_options.force_input_frame`: when the input video stream is not an image sequence, the input framerate is ignored.
    If you want to reintrepret to succession of frames at an exact frame rate, you should set this option.
    Note that this is just a reintrepretation of the timestamp of each frame and no frame is actually dropped nor duplicated.
    If you want to drop/duplicate frames to reach another framerate, you should set `ffmpeg_handle.output.framerate` instead.

### Reader example

```c
const char* filename = "input.mkv";
ffmpeg_handle reader;
ffmpeg_init(&reader);

ffmpeg_probe(&reader, filename, NULL);

int width = reader.input.width;
int height = reader.input.height;
reader.output.pixfmt = ffmpeg_str2pixfmt("gray");

if (ffmpeg_start_reader(&reader, filename, NULL)) {
  int pixsize = ffmpeg_pixel_size(reader.output.pixfmt); // 1 for "gray"
  int pitch = width * pixsize;
  uint8_t* img = (uint8_t*)malloc(height * pitch);

  while (ffmpeg_read1d(&reader, img, pitch)) {
    /* process img */
  }

  free(img);

  if (reader.error != ffmpeg_eof_error) {
    fprintf(stderr, "Error: %s\n", ffmpeg_error2str(reader.error));
  }
} else {
  fprintf(stderr, "Error: %s\n", ffmpeg_error2str(reader.error));
}

ffmpeg_stop_reader(&reader);
```

## Writers

A writer is an object that is able to write a video stream into a video file or a sequence of images.
It a the symetric of a reader.

### Usage

```c
// ffmpeg-io/writer.h

// void ffmpeg_init(ffmpeg_handle*);
// void ffmpeg_compatible_writer(ffmpeg_handle* writer, const ffmpeg_handle* reader);
int ffmpeg_start_writer(ffmpeg_handle*, const char* filename, const ffmpeg_options* /* = NULL */);
int ffmpeg_write_raw(ffmpeg_handle*, size_t size, size_t nmemb, const void*);
int ffmpeg_write1d(ffmpeg_handle*, const uint8_t* data, size_t pitch);
int ffmpeg_write2d(ffmpeg_handle*, uint8_t** data);
int ffmpeg_stop_writer(ffmpeg_handle*);
```

The usual to use a writer is to first set its property.
This can be done with `ffmpeg_compatible_writer` if the video stream you want to write is coming from a reader (with processing in between).
This is especially useful to keep the framerate information.
Or you can manually set the `input` descriptor of your writer.
In the latter case, you must set the following members of the `input`: `width`, `height` and `pixfmt`.
Framerate is optional and is defaulted to 25 fps (default value of ffmpeg).

The file format, codec and pixel format of the file written are chosen automatically according to the extension of the filename and the input pixel format.
You can also set a different pixel format, width and height than your input descriptor if you want to save in a different pixel format or resolution than what you work with.

### Special options

- `ffmpeg_options.keep_aspect`: when rescaling, if the target resolution has not the same aspect ratio as the input resolution, cropping is adding on top to keep the same aspect for the content.
- `ffmpeg_options.lossless`: choose a lossless codec if possible (and if the codec is not manually set). `.mkv` and `.avi` are known to have lossless codecs available.

### Example

```c
int width = /* ... */, height = /* ... */;
const char* filename = "output-%06d.png";
ffmpeg_handle writer;
ffmpeg_init(&writer);

writer.input.width = width;
writer.input.height = height;
writer.input.pixfmt = ffmpeg_str2pixfmt("rgb24");

if (ffmpeg_start_writer(&writer, filename, NULL)) {
  int pixsize = ffmpeg_pixel_size(writer.input.pixfmt); // 3 for "rgb24"
  int pitch = width * pixsize;
  uint8_t* img = (uint8_t*)malloc(height * pitch);

  while (/* ... */) {
    /* generate img */
    if (!ffmpeg_write1d(&writer, img, pitch)) {
      fprintf(stderr, "Error: %s\n", ffmpeg_error2str(writer.error));
      break;
    }
  }

  free(img);
} else {
  fprintf(stderr, "Error: %s\n", ffmpeg_error2str(writer.error));
}

ffmpeg_stop_writer(&writer);
```

## Players

Players are just special writers that display the written frame into a window instead of writing into a disk.

### Usage

```c
// ffmpeg-io/player.h

// void ffmpeg_init(ffmpeg_handle*);
int ffmpeg_start_player(ffmpeg_handle*, const ffmpeg_options* /* = NULL */);
int ffmpeg_play1d(ffmpeg_handle*, const uint8_t* data, size_t pitch);
int ffmpeg_play2d(ffmpeg_handle*, uint8_t** data);
int ffmpeg_stop_player(ffmpeg_handle*);
```

Players are used in the same as a writer, except that it is created with `ffmpeg_start_player` instead.
The other difference is that the output descriptor is ignored (because ffplay is not able to perform transformations on the video stream).

The functions `ffmpeg_play1d`, `ffmpeg_play2d` and `ffmpeg_stop_player` are strictly identical to `ffmpeg_write1d`, `ffmpeg_write2d` and `ffmpeg_stop_writer`.
The formers are just provided as a way to add semantic to your code, but both can be used interchangeably.

### Special Options

- `ffmpeg_options.window_title`: allows you to define the title of the player window.
- `ffmpeg_options.infinite_buffer`: by default, ffplay uses a limited buffer. If frames are sent faster than they are displayed, ffplay will drop them. Use this option to make the buffer infinite, forcing ffplay to keep all the frames, even if it displays them slower.


### Example

```c
const char* filename = "test.mkv";
ffmpeg_handle reader, player;
ffmpeg_options opts;
ffmpeg_init(&reader);
ffmpeg_init(&player);
ffmpeg_init_options(&opts);

ffmpeg_probe(&reader, filename, NULL);

// We want 640x480, even if it means to rescale
int width = 640;
int height = 480;
reader.output.width = width;
reader.output.height = height;
reader.output.pixfmt = ffmpeg_str2pixfmt("rgb24");
opts.window_title = "my awesome reader";
opts.keep_aspect = 1;

if (!ffmpeg_start_reader(&reader, filename, &opts)) {
  fprintf(stderr, "Error: %s\n", ffmpeg_error2str(reader.error));
  abort();
}

ffmpeg_compatible_writer(&player, &reader);

if (!ffmpeg_start_player(&player, &opts)) {
  fprintf(stderr, "Error: %s\n", ffmpeg_error2str(player.error));
  abort();
}

int pixsize = ffmpeg_pixel_size(reader.output.pixfmt); // 3 for "rgb24"
int pitch = width * pixsize;
uint8_t* img = (uint8_t*)malloc(height * pitch);

while (ffmpeg_read1d(&reader, img, pitch)) {
  /* process img */

  if (!ffmpeg_play1d(&player, img, pitch)) {
    fprintf(stderr, "Error: %s\n", ffmpeg_error2str(player.error));
    abort();
  }
}
if (reader.error != ffmpeg_eof_error) {
  fprintf(stderr, "Error: %s\n", ffmpeg_error2str(reader.error));
}

free(img);

ffmpeg_stop_reader(&reader);
ffmpeg_stop_player(&player);
```

# Misc

## Webcam acquisition

### Linux

To read the video stream from a webcam on Linux, you can just specify `/dev/video0` as the input filename.
If mutliple resolutions/framerate are available, you can manually set them in the `input` descriptor of your reader handle before starting the reader.

On some USB webcam, high resolutions and high framerate might require to read an mpeg stream.
In that case, you should force the input codec to `mjpeg`:

```c
reader.input.codec = ffmpeg_str2codec("mjpeg");
```

The easiest way to know what resolutions are available is with `v4l-utils` package:

```c
v4l2-ctl --list-formats-ext -d /dev/video0
```

A list of v4l2 devices is given by the command `v4l2-ctl --list-devices` (or just `ls /dev/video*`).

### MacOS

To read a video stream from a webcam on MacOS, you need to set the input file format to `avfoundation`:

```c
reader.input.fileformat = ffmpeg_str2fileformat("avfoundation");
```

The filename shall be set to the id of the webcam, like `"0"` for the main one.

The easiest way to find what resolutions and framerates are supported is to try an invalid resolution or framerate.
The avalaible ones will be printed into stderr.

A list of the devices available is given by the command `ffmpeg -f avfoundation -list_devices true -i 0`.


### Find Webcam on Windows

List devices:

```
ffmpeg -f dshow -list_devices true -i 0

...
[dshow @ 0000020a78e0d540] DirectShow video devices (some may be both video and audio devices)
[dshow @ 0000020a78e0d540]  "Hercules HD Sunset"
[dshow @ 0000020a78e0d540]     Alternative name "@device_pnp_\\?\usb#vid_06f8&pid_3017&mi_00#6&ea49103&0&0000#{65e8773d-8f56-11d0-a3b9-00a0c9223196}\global"
[dshow @ 0000020a78e0d540] DirectShow audio devices
[dshow @ 0000020a78e0d540]  "Microphone (Hercules HD Sunset Mic)"
[dshow @ 0000020a78e0d540]     Alternative name "@device_cm_{33D9A762-90C8-11D0-BD43-00A0C911CE86}\wave_{86EA91AF-AD0B-41A7-84A5-0D205C9C7BCC}"
[dshow @ 0000020a78e0d540]  "Microphone (Steam Streaming Microphone)"
[dshow @ 0000020a78e0d540]     Alternative name "@device_cm_{33D9A762-90C8-11D0-BD43-00A0C911CE86}\wave_{0CC86501-581A-49E6-9672-939EDE2B4093}"
0: Immediate exit requested
```

List option For select device:

```
ffmpeg -f dshow -list_options true -i video="Hercules HD Sunset"

...
[dshow @ 000002b07fe4d580] DirectShow video device options (from video devices)
[dshow @ 000002b07fe4d580]  Pin "Capture" (alternative pin name "0")
[dshow @ 000002b07fe4d580]   pixel_format=yuyv422  min s=640x480 fps=7.5 max s=640x480 fps=30
[dshow @ 000002b07fe4d580]   pixel_format=yuyv422  min s=640x480 fps=7.5 max s=640x480 fps=30
...
[dshow @ 000002b07fe4d580]   pixel_format=yuyv422  min s=1280x720 fps=7.5 max s=1280x720 fps=10
[dshow @ 000002b07fe4d580]   pixel_format=yuyv422  min s=1280x800 fps=7.5 max s=1280x800 fps=10
[dshow @ 000002b07fe4d580]   pixel_format=yuyv422  min s=1280x800 fps=7.5 max s=1280x800 fps=10
[dshow @ 000002b07fe4d580]   vcodec=mjpeg  min s=640x480 fps=7.5 max s=640x480 fps=30
[dshow @ 000002b07fe4d580]   vcodec=mjpeg  min s=640x480 fps=7.5 max s=640x480 fps=30
...
[dshow @ 000002b07fe4d580]   vcodec=mjpeg  min s=1280x800 fps=7.5 max s=1280x800 fps=30
[dshow @ 000002b07fe4d580]   vcodec=mjpeg  min s=1280x800 fps=7.5 max s=1280x800 fps=30
video=Hercules HD Sunset: Immediate exit requested
```

Acquisition with mjpeg:
```
ffmpeg -f dshow -vcodec mjpeg -i video="Hercules HD Sunset" OUTPUT_OPTIONS...
```
