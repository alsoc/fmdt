#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include "cmd.h"
#include "formatter.h"
#include "ffmpeg-io/writer.h"


int ffmpeg_start_writer_cmd_raw(ffmpeg_handle* h, const char* command) {
  ffmpeg_formatter cmd;
  ffmpeg_formatter_init(&cmd);
  ffmpeg_formatter_append(&cmd, "exec %s", command);

  h->pipe = popen(cmd.str, "w");
  int success = 1;
  if (!h->pipe) {
    h->error = ffmpeg_pipe_error;
    success = 0;
  }
  ffmpeg_formatter_fini(&cmd);
  return success;
}
int ffmpeg_start_writer(ffmpeg_handle* h, const char* filename, const ffmpeg_options* opts) {
  static const ffmpeg_options no_opts;
  if (opts == NULL) opts = &no_opts;
  ffmpeg_merge_descriptor(&h->output, &h->input);
  if (!ffmpeg_valid_descriptor(&h->input,  &h->error)) return 0;
  if (!ffmpeg_valid_descriptor(&h->output, &h->error)) return 0;
  unsigned iwidth  = h->input.width;
  unsigned iheight = h->input.height;
  ffmpeg_ratio iframerate = h->input.framerate;
  const char* ifmt = ffmpeg_pixfmt2str(&h->input.pixfmt);
  unsigned owidth  = h->output.width;
  unsigned oheight = h->output.height;
  ffmpeg_ratio oframerate = h->output.framerate;
  const char* ofmt = ffmpeg_pixfmt2str(&h->output.pixfmt);

  char const* dot = filename;
  for (char const* c = filename; *c != '\0'; ++c) {
    if (*c == '.') dot = c;
  }
  const char* codec = NULL;
  if (h->output.codec.s[0] != '\0') {
    codec = ffmpeg_codec2str(&h->output.codec);
  } else if (opts->lossless) {
    if (strcmp(dot, ".mkv") == 0 || strcmp(dot, ".avi") == 0 || strcmp(dot, ".mov") == 0) {
      codec = "ffv1";
    }
  }

  const char* ffmpeg = opts->ffmpeg_path;
  if (ffmpeg == NULL) ffmpeg = get_ffmpeg();
  if (ffmpeg == NULL) {
    h->error = ffmpeg_missing_ffmpeg;
    return 0;
  }

  ffmpeg_formatter cmd;
  ffmpeg_formatter_init(&cmd);
  ffmpeg_formatter_append(&cmd, "exec %s -loglevel error -y", ffmpeg);
  if (opts->extra_general_options != NULL) {
    ffmpeg_formatter_append(&cmd, " %s", opts->extra_general_options);
  }

  {
    const char* format = "rawvideo";
    const char* codec = "rawvideo";
    if (h->input.fileformat.s[0] != '\0') {
      format = ffmpeg_fileformat2str(&h->input.fileformat);
    }
    if (h->input.codec.s[0] != '\0') {
      codec = ffmpeg_codec2str(&h->input.codec);
    }

    ffmpeg_formatter_append(&cmd, " -f %s -vcodec %s -pix_fmt %s -s %dx%d", format, codec, ifmt, iwidth, iheight);
    if (iframerate.num > 0 && iframerate.den > 0) {
      ffmpeg_formatter_append(&cmd, " -framerate %d/%d", iframerate.num, iframerate.den);
    }
  }
  if (opts->extra_input_options != NULL) {
    ffmpeg_formatter_append(&cmd, " %s", opts->extra_input_options);
  }
  ffmpeg_formatter_append(&cmd, " -i - -an");

  const char* filter_prefix = " -filter:v ";
  if ((iframerate.num != oframerate.num || iframerate.den != oframerate.den) && oframerate.num > 0 && oframerate.den > 0) {
    ffmpeg_formatter_append(&cmd, "%sfps=fps=%d/%d", filter_prefix, oframerate.num, oframerate.den);
    filter_prefix = ",";
  }
  if (iwidth != owidth || iheight != oheight) {
    if (opts->keep_aspect) {
      ffmpeg_formatter_append(&cmd, "%sscale=%d:%d:force_original_aspect_ratio=increase,crop=%d:%d", filter_prefix, owidth, oheight, owidth, oheight);
    } else {
      ffmpeg_formatter_append(&cmd, "%sscale=%d:%d", filter_prefix, owidth, oheight);
    }
    filter_prefix = ",";
  }
  if (opts->extra_filter_options != NULL) {
    ffmpeg_formatter_append(&cmd, "%s%s", filter_prefix, opts->extra_filter_options);
  }
  if (h->output.fileformat.s[0] != '\0') {
    ffmpeg_formatter_append(&cmd, " -f %s", ffmpeg_fileformat2str(&h->output.fileformat));
  }

  if (codec != NULL) {
    ffmpeg_formatter_append(&cmd, " -vcodec %s", codec);
  }
  if (strcmp(ifmt, ofmt) != 0) {
    ffmpeg_formatter_append(&cmd, " -pix_fmt %s", ofmt);
  }
  if (opts->extra_output_options != NULL) {
    ffmpeg_formatter_append(&cmd, " %s", opts->extra_output_options);
  }
  ffmpeg_formatter_append(&cmd, " -start_number 0 '%s'", filename);
  if (opts->debug) printf("cmd: %s\n", cmd.str);

  h->pipe = popen(cmd.str, "w");
  int success = 1;
  if (!h->pipe) {
    h->error = ffmpeg_pipe_error;
    success = 0;
  }
  ffmpeg_formatter_fini(&cmd);
  return success;
}

