#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "fmdt/macros.h"
#include "fmdt/tools.h"
#include "fmdt/features.h"

ROI_t* features_alloc_ROI_array(const size_t max_size) {
    ROI_t* ROI_array = (ROI_t*)malloc(sizeof(ROI_t));
    ROI_array->id = (uint16_t*)malloc(max_size * sizeof(uint16_t));
    ROI_array->frame = (uint32_t*)malloc(max_size * sizeof(uint32_t));
    ROI_array->xmin = (uint16_t*)malloc(max_size * sizeof(uint16_t));
    ROI_array->xmax = (uint16_t*)malloc(max_size * sizeof(uint16_t));
    ROI_array->ymin = (uint16_t*)malloc(max_size * sizeof(uint16_t));
    ROI_array->ymax = (uint16_t*)malloc(max_size * sizeof(uint16_t));
    ROI_array->S = (uint32_t*)malloc(max_size * sizeof(uint32_t));
    ROI_array->Sx = (uint32_t*)malloc(max_size * sizeof(uint32_t));
    ROI_array->Sy = (uint32_t*)malloc(max_size * sizeof(uint32_t));
    ROI_array->Sx2 = (uint32_t*)malloc(max_size * sizeof(uint32_t));
    ROI_array->Sy2 = (uint32_t*)malloc(max_size * sizeof(uint32_t));
    ROI_array->Sxy = (uint32_t*)malloc(max_size * sizeof(uint32_t));
    ROI_array->x = (float*)malloc(max_size * sizeof(float));
    ROI_array->y = (float*)malloc(max_size * sizeof(float));
    ROI_array->a = (float*)malloc(max_size * sizeof(float));
    ROI_array->b = (float*)malloc(max_size * sizeof(float));
    ROI_array->dx = (float*)malloc(max_size * sizeof(float));
    ROI_array->dy = (float*)malloc(max_size * sizeof(float));
    ROI_array->error = (float*)malloc(max_size * sizeof(float));
    ROI_array->time = (int32_t*)malloc(max_size * sizeof(int32_t));
    ROI_array->time_motion = (int32_t*)malloc(max_size * sizeof(int32_t));
    ROI_array->prev_id = (int32_t*)malloc(max_size * sizeof(int32_t));
    ROI_array->next_id = (int32_t*)malloc(max_size * sizeof(int32_t));
    ROI_array->is_moving = (uint8_t*)malloc(max_size * sizeof(uint8_t));
    ROI_array->_max_size = max_size;
    return ROI_array;
}

void features_init_ROI_array(ROI_t* ROI_array) {
    memset(ROI_array->id, 0, ROI_array->_max_size * sizeof(uint16_t));
    memset(ROI_array->frame, 0, ROI_array->_max_size * sizeof(uint32_t));
    memset(ROI_array->xmin, 0, ROI_array->_max_size * sizeof(uint16_t));
    memset(ROI_array->xmax, 0, ROI_array->_max_size * sizeof(uint16_t));
    memset(ROI_array->ymin, 0, ROI_array->_max_size * sizeof(uint16_t));
    memset(ROI_array->ymax, 0, ROI_array->_max_size * sizeof(uint16_t));
    memset(ROI_array->S, 0, ROI_array->_max_size * sizeof(uint32_t));
    memset(ROI_array->Sx, 0, ROI_array->_max_size * sizeof(uint32_t));
    memset(ROI_array->Sy, 0, ROI_array->_max_size * sizeof(uint32_t));    
    memset(ROI_array->Sx2, 0, ROI_array->_max_size * sizeof(uint32_t));
    memset(ROI_array->Sy2, 0, ROI_array->_max_size * sizeof(uint32_t));
    memset(ROI_array->Sxy, 0, ROI_array->_max_size * sizeof(uint32_t));
    memset(ROI_array->x, 0, ROI_array->_max_size * sizeof(float));
    memset(ROI_array->y, 0, ROI_array->_max_size * sizeof(float));
    memset(ROI_array->a, 0, ROI_array->_max_size * sizeof(float));
    memset(ROI_array->b, 0, ROI_array->_max_size * sizeof(float));
    memset(ROI_array->dx, 0, ROI_array->_max_size * sizeof(float));
    memset(ROI_array->dy, 0, ROI_array->_max_size * sizeof(float));
    memset(ROI_array->error, 0, ROI_array->_max_size * sizeof(float));
    memset(ROI_array->time, 0, ROI_array->_max_size * sizeof(int32_t));
    memset(ROI_array->time_motion, 0, ROI_array->_max_size * sizeof(int32_t));
    memset(ROI_array->prev_id, 0, ROI_array->_max_size * sizeof(int32_t));
    memset(ROI_array->next_id, 0, ROI_array->_max_size * sizeof(int32_t));
    memset(ROI_array->is_moving, 0, ROI_array->_max_size * sizeof(uint8_t));
    ROI_array->_size = 0;
}

void features_copy_elmt_ROI_array(const ROI_t* ROI_array_src, ROI_t* ROI_array_dest, const int i_src,
                                  const int i_dest) {
    ROI_array_dest->id[i_dest] = ROI_array_src->id[i_src];
    ROI_array_dest->frame[i_dest] = ROI_array_src->frame[i_src];
    ROI_array_dest->xmin[i_dest] = ROI_array_src->xmin[i_src];
    ROI_array_dest->xmax[i_dest] = ROI_array_src->xmax[i_src];
    ROI_array_dest->ymin[i_dest] = ROI_array_src->ymin[i_src];
    ROI_array_dest->ymax[i_dest] = ROI_array_src->ymax[i_src];
    ROI_array_dest->S[i_dest] = ROI_array_src->S[i_src];
    ROI_array_dest->Sx[i_dest] = ROI_array_src->Sx[i_src];
    ROI_array_dest->Sy[i_dest] = ROI_array_src->Sy[i_src];    
    ROI_array_dest->Sx2[i_dest] = ROI_array_src->Sx2[i_src];
    ROI_array_dest->Sy2[i_dest] = ROI_array_src->Sy2[i_src];
    ROI_array_dest->Sxy[i_dest] = ROI_array_src->Sxy[i_src];
    ROI_array_dest->x[i_dest] = ROI_array_src->x[i_src];
    ROI_array_dest->y[i_dest] = ROI_array_src->y[i_src];
    ROI_array_dest->a[i_dest] = ROI_array_src->a[i_src];
    ROI_array_dest->b[i_dest] = ROI_array_src->b[i_src];
    ROI_array_dest->dx[i_dest] = ROI_array_src->dx[i_src];
    ROI_array_dest->dy[i_dest] = ROI_array_src->dy[i_src];
    ROI_array_dest->error[i_dest] = ROI_array_src->error[i_src];
    ROI_array_dest->time[i_dest] = ROI_array_src->time[i_src];
    ROI_array_dest->time_motion[i_dest] = ROI_array_src->time_motion[i_src];
    ROI_array_dest->prev_id[i_dest] = ROI_array_src->prev_id[i_src];
    ROI_array_dest->next_id[i_dest] = ROI_array_src->next_id[i_src];
    ROI_array_dest->is_moving[i_dest] = ROI_array_src->is_moving[i_src];
}

