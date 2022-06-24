#include <string.h>
#include "ffmpeg-io/common.h"

typedef struct ffmpeg_pixel_format {
  char name[32];
  char spec[8];
  int channels, bits;
} ffmpeg_pixel_format;

static const ffmpeg_pixel_format ffmpeg_pixel_formats[] = {
  {"yuv420p",                "IO...", 3,            12},
  {"yuyv422",                "IO...", 3,            16},
  {"rgb24",                  "IO...", 3,            24},
  {"bgr24",                  "IO...", 3,            24},
  {"yuv422p",                "IO...", 3,            16},
  {"yuv444p",                "IO...", 3,            24},
  {"yuv410p",                "IO...", 3,             9},
  {"yuv411p",                "IO...", 3,            12},
  {"gray",                   "IO...", 1,             8},
  {"monow",                  "IO..B", 1,             1},
  {"monob",                  "IO..B", 1,             1},
  {"pal8",                   "I..P.", 1,             8},
  {"yuvj420p",               "IO...", 3,            12},
  {"yuvj422p",               "IO...", 3,            16},
  {"yuvj444p",               "IO...", 3,            24},
  {"uyvy422",                "IO...", 3,            16},
  {"uyyvyy411",              ".....", 3,            12},
  {"bgr8",                   "IO...", 3,             8},
  {"bgr4",                   ".O..B", 3,             4},
  {"bgr4_byte",              "IO...", 3,             4},
  {"rgb8",                   "IO...", 3,             8},
  {"rgb4",                   ".O..B", 3,             4},
  {"rgb4_byte",              "IO...", 3,             4},
  {"nv12",                   "IO...", 3,            12},
  {"nv21",                   "IO...", 3,            12},
  {"argb",                   "IO...", 4,            32},
  {"rgba",                   "IO...", 4,            32},
  {"abgr",                   "IO...", 4,            32},
  {"bgra",                   "IO...", 4,            32},
  {"gray16be",               "IO...", 1,            16},
  {"gray16le",               "IO...", 1,            16},
  {"yuv440p",                "IO...", 3,            16},
  {"yuvj440p",               "IO...", 3,            16},
  {"yuva420p",               "IO...", 4,            20},
  {"rgb48be",                "IO...", 3,            48},
  {"rgb48le",                "IO...", 3,            48},
  {"rgb565be",               "IO...", 3,            16},
  {"rgb565le",               "IO...", 3,            16},
  {"rgb555be",               "IO...", 3,            15},
  {"rgb555le",               "IO...", 3,            15},
  {"bgr565be",               "IO...", 3,            16},
  {"bgr565le",               "IO...", 3,            16},
  {"bgr555be",               "IO...", 3,            15},
  {"bgr555le",               "IO...", 3,            15},
  {"vaapi_moco",             "..H..", 0,             0},
  {"vaapi_idct",             "..H..", 0,             0},
  {"vaapi_vld",              "..H..", 0,             0},
  {"yuv420p16le",            "IO...", 3,            24},
  {"yuv420p16be",            "IO...", 3,            24},
  {"yuv422p16le",            "IO...", 3,            32},
  {"yuv422p16be",            "IO...", 3,            32},
  {"yuv444p16le",            "IO...", 3,            48},
  {"yuv444p16be",            "IO...", 3,            48},
  {"dxva2_vld",              "..H..", 0,             0},
  {"rgb444le",               "IO...", 3,            12},
  {"rgb444be",               "IO...", 3,            12},
  {"bgr444le",               "IO...", 3,            12},
  {"bgr444be",               "IO...", 3,            12},
  {"ya8",                    "IO...", 2,            16},
  {"bgr48be",                "IO...", 3,            48},
  {"bgr48le",                "IO...", 3,            48},
  {"yuv420p9be",             "IO...", 3,            13},
  {"yuv420p9le",             "IO...", 3,            13},
  {"yuv420p10be",            "IO...", 3,            15},
  {"yuv420p10le",            "IO...", 3,            15},
  {"yuv422p10be",            "IO...", 3,            20},
  {"yuv422p10le",            "IO...", 3,            20},
  {"yuv444p9be",             "IO...", 3,            27},
  {"yuv444p9le",             "IO...", 3,            27},
  {"yuv444p10be",            "IO...", 3,            30},
  {"yuv444p10le",            "IO...", 3,            30},
  {"yuv422p9be",             "IO...", 3,            18},
  {"yuv422p9le",             "IO...", 3,            18},
  {"gbrp",                   "IO...", 3,            24},
  {"gbrp9be",                "IO...", 3,            27},
  {"gbrp9le",                "IO...", 3,            27},
  {"gbrp10be",               "IO...", 3,            30},
  {"gbrp10le",               "IO...", 3,            30},
  {"gbrp16be",               "IO...", 3,            48},
  {"gbrp16le",               "IO...", 3,            48},
  {"yuva422p",               "IO...", 4,            24},
  {"yuva444p",               "IO...", 4,            32},
  {"yuva420p9be",            "IO...", 4,            22},
  {"yuva420p9le",            "IO...", 4,            22},
  {"yuva422p9be",            "IO...", 4,            27},
  {"yuva422p9le",            "IO...", 4,            27},
  {"yuva444p9be",            "IO...", 4,            36},
  {"yuva444p9le",            "IO...", 4,            36},
  {"yuva420p10be",           "IO...", 4,            25},
  {"yuva420p10le",           "IO...", 4,            25},
  {"yuva422p10be",           "IO...", 4,            30},
  {"yuva422p10le",           "IO...", 4,            30},
  {"yuva444p10be",           "IO...", 4,            40},
  {"yuva444p10le",           "IO...", 4,            40},
  {"yuva420p16be",           "IO...", 4,            40},
  {"yuva420p16le",           "IO...", 4,            40},
  {"yuva422p16be",           "IO...", 4,            48},
  {"yuva422p16le",           "IO...", 4,            48},
  {"yuva444p16be",           "IO...", 4,            64},
  {"yuva444p16le",           "IO...", 4,            64},
  {"vdpau",                  "..H..", 0,             0},
  {"xyz12le",                "IO...", 3,            36},
  {"xyz12be",                "IO...", 3,            36},
  {"nv16",                   ".....", 3,            16},
  {"nv20le",                 ".....", 3,            20},
  {"nv20be",                 ".....", 3,            20},
  {"rgba64be",               "IO...", 4,            64},
  {"rgba64le",               "IO...", 4,            64},
  {"bgra64be",               "IO...", 4,            64},
  {"bgra64le",               "IO...", 4,            64},
  {"yvyu422",                "IO...", 3,            16},
  {"ya16be",                 "IO...", 2,            32},
  {"ya16le",                 "IO...", 2,            32},
  {"gbrap",                  "IO...", 4,            32},
  {"gbrap16be",              "IO...", 4,            64},
  {"gbrap16le",              "IO...", 4,            64},
  {"qsv",                    "..H..", 0,             0},
  {"mmal",                   "..H..", 0,             0},
  {"d3d11va_vld",            "..H..", 0,             0},
  {"cuda",                   "..H..", 0,             0},
  {"0rgb",                   "IO...", 3,            24},
  {"rgb0",                   "IO...", 3,            24},
  {"0bgr",                   "IO...", 3,            24},
  {"bgr0",                   "IO...", 3,            24},
  {"yuv420p12be",            "IO...", 3,            18},
  {"yuv420p12le",            "IO...", 3,            18},
  {"yuv420p14be",            "IO...", 3,            21},
  {"yuv420p14le",            "IO...", 3,            21},
  {"yuv422p12be",            "IO...", 3,            24},
  {"yuv422p12le",            "IO...", 3,            24},
  {"yuv422p14be",            "IO...", 3,            28},
  {"yuv422p14le",            "IO...", 3,            28},
  {"yuv444p12be",            "IO...", 3,            36},
  {"yuv444p12le",            "IO...", 3,            36},
  {"yuv444p14be",            "IO...", 3,            42},
  {"yuv444p14le",            "IO...", 3,            42},
  {"gbrp12be",               "IO...", 3,            36},
  {"gbrp12le",               "IO...", 3,            36},
  {"gbrp14be",               "IO...", 3,            42},
  {"gbrp14le",               "IO...", 3,            42},
  {"yuvj411p",               "IO...", 3,            12},
  {"bayer_bggr8",            "I....", 3,             8},
  {"bayer_rggb8",            "I....", 3,             8},
  {"bayer_gbrg8",            "I....", 3,             8},
  {"bayer_grbg8",            "I....", 3,             8},
  {"bayer_bggr16le",         "I....", 3,            16},
  {"bayer_bggr16be",         "I....", 3,            16},
  {"bayer_rggb16le",         "I....", 3,            16},
  {"bayer_rggb16be",         "I....", 3,            16},
  {"bayer_gbrg16le",         "I....", 3,            16},
  {"bayer_gbrg16be",         "I....", 3,            16},
  {"bayer_grbg16le",         "I....", 3,            16},
  {"bayer_grbg16be",         "I....", 3,            16},
  {"xvmc",                   "..H..", 0,             0},
  {"yuv440p10le",            "IO...", 3,            20},
  {"yuv440p10be",            "IO...", 3,            20},
  {"yuv440p12le",            "IO...", 3,            24},
  {"yuv440p12be",            "IO...", 3,            24},
  {"ayuv64le",               "IO...", 4,            64},
  {"ayuv64be",               ".....", 4,            64},
  {"videotoolbox_vld",       "..H..", 0,             0},
  {"p010le",                 "IO...", 3,            15},
  {"p010be",                 "IO...", 3,            15},
  {"gbrap12be",              "IO...", 4,            48},
  {"gbrap12le",              "IO...", 4,            48},
  {"gbrap10be",              "IO...", 4,            40},
  {"gbrap10le",              "IO...", 4,            40},
  {"mediacodec",             "..H..", 0,             0},
  {"gray12be",               "IO...", 1,            12},
  {"gray12le",               "IO...", 1,            12},
  {"gray10be",               "IO...", 1,            10},
  {"gray10le",               "IO...", 1,            10},
  {"p016le",                 "IO...", 3,            24},
  {"p016be",                 "IO...", 3,            24},
  {"d3d11",                  "..H..", 0,             0},
  {"gray9be",                "IO...", 1,             9},
  {"gray9le",                "IO...", 1,             9},
  {"gbrpf32be",              "IO...", 3,            96},
  {"gbrpf32le",              "IO...", 3,            96},
  {"gbrapf32be",             "IO...", 4,           128},
  {"gbrapf32le",             "IO...", 4,           128},
  {"drm_prime",              "..H..", 0,             0},
  {"opencl",                 "..H..", 0,             0},
  {"gray14be",               "IO...", 1,            14},
  {"gray14le",               "IO...", 1,            14},
  {"grayf32be",              "IO...", 1,            32},
  {"grayf32le",              "IO...", 1,            32},
  {"yuva422p12be",           "IO...", 4,            36},
  {"yuva422p12le",           "IO...", 4,            36},
  {"yuva444p12be",           "IO...", 4,            48},
  {"yuva444p12le",           "IO...", 4,            48},
  {"nv24",                   "IO...", 3,            24},
  {"nv42",                   "IO...", 3,            24},
  {"vulkan",                 "..H..", 0,             0},
  {"y210be",                 ".....", 3,            20},
  {"y210le",                 "I....", 3,            20},
  {{0}, {0}, 0, 0}
};


