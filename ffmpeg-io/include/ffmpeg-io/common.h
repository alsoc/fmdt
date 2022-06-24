#pragma once

#include <stddef.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ffmpeg_pixfmt {
  char s[32];
} ffmpeg_pixfmt;
typedef struct ffmpeg_codec {
  char s[32];
} ffmpeg_codec;
typedef struct ffmpeg_fileformat {
  char s[32];
} ffmpeg_fileformat;

typedef enum ffmpeg_error {
  ffmpeg_noerror = 0,
  ffmpeg_ffprobe_invalid_section_start,
  ffmpeg_ffprobe_invalid_section_stop,
  ffmpeg_ffprobe_invalid_statement,
  ffmpeg_ffprobe_unclosed_section,
  ffmpeg_ffprobe_multiple_codec,
  ffmpeg_ffprobe_multiple_width,
  ffmpeg_ffprobe_multiple_height,
  ffmpeg_ffprobe_multiple_framerate,
  ffmpeg_ffprobe_multiple_pixfmt,
  ffmpeg_ffprobe_no_codec,
  ffmpeg_ffprobe_no_width,
  ffmpeg_ffprobe_no_height,
  ffmpeg_ffprobe_no_pixfmt,
  ffmpeg_ffprobe_no_video_section,
  ffmpeg_pipe_error,
  ffmpeg_invalid_width,
  ffmpeg_invalid_height,
  ffmpeg_invalid_framerate,
  ffmpeg_invalid_codec,
  ffmpeg_invalid_pixfmt,
  ffmpeg_closed_pipe,
  ffmpeg_eof_error,
  ffmpeg_partial_read,
  ffmpeg_missing_ffmpeg,
  ffmpeg_missing_ffprobe,
  ffmpeg_missing_ffplay,
  ffmpeg_unknown_error,
} ffmpeg_error;

typedef struct ffmpeg_ratio {
  unsigned num, den;
} ffmpeg_ratio;
typedef struct ffmpeg_descriptor {
  unsigned width, height;
  ffmpeg_ratio framerate;
  ffmpeg_pixfmt pixfmt;
  ffmpeg_codec codec;
  ffmpeg_fileformat fileformat;
} ffmpeg_descriptor;
typedef struct ffmpeg_handle {
  FILE* pipe;
  ffmpeg_descriptor input, output;
  ffmpeg_error error;
} ffmpeg_handle;
typedef struct ffmpeg_options {
  const char* window_title;
  const char* ffmpeg_path;
  const char* ffprobe_path;
  const char* ffplay_path;
  const char* extra_general_options;
  const char* extra_input_options;
  const char* extra_filter_options;
  const char* extra_output_options;
  unsigned infinite_buffer:1;
  unsigned debug:1;
  unsigned force_input_framerate:1;
  unsigned lossless:1;
  unsigned keep_aspect:1;
} ffmpeg_options;

size_t ffmpeg_pixel_bitsize(ffmpeg_pixfmt);
size_t ffmpeg_pixel_size(ffmpeg_pixfmt);
size_t ffmpeg_pixel_nchannel(ffmpeg_pixfmt);

const char* ffmpeg_pixfmt2str(const ffmpeg_pixfmt*);
ffmpeg_pixfmt ffmpeg_str2pixfmt(const char*);
const char* ffmpeg_codec2str(const ffmpeg_codec*);
ffmpeg_codec ffmpeg_str2codec(const char*);
const char* ffmpeg_fileformat2str(const ffmpeg_fileformat*);
ffmpeg_fileformat ffmpeg_str2fileformat(const char*);
const char* ffmpeg_error2str(ffmpeg_error);

void ffmpeg_init(ffmpeg_handle*);
int ffmpeg_valid_descriptor(const ffmpeg_descriptor*, ffmpeg_error* /* = NULL */);
void ffmpeg_merge_descriptor(ffmpeg_descriptor*, const ffmpeg_descriptor*);
void ffmpeg_compatible_writer(ffmpeg_handle* writer, const ffmpeg_handle* reader);

void ffmpeg_options_init(ffmpeg_options*);

#ifdef __cplusplus
}
#endif
