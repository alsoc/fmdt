/**
 * Copyright (c) 2017-2018, Arthur Hennequin
 * LIP6, UPMC, CNRS
 */

#ifndef __ELLIPSE_H__
#define __ELLIPSE_H__

#include <nrutil.h>
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  uint16  xmin;
  uint16  xmax;
  uint16  ymin;
  uint16  ymax;

  uint32  S;       // number of points

  uint16  ID;       // ID

  float32  x;       // abscisse du centre d'inertie x = Sx / S
  float32  y;       // ordonnee du centre d'inertie y = Sy / S

  uint32  Sx;      // sum of x properties
  uint32  Sy;      // sum of y properties

  uint64  Sx2;
  uint64  Sxy;
  uint64  Sy2;

  float32  dx;       // erreur par rapport a l`image recalee
  float32  dy;       // erreur par rapport a l`image recalee
  float32  error;  

  int time;
  int prev;          // associated CC from t-1 -> t -> t+1         
  int next;          // associated CC from t-1 -> t -> t+1

  uint8 motion; // debug
  uint8 state; 
  
} MeteorROI;

void init_MeteorROI (MeteorROI *stats, int n);
void extract_features(uint32** img, int i0, int i1, int j0, int j1, MeteorROI* stats, int n);
int analyse_features(MeteorROI* stats, int n, float e_threshold);
float Features_calc_circular(MeteorROI* stats, int ne, uint8** img, float32**U, float32**V);
float filter_speed(MeteorROI* stats, int n, uint32** img, float32**U, float32**V, float32 threshold);
void filter_surface(MeteorROI* stats, int n, uint32** img, uint32 threshold_min, uint32 threshold_max);
void merge_HI_CCL_v2(uint32** HI, uint32** M, int i0, int i1, int j0, int j1, MeteorROI* stats, int n, int S_min, int S_max);
void rigid_registration(MeteorROI* stats0, MeteorROI* stats1, int n0, int n1, double* theta, double* tx, double* ty);
int shrink_stats(MeteorROI *stats_src, MeteorROI *stats_dest, int n);

double ecartType(MeteorROI *stats, int n, double errMoy);
double errorMoy(MeteorROI *stats, int n);

void motion(MeteorROI *stats0, MeteorROI *stats1, int n0, int n1, double *theta, double *tx, double *ty);

void motion_extraction(MeteorROI *stats0, MeteorROI *stats1, int nc0, double theta, double tx, double ty);
#ifdef __cplusplus
}
#endif

#endif
