#include <stdlib.h>
#include <string.h>
#include <ffmpeg-io/writer.h>
#include <ffmpeg-io/reader.h>
#include <nrc2.h>
#include <assert.h>

#include "fmdt/video.h"

#define MAX_BUFF_SIZE 16384

video_t* video_init_from_path(const char* path, const size_t start, const size_t end, const size_t skip,
                              const int bufferize, const size_t n_ffmpeg_threads, int* i0, int* i1, int* j0, int* j1) {
    assert(start <= end);
    video_t* video = (video_t*)malloc(sizeof(video_t));
    if (!video) {
        fprintf(stderr, "(EE) can't allocate video structure\n");
        exit(1);
    }

    snprintf(video->path, sizeof(video->path), "%s", path);

    ffmpeg_options_init(&video->ffmpeg_opts);
    if (n_ffmpeg_threads)
        video->ffmpeg_opts.threads_input = n_ffmpeg_threads;
    if (start)
        video->ffmpeg_opts.start_number = start;
    if (end)
        video->ffmpeg_opts.vframes = (end - start) + 1;

    ffmpeg_init(&video->ffmpeg);
    if (!ffmpeg_probe(&video->ffmpeg, video->path, &video->ffmpeg_opts)) {
        fprintf(stderr, "(EE) can't open file %s\n", video->path);
        free(video);
        exit(1);
    }

    video->frame_start = start;
    video->frame_end = end;
    video->frame_skip = skip;
    video->frame_current = 0;
    video->ffmpeg.output.pixfmt = ffmpeg_str2pixfmt("gray");

    if (!ffmpeg_start_reader(&video->ffmpeg, video->path, &video->ffmpeg_opts)) {
        fprintf(stderr, "(EE) can't open file %s\n", video->path);
        free(video);
        exit(1);
    }

    *i0 = 0;
    *j0 = 0;
    *i1 = video->ffmpeg.input.height - 1;
    *j1 = video->ffmpeg.input.width - 1;

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
            uint8_t **I = ui8matrix(*i0, *i1, *j0, *j1);
            frame_id = video_get_next_frame(video, I);
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

static int video_get_frame(video_t* video, uint8_t** I) {
    if (video->frame_end && video->frame_start + video->frame_current > video->frame_end)
        return -1;

    if (!ffmpeg_read2d(&video->ffmpeg, I)) {
        if (video->ffmpeg.error != 22) // 22 == EOF
            fprintf(stderr, "(EE) %s\n", ffmpeg_error2str(video->ffmpeg.error));
        return -1;
    }

    int cur_fra = (int)video->frame_current;
    video->frame_current++;
    return cur_fra;
}

int video_get_next_frame(video_t* video, uint8_t** I) {
retry:
    if (video->fra_buffer == NULL) {
        int r;
        size_t skip = video->frame_skip;
        do {
            r = video_get_frame(video, I);
            // restart reader
            if (r == -1 && video->cur_loop < video->loop_size) {
                video->cur_loop++;
                video->frame_current = 0;
                ffmpeg_stop_reader(&video->ffmpeg);
                if (!ffmpeg_start_reader(&video->ffmpeg, video->path, &video->ffmpeg_opts)) {
                    fprintf(stderr, "(EE) can't open file %s\n", video->path);
                    exit(1);
                }
                goto retry;
            }
        } while ((r != -1) && skip--);
        if (video->cur_loop == 1 && r != -1)
            video->fra_count++;
        return (r == -1) ? r : video->frame_start + r + (video->cur_loop -1) * video->fra_count *
                               (1 + video->frame_skip);
    } else {
        if (video->frame_current < video->fra_count || video->cur_loop < video->loop_size) {
            if (video->frame_current == video->fra_count) {
                video->cur_loop++;
                video->frame_current = 0;
            }
            for (unsigned l = 0; l < video->ffmpeg.input.height; l++)
                memcpy(I[l], video->fra_buffer[video->frame_current][l], video->ffmpeg.input.width);
            int cur_fra = video->frame_start + (video->frame_current + (video->cur_loop -1) * video->fra_count) *
                          (1 + video->frame_skip);
            video->frame_current++;
            return cur_fra;
        } else
            return -1;
    }
}

void video_free(video_t* video) {
    ffmpeg_stop_reader(&video->ffmpeg);
    if (video->fra_buffer) {
        for (size_t i = 0; i < video->fra_count; i++)
            free_ui8matrix(video->fra_buffer[i], 0, video->ffmpeg.input.height - 1, 0, video->ffmpeg.input.width - 1);
        free(video->fra_buffer);
    }
    free(video);
}