void features_copy_ROI_array(const ROI_t* ROI_array_src, ROI_t* ROI_array_dest) {
    ROI_array_dest->_size = ROI_array_src->_size;
    memcpy(ROI_array_dest->id, ROI_array_src->id, ROI_array_dest->_size * sizeof(uint16_t));
    memcpy(ROI_array_dest->frame, ROI_array_src->frame, ROI_array_dest->_size * sizeof(uint32_t));
    memcpy(ROI_array_dest->xmin, ROI_array_src->xmin, ROI_array_dest->_size * sizeof(uint16_t));
    memcpy(ROI_array_dest->xmax, ROI_array_src->xmax, ROI_array_dest->_size * sizeof(uint16_t));
    memcpy(ROI_array_dest->ymin, ROI_array_src->ymin, ROI_array_dest->_size * sizeof(uint16_t));
    memcpy(ROI_array_dest->ymax, ROI_array_src->ymax, ROI_array_dest->_size * sizeof(uint16_t));
    memcpy(ROI_array_dest->S, ROI_array_src->S, ROI_array_dest->_size * sizeof(uint32_t));
    memcpy(ROI_array_dest->Sx, ROI_array_src->Sx, ROI_array_dest->_size * sizeof(uint32_t));
    memcpy(ROI_array_dest->Sy, ROI_array_src->Sy, ROI_array_dest->_size * sizeof(uint32_t));
    memcpy(ROI_array_dest->Sx2, ROI_array_src->Sx2, ROI_array_dest->_size * sizeof(uint32_t));
    memcpy(ROI_array_dest->Sy2, ROI_array_src->Sy2, ROI_array_dest->_size * sizeof(uint32_t));
    memcpy(ROI_array_dest->Sxy, ROI_array_src->Sxy, ROI_array_dest->_size * sizeof(uint32_t));
    memcpy(ROI_array_dest->x, ROI_array_src->x, ROI_array_dest->_size * sizeof(float));
    memcpy(ROI_array_dest->y, ROI_array_src->y, ROI_array_dest->_size * sizeof(float));
    memcpy(ROI_array_dest->a, ROI_array_src->a, ROI_array_dest->_size * sizeof(float));
    memcpy(ROI_array_dest->b, ROI_array_src->b, ROI_array_dest->_size * sizeof(float));
    memcpy(ROI_array_dest->dx, ROI_array_src->dx, ROI_array_dest->_size * sizeof(float));
    memcpy(ROI_array_dest->dy, ROI_array_src->dy, ROI_array_dest->_size * sizeof(float));
    memcpy(ROI_array_dest->error, ROI_array_src->error, ROI_array_dest->_size * sizeof(float));
    memcpy(ROI_array_dest->time, ROI_array_src->time, ROI_array_dest->_size * sizeof(int32_t));
    memcpy(ROI_array_dest->time_motion, ROI_array_src->time_motion, ROI_array_dest->_size * sizeof(int32_t));
    memcpy(ROI_array_dest->prev_id, ROI_array_src->prev_id, ROI_array_dest->_size * sizeof(int32_t));
    memcpy(ROI_array_dest->next_id, ROI_array_src->next_id, ROI_array_dest->_size * sizeof(int32_t));
    memcpy(ROI_array_dest->is_moving, ROI_array_src->is_moving, ROI_array_dest->_size * sizeof(uint8_t));
}

void features_free_ROI_array(ROI_t* ROI_array) {
    free(ROI_array->id);
    free(ROI_array->frame);
    free(ROI_array->xmin);
    free(ROI_array->xmax);
    free(ROI_array->ymin);
    free(ROI_array->ymax);
    free(ROI_array->S);
    free(ROI_array->Sx);
    free(ROI_array->Sy);
    free(ROI_array->Sx2);
    free(ROI_array->Sy2);
    free(ROI_array->Sxy);
    free(ROI_array->x);
    free(ROI_array->y);
    free(ROI_array->a);
    free(ROI_array->b);
    free(ROI_array->dx);
    free(ROI_array->dy);
    free(ROI_array->error);
    free(ROI_array->time);
    free(ROI_array->time_motion);
    free(ROI_array->prev_id);
    free(ROI_array->next_id);
    free(ROI_array->is_moving);
    free(ROI_array);
}

void features_clear_index_ROI_array(ROI_t* ROI_array, const size_t r) {
    ROI_array->id[r] = 0;
    ROI_array->frame[r] = 0;
    ROI_array->xmin[r] = 0;
    ROI_array->xmax[r] = 0;
    ROI_array->ymin[r] = 0;
    ROI_array->ymax[r] = 0;
    ROI_array->S[r] = 0;
    ROI_array->Sx[r] = 0;
    ROI_array->Sy[r] = 0;
    ROI_array->Sx2[r] = 0;
    ROI_array->Sy2[r] = 0;
    ROI_array->Sxy[r] = 0;
    ROI_array->x[r] = 0;
    ROI_array->y[r] = 0;
    ROI_array->a[r] = 0;
    ROI_array->b[r] = 0;
    ROI_array->dx[r] = 0;
    ROI_array->dy[r] = 0;
    ROI_array->error[r] = 0;
    ROI_array->time[r] = 0;
    ROI_array->time_motion[r] = 0;
    ROI_array->prev_id[r] = 0;
    ROI_array->next_id[r] = 0;
    ROI_array->is_moving[r] = 0;
}

void features_init_ROI(ROI_t* stats, int n) {
    for (int i = 0; i < n; i++)
        memset(stats + i, 0, sizeof(ROI_t));
}

