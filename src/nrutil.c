/* -------------- */
/* --- nrutil --- */
/* -------------- */

/*
 * Copyright (c) 2000 - 2013, Lionel Lacassagne, All rights reserved
 * University of Paris Sud, Laboratoire de Recherche en Informatique 
 */

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h> /* fabs */

#include "nrtype.h"
#include "nrutil.h"
#include "macro_debug.h"

//NR_END est maintenant defini dans nrutil.h

//#define NR_END 1
//#define FREE_ARG char*

#define INF32 0XFFFFFFFF

long nr_end = NR_END;

/* ------------------------- */
void nrerror(char error_text[])
/* ------------------------- */
/* Numerical Recipes standard error handler */
{
  fprintf(stderr,"Numerical Recipes run-time error...\n");
  fprintf(stderr,"%s\n",error_text);
  fprintf(stderr,"...now exiting to system...\n");
  exit(1);
}
/* ------------------------------------------------------------------------------------- */
void generate_filename_k_ndigit(char *filename, int k, int ndigit, char *complete_filename)
/* ------------------------------------------------------------------------------------- */
{
  char *format = "%s%d.txt";

  switch(ndigit) {
    case 0 : format = "%s%d.txt";   break;
    case 1 : format = "%s%01d.txt"; break;
    case 2 : format = "%s%02d.txt"; break;
    case 3 : format = "%s%03d.txt"; break;
    case 4 : format = "%s%04d.txt"; break;
    case 5 : format = "%s%05d.txt"; break;
    case 6 : format = "%s%06d.txt"; break;
    case 7 : format = "%s%07d.txt"; break;
    case 8 : format = "%s%08d.txt"; break;
    case 9 : format = "%s%09d.txt"; break;
  }
  sprintf(complete_filename, format, filename, k);
}
/* ---------------------------------------------------------------------------------------------------------------- */
void generate_filename_k_ndigit_extension(char *filename, int k, int ndigit, char *extension, char *complete_filename)
/* ---------------------------------------------------------------------------------------------------------------- */
{
  char *format = "%s%d.%s";

  switch(ndigit) {
    case 0 : format = "%s%d.%s";   break;
    case 1 : format = "%s%01d.%s"; break;
    case 2 : format = "%s%02d.%s"; break;
    case 3 : format = "%s%03d.%s"; break;
    case 4 : format = "%s%04d.%s"; break;

    case 5 : format = "%s%05d.%s"; break;
    case 6 : format = "%s%06d.%s"; break;
    case 7 : format = "%s%07d.%s"; break;
    case 8 : format = "%s%08d.%s"; break;
    case 9 : format = "%s%09d.%s"; break;
  }
  sprintf(complete_filename, format, filename, k, extension);
}
/* -------------------------------------------------------------------------- */
void generate_path_filename(char *path, char *filename, char *complete_filename)
/* -------------------------------------------------------------------------- */
{
  sprintf(complete_filename, "%s%s", path, filename);
}
/* ----------------------------------------------------------------------------------------------------- */
void generate_path_filename_extension(char *path, char *filename, char *extension, char *complete_filename)
/* ----------------------------------------------------------------------------------------------------- */
{
  sprintf(complete_filename, "%s%s.%s", path, filename, extension);
}
/* --------------------------------------------------------------------------------------------------------------------------------- */
void generate_path_filename_k_ndigit_extension(char *path, char *filename, int k, int ndigit, char *extension, char *complete_filename)
/* --------------------------------------------------------------------------------------------------------------------------------- */
{
  char *format = "%s%s%d.%s";

  switch(ndigit) {
    case 0 : format = "%s%s%d.%s";   break;
    case 1 : format = "%s%s%01d.%s"; break;
    case 2 : format = "%s%s%02d.%s"; break;
    case 3 : format = "%s%s%03d.%s"; break;
    case 4 : format = "%s%s%04d.%s"; break;
    case 5 : format = "%s%s%05d.%s"; break;
    case 6 : format = "%s%s%06d.%s"; break;
    case 7 : format = "%s%s%07d.%s"; break;
    case 8 : format = "%s%s%08d.%s"; break;
    case 9 : format = "%s%s%09d.%s"; break;
  }
  sprintf(complete_filename, format, path, filename, k, extension);
}
/* --------------------------------------------------------------------------------------------------------------------------------- */
void generate_path_filename_k_parenthesis_extension(char *path, char *filename, int k, char *extension, char *complete_filename)
/* --------------------------------------------------------------------------------------------------------------------------------- */
{
    char *format = "%s%s(%d).%s";
    
    sprintf(complete_filename, format, path, filename, k, extension);
}
/* ------------------------------------------------------------------------------------------------------------------------------------------ */
void generate_path_filename_k_ndigit_l_extension(char *path, char *filename, int k, int ndigit, int l, char *extension, char *complete_filename)
/* ------------------------------------------------------------------------------------------------------------------------------------------ */
{
  char *format = "%s%s%d_%d.%s";

  switch(ndigit) {
    case 0 : format = "%s%s%d_%d.%s";   break;
    case 1 : format = "%s%s%01d_%01d.%s"; break;
    case 2 : format = "%s%s%02d_%02d.%s"; break;
    case 3 : format = "%s%s%03d_%03d.%s"; break;
    case 4 : format = "%s%s%04d_%04d.%s"; break;
    case 5 : format = "%s%s%05d_%05d.%s"; break;
    case 6 : format = "%s%s%06d_%06d.%s"; break;
    case 7 : format = "%s%s%07d_%07d.%s"; break;
    case 8 : format = "%s%s%08d_%08d.%s"; break;
    case 9 : format = "%s%s%09d_%09d.%s"; break;
  }
  sprintf(complete_filename, format, path, filename, k, l, extension);
}
/* ------------------------------ */
float32* f32vector(long nl, long nh)
/* ------------------------------ */
{
  float32 *v;

  v=(float32 *)malloc((size_t) ((nh-nl+1+NR_END)*sizeof(float32)));
  if (!v) nrerror("allocation failure in f32vector()");
  if(!v) return NULL;
  return v-nl+NR_END;
}

