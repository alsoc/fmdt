/*!
 * \file
 * \brief *Feature* functions. Connected-Component Analysis (CCA) functions.
 */

#pragma once

#include "fmdt/features/features_struct.h"

/**
 * Allocation of the basic features.
 * @param max_size Maximum capacity of each *feature* field (= maximum number of elements in the arrays).
 * @param RoIs_id Pointer of `max_size` elements to use for the `id` field. If set to NULL, the `id` field is allocated.
 * @return Pointer of allocated RoIs.
 */
RoIs_basic_t* features_alloc_RoIs_basic(const size_t max_size, uint32_t* RoIs_id);

/**
 * Initialization of the basic features. Set all zeros.
 * @param RoIs_basic Pointer of RoIs.
 * @param init_id Boolean to initialize or not the `id` field.
 */
void features_init_RoIs_basic(RoIs_basic_t* RoIs_basic, const uint8_t init_id);

/**
 * Free the features.
 * @param RoIs_basic Pointer of RoIs.
 * @param free_id Boolean to free or not the `id` field.
 */
void features_free_RoIs_basic(RoIs_basic_t* RoIs_basic, const uint8_t free_id);

/**
 * Allocation of the association features.
 * @param max_size Maximum capacity of each *feature* field (= maximum number of elements in the arrays).
 * @param RoIs_id Pointer of `max_size` elements to use for the `id` field. If set to NULL, the `id` field is allocated.
 * @return Pointer of allocated RoIs.
 */
RoIs_asso_t* features_alloc_RoIs_asso(const size_t max_size, uint32_t* RoIs_id);

/**
 * Initialization of the association features. Set all zeros.
 * @param RoIs_asso Pointer of RoIs.
 * @param init_id Boolean to initialize or not the `id` field.
 */
void features_init_RoIs_asso(RoIs_asso_t* RoIs_asso, const uint8_t init_id);

/**
 * Free the features.
 * @param RoIs_asso Pointer of RoIs.
 * @param free_id Boolean to free or not the `id` field.
 */
void features_free_RoIs_asso(RoIs_asso_t* RoIs_asso, const uint8_t free_id);

/**
 * Allocation of the motion features.
 * @param max_size Maximum capacity of each *feature* field (= maximum number of elements in the arrays).
 * @param RoIs_id Pointer of `max_size` elements to use for the `id` field. If set to NULL, the `id` field is allocated.
 * @return Pointer of allocated RoIs.
 */
RoIs_motion_t* features_alloc_RoIs_motion(const size_t max_size, uint32_t* RoIs_id);

/**
 * Initialization of the motion features. Set all zeros.
 * @param RoIs_motion Pointer of RoIs.
 * @param init_id Boolean to initialize or not the `id` field.
 */
void features_init_RoIs_motion(RoIs_motion_t* RoIs_motion, const uint8_t init_id);

/**
 * Free the features.
 * @param RoIs_motion Pointer of RoIs.
 * @param free_id Boolean to free or not the `id` field.
 */
void features_free_RoIs_motion(RoIs_motion_t* RoIs_motion, const uint8_t free_id);

/**
 * Allocation of the miscellaneous features.
 * @param enable_magnitude Boolean to allocate the buffer of magnitudes.
 * @param enable_sat_count Boolean to allocate the buffer of saturation counters.
 * @param enable_ellipse Boolean to allocate the buffers of ellipse features.
 * @param max_size Maximum capacity of each *feature* field (= maximum number of elements in the arrays).
 * @param RoIs_id Pointer of `max_size` elements to use for the `id` field. If set to NULL, the `id` field is allocated.
 * @return Pointer of allocated RoIs.
 */
RoIs_misc_t* features_alloc_RoIs_misc(const uint8_t enable_magnitude, const uint8_t enable_sat_count,
                                      const uint8_t enable_ellipse, const size_t max_size, uint32_t* RoIs_id);