void _features_extract(const uint32_t** img, const int i0, const int i1, const int j0, const int j1, uint16_t* ROI_id,
                       uint16_t* ROI_xmin, uint16_t* ROI_xmax, uint16_t* ROI_ymin, uint16_t* ROI_ymax, uint32_t* ROI_S,
                       uint32_t* ROI_Sx, uint32_t* ROI_Sy, uint32_t* ROI_Sx2, uint32_t* ROI_Sy2, uint32_t* ROI_Sxy, float* ROI_x, float* ROI_y, const size_t n_ROI) {
    memset(ROI_xmin, j1, n_ROI * sizeof(uint16_t));
    memset(ROI_xmax, j0, n_ROI * sizeof(uint16_t));
    memset(ROI_ymin, i1, n_ROI * sizeof(uint16_t));
    memset(ROI_ymax, i0, n_ROI * sizeof(uint16_t));
    memset(ROI_S, 0, n_ROI * sizeof(uint32_t));
    memset(ROI_Sx, 0, n_ROI * sizeof(uint32_t));
    memset(ROI_Sy, 0, n_ROI * sizeof(uint32_t));    
    memset(ROI_Sx2, 0, n_ROI * sizeof(uint32_t));
    memset(ROI_Sy2, 0, n_ROI * sizeof(uint32_t));
    memset(ROI_Sxy, 0, n_ROI * sizeof(uint32_t));


    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            uint32_t e = (uint32_t)img[i][j];
            if (e > 0) {
                uint32_t r = e - 1;
                ROI_S[r] += 1;
                ROI_id[r] = e;
                ROI_Sx[r] += j;
                ROI_Sy[r] += i;
                ROI_Sx2[r] += j*j;
                ROI_Sy2[r] += i*i;
                ROI_Sxy[r] += j*i;

                if (j < ROI_xmin[r])
                    ROI_xmin[r] = j;
                if (j > ROI_xmax[r])
                    ROI_xmax[r] = j;
                if (i < ROI_ymin[r])
                    ROI_ymin[r] = i;
                if (i > ROI_ymax[r])
                    ROI_ymax[r] = i;
            }
        }
    }

    for (size_t i = 0; i < n_ROI; i++) {
        ROI_x[i] = (double)ROI_Sx[i] / (double)ROI_S[i];
        ROI_y[i] = (double)ROI_Sy[i] / (double)ROI_S[i];
    }
}

void features_extract(const uint32_t** img, const int i0, const int i1, const int j0, const int j1, const size_t n_ROI,
                      ROI_t* ROI_array) {
    features_init_ROI_array(ROI_array);
    ROI_array->_size = n_ROI;
    _features_extract(img, i0, i1, j0, j1, ROI_array->id, ROI_array->xmin, ROI_array->xmax, ROI_array->ymin,
                      ROI_array->ymax, ROI_array->S, ROI_array->Sx, ROI_array->Sy, ROI_array->Sx2, ROI_array->Sy2,
                      ROI_array->Sxy, ROI_array->x, ROI_array->y, ROI_array->_size);
}

void _features_merge_HI_CCL_v2(const uint32_t** M, const uint8_t** HI_in, uint8_t** HI_out, const int i0, const int i1,
                               const int j0, const int j1, const uint16_t* ROI_id, const uint16_t* ROI_xmin,
                               const uint16_t* ROI_xmax, const uint16_t* ROI_ymin, const uint16_t* ROI_ymax,
                               uint32_t* ROI_S, const size_t n_ROI, const uint32_t S_min, const uint32_t S_max) {
    if ((void*)HI_in != (void*)HI_out)
        for (int i = i0; i <= i1; i++)
            // memcpy(HI_out[i] + j0, HI_in[i] + j0, sizeof(uint8_t) * ((j1 - j0) + 1));
            for (int j = j0; j <= j1; j++){
                HI_out[i][j] = 255 * HI_in[i][j];
            }


    int x0, x1, y0, y1, id;
    for (size_t i = 0; i < n_ROI; i++) {
        if (ROI_S[i]) {
            id = ROI_id[i];
            x0 = ROI_ymin[i];
            x1 = ROI_ymax[i];
            y0 = ROI_xmin[i];
            y1 = ROI_xmax[i];
            if (S_min > ROI_S[i] || ROI_S[i] > S_max) {
                ROI_S[i] = 0;
                // JUSTE POUR DEBUG (Affichage frames)
                for (int k = x0; k <= x1; k++) {
                    for (int l = y0; l <= y1; l++) {
                        if ((uint32_t)M[k][l] == (uint32_t)id)
                            HI_out[k][l] = (uint8_t)0;
                    }
                }
                continue;
            }
            for (int k = x0; k <= x1; k++) {
                for (int l = y0; l <= y1; l++) {
                    if (HI_out[k][l]) {
                        for (k = x0; k <= x1; k++) {
                            for (l = y0; l <= y1; l++) {
                                if ((uint32_t)M[k][l] == (uint32_t)id) {
                                    HI_out[k][l] = (uint8_t)MAX(MIN(i + 1, 255), 80);
                                }
                            }
                        }
                        goto next;
                    }
                }
            }
            ROI_S[i] = 0;
        next:;
        }
    }
}

void features_merge_HI_CCL_v2(const uint32_t** M, const uint8_t** HI_in, uint8_t** HI_out, const int i0, const int i1,
                              const int j0, const int j1, ROI_t* ROI_array, const uint32_t S_min, const uint32_t S_max)
{
    _features_merge_HI_CCL_v2(M, HI_in, HI_out, i0,i1, j0, j1, ROI_array->id, ROI_array->xmin, ROI_array->xmax,
                              ROI_array->ymin, ROI_array->ymax, ROI_array->S,  ROI_array->_size, S_min, S_max);
}

void features_filter_surface(ROI_t* ROI_array, uint32_t** img, uint32_t threshold_min, uint32_t threshold_max) {
    // Doit on vraiment modifier l'image de départ? ou juste les stats.
    uint32_t S, e;
    int i0, i1, j0, j1;
    uint16_t id;

    for (size_t i = 0; i < ROI_array->_size; i++) {
        S = ROI_array->S[i];
        id = ROI_array->id[i];

        if (S == 0)
            continue; // DEBUG

        if (S < threshold_min || S > threshold_max) {
            ROI_array->S[i] = 0;

            // pour affichage debbug
            i0 = ROI_array->ymin[i];
            i1 = ROI_array->ymax[i];
            j0 = ROI_array->xmin[i];
            j1 = ROI_array->xmax[i];
            for (int i = i0; i <= i1; i++) {
                for (int j = j0; j <= j1; j++) {
                    e = img[i][j];
                    if (e == id) {
                        img[i][j] = 0;
                    }
                }
            }
        }
    }
}