/* ------------------------------------------ */
uint8* ui8vector(long nl, long nh)
/* ------------------------------------------ */
/* allocate an uint8 vector with subscript range v[nl..nh] */
{
    uint8 *v;

    v=(uint8 *)malloc((size_t) ((nh-nl+1+NR_END)*sizeof(uint8)));
    if (!v) nrerror("allocation failure in ui8vector()");
    return v-nl+NR_END;
}
/* ------------------------------------------ */
uint16* ui16vector(long nl, long nh)
/* ------------------------------------------ */
/* allocate an uint16 vector with subscript range v[nl..nh] */
{
    uint16 *v;

    v=(uint16 *)malloc((size_t) ((nh-nl+1+NR_END)*sizeof(uint16)));
    if (!v) nrerror("allocation failure in ui16vector()");
    return v-nl+NR_END;
}
/* ------------------------------------------ */
uint32* ui32vector(long nl, long nh)
/* ------------------------------------------ */
/* allocate an uint8 vector with subscript range v[nl..nh] */
{
    uint32 *v;

    v=(uint32 *)malloc((size_t) ((nh-nl+1+NR_END)*sizeof(uint32)));
    if (!v) nrerror("allocation failure in ui32vector()");
    return v-nl+NR_END;
}
/* ------------------------------------------ */
uint64* ui64vector(long nl, long nh)
/* ------------------------------------------ */
/* allocate an uint64 vector with subscript range v[nl..nh] */
{
    uint64 *v;

    v=(uint64 *)malloc((size_t) ((nh-nl+1+NR_END)*sizeof(uint64)));
    if (!v) nrerror("allocation failure in ui64vector()");
    return v-nl+NR_END;
}
/* --------------------------------------------------- */
uint8** ui8matrix(long nrl, long nrh, long ncl, long nch)
/* --------------------------------------------------- */
/* allocate an uint8 matrix with subscript range m[nrl..nrh][ncl..nch] */
{
  long i, nrow=nrh-nrl+1,ncol=nch-ncl+1;
  uint8 **m;

  /* allocate pointers to rows */
  m=(uint8 **) malloc((size_t)((nrow+NR_END)*sizeof(uint8*)));
  if (!m) nrerror("allocation failure 1 in ui8matrix()");
  m += NR_END;
  m -= nrl;

  /* allocate rows and set pointers to them */
  m[nrl]=(uint8 *) malloc((size_t)((nrow*ncol+NR_END)*sizeof(uint8)));
  if (!m[nrl]) nrerror("allocation failure 2 in ui8matrix()");
  m[nrl] += NR_END;
  m[nrl] -= ncl;

  for(i=nrl+1;i<=nrh;i++) m[i]=m[i-1]+ncol;

  /* return pointer to array of pointers to rows */
  return m;
}
/* --------------------------------------------------- */
sint8** si8matrix(long nrl, long nrh, long ncl, long nch)
/* --------------------------------------------------- */
/* allocate an sint8 matrix with subscript range m[nrl..nrh][ncl..nch] */
{
  long i, nrow=nrh-nrl+1,ncol=nch-ncl+1;
  sint8 **m;

  /* allocate pointers to rows */
  m=(sint8 **) malloc((size_t)((nrow+NR_END)*sizeof(sint8*)));
  if (!m) nrerror("allocation failure 1 in si8matrix()");
  m += NR_END;
  m -= nrl;

  /* allocate rows and set pointers to them */
  m[nrl]=(sint8 *) malloc((size_t)((nrow*ncol+NR_END)*sizeof(sint8)));
  if (!m[nrl]) nrerror("allocation failure 2 in si8matrix()");
  m[nrl] += NR_END;
  m[nrl] -= ncl;

  for(i=nrl+1;i<=nrh;i++) m[i]=m[i-1]+ncol;

  /* return pointer to array of pointers to rows */
  return m;
}
/* --------------------------------------------------- */
uint16** ui16matrix(long nrl, long nrh, long ncl, long nch)
/* --------------------------------------------------- */
/* allocate an uint16 matrix with subscript range m[nrl..nrh][ncl..nch] */
{
  long i, nrow=nrh-nrl+1,ncol=nch-ncl+1;
  uint16 **m;

  /* allocate pointers to rows */
  m=(uint16 **) malloc((size_t)((nrow+NR_END)*sizeof(uint16*)));
  if (!m) nrerror("allocation failure 1 in ui16matrix()");
  m += NR_END;
  m -= nrl;

  /* allocate rows and set pointers to them */
  m[nrl]=(uint16 *) malloc((size_t)((nrow*ncol+NR_END)*sizeof(uint16)));
  if (!m[nrl]) nrerror("allocation failure 2 in ui16matrix()");
  m[nrl] += NR_END;
  m[nrl] -= ncl;

  for(i=nrl+1;i<=nrh;i++) m[i]=m[i-1]+ncol;

  /* return pointer to array of pointers to rows */
  return m;
}
/* ----------------------------------------------------- */
sint16** si16matrix(long nrl, long nrh, long ncl, long nch)
/* ----------------------------------------------------- */
/* allocate an sint16 matrix with subscript range m[nrl..nrh][ncl..nch] */
{
  long i, nrow=nrh-nrl+1,ncol=nch-ncl+1;
  sint16 **m;

  /* allocate pointers to rows */
  m=(sint16 **) malloc((size_t)((nrow+NR_END)*sizeof(sint16*)));
  if (!m) nrerror("allocation failure 1 in si16matrix()");
  m += NR_END;
  m -= nrl;

  /* allocate rows and set pointers to them */
  m[nrl]=(sint16 *) malloc((size_t)((nrow*ncol+NR_END)*sizeof(sint16)));
  if (!m[nrl]) nrerror("allocation failure 2 in si16matrix()");
  m[nrl] += NR_END;
  m[nrl] -= ncl;

  for(i=nrl+1;i<=nrh;i++) m[i]=m[i-1]+ncol;

  /* return pointer to array of pointers to rows */
  return m;
}
/* --------------------------------------------------- */
uint32** ui32matrix(long nrl, long nrh, long ncl, long nch)
/* --------------------------------------------------- */
/* allocate an uint32 matrix with subscript range m[nrl..nrh][ncl..nch] */
{
  long i, nrow=nrh-nrl+1,ncol=nch-ncl+1;
  uint32 **m;

  /* allocate pointers to rows */
  m=(uint32 **) malloc((size_t)((nrow+NR_END)*sizeof(uint32*)));
  if (!m) nrerror("allocation failure 1 in ui16matrix()");
  m += NR_END;
  m -= nrl;

  /* allocate rows and set pointers to them */
  m[nrl]=(uint32 *) malloc((size_t)((nrow*ncol+NR_END)*sizeof(uint32)));
  if (!m[nrl]) nrerror("allocation failure 2 in ui16matrix()");
  m[nrl] += NR_END;
  m[nrl] -= ncl;

  for(i=nrl+1;i<=nrh;i++) m[i]=m[i-1]+ncol;

  /* return pointer to array of pointers to rows */
  return m;
}
/* --------------------------------------------------- */
sint32** si32matrix(long nrl, long nrh, long ncl, long nch)
/* --------------------------------------------------- */
/* allocate an sint32 matrix with subscript range m[nrl..nrh][ncl..nch] */
{
  long i, nrow=nrh-nrl+1,ncol=nch-ncl+1;
  sint32 **m;

  /* allocate pointers to rows */
  m=(sint32 **) malloc((size_t)((nrow+NR_END)*sizeof(sint32*)));
  if (!m) nrerror("allocation failure 1 in ui16matrix()");
  m += NR_END;
  m -= nrl;

  /* allocate rows and set pointers to them */
  m[nrl]=(sint32 *) malloc((size_t)((nrow*ncol+NR_END)*sizeof(sint32)));
  if (!m[nrl]) nrerror("allocation failure 2 in ui16matrix()");
  m[nrl] += NR_END;
  m[nrl] -= ncl;

  for(i=nrl+1;i<=nrh;i++) m[i]=m[i-1]+ncol;

  /* return pointer to array of pointers to rows */
  return m;
}

/* --------------------------------------------------- */
uint64** ui64matrix(long nrl, long nrh, long ncl, long nch)
/* --------------------------------------------------- */
/* allocate an uint64 matrix with subscript range m[nrl..nrh][ncl..nch] */
{
  long i, nrow=nrh-nrl+1,ncol=nch-ncl+1;
  uint64 **m;

  /* allocate pointers to rows */
  m=(uint64 **) malloc((size_t)((nrow+NR_END)*sizeof(uint64*)));
  if (!m) nrerror("allocation failure 1 in ui64matrix()");
  m += NR_END;
  m -= nrl;

  /* allocate rows and set pointers to them */
  m[nrl]=(uint64 *) malloc((size_t)((nrow*ncol+NR_END)*sizeof(uint64)));
  if (!m[nrl]) nrerror("allocation failure 2 in ui64matrix()");
  m[nrl] += NR_END;
  m[nrl] -= ncl;

  for(i=nrl+1;i<=nrh;i++) m[i]=m[i-1]+ncol;

  /* return pointer to array of pointers to rows */
  return m;
}
/* --------------------------------------------------- */
sint64** si64matrix(long nrl, long nrh, long ncl, long nch)
/* --------------------------------------------------- */
/* allocate an sint64 matrix with subscript range m[nrl..nrh][ncl..nch] */
{
  long i, nrow=nrh-nrl+1,ncol=nch-ncl+1;
  sint64 **m;

  /* allocate pointers to rows */
  m=(sint64 **) malloc((size_t)((nrow+NR_END)*sizeof(sint64*)));
  if (!m) nrerror("allocation failure 1 in ui16matrix()");
  m += NR_END;
  m -= nrl;

  /* allocate rows and set pointers to them */
  m[nrl]=(sint64 *) malloc((size_t)((nrow*ncol+NR_END)*sizeof(sint64)));
  if (!m[nrl]) nrerror("allocation failure 2 in ui16matrix()");
  m[nrl] += NR_END;
  m[nrl] -= ncl;

  for(i=nrl+1;i<=nrh;i++) m[i]=m[i-1]+ncol;

  /* return pointer to array of pointers to rows */
  return m;
}


/* ----------------------------------------------------- */
float32** f32matrix(long nrl, long nrh, long ncl, long nch)
/* ----------------------------------------------------- */
/* allocate an float32 matrix with subscript range m[nrl..nrh][ncl..nch] */
{
  long i, nrow=nrh-nrl+1,ncol=nch-ncl+1;
  float32 **m;

  /* allocate pointers to rows */
  m=(float32 **) malloc((size_t)((nrow+NR_END)*sizeof(float32*)));
  if (!m) nrerror("allocation failure 1 in f32matrix()");
  m += NR_END;
  m -= nrl;

  /* allocate rows and set pointers to them */
  m[nrl]=(float32 *) malloc((size_t)((nrow*ncol+NR_END)*sizeof(float32)));
  if (!m[nrl]) nrerror("allocation failure 2 in f32matrix()");
  m[nrl] += NR_END;
  m[nrl] -= ncl;

  for(i=nrl+1;i<=nrh;i++) m[i]=m[i-1]+ncol;

  /* return pointer to array of pointers to rows */
  return m;
}
/* --------------------------------------------------- */
rgb8** rgb8matrix(long nrl, long nrh, long ncl, long nch)
/* -------------------------------------------------- */
/* allocate an rgb8 matrix with subscript range m[nrl..nrh][ncl..nch] */
{
  long i, nrow=nrh-nrl+1,ncol=nch-ncl+1;
  rgb8 **m;

  /* allocate pointers to rows */
  m=(rgb8**) malloc((size_t)((nrow+NR_END)*sizeof(rgb8*)));
  if (!m) nrerror("allocation failure 1 in rgb8matrix()");
  m += NR_END;
  m -= nrl;

  /* allocate rows and set pointers to them */
  m[nrl]=(rgb8*) malloc((size_t)((nrow*ncol+NR_END)*sizeof(rgb8)));
  if (!m[nrl]) nrerror("allocation failure 2 in rgb8matrix()");
  m[nrl] += NR_END;
  m[nrl] -= ncl;

  for(i=nrl+1;i<=nrh;i++) m[i]=m[i-1]+ncol;

  /* return pointer to array of pointers to rows */
  return m;
}
/* ------------------------------------------- */
void free_f64vector(float64 *v, long nl, long nh)
/* ------------------------------------------- */
/* free a double vector allocated with f64vector() */
{
  free((FREE_ARG) (v+nl-NR_END));
}
/* ------------------------------------------- */
void free_ui16vector(uint16 *v, long nl, long nh)
/* ------------------------------------------- */
{
  free((FREE_ARG) (v+nl-NR_END));
}
/* ---------------------------------------------------------------- */
void free_ui8matrix(uint8 **m, long nrl, long nrh, long ncl, long nch)
/* ---------------------------------------------------------------- */
{
  free((FREE_ARG) (m[nrl]+ncl-NR_END));
  free((FREE_ARG) (m+nrl-NR_END));
}
/* ---------------------------------------------------------------- */
void free_si8matrix(sint8 **m, long nrl, long nrh, long ncl, long nch)
/* ---------------------------------------------------------------- */
{
  free((FREE_ARG) (m[nrl]+ncl-NR_END));
  free((FREE_ARG) (m+nrl-NR_END));
}
/* ------------------------------------------------------------------ */
void free_ui16matrix(uint16 **m, long nrl, long nrh, long ncl, long nch)
/* ------------------------------------------------------------------ */
{
  free((FREE_ARG) (m[nrl]+ncl-NR_END));
  free((FREE_ARG) (m+nrl-NR_END));
}
/* ------------------------------------------------------------------ */
void free_si16matrix(sint16 **m, long nrl, long nrh, long ncl, long nch)
/* ------------------------------------------------------------------ */
{
  free((FREE_ARG) (m[nrl]+ncl-NR_END));
  free((FREE_ARG) (m+nrl-NR_END));
}
/* ------------------------------------------------------------------ */
void free_ui32matrix(uint32 **m, long nrl, long nrh, long ncl, long nch)
/* ------------------------------------------------------------------ */
{
  free((FREE_ARG) (m[nrl]+ncl-NR_END));
  free((FREE_ARG) (m+nrl-NR_END));
}
/* ------------------------------------------------------------------ */
void free_si32matrix(sint32 **m, long nrl, long nrh, long ncl, long nch)
/* ------------------------------------------------------------------ */
{
  free((FREE_ARG) (m[nrl]+ncl-NR_END));
  free((FREE_ARG) (m+nrl-NR_END));
}

