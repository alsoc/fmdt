#pragma once

#include <stdint.h>
#include "ffmpeg-io/common.h"

#ifdef __cplusplus
extern "C" {
#endif

int ffmpeg_start_writer_cmd_raw(ffmpeg_handle*, const char* cmd);
int ffmpeg_start_writer(ffmpeg_handle*, const char* filename, const ffmpeg_options* /* = NULL */);
int ffmpeg_write_raw(ffmpeg_handle*, size_t size, size_t nmemb, const void*);
int ffmpeg_write1d(ffmpeg_handle*, const uint8_t* data, size_t pitch);
int ffmpeg_write2d(ffmpeg_handle*, uint8_t** data);
int ffmpeg_stop_writer(ffmpeg_handle*);

#ifdef __cplusplus
}
#endif