void _calc_ellipse_status(uint32_t* ROI_S, float* ROI_a, float* ROI_b, uint32_t* ROI_Sx, uint32_t* ROI_Sy, uint32_t* ROI_Sx2, uint32_t* ROI_Sy2, uint32_t* ROI_Sxy, const size_t n_ROI)
{

    for(int e = 0; e < n_ROI; e++) {
        
        if(ROI_S[e]>0) {

            double S = ROI_S[e];
            
            double Sx = ROI_Sx[e];
            double Sy = ROI_Sy[e];
            
            // moments non centres
            double Sxx = ROI_Sx2[e];
            double Sxy = ROI_Sxy[e];
            double Syy = ROI_Sy2[e];
            
            // moments centres
            double m20 = S * Sxx - Sx * Sx;
            double m11 = S * Sxy - Sx * Sy;
            double m02 = S * Syy - Sy * Sy;
            
            // par construction a > b
            double a2 = (m20 + m02 + sqrtf( (m20 - m02) * (m20 - m02) + 4 * m11 * m11)) / (2 * S);
            double b2 = (m20 + m02 - sqrtf( (m20 - m02) * (m20 - m02) + 4 * m11 * m11)) / (2 * S);
            
            float a = sqrt(a2);
            float b = sqrt(b2);
            float ratio = a / b; // metrique plus simple a visualiser que l'applatissement
            
            
            ROI_a[e] = a;
            ROI_b[e] = b;
            // fprintf(stderr, "# %2d   S = %f   m20 = %f a = %5.1f   b = %5.1f   ratio = %5.1f \n", e, S, m11, ROI_a[e], ROI_b[e], ratio);
            
            //putchar('\n');
        }
    }
}

void calc_ellipse_status(ROI_t* ROI_array) {
    _calc_ellipse_status(ROI_array->S, ROI_array->a, ROI_array->b, ROI_array->Sx, ROI_array->Sy, ROI_array->Sx2, ROI_array->Sy2, ROI_array->Sxy, ROI_array->_size);
}

int _filter_features_ellipse_ratio_status(const float* ROI_a, const float* ROI_b, const size_t n_ROI, const float min_ratio, uint32_t* ROI_S)
{
    int c = 0;
    for(int e = 0; e < n_ROI; e++) {
        
        if(ROI_S[e] >0) {
            
            float a = ROI_a[e];
            float b = ROI_b[e];
            float ratio = a / b;
            
            if(ratio < min_ratio) {
                ROI_S[e] = 0;
            } else {
                c++;
            }
        }
    }
    return c;
}

int filter_features_ellipse_ratio_status(ROI_t* ROI_array, const float min_ratio) 
{
    _filter_features_ellipse_ratio_status((const float*)ROI_array->a, (const float*)ROI_array->b, ROI_array->_size, min_ratio, ROI_array->S);
}





size_t _features_shrink_ROI_array(const uint16_t* ROI_src_id, const uint16_t* ROI_src_xmin,
                                  const uint16_t* ROI_src_xmax, const uint16_t* ROI_src_ymin,
                                  const uint16_t* ROI_src_ymax, const uint32_t* ROI_src_S, const uint32_t* ROI_src_Sx,
                                  const uint32_t* ROI_src_Sy, const uint32_t* ROI_src_Sx2, const uint32_t* ROI_src_Sy2, const uint32_t* ROI_src_Sxy,
                                  const float* ROI_src_x, const float* ROI_src_y, const float* ROI_src_a, const float* ROI_src_b, const size_t n_ROI_src, 
                                  uint16_t* ROI_dest_id, uint16_t* ROI_dest_xmin, uint16_t* ROI_dest_xmax, 
                                  uint16_t* ROI_dest_ymin, uint16_t* ROI_dest_ymax, uint32_t* ROI_dest_S, 
                                  uint32_t* ROI_dest_Sx, uint32_t* ROI_dest_Sy, uint32_t* ROI_dest_Sx2,
                                  uint32_t* ROI_dest_Sy2,  uint32_t* ROI_dest_Sxy, float* ROI_dest_x, float* ROI_dest_y, float* ROI_dest_a, float* ROI_dest_b) {
    size_t cpt = 0;
    for (size_t i = 0; i < n_ROI_src; i++) {
        if (ROI_src_S[i] > 0) {
            ROI_dest_id[cpt] = cpt + 1;
            ROI_dest_xmin[cpt] = ROI_src_xmin[i];
            ROI_dest_xmax[cpt] = ROI_src_xmax[i];
            ROI_dest_ymin[cpt] = ROI_src_ymin[i];
            ROI_dest_ymax[cpt] = ROI_src_ymax[i];
            ROI_dest_S[cpt] = ROI_src_S[i];
            ROI_dest_Sx[cpt] = ROI_src_Sx[i];
            ROI_dest_Sy[cpt] = ROI_src_Sy[i];
            ROI_dest_Sx2[cpt] = ROI_src_Sx2[i];
            ROI_dest_Sy2[cpt] = ROI_src_Sy2[i];
            ROI_dest_Sxy[cpt] = ROI_src_Sxy[i];
            ROI_dest_x[cpt] = ROI_src_x[i];
            ROI_dest_y[cpt] = ROI_src_y[i];
            ROI_dest_a[cpt] = ROI_src_a[i];
            ROI_dest_b[cpt] = ROI_src_b[i];

            cpt++;
        }
    }
    return cpt;
}

void features_shrink_ROI_array(const ROI_t* ROI_array_src, ROI_t* ROI_array_dest) {
    ROI_array_dest->_size = _features_shrink_ROI_array(ROI_array_src->id, ROI_array_src->xmin, ROI_array_src->xmax,
                                                       ROI_array_src->ymin, ROI_array_src->ymax, ROI_array_src->S,
                                                       ROI_array_src->Sx, ROI_array_src->Sy, ROI_array_src->Sx2, ROI_array_src->Sy2, ROI_array_src->Sxy,
                                                       ROI_array_src->x,
                                                       ROI_array_src->y, ROI_array_src->a, ROI_array_src->b, ROI_array_src->_size, ROI_array_dest->id,
                                                       ROI_array_dest->xmin, ROI_array_dest->xmax, ROI_array_dest->ymin,
                                                       ROI_array_dest->ymax, ROI_array_dest->S, ROI_array_dest->Sx,
                                                       ROI_array_dest->Sy, ROI_array_dest->Sx2, ROI_array_dest->Sy2, ROI_array_dest->Sxy, 
                                                       ROI_array_dest->x, ROI_array_dest->y, ROI_array_dest->a, ROI_array_dest->b);
}