/* ------------------------------------------------------------------ */
void free_ui64matrix(uint64 **m, long nrl, long nrh, long ncl, long nch)
/* ------------------------------------------------------------------ */
{
  free((FREE_ARG) (m[nrl]+ncl-NR_END));
  free((FREE_ARG) (m+nrl-NR_END));
}
/* ------------------------------------------------------------------ */
void free_si64matrix(sint64 **m, long nrl, long nrh, long ncl, long nch)
/* ------------------------------------------------------------------ */
{
  free((FREE_ARG) (m[nrl]+ncl-NR_END));
  free((FREE_ARG) (m+nrl-NR_END));
}


/* ------------------------------------------------------------------ */
void free_f32matrix(float32 **m, long nrl, long nrh, long ncl, long nch)
/* ------------------------------------------------------------------ */
{
  free((FREE_ARG) (m[nrl]+ncl-NR_END));
  free((FREE_ARG) (m+nrl-NR_END));
}
/* ------------------------------------------------------- */
void free_ui8vector(uint8 *v, long nl, long nh)
/* ------------------------------------------------------- */
/* free a byte vector allocated with ui8vector() */
{
    free((FREE_ARG) (v+nl-NR_END));
}

/* ------------------------------------------------------- */
void free_ui32vector(uint32 *v, long nl, long nh)
/* ------------------------------------------------------- */
/* free a byte vector allocated with ui32vector() */
{
    free((FREE_ARG) (v+nl-NR_END));
}

/* ------------------------------------------------------- */
void free_f32vector(float32 *v, long nl, long nh)
/* ------------------------------------------------------- */
/* free a byte vector allocated with f32vector() */
{
    free((FREE_ARG) (v+nl-NR_END));
}
/* ------------------------------------------------------------------ */
void resize_ui8matrix(uint8 **m, long nrl, long nrh, long ncl, long nch)
/* ------------------------------------------------------------------ */
{
    long i, ncol=nch-ncl+1;
    for(i=nrl+1;i<=nrh;i++) m[i]=m[i-1]+ncol;
}
/* -------------------------------------------------------------------- */
void resize_ui16matrix(uint16 **m, long nrl, long nrh, long ncl, long nch)
/* -------------------------------------------------------------------- */
{
    long i, ncol=nch-ncl+1;
    for(i=nrl+1;i<=nrh;i++) m[i]=m[i-1]+ncol;
}
/* -------------------------------------------------------------------- */
void resize_ui32matrix(uint32 **m, long nrl, long nrh, long ncl, long nch)
/* -------------------------------------------------------------------- */
{
    long i, ncol=nch-ncl+1;
    for(i=nrl+1;i<=nrh;i++) m[i]=m[i-1]+ncol;
}
/* -------------------------------------------------------------------- */
void resize_ui64matrix(uint64 **m, long nrl, long nrh, long ncl, long nch)
/* -------------------------------------------------------------------- */
{
    long i, ncol=nch-ncl+1;
    for(i=nrl+1;i<=nrh;i++) m[i]=m[i-1]+ncol;
}
/* -------------------------------------------------------------------- */
void resize_f32matrix(float32 **m, long nrl, long nrh, long ncl, long nch)
/* -------------------------------------------------------------------- */
{
    long i, ncol=nch-ncl+1;
    for(i=nrl+1;i<=nrh;i++) m[i]=m[i-1]+ncol;
}
/* ------------------------------------------------------- */
uint8** ui8matrix_map(long nrl, long nrh, long ncl, long nch)
/* ------------------------------------------------------- */
{
    long nrow=nrh-nrl+1;
    uint8 **m;

    /* allocate pointers to rows */
    m=(uint8 **) malloc((size_t)(nrow*sizeof(uint8*)));
    if (!m) nrerror("allocation failure 1 in ui8matrix_map()");
    m -= nrl;

    /* return pointer to array of pointers to rows */
    return m;
}
/* ------------------------------------------------------- */
sint8** si8matrix_map(long nrl, long nrh, long ncl, long nch)
/* ------------------------------------------------------- */
{
    long nrow=nrh-nrl+1;
    sint8 **m;

    /* allocate pointers to rows */
    m=(sint8 **) malloc((size_t)(nrow*sizeof(sint8*)));
    if (!m) nrerror("allocation failure 1 in si8matrix_map()");
    m -= nrl;

    /* return pointer to array of pointers to rows */
    return m;
}
/* --------------------------------------------------------- */
sint16** si16matrix_map(long nrl, long nrh, long ncl, long nch)
/* --------------------------------------------------------- */
{
  long nrow=nrh-nrl+1;
  sint16 **m;

  /* allocate pointers to rows */
  m=(sint16 **) malloc((size_t)(nrow*sizeof(sint16*)));
  if (!m) nrerror("allocation failure 1 in si16matrix_map()");
  m -= nrl;

  /* return pointer to array of pointers to rows */
  return m;
}
/* --------------------------------------------------------- */
uint16** ui16matrix_map(long nrl, long nrh, long ncl, long nch)
/* --------------------------------------------------------- */
{
    long nrow=nrh-nrl+1;
    uint16 **m;

    /* allocate pointers to rows */
    m=(uint16 **) malloc((size_t)(nrow*sizeof(uint16*)));
    if (!m) nrerror("allocation failure 1 in ui16matrix_map()");
    m -= nrl;

    /* return pointer to array of pointers to rows */
    return m;
}
/* --------------------------------------------------------- */
uint32** ui32matrix_map(long nrl, long nrh, long ncl, long nch)
/* --------------------------------------------------------- */
{
    long nrow=nrh-nrl+1;
    uint32 **m;

    /* allocate pointers to rows */
    m=(uint32 **) malloc((size_t)(nrow*sizeof(uint32*)));
    if (!m) nrerror("allocation failure 1 in ui32matrix_map()");
    m -= nrl;

    /* return pointer to array of pointers to rows */
    return m;
}
/* --------------------------------------------------------- */
sint32** si32matrix_map(long nrl, long nrh, long ncl, long nch)
/* --------------------------------------------------------- */
{
    long nrow=nrh-nrl+1;
    sint32 **m;

    /* allocate pointers to rows */
    m=(sint32 **) malloc((size_t)(nrow*sizeof(sint32*)));
    if (!m) nrerror("allocation failure 1 in si32matrix_map()");
    m -= nrl;

    /* return pointer to array of pointers to rows */
    return m;
}

/* --------------------------------------------------------- */
uint64** ui64matrix_map(long nrl, long  nrh, long  ncl, long nch)
/* --------------------------------------------------------- */
{
    long nrow=nrh-nrl+1;
    uint64 **m;

    /* allocate pointers to rows */
    m=(uint64 **) malloc((size_t)(nrow*sizeof(uint64*)));
    if (!m) nrerror("allocation failure 1 in ui64matrix_map()");
    m -= nrl;

    /* return pointer to array of pointers to rows */
    return m;
}
/* --------------------------------------------------------- */
sint64** si64matrix_map(long nrl, long nrh, long ncl, long nch)
/* --------------------------------------------------------- */
{
    long nrow=nrh-nrl+1;
    sint64 **m;

    /* allocate pointers to rows */
    m=(sint64 **) malloc((size_t)(nrow*sizeof(sint64*)));
    if (!m) nrerror("allocation failure 1 in si64matrix_map()");
    m -= nrl;

    /* return pointer to array of pointers to rows */
    return m;
}

/* --------------------------------------------------------- */
float32** f32matrix_map(long nrl, long nrh, long ncl, long nch)
/* --------------------------------------------------------- */
{
    long nrow=nrh-nrl+1;
    float32 **m;

    /* allocate pointers to rows */
    m=(float32 **) malloc((size_t)(nrow*sizeof(float32*)));
    if (!m) nrerror("allocation failure 1 in f32matrix_map()");
    m -= nrl;

    /* return pointer to array of pointers to rows */
    return m;
}
/* ------------------------------------------------------------------------------------------------------ */
sint8** si8matrix_map_1D_pitch(sint8 **m, long nrl, long nrh, long ncl, long nch, void *data_1D, long pitch)
/* ------------------------------------------------------------------------------------------------------ */
{
    long i;
    uint8 *p;

    /* map rows and set pointers to them */
    m[nrl]= (sint8*) data_1D;
    m[nrl] -= ncl;

    //for(i=nrl+1;i<=nrh;i++) m[i]=m[i-1] + pitch;
    p = (uint8*) m[nrl];
    for(i=nrl+1;i<=nrh;i++) {
        p += pitch;
        m[i] = (sint8*) p;
    }

    /* return pointer to array of pointers to rows */
    return m;
}

