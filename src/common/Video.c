#include "Video.h"

#include <stdlib.h>

inline Video* Video_init_from_file(char* filename, int start, int end, int skip, int* i0, int* i1, int* j0, int* j1)
{
    Video* video = malloc(sizeof(Video));
    if(!video) {
        fprintf(stderr, "Error : can't allocate Video structure\n");
        exit(1);
    }

    ffmpeg_init(&video->ffmpeg);
    if(!ffmpeg_probe(&video->ffmpeg, filename, NULL)) {
        puts("test");
        fprintf(stderr, "Error : can't open file %s\n", filename);
        free(video);
        exit(1);
    }

    video->frame_start          = start;
    video->frame_end            = end;
    video->frame_skip           = skip;
    video->frame_current        = 0;
    video->ffmpeg.output.pixfmt = ffmpeg_str2pixfmt("gray");
    
    if(!ffmpeg_start_reader(&video->ffmpeg, filename, NULL)) {
        fprintf(stderr, "Error : can't open file %s\n", filename);
        free(video);
        exit(1);
    }

    *i0 = 0;
    *j0 = 0;
    *i1 = video->ffmpeg.input.height - 1;
    *j1 = video->ffmpeg.input.width  - 1;

    return video;
}

static int Video_getFrame(Video* video, uint8** I)
{
    if(video->frame_current > video->frame_end || video->ffmpeg.error || !ffmpeg_read2d(&video->ffmpeg, I)) {
        //fprintf(stderr, "Error: %s\n", ffmpeg_error2str(video->ffmpeg.error));
        return 0;
    }
    video->frame_current++;
    return video->frame_current <= video->frame_end;
}

int Video_nextFrame(Video* video, uint8** I)
{
    int r;
    int skip =  ((video->frame_current < video->frame_start)? video->frame_start-1 : video->frame_skip);
    do {
        r = Video_getFrame(video, I);
    }while(r && skip--);
    return r;
}

void Video_free(Video* video)
{
    ffmpeg_stop_reader(&video->ffmpeg);
    free(video);
}