void _features_rigid_registration(const int32_t* ROI0_next_id, const float* ROI0_x, const float* ROI0_y,
                                  const size_t n_ROI0, const float* ROI1_x, const float* ROI1_y, double* theta,
                                  double* tx, double* ty) {
    double Sx, Sxp, Sy, Syp, Sx_xp, Sxp_y, Sx_yp, Sy_yp;
    double x0, y0, x1, y1;
    double a, b;
    double xg, yg, xpg, ypg;
    int asso;
    int cpt;

    Sx = 0;
    Sxp = 0;
    Sy = 0;
    Syp = 0;
    Sx_xp = 0;
    Sxp_y = 0;
    Sx_yp = 0;
    Sy_yp = 0;
    cpt = 0;

    // parcours tab assos
    for (size_t i = 0; i < n_ROI0; i++) {
        asso = ROI0_next_id[i];
        if (asso) {
            cpt++;
            Sx += ROI0_x[i];
            Sy += ROI0_y[i];
            Sxp += ROI1_x[asso - 1];
            Syp += ROI1_y[asso - 1];
        }
    }

    xg = Sx / cpt;
    yg = Sy / cpt;
    xpg = Sxp / cpt;
    ypg = Syp / cpt;

    Sx = 0;
    Sxp = 0;
    Sy = 0;
    Syp = 0;

    // parcours tab assos
    for (size_t i = 0; i < n_ROI0; i++) {
        asso = ROI0_next_id[i];
        if (asso) {
            // cpt++;
            x0 = ROI0_x[i] - xg;
            y0 = ROI0_y[i] - yg;
            x1 = ROI1_x[asso - 1] - xpg;
            y1 = ROI1_y[asso - 1] - ypg;

            Sx += x0;
            Sy += y0;
            Sxp += x1;
            Syp += y1;
            Sx_xp += x0 * x1;
            Sxp_y += x1 * y0;
            Sx_yp += x0 * y1;
            Sy_yp += y0 * y1;
        }
    }
    a = cpt * cpt * (Sx_yp - Sxp_y) + (1 - 2 * cpt) * (Sx * Syp - Sxp * Sy);
    b = cpt * cpt * (Sx_xp + Sy_yp) + (1 - 2 * cpt) * (Sx * Sxp + Syp * Sy);

    *theta = atan2(a, b);
    *tx = xpg - cos(*theta) * xg + sin(*theta) * yg;
    *ty = ypg - sin(*theta) * xg - cos(*theta) * yg;
}

void features_rigid_registration(const ROI_t* ROI_array0, const ROI_t* ROI_array1, double* theta, double* tx,
                                 double* ty) {
    _features_rigid_registration(ROI_array0->next_id, ROI_array0->x, ROI_array0->y, ROI_array0->_size, ROI_array1->x,
                                 ROI_array1->y, theta, tx, ty);
}

void _features_rigid_registration_corrected(const int32_t* ROI0_next_id, const float* ROI0_x, const float* ROI0_y,
                                            const float* ROI0_error, uint8_t* ROI0_is_moving, const size_t n_ROI0,
                                            const float* ROI1_x, const float* ROI1_y, double* theta, double* tx,
                                            double* ty, double mean_error, double std_deviation) {
    double Sx, Sxp, Sy, Syp, Sx_xp, Sxp_y, Sx_yp, Sy_yp;
    double x0, y0, x1, y1;
    double a, b;
    double xg, yg, xpg, ypg;
    int asso;
    int cpt;

    Sx = 0;
    Sxp = 0;
    Sy = 0;
    Syp = 0;
    Sx_xp = 0;
    Sxp_y = 0;
    Sx_yp = 0;
    Sy_yp = 0;
    cpt = 0;

    int cpt1 = 0;
    // parcours tab assos
    for (size_t i = 0; i < n_ROI0; i++) {
        if (fabs(ROI0_error[i] - mean_error) > std_deviation) {
            ROI0_is_moving[i] = 1;
            cpt1++;
            continue;
        }
        asso = ROI0_next_id[i];
        if (asso) {
            Sx += ROI0_x[i];
            Sy += ROI0_y[i];
            Sxp += ROI1_x[asso - 1];
            Syp += ROI1_y[asso - 1];
            cpt++;
        }
    }

    xg = Sx / cpt;
    yg = Sy / cpt;
    xpg = Sxp / cpt;
    ypg = Syp / cpt;

    Sx = 0;
    Sxp = 0;
    Sy = 0;
    Syp = 0;

    // parcours tab assos
    for (size_t i = 0; i < n_ROI0; i++) {
        if (fabs(ROI0_error[i] - mean_error) > std_deviation)
            continue;
        asso = ROI0_next_id[i];
        if (asso) {
            // cpt++;
            x0 = ROI0_x[i] - xg;
            y0 = ROI0_y[i] - yg;
            x1 = ROI1_x[asso - 1] - xpg;
            y1 = ROI1_y[asso - 1] - ypg;

            Sx += x0;
            Sy += y0;
            Sxp += x1;
            Syp += y1;
            Sx_xp += x0 * x1;
            Sxp_y += x1 * y0;
            Sx_yp += x0 * y1;
            Sy_yp += y0 * y1;
        }
    }
    a = cpt * cpt * (Sx_yp - Sxp_y) + (1 - 2 * cpt) * (Sx * Syp - Sxp * Sy);
    b = cpt * cpt * (Sx_xp + Sy_yp) + (1 - 2 * cpt) * (Sx * Sxp + Syp * Sy);

    *theta = atan2(a, b);
    *tx = xpg - cos(*theta) * xg + sin(*theta) * yg;
    *ty = ypg - sin(*theta) * xg - cos(*theta) * yg;
}

void features_rigid_registration_corrected(ROI_t* ROI_array0, const ROI_t* ROI_array1, double* theta, double* tx,
                                           double* ty, double mean_error, double std_deviation) {
    _features_rigid_registration_corrected(ROI_array0->next_id, ROI_array0->x, ROI_array0->y, ROI_array0->error,
                                           ROI_array0->is_moving, ROI_array0->_size, ROI_array1->x, ROI_array1->y,
                                           theta, tx, ty, mean_error, std_deviation);
}


// TODO: Pour l'optimisation : faire une version errorMoy_corrected()
double _features_compute_mean_error(const int32_t* ROI_next_id, const float* ROI_error, const uint8_t* ROI_is_moving,
                                    const size_t n_ROI) {
    double S = 0.0;
    int cpt = 0;
    for (size_t i = 0; i < n_ROI; i++) {
        if (ROI_is_moving[i] || !ROI_next_id[i])
            continue;
        S += ROI_error[i];
        cpt++;
    }
    return S / cpt;
}