/**
 * Initialization of the miscellaneous features. Set all zeros.
 * @param RoIs_misc Pointer of RoIs.
 * @param init_id Boolean to initialize or not the `id` field.
 */
void features_init_RoIs_misc(RoIs_misc_t* RoIs_misc, const uint8_t init_id);

/**
 * Free the features.
 * @param RoIs_misc Pointer of RoIs.
 * @param free_id Boolean to free or not the `id` field.
 */
void features_free_RoIs_misc(RoIs_misc_t* RoIs_misc, const uint8_t free_id);

/**
 * Allocation of all the features.
 * @param enable_magnitude Boolean to allocate the buffer of magnitudes.
 * @param enable_sat_count Boolean to allocate the buffer of saturation counters.
 * @param enable_ellipse Boolean to allocate the buffers of ellipse features.
 * @param max_size Maximum capacity of each *feature* field (= maximum number of elements in the arrays).
 * @return Pointer of allocated RoIs.
 */
RoIs_t* features_alloc_RoIs(const uint8_t enable_magnitude, const uint8_t enable_sat_count,
                            const uint8_t enable_ellipse, const size_t max_size);

/**
 * Initialization of the features. Set all zeros.
 * @param RoIs Pointer of RoIs.
 */
void features_init_RoIs(RoIs_t* RoIs);

/**
 * Free the features.
 * @param RoIs Pointer of RoIs.
 */
void features_free_RoIs(RoIs_t* RoIs);

/**
 * Basic features extraction from a 2D array of `labels`.
 * In other words, this function converts a (sparse ?) 2-dimensional representation of connected-components (CCs) into a
 * list of CCs.
 * @param labels 2D array of labels (\f$[i1 - i0 + 1][j1 - j0 + 1]\f$).
 * @param i0 First \f$y\f$ index in the labels (included).
 * @param i1 Last \f$y\f$ index in the labels (included).
 * @param j0 First \f$x\f$ index in the labels (included).
 * @param j1 Last \f$x\f$ index in the labels (included).
 * @param RoIs_id Array of RoI unique identifiers.
 * @param RoIs_xmin Array of minimum \f$x\f$ coordinates of the bounding box.
 * @param RoIs_xmax Array of maximum \f$x\f$ coordinates of the bounding box.
 * @param RoIs_ymin Array of minimum \f$y\f$ coordinates of the bounding box.
 * @param RoIs_ymax Array of maximum \f$y\f$ coordinates of the bounding box.
 * @param RoIs_S Array of RoI surfaces.
 * @param RoIs_Sx Array of sums of \f$x\f$ properties.
 * @param RoIs_Sy Array of sums of \f$y\f$ properties.
 * @param RoIs_Sx2 Array of squared sums of \f$x\f$ properties.
 * @param RoIs_Sy2 Array of squared sums of \f$y\f$ properties.
 * @param RoIs_Sxy Array of sums of \f$x * y\f$ properties.
 * @param RoIs_x Array of centroids abscissa.
 * @param RoIs_y Array of centroids ordinate.
 * @param n_RoIs Number of connected-components (= number of RoIs) in the 2D array of `labels`.
 * @see RoIs_basic_t for more explanations about the features.
 */
void _features_extract(const uint32_t** labels, const int i0, const int i1, const int j0, const int j1,
                       uint32_t* RoIs_id, uint32_t* RoIs_xmin, uint32_t* RoIs_xmax, uint32_t* RoIs_ymin,
                       uint32_t* RoIs_ymax, uint32_t* RoIs_S, uint32_t* RoIs_Sx, uint32_t* RoIs_Sy, 
                       uint64_t* RoIs_Sx2, uint64_t* RoIs_Sy2, uint64_t* RoIs_Sxy, float* RoIs_x,
                       float* RoIs_y, const size_t n_RoIs);

