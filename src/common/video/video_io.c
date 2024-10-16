#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <nrc2.h>

#include "fmdt/video/video_io.h"

#define MAX_BUFF_SIZE 16384

#if FMDT_USE_FFMPEG_IO

#include <ffmpeg-io/common.h>
#include <ffmpeg-io/writer.h>
#include <ffmpeg-io/reader.h>
#include <ffmpeg-io/player.h>

typedef struct {
    ffmpeg_options ffmpeg_opts; /*!< FFMPEG options. */
    ffmpeg_handle ffmpeg; /*!< FFMPEG handle. */
    char hwaccel[2048]; /*!< Hardware acceleration string. */
} video_metadata_ffio_t;

video_reader_t* video_reader_ffio_alloc_init(const char* path, const size_t start, const size_t end, const size_t skip,
                                             const int bufferize, const size_t n_ffmpeg_threads,
                                             const enum video_codec_hwaccel_e hwaccel, const enum pixfmt_e pixfmt,
                                             const uint8_t ffmpeg_debug, const char* ffmpeg_in_extra_opts, int* i0,
                                             int* i1, int* j0, int* j1) {
    assert(!end || start <= end);
    video_reader_t* video = (video_reader_t*)malloc(sizeof(video_reader_t));
    if (!video) {
        fprintf(stderr, "(EE) can't allocate video structure\n");
        exit(1);
    }

    snprintf(video->path, sizeof(video->path), "%s", path);

    video->codec_type = VCDC_FFMPEG_IO;
    video_metadata_ffio_t* metadata = (video_metadata_ffio_t*)malloc(sizeof(video_metadata_ffio_t));
    video->metadata = (void*)metadata;

    ffmpeg_options_init(&metadata->ffmpeg_opts);
    metadata->ffmpeg_opts.debug = ffmpeg_debug;
    if (ffmpeg_in_extra_opts)
        metadata->ffmpeg_opts.extra_input_options = ffmpeg_in_extra_opts;
    if (n_ffmpeg_threads)
        metadata->ffmpeg_opts.threads_input = n_ffmpeg_threads;
    if (start)
        metadata->ffmpeg_opts.start_number = start;
    if (end)
        metadata->ffmpeg_opts.vframes = (end - start) + 1;

    switch(hwaccel) {
        case VCDC_HWACCEL_VIDEOTOOLBOX:
            metadata->ffmpeg_opts.hwaccel_dec = ffmpeg_hwaccel_videotoolbox;
            break;
        case VCDC_HWACCEL_NVDEC:
            metadata->ffmpeg_opts.hwaccel_dec = ffmpeg_hwaccel_nvdec;
            break;
        default:
            break;
    }

    ffmpeg_init(&metadata->ffmpeg);
    if (!ffmpeg_probe(&metadata->ffmpeg, video->path, &metadata->ffmpeg_opts)) {
        fprintf(stderr, "(EE) can't open file %s\n", video->path);
        free(video);
        exit(1);
    }

    video->frame_start = start;
    video->frame_end = end;
    video->frame_skip = skip;
    video->frame_current = 0;

    uint8_t enable_color;
    switch (pixfmt) {
        case PIXFMT_RGB24:
            metadata->ffmpeg.input.pixfmt = ffmpeg_str2pixfmt("rgb24");
            enable_color = 1;
            break;
        case PIXFMT_GRAY8:
            metadata->ffmpeg.input.pixfmt = ffmpeg_str2pixfmt("gray");
            enable_color = 0;
            break;
        default:
            fprintf(stderr, "(EE) Unsupported pixel format.\n");
            exit(1);
    }
    int pixsize = ffmpeg_pixel_size(metadata->ffmpeg.input.pixfmt);

    if (!ffmpeg_start_reader(&metadata->ffmpeg, video->path, &metadata->ffmpeg_opts)) {
        fprintf(stderr, "(EE) can't read file %s\n", video->path);
        free(video);
        exit(1);
    }

    *i0 = 0;
    *j0 = 0;
    *i1 = metadata->ffmpeg.input.height - 1;
    *j1 = metadata->ffmpeg.input.width - 1;

    video->fra_buffer = NULL;
    video->fra_count = 0;

    video->cur_loop = 1;
    video->loop_size = 1;

    int frame_id = 0;
    if (bufferize) {
        uint8_t*** fra_buffer_tmp;
        fra_buffer_tmp = (uint8_t***)malloc(MAX_BUFF_SIZE * sizeof(uint8_t***));
        do {
            if (video->fra_count >= MAX_BUFF_SIZE) {
                fprintf(stderr, "(EE) 'video->fra_count' has to be smaller than 'MAX_BUFF_SIZE' "
                                "('video->fra_count' = %d, 'MAX_BUFF_SIZE' = %d\n)", (int)video->fra_count,
                                (int)MAX_BUFF_SIZE);
                exit(-1);
            }
            uint8_t **I = ui8matrix(*i0, *i1, *j0, *j1 * pixsize);
            frame_id = video_reader_get_frame(video, enable_color ? NULL : I, enable_color ? I : NULL);
            if (frame_id != -1) {
                fra_buffer_tmp[video->fra_count -1] = I;
            } else
                free_ui8matrix(I, *i0, *i1, *j0, *j1 * pixsize);
        } while (frame_id != -1);

        video->fra_buffer = fra_buffer_tmp;
        video->frame_current = 0;
        video->cur_loop = 1;
    }

    return video;
}

