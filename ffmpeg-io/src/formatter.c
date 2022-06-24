#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "formatter.h"


void ffmpeg_formatter_init(ffmpeg_formatter* p) {
  p->str = NULL;
  p->size = 0;
  p->capacity = 0;
}
void ffmpeg_formatter_fini(ffmpeg_formatter* p) {
  if (p->str) free(p->str);
  ffmpeg_formatter_init(p);
}

int ffmpeg_formatter_append(ffmpeg_formatter* p, const char* fmt, ...) {
  va_list args;
  long n;
loop:
  va_start(args, fmt);
  n = vsnprintf(p->str + p->size, p->capacity - p->size, fmt, args);
  va_end(args);
  if (n < 0) return n;
  if (p->capacity < p->size + n + 1) {
    size_t s = p->capacity;
    s = (s * 3)/2; // exponential growth
    if (s < 64) s = 64; // init with a cache line
    if (s < p->size + n) s = p->size + n + 1; // exponential growth was not enough
    char* str = realloc(p->str, s);
    if (str == NULL) return errno; // an error occured
    p->str = str;
    p->capacity = s;
    goto loop; // should never repeat more than once
  }
  p->size += n;
  return 0;
}
