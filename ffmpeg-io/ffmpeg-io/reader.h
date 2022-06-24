#pragma once

#include <stdint.h>
#include "ffmpeg-io/common.h"

#ifdef __cplusplus
extern "C" {
#endif

int ffmpeg_probe(ffmpeg_handle*, const char* filename, const ffmpeg_options* /* = NULL */);
int ffmpeg_start_reader_cmd_raw(ffmpeg_handle*, const char* cmd);
int ffmpeg_start_reader(ffmpeg_handle*, const char* filename, const ffmpeg_options* /* = NULL */);
int ffmpeg_read_raw(ffmpeg_handle*, size_t size, size_t nmemb, void*);
int ffmpeg_read1d(ffmpeg_handle*, uint8_t* data, size_t pitch);
int ffmpeg_read2d(ffmpeg_handle*, uint8_t** data);
int ffmpeg_stop_reader(ffmpeg_handle*);

#ifdef __cplusplus
}
#endif
