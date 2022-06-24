#pragma once

#include "ffmpeg-io/writer.h"

#ifdef __cplusplus
extern "C" {
#endif

int ffmpeg_start_player_cmd(ffmpeg_handle*);
int ffmpeg_start_player(ffmpeg_handle*, const ffmpeg_options* /* = NULL */);
int ffmpeg_play1d(ffmpeg_handle*, const uint8_t* data, size_t pitch);
int ffmpeg_play2d(ffmpeg_handle*, uint8_t** data);
int ffmpeg_stop_player(ffmpeg_handle*);

#ifdef __cplusplus
}
#endif