static int _video_reader_ffio_get_frame(video_reader_t* video, uint8_t** img_gray8_or_rgb24) {
    video_metadata_ffio_t* metadata = (video_metadata_ffio_t*)video->metadata;

    if (video->frame_end && video->frame_start + video->frame_current > video->frame_end)
        return -1;

    if (!ffmpeg_read2d(&metadata->ffmpeg, img_gray8_or_rgb24)) {
        if (metadata->ffmpeg.error != 22) // 22 == EOF
            fprintf(stderr, "(EE) %s\n", ffmpeg_error2str(metadata->ffmpeg.error));
        return -1;
    }

    int cur_fra = (int)video->frame_current;
    video->frame_current++;
    return cur_fra;
}

void _video_convert_rgb24_to_gray8(const uint8_t** img_rgb24, uint8_t** img_gray8, const size_t img_height,
                                   const size_t img_width)
{
    for (unsigned h = 0; h < img_height; h++)
        for (unsigned w = 0; w < img_width; w++)
            img_gray8[h][w] = (uint8_t)(0.2126f * (float)img_rgb24[h][w * 3 + 0] +
                                        0.7152f * (float)img_rgb24[h][w * 3 + 1] +
                                        0.0722f * (float)img_rgb24[h][w * 3 + 2]);
}