double features_compute_mean_error(const ROI_t* ROI_array) {
    return _features_compute_mean_error(ROI_array->next_id, ROI_array->error, ROI_array->is_moving, ROI_array->_size);
}

// TODO: Pour l'optimisation : faire une version ecartType_corrected()
double _features_compute_std_deviation(const int32_t* ROI_next_id, const float* ROI_error, const uint8_t* ROI_is_moving,
                                       const size_t n_ROI, const double mean_error) {
    double S = 0.0;
    int cpt = 0;
    for (size_t i = 0; i < n_ROI; i++) {
        if (ROI_is_moving[i] || !ROI_next_id[i])
            continue;
        float e = ROI_error[i];
        S += ((e - mean_error) * (e - mean_error));
        cpt++;
    }
    return sqrt(S / cpt);
}

double features_compute_std_deviation(const ROI_t* ROI_array, const double mean_error) {
    return _features_compute_std_deviation(ROI_array->next_id, ROI_array->error, ROI_array->is_moving, ROI_array->_size,
                                           mean_error);
}

void _features_motion_extraction(const int32_t* ROI0_next_id, const float* ROI0_x, const float* ROI0_y,
                                 float* ROI0_dx, float* ROI0_dy, float* ROI0_error, const size_t n_ROI0,
                                 const float* ROI1_x, const float* ROI1_y, double theta, double tx, double ty) {
    int cc1;
    double x, y, xp, yp;
    float dx, dy;
    float e;

    for (size_t i = 0; i < n_ROI0; i++) {
        cc1 = ROI0_next_id[i];
        if (cc1) {
            // coordonees du point dans l'image I+1
            xp = ROI1_x[cc1 - 1];
            yp = ROI1_y[cc1 - 1];
            // calcul de (x,y) pour l'image I
            x = cos(theta) * (xp - tx) + sin(theta) * (yp - ty);
            y = cos(theta) * (yp - ty) - sin(theta) * (xp - tx);

            // pas besoin de stocker dx et dy (juste pour l'affichage du debug)
            dx = x - ROI0_x[i];
            dy = y - ROI0_y[i];
            ROI0_dx[i] = dx;
            ROI0_dy[i] = dy;

            e = sqrt(dx * dx + dy * dy);
            ROI0_error[i] = e;
        }
    }
}

void features_motion_extraction(ROI_t* ROI_array0, const ROI_t* ROI_array1, double theta, double tx, double ty) {
    _features_motion_extraction(ROI_array0->next_id, ROI_array0->x, ROI_array0->y, ROI_array0->dx, ROI_array0->dy,
                                ROI_array0->error, ROI_array0->_size, ROI_array1->x, ROI_array1->y, theta, tx, ty);
}

void _features_compute_motion(const int32_t* ROI0_next_id, const float* ROI0_x, const float* ROI0_y, float* ROI0_dx,
                              float* ROI0_dy, float* ROI0_error, uint8_t* ROI0_is_moving, const size_t n_ROI0,
                              const float* ROI1_x, const float* ROI1_y, double* first_theta, double* first_tx,
                              double* first_ty, double* first_mean_error, double* first_std_deviation, double* theta,
                              double* tx, double* ty, double* mean_error, double* std_deviation) {
    _features_rigid_registration(ROI0_next_id, ROI0_x, ROI0_y, n_ROI0, ROI1_x, ROI1_y, first_theta, first_tx, first_ty);
    _features_motion_extraction(ROI0_next_id, ROI0_x, ROI0_y, ROI0_dx, ROI0_dy, ROI0_error, n_ROI0, ROI1_x, ROI1_y,
                                *first_theta, *first_tx, *first_ty);
    *first_mean_error = _features_compute_mean_error(ROI0_next_id, ROI0_error, ROI0_is_moving, n_ROI0);
    *first_std_deviation = _features_compute_std_deviation(ROI0_next_id, ROI0_error, ROI0_is_moving, n_ROI0,
                                                           *first_mean_error);
    // saveErrorMoy("first_error.txt", mean_error, std_deviation);
    _features_rigid_registration_corrected(ROI0_next_id, ROI0_x, ROI0_y, ROI0_error, ROI0_is_moving, n_ROI0, ROI1_x,
                                           ROI1_y, theta, tx, ty, *first_mean_error, *first_std_deviation);
    _features_motion_extraction(ROI0_next_id, ROI0_x, ROI0_y, ROI0_dx, ROI0_dy, ROI0_error, n_ROI0, ROI1_x, ROI1_y,
                                *theta, *tx, *ty);
    *mean_error = _features_compute_mean_error(ROI0_next_id, ROI0_error, ROI0_is_moving, n_ROI0);
    *std_deviation = _features_compute_std_deviation(ROI0_next_id, ROI0_error, ROI0_is_moving, n_ROI0, *mean_error);
}

void features_compute_motion(const ROI_t* ROI_array1, ROI_t* ROI_array0, double* first_theta, double* first_tx,
                             double* first_ty, double* first_mean_error, double* first_std_deviation, double* theta,
                             double* tx, double* ty, double* mean_error, double* std_deviation) {
    _features_compute_motion(ROI_array0->next_id, ROI_array0->x, ROI_array0->y, ROI_array0->dx, ROI_array0->dy,
                             ROI_array0->error, ROI_array0->is_moving, ROI_array0->_size, ROI_array1->x, ROI_array1->y,
                             first_theta, first_tx, first_ty, first_mean_error, first_std_deviation, theta, tx, ty,
                             mean_error, std_deviation);
}

void features_print_stats(ROI_t* stats, int n) {
    int cpt = 0;
    for (int i = 0; i < n; i++) {
        if (stats->S[i] > 0) {
            cpt++;
        }
    }
    printf("Nombre de CC : %d\n", cpt);

    if (cpt == 0)
        return;

    for (int i = 0; i < n; i++) {
        if (stats->S[i] > 0)
            printf("%4d \t %4d \t %4d \t %4d \t %4d \t %3d \t %4d \t %4d \t %7.1f \t %7.1f \t %4d \t %4d \t %4d \t "
                   "%7.1lf \t %d\n",
                   stats->id[i], stats->xmin[i], stats->xmax[i], stats->ymin[i], stats->ymax[i], stats->S[i],
                   stats->Sx[i], stats->Sy[i], stats->x[i], stats->y[i], stats->prev_id[i], stats->next_id[i],
                   stats->time[i], stats->error[i], stats->is_moving[i]);
    }
    printf("\n");
}