/* ------------------------------------------------------------------------------------------------------ */
uint8** ui8matrix_map_1D_pitch(uint8 **m, long nrl, long nrh, long ncl, long nch, void *data_1D, long pitch)
/* ------------------------------------------------------------------------------------------------------ */
{
    long i;
    uint8 *p;

    /* map rows and set pointers to them */
    m[nrl]= (uint8*) data_1D;
    m[nrl] -= ncl;

    //for(i=nrl+1;i<=nrh;i++) m[i]=m[i-1] + pitch;
    p = (uint8*) m[nrl];
    for(i=nrl+1;i<=nrh;i++) {
        p += pitch;
        m[i] = (uint8*) p;
    }

    /* return pointer to array of pointers to rows */
    return m;
}
/* --------------------------------------------------------------------------------------------------------- */
sint16** si16matrix_map_1D_pitch(sint16 **m, long nrl, long nrh, long ncl, long nch, void *data_1D, long pitch)
/* --------------------------------------------------------------------------------------------------------- */
{
    long i;
    uint8 *p;

    /* map rows and set pointers to them */
    m[nrl]= (sint16*) data_1D;
    m[nrl] -= ncl;

    //for(i=nrl+1;i<=nrh;i++) m[i]=m[i-1] + pitch;
    p = (uint8*) m[nrl];
    for(i=nrl+1;i<=nrh;i++) {
        p += pitch;
        m[i] = (sint16*) p;
    }

    /* return pointer to array of pointers to rows */
    return m;
}

/* --------------------------------------------------------------------------------------------------------- */
uint16** ui16matrix_map_1D_pitch(uint16 **m, long nrl, long nrh, long ncl, long nch, void *data_1D, long pitch)
/* --------------------------------------------------------------------------------------------------------- */
{
    long i;
    uint8 *p;

    /* map rows and set pointers to them */
    m[nrl]= (uint16*) data_1D;
    m[nrl] -= ncl;

    //for(i=nrl+1;i<=nrh;i++) m[i]=m[i-1] + pitch;
    p = (uint8*) m[nrl];
    for(i=nrl+1;i<=nrh;i++) {
        p += pitch;
        m[i] = (uint16*) p;
    }

    /* return pointer to array of pointers to rows */
    return m;
}
/* --------------------------------------------------------------------------------------------------------- */
sint32** si32matrix_map_1D_pitch(sint32 **m, long nrl, long nrh, long ncl, long nch, void *data_1D, long pitch)
/* --------------------------------------------------------------------------------------------------------- */
{
    long i;
    uint8 *p;

    /* map rows and set pointers to them */
    m[nrl]= (sint32*) data_1D;
    m[nrl] -= ncl;

    //for(i=nrl+1;i<=nrh;i++) m[i]=m[i-1] + pitch;
    p = (uint8*) m[nrl];
    for(i=nrl+1;i<=nrh;i++) {
        p += pitch;
        m[i] = (sint32*) p;
    }

    /* return pointer to array of pointers to rows */
    return m;
}
/* --------------------------------------------------------------------------------------------------------- */
uint32** ui32matrix_map_1D_pitch(uint32 **m, long nrl, long nrh, long ncl, long nch, void *data_1D, long pitch)
/* --------------------------------------------------------------------------------------------------------- */
{
    long i;
    uint8 *p;

    /* map rows and set pointers to them */
    m[nrl]= (uint32*) data_1D;
    m[nrl] -= ncl;

    //for(i=nrl+1;i<=nrh;i++) m[i]=m[i-1] + pitch;
    p = (uint8*) m[nrl];
    for(i=nrl+1;i<=nrh;i++) {
        p += pitch;
        m[i] = (uint32*) p;
    }

    /* return pointer to array of pointers to rows */
    return m;
}

/* --------------------------------------------------------------------------------------------------------- */
uint64** ui64matrix_map_1D_pitch(uint64 **m, long nrl, long nrh, long ncl, long nch, void *data_1D, long pitch)
/* --------------------------------------------------------------------------------------------------------- */
{
    long i;
    uint8 *p;

    /* map rows and set pointers to them */
    m[nrl]= (uint64*) data_1D;
    m[nrl] -= ncl;

    //for(i=nrl+1;i<=nrh;i++) m[i]=m[i-1] + pitch;
    p = (uint8*) m[nrl];
    for(i=nrl+1;i<=nrh;i++) {
        p += pitch;
        m[i] = (uint64*) p;
    }

    /* return pointer to array of pointers to rows */
    return m;
}


/* ---------------------------------------------------------------------------------------------------------- */
float32** f32matrix_map_1D_pitch(float32 **m, long nrl, long nrh, long ncl, long nch, void *data_1D, long pitch)
/* ---------------------------------------------------------------------------------------------------------- */
{
    long i;
    uint8 *p;

    /* map rows and set pointers to them */
    m[nrl]= (float32*) data_1D;
    m[nrl] -= ncl;

    //for(i=nrl+1;i<=nrh;i++) m[i]=m[i-1] + pitch;
    p = (uint8*) m[nrl];
    for(i=nrl+1;i<=nrh;i++) {
        p += pitch;
        m[i] = (float32*) p;
    }

    /* return pointer to array of pointers to rows */
    return m;
}
/* -------------------------------------------------------------------- */
void free_ui8matrix_map(uint8 **m, long nrl, long nrh, long ncl, long nch)
/* -------------------------------------------------------------------- */
{
    free((FREE_ARG)(m+nrl));
}
/* -------------------------------------------------------------------- */
void free_si8matrix_map(sint8 **m, long nrl, long nrh, long ncl, long nch)
/* -------------------------------------------------------------------- */
{
    free((FREE_ARG)(m+nrl));
}
/* ---------------------------------------------------------------------- */
void free_ui16matrix_map(uint16 **m, long nrl, long nrh, long ncl, long nch)
/* ---------------------------------------------------------------------- */
{
    free((FREE_ARG)(m+nrl));
}
/* ---------------------------------------------------------------------- */
void free_si16matrix_map(sint16 **m, long nrl, long nrh, long ncl, long nch)
/* ---------------------------------------------------------------------- */
{
    free((FREE_ARG)(m+nrl));
}
/* ---------------------------------------------------------------------- */
void free_ui32matrix_map(uint32 **m, long nrl, long nrh, long ncl, long nch)
/* ---------------------------------------------------------------------- */
{
    free((FREE_ARG)(m+nrl));
}
/* ---------------------------------------------------------------------- */
void free_si32matrix_map(sint32 **m, long nrl, long nrh, long ncl, long nch)
/* ---------------------------------------------------------------------- */
{
    free((FREE_ARG)(m+nrl));
}
/* ---------------------------------------------------------------------- */
void free_ui64matrix_map(uint64 **m, long nrl, long nrh, long ncl, long nch)
/* ---------------------------------------------------------------------- */
{
    free((FREE_ARG)(m+nrl));
}
/* ---------------------------------------------------------------------- */
void free_si64matrix_map(sint64 **m, long nrl, long nrh, long ncl, long nch)
/* ---------------------------------------------------------------------- */
{
    free((FREE_ARG)(m+nrl));
}
/* ---------------------------------------------------------------------- */
void free_f32matrix_map(float32 **m, long nrl, long nrh, long ncl, long nch)
/* ---------------------------------------------------------------------- */
{
    free((FREE_ARG)(m+nrl));
}
/* ------------------------------------------------------------------------------ */
void free_rgb8matrix(rgb8 **m, long nrl, long nrh, long ncl, long nch)
/* ------------------------------------------------------------------------------ */
{
    free((FREE_ARG) (m[nrl]+ncl-NR_END));
    free((FREE_ARG) (m+nrl-NR_END));
}
/* --------------------------------------- */
void zero_ui8vector(uint8 *v, int i0, int i1)
/* --------------------------------------- */
{
    int i;
    for(i=i0; i<=i1; i++) {
        v[i] = 0;
    }
}
/* ----------------------------------------- */
void zero_ui16vector(uint16 *v, int i0, int i1)
/* ----------------------------------------- */
{
    int i;
    for(i=i0; i<=i1; i++) {
        v[i] = 0;
    }
}
/* ----------------------------------------- */
void zero_ui32vector(uint32 *v, int i0, int i1)
/* ----------------------------------------- */
{
    int i;
    for(i=i0; i<=i1; i++) {
        v[i] = 0;
    }
}

/* ----------------------------------------- */
void zero_ui64vector(uint64 *v, int i0, int i1)
/* ----------------------------------------- */
{
    int i;
    for(i=i0; i<=i1; i++) {
        v[i] = 0;
    }
}
/* ----------------------------------------- */
void zero_f32vector(float32 *v, int i0, int i1)
/* ----------------------------------------- */
{
    int i;
    for(i=i0; i<=i1; i++) {
        v[i] = 0;
    }
}
/* -------------------------------------------------------- */
void zero_ui8matrix(uint8 **m, int i0, int i1, int j0, int j1)
/* -------------------------------------------------------- */
{
    int i, j;
    for(i=i0; i<=i1; i++) {
        for(j=j0; j<=j1; j++) {
            m[i][j] = 0;
        }
    }
}
/* ---------------------------------------------------------- */
void zero_ui16matrix(uint16 **m, int i0, int i1, int j0, int j1)
/* ---------------------------------------------------------- */
{
    int i, j;
    for(i=i0; i<=i1; i++) {
        for(j=j0; j<=j1; j++) {
            m[i][j] = 0;
        }
    }
}
/* ---------------------------------------------------------- */
void zero_ui32matrix(uint32 **m, int i0, int i1, int j0, int j1)
/* ---------------------------------------------------------- */
{
    int i, j;
    for(i=i0; i<=i1; i++) {
        for(j=j0; j<=j1; j++) {
            m[i][j] = 0;
        }
    }
}
/* ---------------------------------------------------------- */
void zero_f32matrix(float32 **m, int i0, int i1, int j0, int j1)
/* ---------------------------------------------------------- */
{
    int i, j;
    for(i=i0; i<=i1; i++) {
        for(j=j0; j<=j1; j++) {
            m[i][j] = 0;
        }
    }
}
/* ---------------------------------------------------------- */
void inf_ui32matrix(uint32 **m, int i0, int i1, int j0, int j1)
/* ---------------------------------------------------------- */
{
    int i, j;
    for(i=i0; i<=i1; i++) {
        for(j=j0; j<=j1; j++) {
            m[i][j] = INF32;
        }
    }
}