static const ffmpeg_pixel_format* ffmpeg_get_format(ffmpeg_pixfmt pf) {
  const ffmpeg_pixel_format* fmt;
  for (fmt = ffmpeg_pixel_formats; fmt->name[0] != '\0'; ++fmt) {
    if (memcmp(fmt->name, pf.s, sizeof(ffmpeg_pixfmt)) == 0) {
      break;
    }
  }
  return fmt;
}
size_t ffmpeg_pixel_bitsize(ffmpeg_pixfmt fmt) {
  int bits = ffmpeg_get_format(fmt)->bits;
  return bits;
}
size_t ffmpeg_pixel_size(ffmpeg_pixfmt fmt) {
  int bits = ffmpeg_get_format(fmt)->bits;
  if (bits % 8 != 0) return 0;
  return bits / 8;
}
size_t ffmpeg_pixel_nchannel(ffmpeg_pixfmt fmt) {
  int channels = ffmpeg_get_format(fmt)->channels;
  return channels;
}

const char* ffmpeg_pixfmt2str(const ffmpeg_pixfmt* fmt) {
  if (fmt->s[0] == '\0') return "[unknown]";
  return fmt->s;
}
ffmpeg_pixfmt ffmpeg_str2pixfmt(const char* str) {
  ffmpeg_pixfmt fmt;
  memset(&fmt, 0, sizeof(fmt));
  if (str != NULL) {
    // defines some aliases
    if (strcmp(str, "gray8") == 0) {
      str = "gray";
    } else if (strcmp(str, "rgb888") == 0) {
      str = "rgb24";
    } else if (strcmp(str, "rgba8888") == 0) {
      str = "rgba";
    }
    strncpy(fmt.s, str, sizeof(fmt.s)-1);
  }
  return fmt;
}
const char* ffmpeg_codec2str(const ffmpeg_codec* codec) {
  if (codec->s[0] == '\0') return "raw";
  return codec->s;
}
ffmpeg_codec ffmpeg_str2codec(const char* str) {
  ffmpeg_codec codec;
  memset(&codec, 0, sizeof(codec));
  if (str != NULL) {
    if (strcmp(str, "raw") == 0 || strcmp(str, "rawvideo") == 0) {
      str = NULL;
    }
  }
  if (str != NULL ) {
    strncpy(codec.s, str, sizeof(codec.s)-1);
  }
  return codec;
}
const char* ffmpeg_fileformat2str(const ffmpeg_fileformat* fmt) {
  if (fmt->s[0] == '\0') return "[undefined]";
  return fmt->s;
}
ffmpeg_fileformat ffmpeg_str2fileformat(const char* str) {
  ffmpeg_fileformat fmt;
  memset(&fmt, 0, sizeof(fmt));
  if (str != NULL ) {
    strncpy(fmt.s, str, sizeof(fmt.s)-1);
  }
  return fmt;
}