void features_parse_stats(const char* filename, ROI_t* stats, int* n) {
    char lines[200];
    int id, xmin, xmax, ymin, ymax, s, sx, sy, prev_id, next_id;
    double x, y;
    float dx, dy, error;
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "(EE) cannot open file '%s'\n", filename);
        exit(-1);
    }

    // pour l'instant, n représente l'id max des stas mais c'est à changer
    if (fgets(lines, 100, file) == NULL) {
        fprintf(stderr, "(EE) something went wrong when reading '%s'\n", filename);
        exit(-1);
    }
    sscanf(lines, "%d", n);

    while (fgets(lines, 200, file)) {
        sscanf(lines, "%d %d %d %d %d %d %d %d %lf %lf %f %f %f %d %d", &id, &xmin, &xmax, &ymin, &ymax, &s, &sx, &sy,
               &x, &y, &dx, &dy, &error, &prev_id, &next_id);
        stats->id[id - 1] = id;
        stats->xmin[id - 1] = xmin;
        stats->xmax[id - 1] = xmax;
        stats->ymin[id - 1] = ymin;
        stats->ymax[id - 1] = ymax;
        stats->S[id - 1] = s;
        stats->Sx[id - 1] = sx;
        stats->Sy[id - 1] = sy;
        stats->x[id - 1] = x;
        stats->y[id - 1] = y;
        stats->dx[id - 1] = dx;
        stats->dy[id - 1] = dy;
        stats->error[id - 1] = error;
        stats->prev_id[id - 1] = prev_id;
        stats->next_id[id - 1] = next_id;
    }
    fclose(file);
}

int _find_corresponding_track(const uint16_t* track_id, const ROI_light_t* track_end, const size_t n_tracks,
                              const uint16_t* ROI_id, const int sel_ROI_id, const unsigned age) {
    assert(age == 0 || age == 1);
    for (size_t t = 0; t < n_tracks; t++) {
        if (track_id[t]) {
            int cur_ROI_id = (age == 0) ? track_end[t].id : ROI_id[track_end[t].prev_id - 1];
            if (cur_ROI_id <= 0)
                continue;
            if (sel_ROI_id == cur_ROI_id)
                return t;
        }
    }
    return -1;
}

int find_corresponding_track(const track_t* track_array, const ROI_t* ROI_array, const int sel_ROI_id,
                             const unsigned age) {
    return _find_corresponding_track(track_array->id, track_array->end, track_array->_size, ROI_array->id, sel_ROI_id,
                                     age);
}

void _features_ROI_write(FILE* f, const uint16_t* ROI_id, const uint16_t* ROI_xmin, const uint16_t* ROI_xmax,
                         const uint16_t* ROI_ymin, const uint16_t* ROI_ymax, const uint32_t* ROI_S,
                         const uint32_t* ROI_Sx, const uint32_t* ROI_Sy, const uint32_t* ROI_Sx2, const uint32_t* ROI_Sy2, const uint32_t* ROI_Sxy,
                         const float* ROI_x, const float* ROI_y, const float* ROI_a, const float* ROI_b,
                         const int32_t* ROI_time, const int32_t* ROI_time_motion, const size_t n_ROI,
                         const uint16_t* track_id, const ROI_light_t* track_end, const enum obj_e* track_obj_type,
                         const size_t n_tracks, const unsigned age) {
    int cpt = 0;
    for (size_t i = 0; i < n_ROI; i++)
        if (ROI_S[i] != 0)
            cpt++;

    fprintf(f, "# Regions of interest (ROI) [%d]: \n", cpt);
    if (cpt) {
        fprintf(f, "# ------||----------------||---------------------------||-------------------------------------------------------------------||-------------------||------------------------------||-----------------\n");
        fprintf(f, "#   ROI ||      Track     ||        Bounding Box       ||           Surface (S in pixels)                                   ||      Center       ||          Ellipse             ||       Time      \n");
        fprintf(f, "# ------||----------------||---------------------------||-------------------------------------------------------------------||-------------------||------------------------------||-----------------\n");
        fprintf(f, "# ------||------|---------||------|------|------|------||-----|-----------|-----------|-----------|------------|------------||---------|---------||---------|---------|----------||--------|--------\n");
        fprintf(f, "#    ID ||   ID |    Type || xmin | xmax | ymin | ymax ||   S |     Sx    |     Sy    |    Sx2    |     Sy2    |     Sxy    ||       x |       y ||       a |       b |   ratio  ||    All | Motion \n");
        fprintf(f, "# ------||------|---------||------|------|------|------||-----|-----------|-----------|-----------|------------|------------||---------|---------||---------|---------|----------||--------|--------\n");
    }

    for (size_t i = 0; i < n_ROI; i++) {
        if (ROI_S[i] != 0) {
            int t = _find_corresponding_track(track_id, track_end, n_tracks, ROI_id, ROI_id[i], age);
            char task_id_str[16];
            if (t == -1)
                strcpy(task_id_str, "   -");
            else
                sprintf(task_id_str, "%4d", track_id[t]);
            char task_obj_type[64];
            if (t == -1)
                strcpy(task_obj_type, "      -");
            else
                sprintf(task_obj_type, "%s", g_obj_to_string_with_spaces[track_obj_type[t]]);
            fprintf(f, "   %4d || %s | %s || %4d | %4d | %4d | %4d || %4d | %9d | %9d | %9d | %9d | %9d || %7.1f | %7.1f || %7.1f | %7.1f | %7.1f ||%6d | %6d \n",
                    ROI_id[i], task_id_str, task_obj_type, ROI_xmin[i], ROI_xmax[i], ROI_ymin[i], ROI_ymax[i], ROI_S[i],
                    ROI_Sx[i], ROI_Sy[i], ROI_Sx2[i], ROI_Sy2[i], ROI_Sxy[i], ROI_x[i], ROI_y[i], ROI_a[i], ROI_b[i], ROI_a[i]/ROI_b[i], ROI_time[i], ROI_time_motion[i]);
        }
    }
}

void features_ROI_write(FILE* f, const ROI_t* ROI_array, const track_t* track_array, const unsigned age) {
    _features_ROI_write(f, ROI_array->id, ROI_array->xmin, ROI_array->xmax, ROI_array->ymin, ROI_array->ymax,
                        ROI_array->S, ROI_array->Sx, ROI_array->Sy, ROI_array->Sx2, ROI_array->Sy2, ROI_array->Sxy, ROI_array->x, ROI_array->y, ROI_array->a, ROI_array->b,
                        ROI_array->time, ROI_array->time_motion, ROI_array->_size, track_array->id,
                        track_array->end, track_array->obj_type, track_array->_size, age);
}