int video_reader_ffio_get_frame(video_reader_t* video, uint8_t** img_gray8, uint8_t** img_rgb24) {
    assert(video->codec_type == VCDC_FFMPEG_IO);
    video_metadata_ffio_t* metadata = (video_metadata_ffio_t*)video->metadata;
    uint8_t is_rgb = !strncmp(metadata->ffmpeg.input.pixfmt.s, "rgb24", sizeof(metadata->ffmpeg.input.pixfmt.s));

    if (is_rgb && img_rgb24 == NULL) {
        fprintf(stderr, "(EE) In RGB mode, 'img_rgb24' cannot be NULL\n");
        exit(1);
    } else if (!is_rgb && img_gray8 == NULL) {
        fprintf(stderr, "(EE) In GRAY8 mode, 'img_gray8' cannot be NULL\n");
        exit(1);
    }

retry:
    if (video->fra_buffer == NULL) {
        int r;
        size_t skip = video->frame_current == 0 ? 0 : video->frame_skip;
        do {
            r = _video_reader_ffio_get_frame(video, is_rgb ? img_rgb24 : img_gray8);
            // restart reader
            if (r == -1 && video->cur_loop < video->loop_size) {
                video->cur_loop++;
                video->frame_current = 0;
                ffmpeg_stop_reader(&metadata->ffmpeg);
                if (!ffmpeg_start_reader(&metadata->ffmpeg, video->path, &metadata->ffmpeg_opts)) {
                    fprintf(stderr, "(EE) can't open file %s\n", video->path);
                    exit(1);
                }
                goto retry;
            }
        } while ((r != -1) && skip--);
        if (video->cur_loop == 1 && r != -1)
            video->fra_count++;

        if (is_rgb && img_gray8)
            _video_convert_rgb24_to_gray8((const uint8_t**)img_rgb24, img_gray8, metadata->ffmpeg.input.height,
                                          metadata->ffmpeg.input.width);

        return (r == -1) ? r : video->frame_start + r + (video->cur_loop -1) * video->fra_count *
                               (1 + video->frame_skip);
    } else {
        if (video->frame_current < video->fra_count || video->cur_loop < video->loop_size) {
            if (video->frame_current == video->fra_count) {
                video->cur_loop++;
                video->frame_current = 0;
            }
            size_t pixsize = ffmpeg_pixel_size(metadata->ffmpeg.input.pixfmt);
            for (unsigned l = 0; l < metadata->ffmpeg.input.height; l++)
                memcpy(is_rgb ? img_rgb24[l] : img_gray8[l],
                       video->fra_buffer[video->frame_current][l],
                       metadata->ffmpeg.input.width * pixsize);
            int cur_fra = video->frame_start + (video->frame_current + (video->cur_loop -1) * video->fra_count) *
                          (1 + video->frame_skip);
            video->frame_current++;

            if (is_rgb && img_gray8)
                _video_convert_rgb24_to_gray8((const uint8_t**)img_rgb24, img_gray8, metadata->ffmpeg.input.height,
                                              metadata->ffmpeg.input.width);

            return cur_fra;
        } else
            return -1;
    }
}

void video_reader_ffio_free(video_reader_t* video) {
    assert(video->codec_type == VCDC_FFMPEG_IO);
    video_metadata_ffio_t* metadata = (video_metadata_ffio_t*)video->metadata;
    ffmpeg_stop_reader(&metadata->ffmpeg);
    size_t pixsize = ffmpeg_pixel_size(metadata->ffmpeg.input.pixfmt);
    if (video->fra_buffer) {
        for (size_t i = 0; i < video->fra_count; i++)
            free_ui8matrix(video->fra_buffer[i], 0, metadata->ffmpeg.input.height - 1, 0,
                           (metadata->ffmpeg.input.width - 1) * pixsize);
        free(video->fra_buffer);
    }
    free(metadata);
    free(video);
}

static int ffmpeg_start_writer_or_player(ffmpeg_handle* handle, const char* filename, const ffmpeg_options* opt,
                                         const int win_play) {
    if (!win_play) {
        assert(filename);
        return ffmpeg_start_writer(handle, filename, opt);
    } else
        return ffmpeg_start_player(handle, opt);
}

static int ffmpeg_write2d_or_play2d(ffmpeg_handle* handle, uint8_t** data, const int win_play) {
    if (!win_play)
        return ffmpeg_write2d(handle, data);
    else
        return ffmpeg_play2d(handle, data);
}

static int ffmpeg_stop_writer_or_player(ffmpeg_handle* handle, const int win_play) {
    if (!win_play)
        return ffmpeg_stop_writer(handle);
    else
        return ffmpeg_stop_player(handle);
}