const char* ffmpeg_error2str(ffmpeg_error err) {
  switch (err) {
    case ffmpeg_noerror:
      return "";
    case ffmpeg_ffprobe_invalid_section_start:
      return "ffprobe_invalid_section_start";
    case ffmpeg_ffprobe_invalid_section_stop:
      return "ffprobe_invalid_section_stop";
    case ffmpeg_ffprobe_invalid_statement:
      return "ffprobe_invalid_statement";
    case ffmpeg_ffprobe_unclosed_section:
      return "ffprobe_unclosed_section";
    case ffmpeg_ffprobe_multiple_codec:
      return "ffprobe_multiple_codec";
    case ffmpeg_ffprobe_multiple_width:
      return "ffprobe_multiple_width";
    case ffmpeg_ffprobe_multiple_height:
      return "ffprobe_multiple_height";
    case ffmpeg_ffprobe_multiple_framerate:
      return "ffprobe_multiple_framerate";
    case ffmpeg_ffprobe_multiple_pixfmt:
      return "ffprobe_multiple_pixfmt";
    case ffmpeg_ffprobe_no_codec:
      return "ffprobe_no_codec";
    case ffmpeg_ffprobe_no_width:
      return "ffprobe_no_width";
    case ffmpeg_ffprobe_no_height:
      return "ffprobe_no_height";
    case ffmpeg_ffprobe_no_pixfmt:
      return "ffprobe_no_pixfmt";
    case ffmpeg_ffprobe_no_video_section:
      return "ffprobe_no_video_section";
    case ffmpeg_pipe_error:
      return "pipe_error";
    case ffmpeg_invalid_width:
      return "invalid_width";
    case ffmpeg_invalid_height:
      return "invalid_height";
    case ffmpeg_invalid_framerate:
      return "invalid_framerate";
    case ffmpeg_invalid_codec:
      return "invalid_codec";
    case ffmpeg_invalid_pixfmt:
      return "invalid_pixfmt";
    case ffmpeg_closed_pipe:
      return "closed_pipe";
    case ffmpeg_eof_error:
      return "eof";
    case ffmpeg_partial_read:
      return "partial_read";
    case ffmpeg_missing_ffmpeg:
      return "missing_ffmpeg";
    case ffmpeg_missing_ffprobe:
      return "missing_ffprobe";
    case ffmpeg_missing_ffplay:
      return "missing_ffplay";
    case ffmpeg_unknown_error:
      return "[unknown error]";
  }
  return "[unknown error]";
}