/**
 * @param labels Input 2D array of labels (\f$[i1 - i0 + 1][j1 - j0 + 1]\f$).
 * @param i0 First \f$y\f$ index in the labels (included).
 * @param i1 Last \f$y\f$ index in the labels (included).
 * @param j0 First \f$x\f$ index in the labels (included).
 * @param j1 Last \f$x\f$ index in the labels (included).
 * @param n_RoIs Number of connected-components (= number of RoIs) in the 2D array of `labels`.
 * @param RoIs_basic Basic features.
 * @see _features_extract for the explanations about the nature of the processing.
 * @see RoIs_basic_t for more explanations about the features.
 */
void features_extract(const uint32_t** labels, const int i0, const int i1, const int j0, const int j1,
                      const size_t n_RoIs, RoIs_basic_t* RoIs_basic);

/**
 * Hysteresis re-labeling and morphological thresholding. From a 2D array of labels (`in_label`) and a binary image
 * (`img_HI`), the function generates a new 2D array of labels (`out_labels`). The newly produced labels (`out_labels`)
 * are a sub-set of the "old" labels (`in_labels`). Labels from `in_labels` are kept in `out_labels` only if at least
 * one pixel of the current connected-component exists in the binary image (`img_HI`). Finally, this function performs a
 * morphological thresholding as follow: if \f$ S_{min} > S \f$ or \f$ S > S_{max}\f$ then the corresponding `RoIs_id`
 * is set to 0.
 * @param in_labels Input 2D array of labels (\f$[i1 - i0 + 1][j1 - j0 + 1]\f$).
 * @param img_HI Binary image (2D array \f$[i1 - i0 + 1][j1 - j0 + 1]\f$, \f$\{0,1\}\f$ has to be coded as
 *               \f$\{0,255\}\f$). This image results from a threshold filter on the original image. This threshold
 *               filter should be higher than the first one used to compute the initial labels (`in_labels`).
 * @param out_labels Output 2D array of labels (\f$[i1 - i0 + 1][j1 - j0 + 1]\f$). \p out_labels can be NULL, this way
 *                   only the features will be updated. \p out_labels can also be the same pointer as \p in_labels, this
 *                   way the output labels will be computed in place.
 * @param i0 First \f$y\f$ index in the labels (included).
 * @param i1 Last \f$y\f$ index in the labels (included).
 * @param j0 First \f$x\f$ index in the labels (included).
 * @param j1 Last \f$x\f$ index in the labels (included).
 * @param RoIs_id Array of RoI unique identifiers.
 * @param RoIs_xmin Array of minimum \f$x\f$ coordinates of the bounding box.
 * @param RoIs_xmax Array of maximum \f$x\f$ coordinates of the bounding box.
 * @param RoIs_ymin Array of minimum \f$y\f$ coordinates of the bounding box.
 * @param RoIs_ymax Array of maximum \f$y\f$ coordinates of the bounding box.
 * @param RoIs_S Array of RoI surfaces.
 * @param n_RoIs Number of connected-components (= number of RoIs) in the 2D array of `in_labels`.
 * @param S_min Minimum morphological threshold.
 * @param S_max Maximum morphological threshold.
 * @see RoIs_basic_t for more explanations about the features.
 */
void _features_merge_CCL_HI_v2(const uint32_t** in_labels, const uint8_t** img_HI, uint32_t** out_labels, const int i0,
                               const int i1, const int j0, const int j1, uint32_t* RoIs_id, const uint32_t* RoIs_xmin,
                               const uint32_t* RoIs_xmax, const uint32_t* RoIs_ymin, const uint32_t* RoIs_ymax,
                               const uint32_t* RoIs_S, const size_t n_RoIs, const uint32_t S_min, const uint32_t S_max);