void features_motion_write(FILE* f, const double first_theta, const double first_tx, const double first_ty,
                           const double first_mean_error, const double first_std_deviation, const double theta,
                           const double tx, const double ty, const double mean_error, const double std_deviation) {
    fprintf(f, "# Motion:\n");
    fprintf(f, "# * first theta         = %6.7f\n", first_theta);
    fprintf(f, "# * first tx            = %6.4f\n", first_tx);
    fprintf(f, "# * first ty            = %6.4f\n", first_ty);
    fprintf(f, "# * first mean error    = %.3f\n", first_mean_error);
    fprintf(f, "# * first std deviation = %.3f\n", first_std_deviation);
    fprintf(f, "# * theta               = %6.7f\n", theta);
    fprintf(f, "# * tx                  = %6.4f\n", tx);
    fprintf(f, "# * ty                  = %6.4f\n", ty);
    fprintf(f, "# * mean error          = %.3f\n", mean_error);
    fprintf(f, "# * std deviation       = %.3f\n", std_deviation);
}

void _features_ROI0_ROI1_write(FILE* f, const int frame, const uint16_t* ROI0_id, const uint16_t* ROI0_xmin,
                               const uint16_t* ROI0_xmax, const uint16_t* ROI0_ymin, const uint16_t* ROI0_ymax,
                               const uint32_t* ROI0_S, const uint32_t* ROI0_Sx, const uint32_t* ROI0_Sy,
                               const uint32_t* ROI0_Sx2, const uint32_t* ROI0_Sy2, const uint32_t* ROI0_Sxy,
                               const float* ROI0_x, const float* ROI0_y, const float* ROI0_a, const float* ROI0_b,const int32_t* ROI0_time,
                               const int32_t* ROI0_time_motion, const size_t n_ROI0, const uint16_t* ROI1_id,
                               const uint16_t* ROI1_xmin, const uint16_t* ROI1_xmax, const uint16_t* ROI1_ymin,
                               const uint16_t* ROI1_ymax, const uint32_t* ROI1_S, const uint32_t* ROI1_Sx,
                               const uint32_t* ROI1_Sy, const uint32_t* ROI1_Sx2, const uint32_t* ROI1_Sy2, const uint32_t* ROI1_Sxy, 
                               const float* ROI1_x, const float* ROI1_y, const float* ROI1_a, const float* ROI1_b,
                               const int32_t* ROI1_time, const int32_t* ROI1_time_motion, const size_t n_ROI1,
                               const uint16_t* track_id, const ROI_light_t* track_end, const enum obj_e* track_obj_type,
                               const size_t n_tracks) {
    // stats
    fprintf(f, "# Frame n°%05d (cur)\n", frame - 1);
    _features_ROI_write(f, ROI0_id, ROI0_xmin, ROI0_xmax, ROI0_ymin, ROI0_ymax, ROI0_S, ROI0_Sx, ROI0_Sy, ROI0_Sx2, ROI0_Sy2, ROI0_Sxy, ROI0_x,
                        ROI0_y, ROI0_a, ROI0_b, ROI0_time, ROI0_time_motion, n_ROI0, track_id, track_end, track_obj_type, n_tracks, 1);

    fprintf(f, "#\n# Frame n°%05d (next)\n", frame);
    _features_ROI_write(f, ROI1_id, ROI1_xmin, ROI1_xmax, ROI1_ymin, ROI1_ymax, ROI1_S, ROI1_Sx, ROI1_Sy, ROI1_Sx2, ROI1_Sy2, ROI1_Sxy, ROI1_x,
                        ROI1_y, ROI1_a, ROI1_b, ROI1_time, ROI1_time_motion, n_ROI1, track_id, track_end, track_obj_type, n_tracks, 0);
}

void features_ROI0_ROI1_write(FILE* f, const int frame, const ROI_t* ROI_array0, const ROI_t* ROI_array1,
                              const track_t* track_array) {
    _features_ROI0_ROI1_write(f, frame, ROI_array0->id, ROI_array0->xmin, ROI_array0->xmax, ROI_array0->ymin,
                              ROI_array0->ymax, ROI_array0->S, ROI_array0->Sx, ROI_array0->Sy, 
                              ROI_array0->Sx2, ROI_array0->Sy2, ROI_array0->Sxy, ROI_array0->x,
                              ROI_array0->y, ROI_array0->a, ROI_array0->b, ROI_array0->time, ROI_array0->time_motion, ROI_array0->_size,
                              ROI_array1->id, ROI_array1->xmin, ROI_array1->xmax, ROI_array1->ymin, ROI_array1->ymax,
                              ROI_array1->S, ROI_array1->Sx, ROI_array1->Sy, ROI_array1->Sx2, ROI_array1->Sy2, ROI_array1->Sxy,
                               ROI_array1->x, ROI_array1->y, ROI_array1->a, ROI_array1->b,
                              ROI_array1->time, ROI_array1->time_motion, ROI_array1->_size, track_array->id,
                              track_array->end, track_array->obj_type, track_array->_size);
}

void features_save_motion_extraction(const char* filename, const ROI_t* ROI_array, const double theta, const double tx,
                                     const double ty, const int frame) {
    // Version DEBUG : il faut implémenter une version pour le main
    FILE* f = fopen(filename, "a");
    if (f == NULL) {
        fprintf(stderr, "motion : cannot open file\n");
        return;
    }

    double mean_error = features_compute_mean_error(ROI_array);
    double std_deviation = features_compute_std_deviation(ROI_array, mean_error);

    for (size_t i = 0; i < ROI_array->_size; i++) {
        float e = ROI_array->error[i];
        // si mouvement detecté
        if (fabs(e - mean_error) > 1.5 * std_deviation) {
            fprintf(f, "%d - %d\n", frame, frame + 1);
            fprintf(f,
                    "CC en mouvement: %2d \t dx:%.3f \t dy: %.3f \t xmin: %3d \t xmax: %3d \t ymin: %3d \t ymax: %3d\n",
                    ROI_array->id[i], ROI_array->dx[i], ROI_array->dy[i], ROI_array->xmin[i], ROI_array->xmax[i],
                    ROI_array->ymin[i], ROI_array->ymax[i]);
            fprintf(f, "---------------------------------------------------------------\n");
        }
    }
    fclose(f);
}