/* ---------------------------------------------------------- */
void inf_f32matrix(float32 **m, int i0, int i1, int j0, int j1)
/* ---------------------------------------------------------- */
{
    int i, j;
    for(i=i0; i<=i1; i++) {
        for(j=j0; j<=j1; j++) {
            m[i][j] = INF32;
        }
    }
}

/* ---------------------------------------------------------- */
void zero_ui64matrix(uint64 **m, int i0, int i1, int j0, int j1)
/* ---------------------------------------------------------- */
{
    int i, j;
    for(i=i0; i<=i1; i++) {
        for(j=j0; j<=j1; j++) {
            m[i][j] = 0;
        }
    }
}
#
/* -------------------------------------------------------------------- */
void init_ui8vector_param(uint8 *v, int i0, int i1, uint8 x0, uint8 xstep)
/* -------------------------------------------------------------------- */
{
    uint8 x = x0;

    for(int i=i0; i<=i1; i++) {
        v[i] = x;
        x += xstep;
    }
}
/* -------------------------------------------------------------------------------------------------- */
void init_ui8matrix_param(uint8 **m, int i0, int i1, int j0, int j1, uint8 x0, uint8 xstep, uint8 ystep)
/* -------------------------------------------------------------------------------------------------- */
{
    int i, j;
    uint8 x;
    //printf("x0 = %d %d %d\n", x0, xstep, ystep);
    
    for(i=i0; i<=i1; i++) {
        x = x0;
        for(j=j0; j<=j1; j++) {
            //printf("%4d", x);
            m[i][j] = x;
            x += xstep;
        } //putchar('\n');
        x0 += ystep;
    } //putchar('\n');
}
/* ---------------------------------------------------------------------------------------------------------- */
void init_f32matrix_param(float32 **m, int i0, int i1, int j0, int j1, float32 x0, float32 xstep, float32 ystep)
/* ---------------------------------------------------------------------------------------------------------- */
{
    int i, j;
    float32 x;

    for(i=i0; i<=i1; i++) {
        x = x0;
        for(j=j0; j<=j1; j++) {
            m[i][j] = x;
            x += xstep;
        }
        x0 += ystep;
    }
}

/* ----------------------------------------------------------------------------- */
void copy_ui8matrix_ui8matrix(uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y)
/* ----------------------------------------------------------------------------- */
{
    int i, j;

    for(i=i0; i<=i1; i++) {
        for(j=j0; j<=j1; j++) {
            Y[i][j] = X[i][j];
        }
    }
}
/* --------------------------------------------------------------------------------- */
void copy_ui32matrix_ui32matrix(uint32 **X, int i0, int i1, int j0, int j1, uint32 **Y)
/* --------------------------------------------------------------------------------- */
{
    int i, j;

    for(i=i0; i<=i1; i++) {
        for(j=j0; j<=j1; j++) {
            Y[i][j] = X[i][j];
        }
    }
}
/* ------------------------------------------------------------------------------- */
void copy_ui8matrix_ui16matrix(uint8 **X, int i0, int i1, int j0, int j1, uint16 **Y)
/* ------------------------------------------------------------------------------- */
{
    int i, j;

    for(i=i0; i<=i1; i++) {
        for(j=j0; j<=j1; j++) {
            Y[i][j] = X[i][j];
        }
    }
}
/* ------------------------------------------------------------------------------- */
void copy_ui16matrix_ui8matrix(uint16 **X, int i0, int i1, int j0, int j1, uint8 **Y)
/* ------------------------------------------------------------------------------- */
{
    int i, j;

    for(i=i0; i<=i1; i++) {
        for(j=j0; j<=j1; j++) {
            Y[i][j] = (uint8) X[i][j];
        }
    }
}
/* ------------------------------------------------------------------------------- */
void copy_ui8matrix_f32matrix(uint8 **X, int i0, int i1, int j0, int j1, float32 **Y)
/* ------------------------------------------------------------------------------- */
{
    int i, j;

    for(i=i0; i<=i1; i++) {
        for(j=j0; j<=j1; j++) {
            Y[i][j] = X[i][j];
        }
    }
}
/* ------------------------------------------------------------------------------- */
void copy_f32matrix_ui8matrix(float32 **X, int i0, int i1, int j0, int j1, uint8 **Y)
/* ------------------------------------------------------------------------------- */
{
    int i, j;

    for(i=i0; i<=i1; i++) {
        for(j=j0; j<=j1; j++) {
            Y[i][j] = (uint8) X[i][j];
        }
    }
}
/* -------------------------------------------------------------------------------- */
void convert_ui8vector_ui32vector(uint8 *X, long nl, long nh, uint32 *Y)
/* -------------------------------------------------------------------------------- */
{
    long i;
    for(i=nl; i<=nh; i++)
        Y[i] = (uint32) X[i];
}
/* ---------------------------------------------------------------------------------------------------- */
void convert_ui8matrix_ui32matrix(uint8 **X, int nrl, int nrh, int ncl, int nch, uint32 **Y)
/* ---------------------------------------------------------------------------------------------------- */
{
    long i;
    for(i=nrl; i<=nrh; i++) {
        convert_ui8vector_ui32vector(X[i], ncl, nch, Y[i]);
    }
}

/* -------------------------------------------------------------------- */
void display_si8vector(sint8 *v,long nl,long nh, char *format, char *name)
/* -------------------------------------------------------------------- */
{
  long i;

  if(name != NULL) printf("%s", name);

  for(i=nl; i<=nh; i++) {
    printf(format, v[i]);
  }
  putchar('\n');
}
/* -------------------------------------------------------------------- */
void display_ui8vector(uint8 *v,long nl,long nh, char *format, char *name)
/* -------------------------------------------------------------------- */
{
  long i;

  if(name != NULL) printf("%s", name);

  for(i=nl; i<=nh; i++) {
    printf(format, v[i]);
  }
  putchar('\n');
}
/* ---------------------------------------------------------------------- */
void display_si16vector(sint16 *v,long nl,long nh, char *format, char *name)
/* ---------------------------------------------------------------------- */
{
  long i;

  if(name != NULL) printf("%s", name);

  for(i=nl; i<=nh; i++) {
    printf(format, v[i]);
  }
  putchar('\n');
}
/* ---------------------------------------------------------------------- */
void display_ui16vector(uint16 *v,long nl,long nh, char *format, char *name)
/* ---------------------------------------------------------------------- */
{
  long i;

  if(name != NULL) printf("%s", name);

  for(i=nl; i<=nh; i++) {
    printf(format, v[i]);
  }
  putchar('\n');
}
/* ---------------------------------------------------------------------- */
void display_si32vector(sint32 *v,long nl,long nh, char *format, char *name)
/* ---------------------------------------------------------------------- */
{
  long i;

  if(name != NULL) printf("%s", name);

  for(i=nl; i<=nh; i++) {
    printf(format, v[i]);
  }
  putchar('\n');
}
/* ---------------------------------------------------------------------- */
void display_ui32vector(uint32 *v,long nl,long nh, char *format, char *name)
/* ---------------------------------------------------------------------- */
{
  long i;

  if(name != NULL) printf("%s", name);

  for(i=nl; i<=nh; i++) {
    printf(format, v[i]);
  }
  putchar('\n');
}

