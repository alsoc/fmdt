#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "cmd.h"

static const char* get(const char* cmd, ...) {
  va_list vargs;
  va_start(vargs, cmd);
  while (cmd != NULL) {
    char buffer[128];
    snprintf(buffer, sizeof(buffer), "which %s >/dev/null 2>&1", cmd);
    if (system(buffer) == 0) break;
    cmd = va_arg(vargs, const char*);
  }
  if (cmd != NULL) {
    while (va_arg(vargs, const char*) != NULL);
  }
  va_end(vargs);
  return cmd;
}

const char* get_ffmpeg() {
  return get("ffmpeg", "avconv", NULL);
}
const char* get_ffprobe() {
  return get("ffprobe", "avprobe", NULL);
}
const char* get_ffplay() {
  return get("ffplay", "avplay", NULL);
}