void ffmpeg_init(ffmpeg_handle* h) {
  memset(h, 0, sizeof(ffmpeg_handle));
}

void ffmpeg_options_init(ffmpeg_options* o) {
  memset(o, 0, sizeof(ffmpeg_options));
}

int ffmpeg_valid_descriptor(const ffmpeg_descriptor* p, ffmpeg_error* e) {
  ffmpeg_error error_sink;
  if (e == NULL) e = &error_sink;
  if (p->width == 0) {
    *e = ffmpeg_invalid_width;
    return 0;
  }
  if (p->height == 0) {
    *e = ffmpeg_invalid_height;
    return 0;
  }
  if ((p->framerate.num == 0 || p->framerate.den == 0) && !(p->framerate.num == 0 && p->framerate.den == 0)) {
    *e = ffmpeg_invalid_framerate;
    return 0;
  }
  if (p->pixfmt.s[0] == '\0') {
    *e = ffmpeg_invalid_pixfmt;
    return 0;
  }
  return 1;
}
void ffmpeg_merge_descriptor(ffmpeg_descriptor* out, const ffmpeg_descriptor* in) {
  if (out->width == 0) out->width = in->width;
  if (out->height == 0) out->height = in->height;
  if (out->framerate.num == 0 || out->framerate.den == 0) out->framerate = in->framerate;
  if (out->pixfmt.s[0] == '\0') out->pixfmt = in->pixfmt;
}
void ffmpeg_compatible_writer(ffmpeg_handle* writer, const ffmpeg_handle* reader) {
  writer->input = reader->output;
  ffmpeg_merge_descriptor(&writer->input, &reader->input);
}