video_writer_t* video_writer_ffio_alloc_init(const char* path, const size_t start, const size_t n_ffmpeg_threads,
                                             const size_t img_height, const size_t img_width,
                                             const enum pixfmt_e pixfmt, const int win_play, const uint8_t ffmpeg_debug,
                                             const char* ffmpeg_out_extra_opts) {
    video_writer_t* video = (video_writer_t*)malloc(sizeof(video_writer_t));
    if (!video) {
        fprintf(stderr, "(EE) can't allocate video structure\n");
        exit(1);
    }

    snprintf(video->path, sizeof(video->path), "%s", path);

    video->codec_type = VCDC_FFMPEG_IO;
    video->win_play = win_play;
    video_metadata_ffio_t* metadata = (video_metadata_ffio_t*)malloc(sizeof(video_metadata_ffio_t));
    video->metadata = (void*)metadata;

    ffmpeg_options_init(&metadata->ffmpeg_opts);
    metadata->ffmpeg_opts.debug = ffmpeg_debug;
    if (ffmpeg_out_extra_opts)
        metadata->ffmpeg_opts.extra_output_options = ffmpeg_out_extra_opts;
    if (start)
        metadata->ffmpeg_opts.start_number = start;
    if (n_ffmpeg_threads)
        metadata->ffmpeg_opts.threads_input = n_ffmpeg_threads;

    ffmpeg_init(&metadata->ffmpeg);

    metadata->ffmpeg.input.width = img_width;
    metadata->ffmpeg.input.height = img_height;

    switch (pixfmt) {
        case PIXFMT_RGB24:
            metadata->ffmpeg.input.pixfmt = ffmpeg_str2pixfmt("rgb24");
            break;
        case PIXFMT_GRAY8:
            metadata->ffmpeg.input.pixfmt = ffmpeg_str2pixfmt("gray");
            break;
        default:
            fprintf(stderr, "(EE) Unsupported pixel format.\n");
            exit(1);
    }

    if (!ffmpeg_start_writer_or_player(&metadata->ffmpeg, video->path, &metadata->ffmpeg_opts, video->win_play)) {
        fprintf(stderr, "(EE) can't open file %s\n", video->path);
        free(video);
        exit(1);
    }

    return video;
}

void video_writer_ffio_save_frame(video_writer_t* video, const uint8_t** img) {
    assert(video->codec_type == VCDC_FFMPEG_IO);
    video_metadata_ffio_t* metadata = (video_metadata_ffio_t*)video->metadata;
    if (!ffmpeg_write2d_or_play2d(&metadata->ffmpeg, (uint8_t**)img, video->win_play)) {
        fprintf(stderr, "(EE) ffmpeg_write2d_or_play2d: %s\n", ffmpeg_error2str(metadata->ffmpeg.error));
        exit(-1);
    }
}

void video_writer_ffio_free(video_writer_t* video) {
    assert(video->codec_type == VCDC_FFMPEG_IO);
    video_metadata_ffio_t* metadata = (video_metadata_ffio_t*)video->metadata;
    ffmpeg_stop_writer_or_player(&metadata->ffmpeg, video->win_play);
    free(metadata);
    free(video);
}

#endif

#if FMDT_USE_VCODECS_IO

#include <vcodecs-io/vcodecs-io.h>

typedef struct {
    vcio_options_t vcio_opts;
    vcio_reader_t reader;
} video_metadata_vcio_t;

static int _video_reader_vcio_get_frame(video_reader_t* video, uint8_t** img);
int video_reader_vcio_get_frame(video_reader_t* video, uint8_t** img);

