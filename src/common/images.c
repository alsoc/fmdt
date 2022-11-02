#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <nrc2.h>

#include "fmdt/images.h"

static char *readitem(FILE *file, char *buffer) {
    int k = 0;
    char *aux = buffer;
    while (!feof(file)) {
        *aux = fgetc(file);
        switch(k) {
        case 0:
            if (*aux == '#') k = 1;
            if (isalnum(*aux)) k = 2, aux++;
            break;
        case 1:
            if (*aux == 0xA) k = 0;
            break;
        case 2:
            if (!isalnum(*aux)) {
                *aux = 0;
                return buffer;
            }
            aux++;
            break;
        }
    }
    *aux = 0;
    return buffer;
}

static int count_files(const char *dir) {
    size_t count = 0;
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;
    if ((dp = opendir(dir)) == NULL) {
        fprintf(stderr, "(EE) Cannot open directory: '%s'\n", dir);
        return -1;
    }
    chdir(dir);
    while ((entry = readdir(dp)) != NULL) {
        lstat(entry->d_name,&statbuf);
        if (!S_ISDIR(statbuf.st_mode)) {
            count++;
        }
    }
    chdir("..");
    closedir(dp);

    return count;
}

static void fill_path_files(const char *dir, const size_t start, const size_t end, char** path_files,
                            const size_t count) {
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;
    if ((dp = opendir(dir)) == NULL) {
        fprintf(stderr, "(EE) Cannot open directory: '%s'\n", dir);
        return;
    }
    chdir(dir);
    while ((entry = readdir(dp)) != NULL) {
        lstat(entry->d_name,&statbuf);
        if (!S_ISDIR(statbuf.st_mode)) {
            int n = atoi(entry->d_name);
            if (n >= start && n < count && n < (end + 1)) {
                char path[2048];
                sprintf(path, "%s/%s", dir, entry->d_name);
                path_files[n - start] = (char*)malloc((strlen(path) +1) * sizeof(char));
                strcpy(path_files[n - start], path);
            }
        }
    }
    chdir("..");
    closedir(dp);
}

static void read_PGM_metadata(const char *filename, size_t *nrl, size_t *nrh, size_t *ncl, size_t *nch) {
    FILE *file = fopen(filename,"rb");
    if (file == NULL) {
        fprintf(stderr, "(EE) Cannot open '%s' file, exiting.\n", filename);
        exit(-1);
    }

    char buffer[80];
    readitem(file, buffer);
    if (strcmp(buffer, "P5") != 0) {
        fprintf(stderr, "(EE) Wrong file header, PGM 'P5' is expected.\n");
        exit(-1);
    }

    int width  = atoi(readitem(file, buffer));
    int height = atoi(readitem(file, buffer));

    *nrl = 0;
    *nrh = (size_t)(height - 1);
    *ncl = 0;
    *nch = (size_t)(width - 1);

    fclose(file);
}

images_t* images_init_from_path(const char* path, const size_t start, const size_t end, const size_t skip) {
    images_t* images = (images_t*)malloc(sizeof(images_t));

    images->frame_start = start;
    images->frame_end = end;
    images->frame_skip = skip;
    images->frame_current = 0;

    int count = count_files(path);
    images->files_count = count < (end + 1) ? (size_t)count - start : (size_t)(end + 1) - start;

    images->path_files = (char**)malloc(images->files_count * sizeof(char**));
    fill_path_files(path, start, end, images->path_files, count);

    images->i0 = images->i1 = images->j0 = images->j1 = 0;
    size_t i0_tmp, i1_tmp, j0_tmp, j1_tmp;
    for (int i = 0; i < images->files_count; i += 1 + skip) {
        read_PGM_metadata(images->path_files[i], &i0_tmp, &i1_tmp, &j0_tmp, &j1_tmp);
        if (images->i0 == 0 && images->i1 == 0 && images->j0 == 0 && images->j1 == 0) {
            images->i0 = i0_tmp;
            images->i1 = i1_tmp;
            images->j0 = j0_tmp;
            images->j1 = j1_tmp;
        } else if (images->i0 != i0_tmp || images->i1 != i1_tmp || images->j0 != j0_tmp || images->j1 != j1_tmp) {
            fprintf(stderr, "(EE) Size of image '%s' is different from other images "
                            "(images->i0 = %lu, i0_tmp = %lu, images->i1 = %lu, i1_tmp = %lu, images->j0 = %lu, "
                            "j0_tmp = %lu, images->j1 = %lu, j1_tmp = %lu).\n",
                            images->path_files[i], images->i0, i0_tmp, images->i1, i1_tmp, images->j0, j0_tmp,
                            images->j1, j1_tmp);
            exit(-1);
        }
    }
    return images;
}

int images_get_next_frame(images_t* images, uint8_t** I) {
    size_t cur_fra = images->frame_current;

    if (images->frame_current < images->files_count) {
        MLoadPGM_ui8matrix(images->path_files[images->frame_current], images->i0, images->i1, images->j0, images->j1,
                           I);
        images->frame_current += 1 + images->frame_skip;
        return images->frame_start + images->frame_current;
    }
    else
        return 0;
}

void images_free(images_t* images) {
    for (int i = 0; i < images->files_count; i++)
        free(images->path_files[i]);
    free(images->path_files);
    free(images);
}
