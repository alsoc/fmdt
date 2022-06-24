#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include "cmd.h"
#include "formatter.h"
#include "ffmpeg-io/player.h"


int ffmpeg_start_player(ffmpeg_handle* h, const ffmpeg_options* opts) {
  static const ffmpeg_options no_opts;
  if (opts == NULL) opts = &no_opts;
  if (!ffmpeg_valid_descriptor(&h->input, &h->error)) return 0;
  int width  = h->input.width;
  int height = h->input.height;
  ffmpeg_pixfmt pixfmt = h->input.pixfmt;

  ffmpeg_formatter cmd;
  ffmpeg_formatter_init(&cmd);

  const char* ffplay = opts->ffplay_path;
  if (ffplay == NULL) ffplay = get_ffplay();
  if (ffplay == NULL) {
    h->error = ffmpeg_missing_ffplay;
    return 0;
  }

  const char* format = "rawvideo";
  const char* codec = "rawvideo";
  if (h->input.fileformat.s[0] != '\0') {
    format = ffmpeg_fileformat2str(&h->input.fileformat);
  }
  if (h->input.codec.s[0] != '\0') {
    codec = ffmpeg_codec2str(&h->input.codec);
  }
  ffmpeg_formatter_append(&cmd, "exec %s -loglevel error", ffplay);
  if (opts->extra_general_options != NULL) {
    ffmpeg_formatter_append(&cmd, " %s", opts->extra_general_options);
  }
  ffmpeg_formatter_append(&cmd, " -f %s -vcodec %s -pixel_format %s -video_size %dx%d", format, codec, ffmpeg_pixfmt2str(&pixfmt), width, height);
  if (opts->extra_input_options != NULL) {
    ffmpeg_formatter_append(&cmd, " %s", opts->extra_input_options);
  }

  if (opts->window_title != NULL) {
    ffmpeg_formatter_append(&cmd, " -window_title '%s'", opts->window_title);
  }
  if (opts->infinite_buffer) {
    ffmpeg_formatter_append(&cmd, " -infbuf");
  }
  if (h->input.framerate.num > 0 && h->input.framerate.den > 0) {
    ffmpeg_formatter_append(&cmd, " -framerate %d/%d", h->input.framerate.num, h->input.framerate.den);
  }
  ffmpeg_formatter_append(&cmd, " -");
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

int ffmpeg_play1d(ffmpeg_handle* h, const uint8_t* data, size_t pitch) {
  return ffmpeg_write1d(h, data, pitch);
}

int ffmpeg_play2d(ffmpeg_handle* h, uint8_t** data) {
  return ffmpeg_write2d(h, data);
}

int ffmpeg_stop_player(ffmpeg_handle* h) {
  return ffmpeg_stop_writer(h);
}
