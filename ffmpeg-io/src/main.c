#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "ffmpeg-io/reader.h"
#include "ffmpeg-io/player.h"
#include "ffmpeg-io/writer.h"

void usage(FILE* f, const char* cmd) {
  fprintf(f, "USAGE: %s [-f INPUT_DEVICE_FORMAT] [-i:w INPUT_WIDTH] [-i:h INPUT_HEIGHT] [-i:f INPUT_FORMAT] [-o:w OUTPUT_WIDTH] [-o:h OUTPUT_HEIGHT] [-o:f OUTPUT_FORMAT] INPUT_FILE OUTPUT_FILE\n", cmd);
}

int main(int argc, char *argv[]) {
  (void)argc;
  ffmpeg_handle reader, writer;
  ffmpeg_options options;
  ffmpeg_init(&reader);
  ffmpeg_init(&writer);
  ffmpeg_options_init(&options);

  options.debug = 1;
  options.keep_aspect = 1;

  const char*const* arg = 1+(const char*const*) argv;
  const char* input = NULL;
  const char* output = NULL;
  while (*arg != NULL) {
    if (strcmp(*arg, "-f") == 0) {
      if (*++arg == NULL) {
        usage(stderr, argv[0]);
        return 1;
      }
      reader.input.fileformat = ffmpeg_str2fileformat(*arg);
    } else if (strcmp(*arg, "-i:w") == 0) {
      if (*++arg == NULL) {
        usage(stderr, argv[0]);
        return 1;
      }
      reader.output.width = atoi(*arg);
    } else if (strcmp(*arg, "-i:h") == 0) {
      if (*++arg == NULL) {
        usage(stderr, argv[0]);
        return 1;
      }
      reader.output.height = atoi(*arg);
    } else if (strcmp(*arg, "-i:f") == 0) {
      if (*++arg == NULL) {
        usage(stderr, argv[0]);
        return 1;
      }
      reader.output.pixfmt = ffmpeg_str2pixfmt(*arg);
    } else if (strcmp(*arg, "-o:w") == 0) {
      if (*++arg == NULL) {
        usage(stderr, argv[0]);
        return 1;
      }
      writer.output.width = atoi(*arg);
    } else if (strcmp(*arg, "-o:h") == 0) {
      if (*++arg == NULL) {
        usage(stderr, argv[0]);
        return 1;
      }
      writer.output.height = atoi(*arg);
    } else if (strcmp(*arg, "-o:f") == 0) {
      if (*++arg == NULL) {
        usage(stderr, argv[0]);
        return 1;
      }
      writer.output.pixfmt = ffmpeg_str2pixfmt(*arg);
    } else if (strcmp(*arg, "-h") == 0 || strcmp(*arg, "--help") == 0) {
      usage(stdout, argv[0]);
      return 0;
    } else if (strcmp(*arg, "--") == 0) {
      while (*++arg != NULL) {
        if (input == NULL) {
          input = *arg;
        } else if (output == NULL) {
          output = *arg;
        } else {
          usage(stderr, argv[0]);
          return 1;
        }
      }
      break;
    } else if ((*arg)[0] == '-' && (*arg)[1] == '-') {
      usage(stderr, argv[0]);
      return 1;
    } else if ((*arg)[0] == '-') {
      usage(stderr, argv[0]);
      return 1;
    } else {
      if (input == NULL) {
        input = *arg;
      } else if (output == NULL) {
        output = *arg;
      } else {
        usage(stderr, argv[0]);
        return 1;
      }
    }
    arg++;
  }
  if (input == NULL || output == NULL) {
    usage(stderr, argv[0]);
    goto cleanup;
  }
  if (!ffmpeg_probe(&reader, input, NULL)) {
    fprintf(stderr, "error: %s\n", ffmpeg_error2str(reader.error));
    goto cleanup;
  }

  ffmpeg_compatible_writer(&writer, &reader);

  if (!ffmpeg_start_reader(&reader, input, &options)) {
    fprintf(stderr, "error: %s\n", ffmpeg_error2str(reader.error));
    goto cleanup;
  }
  if (strcmp(output, "play") != 0) {
    if (!ffmpeg_start_writer(&writer, output, &options)) {
      fprintf(stderr, "error: %s\n", ffmpeg_error2str(writer.error));
      goto cleanup;
    }
  } else {
    if (!ffmpeg_start_player(&writer, &options)) {
      fprintf(stderr, "error: %s\n", ffmpeg_error2str(writer.error));
      goto cleanup;
    }
  }
  int pixsize = ffmpeg_pixel_size(reader.output.pixfmt);
  uint8_t *img = malloc(reader.output.width * reader.output.height * ffmpeg_pixel_size(reader.output.pixfmt));
  int i = 0;
  while (ffmpeg_read1d(&reader, img, pixsize*reader.output.width)) {
    ffmpeg_write1d(&writer, img, pixsize*writer.input.width);
    if (reader.error || writer.error) break;
    printf("\r%d", i);
    fflush(stdout);
    ++i;
  }
  printf("\n");
  if (reader.error) {
    fprintf(stderr, "error: reader: %s\n", ffmpeg_error2str(reader.error));
  }
  if (writer.error) {
    fprintf(stderr, "error: writer: %s\n", ffmpeg_error2str(writer.error));
  }

  free(img);

cleanup:
  ffmpeg_stop_writer(&writer);
  ffmpeg_stop_reader(&reader);
  return 0;
}