/* ---------------------------------------------------------------------- */
void display_ui32vector_nonull(uint32 *v,long nl,long nh, char *format, char *name)
/* ---------------------------------------------------------------------- */
{
  long i = 0;

  if(name != NULL) printf("%s", name);

  while(v[i]){
    printf(format, v[i]);
    i++;
  }
  putchar('\n');
}
/* ---------------------------------------------------------------------- */
void display_f32vector(float32 *v,long nl,long nh, char *format, char *name)
/* ---------------------------------------------------------------------- */
{
  long i;

  if(name != NULL) printf("%s", name);

  for(i=nl; i<=nh; i++) {
    printf(format, v[i]);
  }
  putchar('\n');
}
/* ---------------------------------------------------------------------------- */
void display_ui8vector_positive(uint8 *v, int j0, int j1, int iformat, char *name)
/* ---------------------------------------------------------------------------- */
{
    int j;
    
    char *format, *str;
    char *format1 = "%1d", *str1 = " ";
    char *format2 = "%2d", *str2 = "  ";
    char *format3 = "%3d", *str3 = "   ";
    char *format4 = "%4d", *str4 = "    ";
    
    switch(iformat) {
        case 1:  format = format1; str = str1; break;
        case 2:  format = format2; str = str2; break;
        case 3:  format = format3; str = str3; break;
        case 4:  format = format4; str = str4; break;
        default: format = format3; str = str3; break;
    }
    
    if(name != NULL) puts(name);
    
    
    for(j=j0; j<=j1; j++) {
        if(v[j]) {
            printf(format, v[j]);
        } else {
            printf("%s", str);
        }
    } 
    putchar('\n');
}
/* ------------------------------------------------------------------------------ */
void display_ui16vector_positive(uint16 *v, int j0, int j1, int iformat, char *name)
/* ------------------------------------------------------------------------------ */
{
    int j;
    
    char *format, *str;
    char *format1 = "%1d", *str1 = " ";
    char *format2 = "%2d", *str2 = "  ";
    char *format3 = "%3d", *str3 = "   ";
    char *format4 = "%4d", *str4 = "    ";
    
    switch(iformat) {
        case 1:  format = format1; str = str1; break;
        case 2:  format = format2; str = str2; break;
        case 3:  format = format3; str = str3; break;
        case 4:  format = format4; str = str4; break;
        default: format = format3; str = str3; break;
    }
    
    if(name != NULL) puts(name);
    
    
    for(j=j0; j<=j1; j++) {
        if(v[j]) {
            printf(format, v[j]);
        } else {
            printf("%s", str);
        }
    }   
    putchar('\n');
}
/* ------------------------------------------------------------------------------ */
void display_ui32vector_positive(uint32 *v, int j0, int j1, int iformat, char *name)
/* ------------------------------------------------------------------------------ */
{
    int j;
    
    char *format, *str;
    char *format1 = "%1d", *str1 = " ";
    char *format2 = "%2d", *str2 = "  ";
    char *format3 = "%3d", *str3 = "   ";
    char *format4 = "%4d", *str4 = "    ";
    
    switch(iformat) {
        case 1:  format = format1; str = str1; break;
        case 2:  format = format2; str = str2; break;
        case 3:  format = format3; str = str3; break;
        case 4:  format = format4; str = str4; break;
        default: format = format3; str = str3; break;
    }
    
    if(name != NULL) puts(name);
    
    
    for(j=j0; j<=j1; j++) {
        if(v[j]) {
            printf(format, v[j]);
        } else {
            printf("%s", str);
        }
    }   
    putchar('\n');
}
/* --------------------------------------------------------------------------------------------- */
void display_ui8matrix_positive(uint8 **m, int i0, int i1, int j0, int j1, int iformat, char *name)
/* --------------------------------------------------------------------------------------------- */
{
    int i,j;

    char *format, *str;
    
    char *format1 = "%1d", *str1 = " ";
    char *format2 = "%2d", *str2 = "  ";
    char *format3 = "%3d", *str3 = "   ";
    char *format4 = "%4d", *str4 = "    ";

    //char *format1 = "%1d", *str1 = ".";
    //char *format2 = "%2d", *str2 = "..";
    //char *format3 = "%3d", *str3 = "...";
    //char *format4 = "%4d", *str4 = "....";
    
    switch(iformat) {
        case 1:  format = format1; str = str1; break;
        case 2:  format = format2; str = str2; break;
        case 3:  format = format3; str = str3; break;
        case 4:  format = format4; str = str4; break;
        default: format = format3; str = str3; break;
    }
        
    if(name != NULL) puts(name);
    
    for(i=i0; i<=i1; i++) {
        for(j=j0; j<=j1; j++) {
            if(m[i][j]) {
                printf(format, m[i][j]);
            } else {
                printf("%s", str);
            }
        }
        putchar('\n');
    }     
}
/* ----------------------------------------------------------------------------------------------- */
void display_ui16matrix_positive(uint16 **m, int i0, int i1, int j0, int j1, int iformat, char *name)
/* ----------------------------------------------------------------------------------------------- */
{
    int i,j;
    
    char *format, *str;
    char *format1 = "%1d", *str1 = " ";
    char *format2 = "%2d", *str2 = "  ";
    char *format3 = "%3d", *str3 = "   ";
    char *format4 = "%4d", *str4 = "    ";
    
    switch(iformat) {
        case 1:  format = format1; str = str1; break;
        case 2:  format = format2; str = str2; break;
        case 3:  format = format3; str = str3; break;
        case 4:  format = format4; str = str4; break;
        default: format = format3; str = str3; break;
    }
    
    if(name != NULL) puts(name);
    
    for(i=i0; i<=i1; i++) {
        for(j=j0; j<=j1; j++) {
            if(m[i][j]) {
                printf(format, m[i][j]);
            } else {
                printf("%s", str);
            }
        }
        putchar('\n');
    }    
}
/* ----------------------------------------------------------------------------------------------- */
void display_ui32matrix_positive(uint32 **m, int i0, int i1, int j0, int j1, int iformat, char *name)
/* ----------------------------------------------------------------------------------------------- */
{
    int i,j;
    
    char *format, *str;
    char *format1 = "%1d", *str1 = " ";
    char *format2 = "%2d", *str2 = "  ";
    char *format3 = "%3d", *str3 = "   ";
    char *format4 = "%4d", *str4 = "    ";
    
    switch(iformat) {
        case 1:  format = format1; str = str1; break;
        case 2:  format = format2; str = str2; break;
        case 3:  format = format3; str = str3; break;
        case 4:  format = format4; str = str4; break;
        default: format = format3; str = str3; break;
    }
    
    if(name != NULL) puts(name);
    
    for(i=i0; i<=i1; i++) {
        for(j=j0; j<=j1; j++) {
            if(m[i][j]) {
                printf(format, m[i][j]);
            } else {
                printf("%s", str);
            }
        }
        putchar('\n');
    }    
}
// ---------------------------------------------------------------------------------------------------------------------
void display_ui16matrix_stride_positive(uint16 **m, int i0, int i1, int j0, int j1, int stride, int iformat, char *name)
// ---------------------------------------------------------------------------------------------------------------------
{
    int i,j,c;
    
    char *format, *str;
    char *format1 = "%1d", *str1 = " ";
    char *format2 = "%2d", *str2 = "  ";
    char *format3 = "%3d", *str3 = "   ";
    char *format4 = "%4d", *str4 = "    ";
    
    switch(iformat) {
        case 1:  format = format1; str = str1; break;
        case 2:  format = format2; str = str2; break;
        case 3:  format = format3; str = str3; break;
        case 4:  format = format4; str = str4; break;
        default: format = format3; str = str3; break;
    }
    
    if(name != NULL) puts(name);
    
    for(i=i0; i<=i1; i++) {
        c = 0;
        for(j=j0; j<=j1; j++) {
            if((c>0) && (c%stride==0)) printf("   ");
            c++;
            if(m[i][j]) {
                printf(format, m[i][j]);
            } else {
                printf("%s", str);
            }
        }
        putchar('\n');
    }    
}
// ---------------------------------------------------------------------------------------------------------------------
void display_ui32matrix_stride_positive(uint32 **m, int i0, int i1, int j0, int j1, int stride, int iformat, char *name)
// ---------------------------------------------------------------------------------------------------------------------
{
    int i,j,c;
    
    char *format, *str;
    char *format1 = "%1d", *str1 = " ";
    char *format2 = "%2d", *str2 = "  ";
    char *format3 = "%3d", *str3 = "   ";
    char *format4 = "%4d", *str4 = "    ";
    
    switch(iformat) {
        case 1:  format = format1; str = str1; break;
        case 2:  format = format2; str = str2; break;
        case 3:  format = format3; str = str3; break;
        case 4:  format = format4; str = str4; break;
        default: format = format3; str = str3; break;
    }
    
    if(name != NULL) puts(name);
    
    for(i=i0; i<=i1; i++) {
        c = 0;
        for(j=j0; j<=j1; j++) {
            if((c>0) && (c%stride==0)) printf("   ");
            c++;
            if(m[i][j]) {
                printf(format, m[i][j]);
            } else {
                printf("%s", str);
            }
        }
        putchar('\n');
    }    
}
// --------------------------------------------------------------------------------
void display_ui32vector_number(uint32 *v, int j0, int j1, char *format, char *name)
// --------------------------------------------------------------------------------
{
    int j;
    
    if(name != NULL) puts(name);
    for(j=j0; j<=j1; j++) {
        printf(format, j);
    }
    putchar('\n');
    
    for(j=j0; j<=j1; j++) {
        printf(format, v[j]);
    }
    putchar('\n');
}
// --------------------------------------------------------------------------------
void display_ui16vector_number(uint16 *v, int j0, int j1, char *format, char *name)
// --------------------------------------------------------------------------------
{
    int j;
    
    if(name != NULL) puts(name);
    for(j=j0; j<=j1; j++) {
        printf(format, j);
    }
    putchar('\n');
    
    for(j=j0; j<=j1; j++) {
        printf(format, v[j]);
    }
    putchar('\n');
}
/* ------------------------------------------------------------------------------------------ */
void display_si8matrix(sint8 **m,long nrl,long nrh,long ncl, long nch, char *format, char *name)
/* ------------------------------------------------------------------------------------------ */
{
  long i,j;

  if(name != NULL) puts(name);

  for(i=nrl; i<=nrh; i++) {
    for(j=ncl; j<=nch; j++) {
      printf(format, m[i][j]);
    }
    putchar('\n');
  }
}
/* ------------------------------------------------------------------------------------------ */
void display_ui8matrix(uint8 **m,long nrl,long nrh,long ncl, long nch, char *format, char *name)
/* ------------------------------------------------------------------------------------------ */
{
  long i,j;

  if(name != NULL) puts(name);

  for(i=nrl; i<=nrh; i++) {
    for(j=ncl; j<=nch; j++) {
      printf(format, m[i][j]);
    }
    putchar('\n');
  }
}
/* -------------------------------------------------------------------------------------------- */
void display_si16matrix(sint16 **m,long nrl,long nrh,long ncl, long nch, char *format, char *name)
/* -------------------------------------------------------------------------------------------- */
{
  long i,j;

  if(name != NULL) puts(name);

  for(i=nrl; i<=nrh; i++) {
    for(j=ncl; j<=nch; j++) {
      printf(format, m[i][j]);
    }
    putchar('\n');
  }
}
/* -------------------------------------------------------------------------------------------- */
void display_ui16matrix(uint16 **m,long nrl,long nrh,long ncl, long nch, char *format, char *name)
/* -------------------------------------------------------------------------------------------- */
{
  long i,j;

  if(name != NULL) puts(name);

  for(i=nrl; i<=nrh; i++) {
    for(j=ncl; j<=nch; j++) {
      printf(format, m[i][j]);
    }
    putchar('\n');
  }
}
/* -------------------------------------------------------------------------------------------- */
void display_si32matrix(sint32 **m,long nrl,long nrh,long ncl, long nch, char *format, char *name)
/* -------------------------------------------------------------------------------------------- */
{
  long i,j;

  if(name != NULL) puts(name);

  for(i=nrl; i<=nrh; i++) {
    for(j=ncl; j<=nch; j++) {
      printf(format, m[i][j]);
    }
    putchar('\n');
  }
}
/* -------------------------------------------------------------------------------------------- */
void display_ui32matrix(uint32 **m,long nrl,long nrh,long ncl, long nch, char *format, char *name)
/* -------------------------------------------------------------------------------------------- */
{
  long i,j;

  if(name != NULL) puts(name);

  for(i=nrl; i<=nrh; i++) {
    for(j=ncl; j<=nch; j++) {
      printf(format, m[i][j]);
    }
    putchar('\n');
  }
}