video_reader_t* video_reader_vcio_alloc_init(const char* path, const size_t start, const size_t end, const size_t skip,
                                             const int bufferize, const size_t n_ffmpeg_threads,
                                             const enum video_codec_hwaccel_e hwaccel, int* i0, int* i1, int* j0,
                                             int* j1) {
    assert(!end || start <= end);
    video_reader_t* video = (video_reader_t*)malloc(sizeof(video_reader_t));
    if (!video) {
        fprintf(stderr, "(EE) can't allocate video structure\n");
        exit(1);
    }

    if (hwaccel != VCDC_HWACCEL_NONE) {
        fprintf(stderr, "(EE) Only 'VCDC_HWACCEL_NONE' is supported at this time.\n");
        exit(1);
    }
    
    snprintf(video->path, sizeof(video->path), "%s", path);

    video->codec_type = VCDC_VCODECS_IO;
    video_metadata_vcio_t* metadata = (video_metadata_vcio_t*)malloc(sizeof(video_metadata_vcio_t));
    video->metadata = (void*)metadata;

    vcio_options_init(&metadata->vcio_opts);
    if (n_ffmpeg_threads) {
        metadata->vcio_opts.thread_count = n_ffmpeg_threads;
    }
    if (start) {
        metadata->vcio_opts.start_number = start;
    }
    if (end) {
        metadata->vcio_opts.vframes = (end - start) + 1; // currently unused by vcio (it's mostly synchronous)
    }

    int status = vcio_reader_alloc(&metadata->reader, video->path, &metadata->vcio_opts);
    if (status < 0) {
        fprintf(stderr, "(EE) can't open file %s\n", video->path);
        free(video);
        exit(1);
    }

    video->frame_start = start;
    video->frame_end = end;
    video->frame_skip = skip;
    video->frame_current = 0;
    // Implicit pix format

    *i0 = 0;
    *j0 = 0;
    *i1 = metadata->reader.height - 1;
    *j1 = metadata->reader.width - 1;

    video->fra_buffer = NULL;
    video->fra_count = 0;

    video->cur_loop = 1;
    video->loop_size = 1;

    int frame_id = 0;
    if (bufferize) {
        fprintf(stdout, "Bufferizing image sequence\n");
        uint8_t*** fra_buffer_tmp;
        fra_buffer_tmp = (uint8_t***)malloc(MAX_BUFF_SIZE * sizeof(uint8_t***));
        do {
            if (video->fra_count >= MAX_BUFF_SIZE) {
                fprintf(stderr, "(EE) 'video->fra_count' has to be smaller than 'MAX_BUFF_SIZE' "
                                "('video->fra_count' = %d, 'MAX_BUFF_SIZE' = %d\n)", (int)video->fra_count,
                                (int)MAX_BUFF_SIZE);
                exit(-1);
            }
            uint8_t **I = ui8matrix(*i0, *i1, *j0, *j1);
            frame_id = video_reader_vcio_get_frame(video, I);
            if (frame_id != -1) {
                fra_buffer_tmp[video->fra_count -1] = I;
            } else
                free_ui8matrix(I, *i0, *i1, *j0, *j1);
        } while (frame_id != -1);
        video->fra_buffer = fra_buffer_tmp;
        video->frame_current = 0;
        video->cur_loop = 1;
    }

    return video;
}

static int _video_reader_vcio_get_frame(video_reader_t* video, uint8_t** img) {
    video_metadata_vcio_t* metadata = (video_metadata_vcio_t*)video->metadata;
    if (video->frame_end && video->frame_start + video->frame_current > video->frame_end) {
        fprintf(stdout, "End of video sequence");
        return -1;
    }

    int status = vcio_read2d(&metadata->reader, img);
    if (status <= 0) {
        if (status < 0) // 0 == EOF
            //fprintf(stderr, "(EE) %s\n", ffmpeg_error2str(video->ffmpeg.error));
            fprintf(stderr, "(EE) Could not read frame\n");
        return -1;
    }

    int cur_fra = (int)video->frame_current;
    video->frame_current++;
    return cur_fra;
}