/**
 * @param in_labels Input 2D array of labels (\f$[i1 - i0 + 1][j1 - j0 + 1]\f$).
 * @param img_HI Binary image (2D array \f$[i1 - i0 + 1][j1 - j0 + 1]\f$ \f$\{0,1\}\f$ has to be coded as
 *               \f$\{0,255\}\f$). This image results from a threshold filter on the original image. This threshold
 *               filter should be higher than the first one used to compute the initial labels (`in_labels`).
 * @param out_labels Output 2D array of labels (\f$[i1 - i0 + 1][j1 - j0 + 1]\f$).
 * @param i0 First \f$y\f$ index in the labels (included).
 * @param i1 Last \f$y\f$ index in the labels (included).
 * @param j0 First \f$x\f$ index in the labels (included).
 * @param j1 Last \f$x\f$ index in the labels (included).
 * @param RoIs_basic Features.
 * @param S_min Minimum morphological threshold.
 * @param S_max Maximum morphological threshold.
 * @see _features_merge_CCL_HI_v2 for the explanations about the nature of the processing.
 * @see RoIs_basic_t for more explanations about the features.
 */
void features_merge_CCL_HI_v2(const uint32_t** in_labels, const uint8_t** img_HI, uint32_t** out_labels, const int i0,
                              const int i1, const int j0, const int j1, RoIs_basic_t* RoIs_basic, const uint32_t S_min,
                              const uint32_t S_max);

/**
 * Shrink features. Remove features when feature identifier value is 0.
 * Source features (`RoIs_src_X`) are copied into destination features (`RoIs_dst_X`) if `RoIs_src_id` > 0.
 * @param RoIs_src_id Source array of RoI unique identifiers.
 * @param RoIs_src_xmin Source array of minimum \f$x\f$ coordinates of the bounding box.
 * @param RoIs_src_xmax Source array of maximum \f$x\f$ coordinates of the bounding box.
 * @param RoIs_src_ymin Source array of minimum \f$y\f$ coordinates of the bounding box.
 * @param RoIs_src_ymax Source array of maximum \f$y\f$ coordinates of the bounding box.
 * @param RoIs_src_S Source array of RoI surfaces.
 * @param RoIs_src_Sx Source array of sums of \f$x\f$ properties.
 * @param RoIs_src_Sy Source array of sums of \f$y\f$ properties.
 * @param RoIs_src_Sx2 Source array of squared sums of \f$x\f$ properties.
 * @param RoIs_src_Sy2 Source array of squared sums of \f$y\f$ properties.
 * @param RoIs_src_Sxy Source array of sums of \f$x * y\f$ properties.
 * @param RoIs_src_x Source array of centroids abscissas.
 * @param RoIs_src_y Source array of centroids ordinates.
 * @param n_RoIs_src Number of RoIs in the previous arrays.
 * @param RoIs_dst_id Destination array of RoI unique identifiers.
 * @param RoIs_dst_xmin Destination array of minimum \f$x\f$ coordinates of the bounding box.
 * @param RoIs_dst_xmax Destination array of maximum \f$x\f$ coordinates of the bounding box.
 * @param RoIs_dst_ymin Destination array of minimum \f$y\f$ coordinates of the bounding box.
 * @param RoIs_dst_ymax Destination array of maximum \f$y\f$ coordinates of the bounding box.
 * @param RoIs_dst_S Destination array of RoI surfaces.
 * @param RoIs_dst_Sx Destination array of sums of \f$x\f$ properties.
 * @param RoIs_dst_Sy Destination array of sums of \f$y\f$ properties.
 * @param RoIs_dst_Sx2 Destination array of squared sums of \f$x\f$ properties.
 * @param RoIs_dst_Sy2 Destination array of squared sums of \f$y\f$ properties.
 * @param RoIs_dst_Sxy Destination array of sums of \f$x * y\f$ properties.
 * @param RoIs_dst_x Destination array of centroids abscissas.
 * @param RoIs_dst_y Destination array of centroids ordinates.
 * @return Number of regions of interest (RoIs) after the data shrink.
 * @see features_merge_CCL_HI_v2 for more explanations about why some identifiers can be set to 0.
 * @see RoIs_basic_t for more explanations about the features.
 */