/* -------------------------------------------------------------------------------------------- */
void display_ui64matrix(uint64 **m,long nrl,long nrh,long ncl, long nch, char *format, char *name)
/* -------------------------------------------------------------------------------------------- */
{
  long i,j;

  if(name != NULL) puts(name);

  for(i=nrl; i<=nrh; i++) {
    for(j=ncl; j<=nch; j++) {
      printf(format, m[i][j]);
    }
    putchar('\n');
  }
}
/* -------------------------------------------------------------------------------------------- */
void display_f32matrix(float32 **m,long nrl,long nrh,long ncl, long nch, char *format, char *name)
/* -------------------------------------------------------------------------------------------- */
{
  long i,j;

  if(name != NULL) puts(name);

  for(i=nrl; i<=nrh; i++) {
    for(j=ncl; j<=nch; j++) {
      printf(format, m[i][j]);
    }
    putchar('\n');
  }
}

/* ------------------------------------------------------------------------------------------------------- */
void display_ui32matrix_distances(uint32 **m,long nrl,long nrh,long ncl, long nch, char *format, char *name)
/* ------------------------------------------------------------------------------------------------------- */
{
        long i,j;

        if(name != NULL) puts(name);
        
        printf("%10s", "ID");
        
        for(j=ncl; j<=nch; j++) {
                if (m[1][j] != 0xFFFFFFFF){
                       printf(format, j);
                }
        }
        putchar('\n');
        for(i=nrl; i<=nrh; i++) {
                if (m[i][1] != 0xFFFFFFFF){
                        printf(format,i);

                        for(j=ncl; j<=nch; j++) {
                                if (m[i][j] != 0xFFFFFFFF)
                                        printf(format, m[i][j]);
                        }
                        putchar('\n');
                }
        }
}
/* ------------------------------------------------------------------------------------------------------- */
void display_f32matrix_distances(float32 **m,long nrl,long nrh,long ncl, long nch, char *format, char *name)
/* ------------------------------------------------------------------------------------------------------- */
{
        long i,j;

        if(name != NULL) puts(name);
        
        printf("%10s", "ID");
        
        for(j=ncl; j<=nch; j++) {
                if (m[1][j] != 0xFFFFFFFF){
                       printf(format, j);
                }
        }
        putchar('\n');
        for(i=nrl; i<=nrh; i++) {
                if (m[i][1] != 0xFFFFFFFF){
                        printf(format,i);

                        for(j=ncl; j<=nch; j++) {
                                if (m[i][j] != 0xFFFFFFFF)
                                        printf(format, m[i][j]);
                        }
                        putchar('\n');
                }
        }
}
/* ------------------------ */
/* -- PGM IO for bmatrix -- */
/* ------------------------ */

char *readitem   (FILE *file, char *buffer);
void  ReadPGMrow (FILE *file, long width, uint8  *line);
void  WritePGMrow(uint8 *line, long width, FILE  *file);

/* --------------------------------- */
char *readitem(FILE *file,char *buffer)
/* --------------------------------- */
/* lecture d'un mot */
{
  char *aux;
  int k;

  k=0;
  aux=buffer;
  while (!feof(file))
    {
      *aux=fgetc(file);
      switch(k)
        {
        case 0:
          if (*aux=='#') k=1;
          if (isalnum(*aux)) k=2,aux++;
          break;
        case 1:
          if (*aux==0xA) k=0;
          break;
        case 2:
          if (!isalnum(*aux))
            {
              *aux=0;
              return buffer;
            }
          aux++;
          break;
        }
    }
  *aux=0;
  return buffer;
}
/* ---------------------------------------------- */
void ReadPGMrow(FILE *file, long width, uint8  *line)
/* ---------------------------------------------- */
{
    /* Le fichier est ouvert (en lecture) et ne sera pas ferme a la fin */
     fread(&(line[0]), sizeof(uint8), width, file);
}
/* ----------------------------------------------- */
void WritePGMrow(uint8 *line, long width, FILE  *file)
/* ----------------------------------------------- */
{
/* Le fichier est deja ouvert et ne sera pas ferme a la fin */

   fwrite(&(line[0]), sizeof(uint8), width, file);
}
/* ------------------------------------------------------------------------------ */
uint8** LoadPGM_ui8matrix(char *filename, long *nrl, long *nrh, long *ncl, long *nch)
/* ------------------------------------------------------------------------------ */
{
  /* cette version ne lit plus que le type P5 */

  long height, width;
  uint8 **m;
  FILE *file;
  //int   format;

  char *buffer;
  char msg[1024];
  //char  c;
  int i;

  buffer = (char*) calloc(80, sizeof(char));
  /* ouverture du fichier */
  file = fopen(filename,"rb");
  if (file==NULL) {
      //nrerror("ouverture du fichier impossible\n");
      sprintf(msg, "ouverture du fichier %s impossible dans LoadPGM_ui8matrix\n", filename);
      nrerror(msg);
  }

  /* lecture de l'entete du fichier pgm */
  readitem(file, buffer);
  /*fscanf(fichier, "%s", buffer);*/
  if(strcmp(buffer, "P5") != 0)
    nrerror("entete du fichier %s invalide\n");
    //nrerror("entete du fichier %s invalide\n", filename);

  width  = atoi(readitem(file, buffer));
  height = atoi(readitem(file, buffer));

  *nrl = 0;
  *nrh = height - 1;
  *ncl = 0;
  *nch = width - 1;
  m = ui8matrix(*nrl, *nrh, *ncl, *nch);

  for(i=0; i<height; i++) {
    ReadPGMrow(file, width, m[i]);
  }

  fclose(file);
  free(buffer);

  return m;
}
/* ------------------------------------------------------------------------------- */
void MLoadPGM_ui8matrix(char *filename, int nrl, int nrh, int ncl, int nch, uint8 **m)
/* ------------------------------------------------------------------------------- */
{
    /* cette version ne lit plus que le type P5 */

    int height, width;
    FILE *file;

    char *buffer;
    char msg[1024];
    int i;

    buffer = (char*) calloc(80, sizeof(char));
    /* ouverture du fichier */
    file = fopen(filename,"rb");
    if (file==NULL) {
        //nrerror("ouverture du fichier impossible\n");
        sprintf(msg, "ouverture du fichier %s impossible dans MLoadPGM_ui8matrix\n", filename);
        nrerror(msg);
    }

    /* lecture de l'entete du fichier pgm */
    readitem(file, buffer);
    /*fscanf(fichier, "%s", buffer);*/
    if(strcmp(buffer, "P5") != 0)
        nrerror("entete du fichier %s invalide\n");
    //nrerror("entete du fichier %s invalide\n", filename);

    width  = atoi(readitem(file, buffer));
    height = atoi(readitem(file, buffer));

    for(i=0; i<height; i++) {
        ReadPGMrow(file, width, m[i]);
    }

    fclose(file);
    free(buffer);
}
/* ----------------------------------------------------------------------------------- */
void SavePGM_ui8matrix(uint8 **m, long nrl, long nrh, long ncl, long nch, char *filename)
/* ----------------------------------------------------------------------------------- */
{
  long nrow = nrh-nrl+1;
  long ncol = nch-ncl+1;

  char buffer[80];

  FILE *file;
  int  i;

  file = fopen(filename, "wb");
  if (file == NULL)
    //nrerror("ouverture du fichier %s impossible dans SavePGM_bmatrix\n", filename);
    nrerror("ouverture du fichier %s impossible dans SavePGM_ui8matrix\n");

  /* enregistrement de l'image au format rpgm */

  sprintf(buffer,"P5\n%d %d\n255\n",(int)ncol, (int)nrow);
  fwrite(buffer,strlen(buffer),1,file);
  for(i=nrl; i<=nrh; i++)
    WritePGMrow(m[i], ncol, file);

  /* fermeture du fichier */
  fclose(file);
}