int video_reader_vcio_get_frame(video_reader_t* video, uint8_t** img) {
    assert(video->codec_type == VCDC_VCODECS_IO);
    video_metadata_vcio_t* metadata = (video_metadata_vcio_t*)video->metadata;
    retry:
    if (video->fra_buffer == NULL) { // Not bufferized
        size_t skip = video->frame_current == 0 ? 0 : video->frame_skip;
        int status;
        int cur_fra = 0;
        do {
            status = _video_reader_vcio_get_frame(video, img);

            // restart reader
            if (status < 0 && video->cur_loop < video->loop_size) {
                video->cur_loop++;
                video->frame_current = 0;

                vcio_reader_seek(&metadata->reader, video->frame_start, 0);
                goto retry;
            }
        } while ((status != -1) && skip--);

        if (video->cur_loop == 1 && status >= 0) {
            video->fra_count++;
        }
        return (status < 0) ? status : video->frame_start + cur_fra + (video->cur_loop) * video->fra_count
            * (1 + video->frame_skip);
    } else {
        if (video->frame_current < video->fra_count || video->cur_loop < video->loop_size) {
            if (video->frame_current == video->fra_count) {
                video->cur_loop++;
                video->frame_current = 0;
            }
            for (unsigned l = 0; l < metadata->reader.height; l++)
                memcpy(img[l], video->fra_buffer[video->frame_current][l], metadata->reader.width);
            int cur_fra = video->frame_start + (video->frame_current + (video->cur_loop -1) * video->fra_count) *
                (1 + video->frame_skip);
            video->frame_current++;
            return cur_fra;
        } else
            return -1;
    }
}

void video_reader_vcio_free(video_reader_t* video) {
    assert(video->codec_type == VCDC_VCODECS_IO);
    video_metadata_vcio_t* metadata = (video_metadata_vcio_t*)video->metadata;
    vcio_reader_free(&metadata->reader);
    if (video->fra_buffer) {
        for (size_t i = 0; i < video->fra_count; i++)
            free_ui8matrix(video->fra_buffer[i], 0, metadata->reader.height - 1, 0, metadata->reader.width - 1);
        free(video->fra_buffer);
    }
    free(metadata);
    free(video);
}

#endif

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

video_reader_t* video_reader_alloc_init(const char* path, const size_t start, const size_t end, const size_t skip,
                                        const int bufferize, const size_t n_ffmpeg_threads,
                                        const enum video_codec_e codec_type, const enum video_codec_hwaccel_e hwaccel,
                                        const enum pixfmt_e pixfmt, const uint8_t ffmpeg_debug,
                                        const char* ffmpeg_in_extra_opts, int* i0, int* i1, int* j0, int* j1) {
    switch (codec_type) {
        case VCDC_FFMPEG_IO: {
#ifdef FMDT_USE_FFMPEG_IO
            return video_reader_ffio_alloc_init(path, start, end, skip, bufferize, n_ffmpeg_threads, hwaccel, pixfmt,
                                                ffmpeg_debug, ffmpeg_in_extra_opts, i0, i1, j0, j1);
            break;
#else
            fprintf(stderr, "(EE) Link with the ffmpeg-io library is required.\n");
            exit(-1);
#endif
        }
        case VCDC_VCODECS_IO: {
#ifdef FMDT_USE_VCODECS_IO
            switch (pixfmt) {
                case PIXFMT_GRAY8:
                    return video_reader_vcio_alloc_init(path, start, end, skip, bufferize, n_ffmpeg_threads, hwaccel,
                                                        i0, i1, j0, j1);
                    break;
                default:
                    fprintf(stderr, "(EE) Unsupported pixel format.\n");
                    exit(1);
            }
            break;
#else
            fprintf(stderr, "(EE) Link with the vcodecs-io library is required.\n");
            exit(-1);
#endif
        }
        default: {
            fprintf(stderr, "(EE) This should never happen.\n");
            exit(-1);
        }
    }
}

int video_reader_get_frame(video_reader_t* video, uint8_t** img_gray8, uint8_t** img_rgb24) {
    switch (video->codec_type) {
        case VCDC_FFMPEG_IO: {
#ifdef FMDT_USE_FFMPEG_IO
            return video_reader_ffio_get_frame(video, img_gray8, img_rgb24);
            break;
#else
            fprintf(stderr, "(EE) Link with the ffmpeg-io library is required.\n");
            exit(-1);
#endif
        }
        case VCDC_VCODECS_IO: {
#ifdef FMDT_USE_VCODECS_IO
            return video_reader_vcio_get_frame(video, img_gray8);
            break;
#else
            fprintf(stderr, "(EE) Link with the vcodecs-io library is required.\n");
            exit(-1);
#endif
        }
        default: {
            fprintf(stderr, "(EE) This should never happen.\n");
            exit(-1);
        }
    }
}

