/* ---------------- */
/* --- nrutil.h --- */
/* ---------------- */

/*
 * Copyright (c) 2000 - 2013, Lionel Lacassagne, All rights reserved
 * University of Paris Sud, Laboratoire de Recherche en Informatique 
 */

#ifndef __NRUTIL_H__
#define __NRUTIL_H__

//#pragma message("  include  nrutil.h")

#ifdef __cplusplus
#pragma message ("C++")
extern "C" {
#endif


#include "nrtype.h"
#include <stdio.h>

#define NR_END 0
#define FREE_ARG char*

extern long nr_end;

void nrerror(char error_text[]);

void generate_filename_k_ndigit                 (            char *filename, int k, int ndigit,                         char *complete_filename);
void generate_filename_k_ndigit_extension       (            char *filename, int k, int ndigit,        char *extension, char *complete_filename);
void generate_path_filename                     (char *path, char *filename,                                            char *complete_filename);
void generate_path_filename_extension           (char *path, char *filename,                           char *extension, char *complete_filename);
void generate_path_filename_k_ndigit_extension  (char *path, char *filename, int k, int ndigit,        char *extension, char *complete_filename);
void generate_path_filename_k_ndigit_l_extension(char *path, char *filename, int k, int ndigit, int l, char *extension, char *complete_filename);
    
void generate_path_filename_k_parenthesis_extension(char *path, char *filename, int k, char *extension, char *complete_filename);
    
/* ------------------------------- */
/* -- vector & matrix allocator -- */
/* ------------------------------- */

uint8*   ui8vector(long nl, long nh);
uint16* ui16vector(long nl, long nh);
uint32* ui32vector(long nl, long nh);
uint64* ui64vector(long nl, long nh);
float32* f32vector(long nl, long nh);

void free_ui8vector (uint8  *v, long nl, long nh);
void free_ui16vector(uint16 *v, long nl, long nh);
void free_ui32vector(uint32 *v, long nl, long nh);
void free_f32vector(float32 *v, long nl, long nh);


uint8**   ui8matrix(long nrl, long nrh, long ncl, long nch);
sint8**   si8matrix(long nrl, long nrh, long ncl, long nch);
uint16** ui16matrix(long nrl, long nrh, long ncl, long nch);
sint16** si16matrix(long nrl, long nrh, long ncl, long nch);
uint32** ui32matrix(long nrl, long nrh, long ncl, long nch);
sint32** si32matrix(long nrl, long nrh, long ncl, long nch);
uint64** ui64matrix(long nrl, long nrh, long ncl, long nch);
sint64** si64matrix(long nrl, long nrh, long ncl, long nch);
float32** f32matrix(long nrl, long nrh, long ncl, long nch);

rgb8** rgb8matrix(long nrl, long nrh, long ncl, long nch);

void free_ui8matrix(uint8   **m, long nrl, long nrh, long ncl, long nch);
void free_si8matrix(sint8   **m, long nrl, long nrh, long ncl, long nch);
void free_ui16matrix(uint16 **m, long nrl, long nrh, long ncl, long nch);
void free_si16matrix(sint16 **m, long nrl, long nrh, long ncl, long nch);
void free_ui32matrix(uint32 **m, long nrl, long nrh, long ncl, long nch);
void free_si32matrix(sint32 **m, long nrl, long nrh, long ncl, long nch);
void free_f32matrix(float32 **m, long nrl, long nrh, long ncl, long nch);
void free_ui64matrix(uint64 **m, long nrl, long nrh, long ncl, long nch);
void free_si64matrix(sint64 **m, long nrl, long nrh, long ncl, long nch);

void resize_ui8matrix (uint8  **m, long nrl, long nrh, long ncl, long nch);
void resize_ui16matrix(uint16 **m, long nrl, long nrh, long ncl, long nch);
void resize_ui32matrix(uint32 **m, long nrl, long nrh, long ncl, long nch);
void resize_ui64matrix(uint64 **m, long nrl, long nrh, long ncl, long nch);
void resize_f32matrix(float32 **m, long nrl, long nrh, long ncl, long nch);
void resize_ui64matrix(uint64 **m, long nrl, long nrh, long ncl, long nch);

uint8**   ui8matrix_map(long nrl, long nrh, long ncl, long nch);
sint8**   si8matrix_map(long nrl, long nrh, long ncl, long nch);
uint16** ui16matrix_map(long nrl, long nrh, long ncl, long nch);
sint16** si16matrix_map(long nrl, long nrh, long ncl, long nch);
uint32** ui23matrix_map(long nrl, long nrh, long ncl, long nch);
sint32** si32matrix_map(long nrl, long nrh, long ncl, long nch);
float32** f32matrix_map(long nrl, long nrh, long ncl, long nch);
uint64** ui64matrix_map(long nrl, long  nrh, long  ncl, long nch);
sint64** si64matrix_map(long nrl, long nrh, long ncl, long nch);

uint8**    ui8matrix_map_1D_pitch(uint8   **m, long nrl, long nrh, long ncl, long nch, void *data_1D, long pitch);
sint8**    si8matrix_map_1D_pitch(sint8   **m, long nrl, long nrh, long ncl, long nch, void *data_1D, long pitch);
uint16**  ui16matrix_map_1D_pitch(uint16  **m, long nrl, long nrh, long ncl, long nch, void *data_1D, long pitch);
sint16**  si16matrix_map_1D_pitch(sint16  **m, long nrl, long nrh, long ncl, long nch, void *data_1D, long pitch);
uint32**  ui32matrix_map_1D_pitch(uint32  **m, long nrl, long nrh, long ncl, long nch, void *data_1D, long pitch);
sint32**  si32matrix_map_1D_pitch(sint32  **m, long nrl, long nrh, long ncl, long nch, void *data_1D, long pitch);
float32** f32matrix_map_1D_pitch (float32 **m, long nrl, long nrh, long ncl, long nch, void *data_1D, long pitch);
uint64** ui64matrix_map_1D_pitch(uint64 **m, long nrl, long nrh, long ncl, long nch, void *data_1D, long pitch);

void free_ui8matrix_map (uint8  **m, long nrl, long nrh, long ncl, long nch);
void free_si8matrix_map (sint8  **m, long nrl, long nrh, long ncl, long nch);
void free_ui16matrix_map(uint16 **m, long nrl, long nrh, long ncl, long nch);
void free_si16matrix_map(sint16 **m, long nrl, long nrh, long ncl, long nch);
void free_ui32matrix_map(uint32 **m, long nrl, long nrh, long ncl, long nch);
void free_si32matrix_map(sint32 **m, long nrl, long nrh, long ncl, long nch);
void free_ui64matrix_map(uint64 **m, long nrl, long nrh, long ncl, long nch);
void free_si64matrix_map(sint64 **m, long nrl, long nrh, long ncl, long nch);

void free_rgb8matrix(rgb8 **m, long nrl, long nrh, long ncl, long nch);

/* ------------ */
/* --- zero --- */
/* ------------ */

void zero_ui8vector (uint8   *v, int i0, int i1);
void zero_ui16vector(uint16  *v, int i0, int i1);
void zero_ui32vector(uint32  *v, int i0, int i1);
void zero_ui64vector(uint64 *v, int i0, int i1);
void zero_f32vector(float32 *v, int i0, int i1);

void zero_ui8matrix (uint8  **m, int i0, int i1, int j0, int j1);
void zero_ui16matrix(uint16 **m, int i0, int i1, int j0, int j1);
void zero_ui32matrix(uint32 **m, int i0, int i1, int j0, int j1);
void zero_ui64matrix(uint64 **m, int i0, int i1, int j0, int j1);
void zero_f32matrix(float32 **m, int i0, int i1, int j0, int j1);

/* ----------- */
/* --- inf --- */
/* ----------- */

void inf_ui32matrix(uint32 **m, int i0, int i1, int j0, int j1);
void inf_f32matrix(float32 **m, int i0, int i1, int j0, int j1);

/* ------------ */
/* --- init --- */
/* ------------ */

void init_ui8vector_param(uint8   *v, int i0, int i1, uint8   x0, uint8  xstep);

void init_ui8matrix_param(uint8   **m, int i0, int i1, int j0, int j1, uint8   x0, uint8   xstep, uint8   ystep);
void init_f32matrix_param(float32 **m, int i0, int i1, int j0, int j1, float32 x0, float32 xstep, float32 ystep);

/* ------------ */
/* --- copy --- */
/* ------------ */
void copy_ui8matrix_ui8matrix (uint8   **X, int i0, int i1, int j0, int j1, uint8   **Y);
void copy_ui32matrix_ui32matrix (uint32   **X, int i0, int i1, int j0, int j1, uint32   **Y);
void copy_ui8matrix_ui16matrix(uint8   **X, int i0, int i1, int j0, int j1, uint16  **Y);
void copy_ui16matrix_ui8matrix(uint16  **X, int i0, int i1, int j0, int j1, uint8   **Y);
void copy_ui8matrix_f32matrix (uint8   **X, int i0, int i1, int j0, int j1, float32 **Y);
void copy_f32matrix_ui8matrix (float32 **X, int i0, int i1, int j0, int j1, uint8   **Y);

/* --------------- */
/* --- convert --- */
/* --------------- */

void convert_ui8matrix_ui32matrix (uint8 **X, int nrl, int nrh, int ncl, int nch, uint32  **Y);


/* ----------------------------- */
/* -- display vector & matrix -- */
/* ----------------------------- */

void display_si8vector (sint8  *v,long nl,long nh, char *format, char *name);
void display_ui8vector (uint8  *v,long nl,long nh, char *format, char *name);
void display_si16vector(sint16 *v,long nl,long nh, char *format, char *name);
void display_ui16vector(uint16 *v,long nl,long nh, char *format, char *name);
void display_si32vector(sint32 *v,long nl,long nh, char *format, char *name);
void display_ui32vector(uint32 *v,long nl,long nh, char *format, char *name);
void display_f32vector(float32 *v,long nl,long nh, char *format, char *name);

void display_ui32vector_nonull(uint32 *v,long nl,long nh, char *format, char *name);
void display_ui16vector_number(uint16 *v,int j0, int j1, char *format, char *name);
void display_ui32vector_number(uint32 *v,int j0, int j1, char *format, char *name);


void display_ui8vector_positive (uint8  *v, int j0, int j1, int iformat, char *name);
void display_ui16vector_positive(uint16 *v, int j0, int j1, int iformat, char *name);
void display_ui32vector_positive(uint32 *v, int j0, int j1, int iformat, char *name);


void display_si8matrix (sint8  **m,long nrl,long nrh,long ncl, long nch, char *format, char *name);
void display_ui8matrix (uint8  **m,long nrl,long nrh,long ncl, long nch, char *format, char *name);
void display_si16matrix(sint16 **m,long nrl,long nrh,long ncl, long nch, char *format, char *name);
void display_ui16matrix(uint16 **m,long nrl,long nrh,long ncl, long nch, char *format, char *name);
void display_si32matrix(sint32 **m,long nrl,long nrh,long ncl, long nch, char *format, char *name);
void display_ui32matrix(uint32 **m,long nrl,long nrh,long ncl, long nch, char *format, char *name);
void display_f32matrix(float32 **m,long nrl,long nrh,long ncl, long nch, char *format, char *name);
void display_ui64matrix(uint64 **m,long nrl,long nrh,long ncl, long nch, char *format, char *name);



void display_ui8matrix_positive(uint8 **m, int i0, int i1, int j0, int j1, int iformat, char *name);
void display_ui16matrix_positive(uint16 **m, int i0, int i1, int j0, int j1, int iformat, char *name);
void display_ui32matrix_positive(uint32 **m, int i0, int i1, int j0, int j1, int iformat, char *name);
void display_ui16matrix_stride_positive(uint16 **m, int i0, int i1, int j0, int j1, int stride, int iformat, char *name);
void display_ui32matrix_stride_positive(uint32 **m, int i0, int i1, int j0, int j1, int stride, int iformat, char *name);

void display_ui32matrix_distances(uint32 **m,long nrl,long nrh,long ncl, long nch, char *format, char *name);
void display_f32matrix_distances(float32 **m,long nrl,long nrh,long ncl, long nch, char *format, char *name);

/* ------------- */
/* -- wrapper -- */
/* ------------- */

/* ------------------------------- */
/* -- PGM and PNM binary format -- */
/* ------------------------------- */

uint8** LoadPGM_ui8matrix(char *filename, long *nrl, long *nrh, long *ncl, long *nch);
void   MLoadPGM_ui8matrix(char *filename, int nrl, int nrh, int ncl, int nch, uint8 **m);
void    SavePGM_ui8matrix(uint8 **m,       long  nrl, long  nrh, long  ncl, long  nch, char *filename);
void    SavePGM_ui32matrix(uint32 **m,       long  nrl, long  nrh, long  ncl, long  nch, char *filename);
void    SavePGM_CCL      (uint8 **m, int nrl, int nrh, int ncl, int nch, char *filename);
void    SavePPM_CCL      (uint8 **m, int n, long nrl, long nrh, long ncl, long nch, char *filename);

rgb8 ** LoadPPM_rgb8matrix(char *filename, long *nrl, long *nrh, long *ncl, long *nch);
void    SavePPM_rgb8matrix(rgb8 **m,       long  nrl, long  nrh, long  ncl, long  nch, char *filename);

/* --------------------------- */
/* -- PNM IO for rgb8matrix -- */
/* --------------------------- */
void ReadPNMrow(FILE  *file, long width, uint8  *line);
void WritePNMrow(uint8  *line, long width, FILE  *file);
rgb8** LoadPPM_rgb8matrix(char *filename, long *nrl, long *nrh, long *ncl, long *nch);
void SavePPM_rgb8matrix(rgb8 **m, long nrl, long nrh, long ncl, long nch, char *filename);
void SavePPM_ui32matrix_CCL(uint32 **m, long nrl, long nrh, long ncl, long nch, char *filename);

void HsvToRgb(rgb8* pixel, uint8 h, uint8 s, uint8 v);
    
// -- 16 bit
void fread_ui16trimatrix(char *filename, uint16 **m,long i0,long i1,long j0, long j1);
void fwrite_ui16trimatrix(uint16 **m,long i0,long i1,long j0, long j1, char *filename);
    
#ifdef __cplusplus
}
#endif

#endif // __NRUTL_H__