int ffmpeg_write_raw(ffmpeg_handle* h, size_t size, size_t nmemb, const void* in) {
  FILE* pipe = h->pipe;

  if (pipe == NULL) {
    h->error = ffmpeg_closed_pipe;
    return 0;
  }
  if (feof(pipe)) {
    h->error = ffmpeg_eof_error;
    return 0;
  }

  size_t n = fwrite(in, size, nmemb, pipe);

  return n;
}
int ffmpeg_write1d(ffmpeg_handle* h, const uint8_t* data, size_t pitch) {
  size_t width = h->input.width;
  size_t height = h->input.height;
  ffmpeg_pixfmt pixfmt = h->input.pixfmt;
  size_t elsize = ffmpeg_pixel_size(pixfmt);
  FILE* pipe = h->pipe;

  if (pipe == NULL) {
    h->error = ffmpeg_closed_pipe;
    return 0;
  }
  if (width == 0) {
    h->error = ffmpeg_invalid_width;
    return 0;
  }
  if (height == 0) {
    h->error = ffmpeg_invalid_height;
    return 0;
  }
  if (elsize == 0) {
    h->error = ffmpeg_invalid_pixfmt;
    return 0;
  }

  for (size_t i = 0; i < height; i++) {
    fwrite(data, elsize, width, pipe);
    data += pitch;
  }
  return 1;
}

int ffmpeg_write2d(ffmpeg_handle* h, uint8_t** data) {
  size_t width = h->input.width;
  size_t height = h->input.height;
  ffmpeg_pixfmt pixfmt = h->input.pixfmt;
  size_t elsize = ffmpeg_pixel_size(pixfmt);
  FILE* pipe = h->pipe;

  if (pipe == NULL) {
    h->error = ffmpeg_closed_pipe;
    return 0;
  }
  if (width == 0) {
    h->error = ffmpeg_invalid_width;
    return 0;
  }
  if (height == 0) {
    h->error = ffmpeg_invalid_height;
    return 0;
  }
  if (elsize == 0) {
    h->error = ffmpeg_invalid_pixfmt;
    return 0;
  }

  for (size_t i = 0; i < height; i++) {
    fwrite(data[i], elsize, width, pipe);
  }
  return 1;
}

int ffmpeg_stop_writer(ffmpeg_handle* h) {
  FILE* p = h->pipe;
  h->pipe = NULL;
  if (p != NULL) {
    pclose(p);
  }
  return 1;
}