void video_reader_free(video_reader_t* video) {
    switch (video->codec_type) {
        case VCDC_FFMPEG_IO: {
#ifdef FMDT_USE_FFMPEG_IO
            video_reader_ffio_free(video);
            break;
#else
            fprintf(stderr, "(EE) Link with the ffmpeg-io library is required.\n");
            exit(-1);
#endif
        }
        case VCDC_VCODECS_IO: {
#ifdef FMDT_USE_VCODECS_IO
            video_reader_vcio_free(video);
            break;
#else
            fprintf(stderr, "(EE) Link with the vcodecs-io library is required.\n");
            exit(-1);
#endif
        }
        default: {
            fprintf(stderr, "(EE) This should never happen.\n");
            exit(-1);
        }
    }
}

video_writer_t* video_writer_alloc_init(const char* path, const size_t start, const size_t n_ffmpeg_threads,
                                        const size_t img_height, const size_t img_width, const enum pixfmt_e pixfmt,
                                        const enum video_codec_e codec_type, const int win_play,
                                        const uint8_t ffmpeg_debug, const char* ffmpeg_out_extra_opts) {
    switch (codec_type) {
        case VCDC_FFMPEG_IO: {
#ifdef FMDT_USE_FFMPEG_IO
            return video_writer_ffio_alloc_init(path, start, n_ffmpeg_threads, img_height, img_width, pixfmt, win_play,
                                                ffmpeg_debug, ffmpeg_out_extra_opts);
            break;
#else
            fprintf(stderr, "(EE) Link with the ffmpeg-io library is required.\n");
            exit(-1);
#endif
        }
        case VCDC_VCODECS_IO: {
#ifdef FMDT_USE_VCODECS_IO
            fprintf(stderr, "(EE) vcodecs-io is not supported yet for video writer.\n");
            exit(-1);
#else
            fprintf(stderr, "(EE) Link with the vcodecs-io library is required.\n");
            exit(-1);
#endif
        }
        default: {
            fprintf(stderr, "(EE) This should never happen.\n");
            exit(-1);
        }
    }
}

void video_writer_save_frame(video_writer_t* video, const uint8_t** img) {
    switch (video->codec_type) {
        case VCDC_FFMPEG_IO: {
#ifdef FMDT_USE_FFMPEG_IO
            video_writer_ffio_save_frame(video, img);
            break;
#else
            fprintf(stderr, "(EE) Link with the ffmpeg-io library is required.\n");
            exit(-1);
#endif
        }
        case VCDC_VCODECS_IO: {
#ifdef FMDT_USE_VCODECS_IO
            fprintf(stderr, "(EE) vcodecs-io is not supported yet for video writer.\n");
            exit(-1);
#else
            fprintf(stderr, "(EE) Link with the vcodecs-io library is required.\n");
            exit(-1);
#endif
        }
        default: {
            fprintf(stderr, "(EE) This should never happen.\n");
            exit(-1);
        }
    }
}

void video_writer_free(video_writer_t* video) {
    switch (video->codec_type) {
        case VCDC_FFMPEG_IO: {
#ifdef FMDT_USE_FFMPEG_IO
            video_writer_ffio_free(video);
            break;
#else
            fprintf(stderr, "(EE) Link with the ffmpeg-io library is required.\n");
            exit(-1);
#endif
        }
        case VCDC_VCODECS_IO: {
#ifdef FMDT_USE_VCODECS_IO
            fprintf(stderr, "(EE) vcodecs-io is not supported yet for video writer.\n");
            exit(-1);
#else
            fprintf(stderr, "(EE) Link with the vcodecs-io library is required.\n");
            exit(-1);
#endif
        }
        default: {
            fprintf(stderr, "(EE) This should never happen.\n");
            exit(-1);
        }
    }
}
