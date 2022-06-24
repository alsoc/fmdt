#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ffmpeg_formatter {
  char* str;
  size_t size, capacity;
} ffmpeg_formatter;

void ffmpeg_formatter_init(ffmpeg_formatter*);
int ffmpeg_formatter_append(ffmpeg_formatter*, const char*, ...);
void ffmpeg_formatter_fini(ffmpeg_formatter*);

#ifdef __cplusplus
}
#endif