size_t _features_shrink_basic(const uint32_t* RoIs_src_id, const uint32_t* RoIs_src_xmin,
                              const uint32_t* RoIs_src_xmax, const uint32_t* RoIs_src_ymin,
                              const uint32_t* RoIs_src_ymax, const uint32_t* RoIs_src_S, const uint32_t* RoIs_src_Sx,
                              const uint32_t* RoIs_src_Sy, const uint64_t* RoIs_src_Sx2, const uint64_t* RoIs_src_Sy2,
                              const uint64_t* RoIs_src_Sxy, const float* RoIs_src_x, const float* RoIs_src_y,
                              const size_t n_RoIs_src, uint32_t* RoIs_dst_id, uint32_t* RoIs_dst_xmin,
                              uint32_t* RoIs_dst_xmax, uint32_t* RoIs_dst_ymin, uint32_t* RoIs_dst_ymax,
                              uint32_t* RoIs_dst_S, uint32_t* RoIs_dst_Sx, uint32_t* RoIs_dst_Sy,
                              uint64_t* RoIs_dst_Sx2, uint64_t* RoIs_dst_Sy2, uint64_t* RoIs_dst_Sxy, float* RoIs_dst_x,
                              float* RoIs_dst_y);

/**
 * @param RoIs_basic_src Source features.
 * @param RoIs_basic_dst Destination features.
 * @see _features_shrink_basic for the explanations about the nature of the processing.
 * @see RoIs_basic_t for more explanations about the features.
 */
void features_shrink_basic(const RoIs_basic_t* RoIs_basic_src, RoIs_basic_t* RoIs_basic_dst);

/**
 * Shrink features. Remove features when feature identifier value is 0.
 * Source features (`RoIs_src_X`) are copied into destination features (`RoIs_dst_X`) if `RoIs_src_id` > 0.
 * This function adds the miscellaneous features compared to the `_features_shrink_basic` function.
 * @param RoIs_src_id Source array of RoI unique identifiers.
 * @param RoIs_src_xmin Source array of minimum \f$x\f$ coordinates of the bounding box.
 * @param RoIs_src_xmax Source array of maximum \f$x\f$ coordinates of the bounding box.
 * @param RoIs_src_ymin Source array of minimum \f$y\f$ coordinates of the bounding box.
 * @param RoIs_src_ymax Source array of maximum \f$y\f$ coordinates of the bounding box.
 * @param RoIs_src_S Source array of RoI surfaces.
 * @param RoIs_src_Sx Source array of sums of \f$x\f$ properties.
 * @param RoIs_src_Sy Source array of sums of \f$y\f$ properties.
 * @param RoIs_src_Sx2 Source array of squared sums of \f$x\f$ properties.
 * @param RoIs_src_Sy2 Source array of squared sums of \f$y\f$ properties.
 * @param RoIs_src_Sxy Source array of sums of \f$x * y\f$ properties.
 * @param RoIs_src_x Source array of centroids abscissas.
 * @param RoIs_src_y Source array of centroids ordinates.
 * @param RoIs_src_magnitude Array of RoI magnitudes.
 * @param RoIs_src_sat_count Array of saturation counters.
 * @param RoIs_src_a Array of RoI semi-major axis.
 * @param RoIs_src_b Array of RoI semi-minor axis.
 * @param n_RoIs_src Number of RoIs in the previous arrays.
 * @param RoIs_dst_id Destination array of RoI unique identifiers.
 * @param RoIs_dst_xmin Destination array of minimum \f$x\f$ coordinates of the bounding box.
 * @param RoIs_dst_xmax Destination array of maximum \f$x\f$ coordinates of the bounding box.
 * @param RoIs_dst_ymin Destination array of minimum \f$y\f$ coordinates of the bounding box.
 * @param RoIs_dst_ymax Destination array of maximum \f$y\f$ coordinates of the bounding box.
 * @param RoIs_dst_S Destination array of RoI surfaces.
 * @param RoIs_dst_Sx Destination array of sums of \f$x\f$ properties.
 * @param RoIs_dst_Sy Destination array of sums of \f$y\f$ properties.
 * @param RoIs_dst_Sx2 Destination array of squared sums of \f$x\f$ properties.
 * @param RoIs_dst_Sy2 Destination array of squared sums of \f$y\f$ properties.
 * @param RoIs_dst_Sxy Destination array of sums of \f$x * y\f$ properties.
 * @param RoIs_dst_x Destination array of centroids abscissas.
 * @param RoIs_dst_y Destination array of centroids ordinates.
 * @param RoIs_dst_magnitude Destination array of RoI magnitudes.
 * @param RoIs_dst_sat_count Destination array of saturation counters.
 * @param RoIs_dst_a Destination array of RoI semi-major axis.
 * @param RoIs_dst_b Destination array of RoI semi-minor axis.
 * @return size_t Number of regions of interest (RoIs) after the data shrink.
 * @see features_merge_CCL_HI_v2 for more explanations about why some identifiers can be set to 0.
 * @see RoIs_basic_t for more explanations about the features.
 * @see RoIs_misc_t for more explanations about the features.
 */