/* ----------------------------------------------------------------------------------- */
void SavePGM_ui32matrix(uint32 **m, long nrl, long nrh, long ncl, long nch, char *filename)
/* ----------------------------------------------------------------------------------- */
{
  long nrow = nrh-nrl+1;
  long ncol = nch-ncl+1;

  char buffer[80];

  FILE *file;
  int  i;

  file = fopen(filename, "wb");
  if (file == NULL)
    //nrerror("ouverture du fichier %s impossible dans SavePGM_bmatrix\n", filename);
    nrerror("ouverture du fichier %s impossible dans SavePGM_ui8matrix\n");

  /* enregistrement de l'image au format rpgm */

  sprintf(buffer,"P5\n%d %d\n255\n",(int)ncol, (int)nrow);
  fwrite(buffer,strlen(buffer),1,file);
  for(i=nrl; i<=nrh; i++)
    WritePGMrow((uint8 *)m[i], ncol, file);

  /* fermeture du fichier */
  fclose(file);
}
// ----------------------------------------------------------------------------------
void SavePGM_CCL(uint8 **m, int nrl, int nrh, int ncl, int nch, char *filename)
// ----------------------------------------------------------------------------------
{
    int nrow = nrh-nrl+1;
    int ncol = nch-ncl+1;

    uint8 *tmp = malloc(sizeof(uint8) * ncol);
    char buffer[80];
    char msg[1024];
    
    FILE *file;
    int  i;
    
    file = fopen(filename, "wb");
    if (file == NULL) {
        //nrerror("ouverture du fichier %s impossible dans SavePGM_bmatrix\n", filename);
        sprintf(msg, "ouverture du fichier %s impossible dans SavePGM_bmatrix\n", filename);
        nrerror(msg);
    }
    
    /* enregistrement de l'image au format rpgm */
    sprintf(buffer,"P5\n%d %d\n255\n",ncol, nrow);
    fwrite(buffer,strlen(buffer),1,file);
    for(i=nrl; i<=nrh; i++){
        for(int k = ncl; k < nch ; k++){
          tmp[k] = (m[i][k]==0) ? 0 : 255;
        }
        WritePGMrow(tmp+ncl, ncol, file);
    }
    /* fermeture du fichier */
    fclose(file);
    free(tmp);
}
/* --------------------------- */
/* -- PNM IO for rgb8matrix -- */
/* --------------------------- */

/* ----------------------------------------------- */
void ReadPNMrow(FILE  *file, long width, uint8  *line)
/* ----------------------------------------------- */
{
    /* Le fichier est ouvert (en lecture) et ne sera pas ferme a la fin */
     fread(&(line[0]), sizeof(uint8), 3*sizeof(uint8)*width, file);
}
/* ------------------------------------------------ */
void WritePNMrow(uint8  *line, long width, FILE  *file)
/* ------------------------------------------------ */
{
/* Le fichier est deja ouvert et ne sera pas ferme a la fin */

   fwrite(&(line[0]), sizeof(uint8), 3*sizeof(uint8)*width, file);
}
/* ------------------------------------------------------------------------------- */
rgb8** LoadPPM_rgb8matrix(char *filename, long *nrl, long *nrh, long *ncl, long *nch)
/* ------------------------------------------------------------------------------- */
{
  /* cette version ne lit plus que le type P6 */

  long height, width;
  rgb8 **m;
  FILE *file;
  /*int   format;*/

  char *buffer;
  //char  c;
  int i;

  buffer = (char*) calloc(80, sizeof(char));
  /* ouverture du fichier */
  file = fopen(filename,"rb");
  if (file==NULL)
    nrerror("ouverture du fichier impossible\n");
    //nrerror("ouverture du fichier %s impossible\n", filename);

  /* lecture de l'entete du fichier pgm */
  readitem(file, buffer);
  /*fscanf(fichier, "%s", buffer);*/
  if(strcmp(buffer, "P6") != 0)
    nrerror("entete du fichier %s invalide\n");
    //nrerror("entete du fichier %s invalide\n", filename);

  width  = atoi(readitem(file, buffer));
  height = atoi(readitem(file, buffer));

  *nrl = 0;
  *nrh = height - 1;
  *ncl = 0;
  *nch = width - 1;
  m = rgb8matrix(*nrl, *nrh, *ncl, *nch);

  for(i=0; i<height; i++) {
    ReadPNMrow(file, width, (uint8*)m[i]);
  }

  fclose(file);
  free(buffer);

  return m;
}
/* ----------------------------------------------------------------------------------- */
void SavePPM_rgb8matrix(rgb8 **m, long nrl, long nrh, long ncl, long nch, char *filename)
/* ----------------------------------------------------------------------------------- */
{
  long nrow = nrh-nrl+1;
  long ncol = nch-ncl+1;

  char buffer[80];

  FILE *file;
  int  i;

  file = fopen(filename, "wb");
  if (file == NULL)
    //nrerror("ouverture du fichier %s impossible dans SavePGM_bmatrix\n", filename);
    nrerror("ouverture du fichier %s impossible dans SavePPM_bmatrix\n");

  /* enregistrement de l'image au format rpgm */

  sprintf(buffer,"P6\n%d %d\n255\n",(int)ncol, (int)nrow);
  fwrite(buffer,strlen(buffer),1,file);
  for(i=nrl; i<=nrh; i++)
    WritePNMrow((uint8*)m[i], ncol, file);

  /* fermeture du fichier */
  fclose(file);
}

// --------------------------------------------------
void HsvToRgb(rgb8* pixel, uint8 h, uint8 s, uint8 v)
// --------------------------------------------------
{
    unsigned char region, remainder, p, q, t;

    if (s == 0) {
        pixel->r = v;
        pixel->g = v;
        pixel->b = v;
        return;
    }

    region = h / 43;
    remainder = (h - (region * 43)) * 6; 

    p = (v * (255 - s)) >> 8;
    q = (v * (255 - ((s * remainder) >> 8))) >> 8;
    t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

    switch (region) {
        case 0:
            pixel->r = v; pixel->g = t; pixel->b = p;
            break;
        case 1:
            pixel->r = q; pixel->g = v; pixel->b = p;
            break;
        case 2:
            pixel->r = p; pixel->g = v; pixel->b = t;
            break;
        case 3:
            pixel->r = p; pixel->g = q; pixel->b = v;
            break;
        case 4:
            pixel->r = t; pixel->g = p; pixel->b = v;
            break;
        default:
            pixel->r = v; pixel->g = p; pixel->b = q;
            break;
    }
}
/* ----------------------------------------------------------------------------------- */
void SavePPM_CCL(uint8 **m, int n, long nrl, long nrh, long ncl, long nch, char *filename)
/* ----------------------------------------------------------------------------------- */
{
  long nrow = nrh-nrl+1;
  long ncol = nch-ncl+1;

  char buffer[80];

  FILE *file;
  int  i;
  rgb8** img = rgb8matrix(nrl, nrh, ncl, nch);
  for (int i=nrl ; i<=nrh ; i++) {
      for (int j=ncl ; j<=nch ; j++) {
          if(m[i][j] != 0){
              HsvToRgb((img[i])+ j, m[i][j]*360/n, 255, 255);
          }
          else{
              HsvToRgb((img[i])+ j, 0, 0, 0);
          }
      }
  }

  file = fopen(filename, "wb");
  if (file == NULL)
    nrerror("ouverture du fichier %s impossible dans SavePPM_bmatrix\n");

  /* enregistrement de l'image au format rpgm */
  sprintf(buffer,"P6\n%d %d\n255\n",(int)ncol, (int)nrow);
  fwrite(buffer,strlen(buffer),1,file);
  for(i=nrl; i<=nrh; i++)
    WritePNMrow((uint8*)img[i], ncol, file);

  /* fermeture du fichier */
  fclose(file);
  free_rgb8matrix(img, nrl, nrh, ncl, nch);
}

/* ----------------------------------------------------------------------------------- */
void SavePPM_ui32matrix_CCL(uint32 **m, long nrl, long nrh, long ncl, long nch, char *filename)
/* ----------------------------------------------------------------------------------- */
{
  long nrow = nrh-nrl+1;
  long ncol = nch-ncl+1;

  char buffer[80];

  FILE *file;
  int  i;
  rgb8** img = rgb8matrix(nrl, nrh, ncl, nch);
  for (int i=nrl ; i<=nrh ; i++) {
      for (int j=ncl ; j<=nch ; j++) {
          if(m[i][j] != 0){
              HsvToRgb((img[i])+ j, m[i][j]*360, 255, 255);
          }
          else{
              HsvToRgb((img[i])+ j, 0, 0, 0);
          }
      }
  }

  file = fopen(filename, "wb");
  if (file == NULL)
    nrerror("ouverture du fichier %s impossible dans SavePPM_bmatrix\n");

  /* enregistrement de l'image au format rpgm */
  sprintf(buffer,"P6\n%d %d\n255\n",(int)ncol, (int)nrow);
  fwrite(buffer,strlen(buffer),1,file);
  for(i=nrl; i<=nrh; i++)
    WritePNMrow((uint8*)img[i], ncol, file);

  /* fermeture du fichier */
  fclose(file);
  free_rgb8matrix(img, nrl, nrh, ncl, nch);
}

/* -------------------------------------------------------------------------------------------------------- */
void fread_ui16trimatrix(char *filename, uint16 **m,long i0,long i1,long j0, long j1)
/* -------------------------------------------------------------------------------------------------------- */
{
    long  i, ncol = j1-j0+1, nread;
    FILE *f;
    
    f = fopen(filename, "rb");
    if(f == NULL) nrerror("Can't open file in fread_ui16trimatrix"); 
    
    for(i=i0; i<=i1; i++) {
        nread = fread(m[i]+j0, sizeof(uint16), ncol, f);
        if(nread != ncol) 
            nrerror("fread_ui16trimatrix : can't read data");
    }
    fclose(f);
}
/* --------------------------------------------------------------------------------------------------------- */
void fwrite_ui16trimatrix(uint16 **m,long i0,long i1,long j0, long j1, char *filename)
/* --------------------------------------------------------------------------------------------------------- */
{
    long  i, ncol = j1-j0+1;
    FILE *f;
    
    f = fopen(filename, "wb");
    if(f == NULL) nrerror("Can't open file in fwrite_ui16trimatrix");
    
    for(i=i0; i<=i1; i++) {
        fwrite(m[i]+j0, sizeof(uint16), ncol, f);
    }
    fclose(f);
}