size_t _features_shrink_basic_misc(const uint32_t* RoIs_src_id, const uint32_t* RoIs_src_xmin,
                                   const uint32_t* RoIs_src_xmax, const uint32_t* RoIs_src_ymin,
                                   const uint32_t* RoIs_src_ymax, const uint32_t* RoIs_src_S,
                                   const uint32_t* RoIs_src_Sx, const uint32_t* RoIs_src_Sy,
                                   const uint64_t* RoIs_src_Sx2, const uint64_t* RoIs_src_Sy2,
                                   const uint64_t* RoIs_src_Sxy, const float* RoIs_src_x, const float* RoIs_src_y,
                                   const uint32_t* RoIs_src_magnitude, const uint32_t* RoIs_src_sat_count,
                                   const float* RoIs_src_a, const float* RoIs_src_b, const size_t n_RoIs_src,
                                   uint32_t* RoIs_dst_id, uint32_t* RoIs_dst_xmin, uint32_t* RoIs_dst_xmax,
                                   uint32_t* RoIs_dst_ymin, uint32_t* RoIs_dst_ymax, uint32_t* RoIs_dst_S,
                                   uint32_t* RoIs_dst_Sx, uint32_t* RoIs_dst_Sy, uint64_t* RoIs_dst_Sx2,
                                   uint64_t* RoIs_dst_Sy2, uint64_t* RoIs_dst_Sxy, float* RoIs_dst_x,
                                   float* RoIs_dst_y, uint32_t* RoIs_dst_magnitude, uint32_t* RoIs_dst_sat_count,
                                   float* RoIs_dst_a, float* RoIs_dst_b);

/**
 * @param RoIs_basic_src Source features.
 * @param RoIs_misc_src Source features.
 * @param RoIs_basic_dst Destination features.
 * @param RoIs_misc_dst Destination features.
 * @see _features_shrink_basic_misc for the explanations about the nature of the processing.
 */
void features_shrink_basic_misc(const RoIs_basic_t* RoIs_basic_src, const RoIs_misc_t* RoIs_misc_src,
                                RoIs_basic_t* RoIs_basic_dst, RoIs_misc_t* RoIs_misc_dst);

/**
 * Compute magnitude features. The magnitude represents the brightness of a RoI.
 * In a first time, the sum of the pixels intensities is performed. In a second time, the noise level around the
 * connected-component is subtracted to give a better estimation of the real brightness.
 * The magnitude can be defined as follow:
 * \f$ M = \displaystyle\sum_{p=0} ^{P} i_p - \big((\displaystyle\sum_{n=0} ^{N} i_n) / N\big) \times P\f$,
 * where \f$P\f$ is the the number of pixels in the current CC, \f$i_x\f$ is the brightness of the pixel \f$x\f$
 * and \f$N\f$ is the number of noisy pixels considered.
 * In addition, this function can also compute the saturation counter for each RoI (e. g. the number of pixels that have
 * an intensity \f$i_x = 255\f$).
 * @param img Image in grayscale (\f$[\texttt{img\_height}][\texttt{img\_width}]\f$, the values of the pixel
 *            range are \f$ [ 0;255 ] \f$).
 * @param img_width Image width.
 * @param img_height Image height.
 * @param labels 2D array of labels (\f$[\texttt{img\_height}][\texttt{img\_width}]\f$).
 * @param RoIs_xmin Array of minimum \f$x\f$ coordinates of the bounding box.
 * @param RoIs_xmax Array of maximum \f$x\f$ coordinates of the bounding box.
 * @param RoIs_ymin Array of minimum \f$y\f$ coordinates of the bounding box.
 * @param RoIs_ymax Array of maximum \f$y\f$ coordinates of the bounding box.
 * @param RoIs_S Array of RoI surfaces.
 * @param RoIs_magnitude Array of RoI magnitudes.
 * @param RoIs_sat_count Array of RoI saturation counters (if NULL, the saturation counter is not computed).
 * @param n_RoIs Number of connected-components (= number of RoIs).
 * @see RoIs_basic_t for more explanations about the basic features.
 * @see RoIs_misc_t for more explanations about the miscellaneous features.
 */
void _features_compute_magnitude(const uint8_t** img, const uint32_t img_width, const uint32_t img_height,
                                 const uint32_t** labels, const uint32_t* RoIs_xmin, const uint32_t* RoIs_xmax,
                                 const uint32_t* RoIs_ymin, const uint32_t* RoIs_ymax, const uint32_t* RoIs_S,
                                 uint32_t* RoIs_magnitude, uint32_t* RoIs_sat_count, const size_t n_RoIs);

/**
 * @param img Image in grayscale (\f$[\texttt{img\_height}][\texttt{img\_width}]\f$, the values of the pixel
 *            range are \f$ [ 0;255 ] \f$).
 * @param img_width Image width.
 * @param img_height Image height.
 * @param labels 2D array of labels (\f$[\texttt{img\_height}][\texttt{img\_width}]\f$).
 * @param RoIs_basic Basic features.
 * @param RoIs_misc Miscellaneous features (including the magnitudes).
 * @see _features_compute_magnitude for the explanations about the nature of the processing.
 * @see RoIs_basic_t for more explanations about the basic features.
 * @see RoIs_misc_t for more explanations about the miscellaneous features.
 */
void features_compute_magnitude(const uint8_t** img, const uint32_t img_width, const uint32_t img_height,
                                const uint32_t** labels, const RoIs_basic_t* RoIs_basic, RoIs_misc_t* RoIs_misc);

/**
 * Compute the semi-major and the semi-minor axes of RoIs.
 * 
 * @param RoIs_S 
 * @param RoIs_Sx Sums of \f$x\f$ properties.
 * @param RoIs_Sy Sums of \f$y\f$ properties.
 * @param RoIs_Sx2 Sums of squared \f$x\f$ properties.
 * @param RoIs_Sy2 Sums of squared \f$x\f$ properties.
 * @param RoIs_Sxy Sums of \f$xy\f$ properties.
 * @param RoIs_a Semi-major axis.
 * @param RoIs_b Semi-minor axis.
 * @param n_RoIs Number of connected-components (= number of RoIs).
 */
void _features_compute_ellipse(const uint32_t *RoIs_S, const uint32_t *RoIs_Sx, const uint32_t *RoIs_Sy, 
                               const uint64_t *RoIs_Sx2, const uint64_t *RoIs_Sy2, const uint64_t* RoIs_Sxy, 
                               float *RoIs_a, float *RoIs_b, const size_t n_RoIs);

/**
 * Compute the semi-major and the semi-minor axes of RoIs.
 *
 * @param RoIs_basic Basic features.
 * @param RoIs_misc Miscellaneous features (including the `a` and `b` features).
 */
void features_compute_ellipse(const RoIs_basic_t* RoIs_basic, RoIs_misc_t* RoIs_misc);
