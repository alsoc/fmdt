/*!
 * \file
 * \brief *Feature* functions. Connected-Component Analysis (CCA) functions.
 */

#pragma once

#include "fmdt/features/features_struct.h"

/**
 * Allocation of the basic features.
 * @param max_size The maximum number of elements contained in *feature* arrays.
 * @param ROI_id A pointer of `max_size` elements to use for `id` field. If set to NULL, the `id` field is allocated.
 * @return The allocated ROIs.
 */
ROI_basic_t* features_alloc_ROI_basic(const size_t max_size, uint32_t* ROI_id);

/**
 * Zero initialization of the basic features.
 * @param ROI_basic_array The ROIs pointer.
 * @param init_id A boolean to initialize or not the `id` field.
 */
void features_init_ROI_basic(ROI_basic_t* ROI_basic_array, const uint8_t init_id);

/**
 * Free the features.
 * @param ROI_basic_array The ROIs pointer.
 * @param free_id A boolean to free or not the `id` field.
 */
void features_free_ROI_basic(ROI_basic_t* ROI_basic_array, const uint8_t free_id);

/**
 * Allocation of the association features.
 * @param max_size The maximum number of elements contained in *feature* arrays.
 * @param ROI_id A pointer of `max_size` elements to use for `id` field. If set to NULL, the `id` field is allocated.
 * @return The allocated ROIs.
 */
ROI_asso_t* features_alloc_ROI_asso(const size_t max_size, uint32_t* ROI_id);

/**
 * Zero initialization of the association features.
 * @param ROI_asso_array The ROIs pointer.
 * @param init_id A boolean to initialize or not the `id` field.
 */
void features_init_ROI_asso(ROI_asso_t* ROI_asso_array, const uint8_t init_id);

/**
 * Free the features.
 * @param ROI_asso_array The ROIs pointer.
 * @param free_id A boolean to free or not the `id` field.
 */
void features_free_ROI_asso(ROI_asso_t* ROI_asso_array, const uint8_t free_id);

/**
 * Allocation of the motion features.
 * @param max_size The maximum number of elements contained in *feature* arrays.
 * @param ROI_id A pointer of `max_size` elements to use for `id` field. If set to NULL, the `id` field is allocated.
 * @return The allocated ROIs.
 */
ROI_motion_t* features_alloc_ROI_motion(const size_t max_size, uint32_t* ROI_id);

/**
 * Zero initialization of the motion features.
 * @param ROI_motion_array The ROIs pointer.
 * @param init_id A boolean to initialize or not the `id` field.
 */
void features_init_ROI_motion(ROI_motion_t* ROI_motion_array, const uint8_t init_id);

/**
 * Free the features.
 * @param ROI_motion_array The ROIs pointer.
 * @param free_id A boolean to free or not the `id` field.
 */
void features_free_ROI_motion(ROI_motion_t* ROI_motion_array, const uint8_t free_id);

/**
 * Allocation of the miscellaneous features.
 * @param max_size The maximum number of elements contained in *feature* arrays.
 * @param ROI_id A pointer of `max_size` elements to use for `id` field. If set to NULL, the `id` field is allocated.
 * @return The allocated ROIs.
 */
ROI_misc_t* features_alloc_ROI_misc(const size_t max_size, uint32_t* ROI_id);

/**
 * Zero initialization of the miscellaneous features.
 * @param ROI_misc_array The ROIs pointer.
 * @param init_id A boolean to initialize or not the `id` field.
 */
void features_init_ROI_misc(ROI_misc_t* ROI_misc_array, const uint8_t init_id);

/**
 * Free the features.
 * @param ROI_misc_array The ROIs pointer.
 * @param free_id A boolean to free or not the `id` field.
 */
void features_free_ROI_misc(ROI_misc_t* ROI_misc_array, const uint8_t free_id);

/**
 * Allocation of all the features.
 * @param max_size The maximum number of elements contained in feature arrays.
 * @return The allocated ROIs.
 */
ROI_t* features_alloc_ROI(const size_t max_size);

/**
 * Zero initialization of all the features.
 * @param ROI_array Allocated ROIs.
 */
void features_init_ROI(ROI_t* ROI_array);

/**
 * Free the features.
 * @param ROI_array Allocated ROIs.
 */
void features_free_ROI(ROI_t* ROI_array);

/**
 * Basic features extraction from a 2D array of `labels.
 * @param labels Input 2D array of labels (\f$[i1 - i0 + 1][j1 - j0 + 1]\f$).
 * @param i0 The first y index in the labels (included).
 * @param i1 The last y index in the labels (included).
 * @param j0 The first x index in the labels (included).
 * @param j1 The last x index in the labels (included).
 * @param ROI_id Output array of ROI unique identifiers.
 * @param ROI_xmin Output array of minimum \f$x\f$ values of the bounding box.
 * @param ROI_xmax Output array of maximum \f$x\f$ values of the bounding box.
 * @param ROI_ymin Output array of minimum \f$y\f$ values of the bounding box.
 * @param ROI_ymax Output array of maximum \f$y\f$ values of the bounding box.
 * @param ROI_S Output array of ROI surfaces.
 * @param ROI_Sx Output array of sums of \f$x\f$ properties.
 * @param ROI_Sy Output array of sums of \f$y\f$ properties.
 * @param ROI_x Output array of centroid abscissas.
 * @param ROI_y Output array of centroid ordinates.
 * @param n_ROI Number of connected-components (= ROIs) in the 2D array of `labels`.
 * @see ROI_basic_t for more explanations about the features .
 */
void _features_extract(const uint32_t** labels, const int i0, const int i1, const int j0, const int j1,
                       uint32_t* ROI_id, uint32_t* ROI_xmin, uint32_t* ROI_xmax, uint32_t* ROI_ymin, uint32_t* ROI_ymax,
                       uint32_t* ROI_S, uint32_t* ROI_Sx, uint32_t* ROI_Sy, float* ROI_x, float* ROI_y,
                       const size_t n_ROI);

/**
 * Basic features extraction from a 2D array of `labels`.
 * @param labels Input 2D array of labels (\f$[i1 - i0 + 1][j1 - j0 + 1]\f$).
 * @param i0 The first y index in the labels (included).
 * @param i1 The last y index in the labels (included).
 * @param j0 The first x index in the labels (included).
 * @param j1 The last x index in the labels (included).
 * @param n_ROI Number of connected-components (= ROIs) in the 2D array of `labels`.
 * @param ROI_basic_array Output basic features.
 * @see ROI_basic_t for more explanations about the features.
 */
void features_extract(const uint32_t** labels, const int i0, const int i1, const int j0, const int j1,
                      const size_t n_ROI, ROI_basic_t* ROI_basic_array);


/**
 * Hysteresis re-labeling and morphological thresholding. From a 2D array of labels (`in_label`) and a binary image
 * (`img_HI`), it generates a new 2D array of labels (`out_labels`). The newly produced `out_labels` are a sub-set of
 * `in_labels`. Labels from `in_labels` are kept in `out_labels` only if at least one pixel of the current
 * connected-component exists in the `img_HI` binary image. Additionally, this function perform a morphological
 * threshold: if \f$ S_{min} > S \f$ or \f$ S > S_{max}\f$ then the corresponding `ROI_id` is set to 0.
 * @param in_labels Input 2D array of labels (\f$[i1 - i0 + 1][j1 - j0 + 1]\f$).
 * @param img_HI A binary image (2D array \f$[i1 - i0 + 1][j1 - j0 + 1]\f$, \f$\{0,1\}\f$ has to be coded as
 *               \f$\{0,255\}\f$), resulting from a threshold filter on the original image. This threshold filter
 *               should be HIgher than the first one used to compute `in_labels`).
 * @param out_labels Output 2D array of labels (\f$[i1 - i0 + 1][j1 - j0 + 1]\f$).
 * @param i0 The first y index in the labels (included).
 * @param i1 The last y index in the labels (included).
 * @param j0 The first x index in the labels (included).
 * @param j1 The last x index in the labels (included).
 * @param ROI_id Input / output array of ROI unique identifiers.
 * @param ROI_xmin Input array of minimum \f$x\f$ values of the bounding box.
 * @param ROI_xmax Input array of maximum \f$x\f$ values of the bounding box.
 * @param ROI_ymin Input array of minimum \f$y\f$ values of the bounding box.
 * @param ROI_ymax Input array of maximum \f$y\f$ values of the bounding box.
 * @param ROI_S Input array of ROI surfaces.
 * @param n_ROI Number of connected-components (= ROIs) in the 2D array of `in_labels`.
 * @param S_min Minimum morphological threshold.
 * @param S_max Maximum morphological threshold.
 * @see ROI_basic_t for more explanations about the features.
 */
void _features_merge_CCL_HI_v2(const uint32_t** in_labels, const uint8_t** img_HI, uint32_t** out_labels, const int i0,
                               const int i1, const int j0, const int j1, uint32_t* ROI_id, const uint32_t* ROI_xmin,
                               const uint32_t* ROI_xmax, const uint32_t* ROI_ymin, const uint32_t* ROI_ymax,
                               const uint32_t* ROI_S, const size_t n_ROI, const uint32_t S_min, const uint32_t S_max);

/**
 * Hysteresis re-labeling and morphological thresholding. From a 2D array of labels (`in_label`) and a binary image
 * (`img_HI`), it generates a new 2D array of labels (`out_labels`). The newly produced `out_labels` are a sub-set of
 * `in_labels`. Labels from `in_labels` are kept in `out_labels` only if at least one pixel of the current
 * connected-component exists in the `img_HI` binary image. Additionally, this function perform a morphological
 * threshold: if \f$ S_{min} > S \f$ or \f$ S > S_{max}\f$ then the corresponding `ROI_id` is set to 0.
 * @param in_labels Input 2D array of labels (\f$[i1 - i0 + 1][j1 - j0 + 1]\f$).
 * @param img_HI A binary image (2D array \f$[i1 - i0 + 1][j1 - j0 + 1]\f$, \f$\{0,1\}\f$ has to be coded as
 *               \f$\{0,255\}\f$), resulting from a threshold filter on the original image. This threshold filter
 *               should be HIgher than the first one used to compute `in_labels`).
 * @param out_labels Output 2D array of labels (\f$[i1 - i0 + 1][j1 - j0 + 1]\f$).
 * @param i0 The first y index in the labels (included).
 * @param i1 The last y index in the labels (included).
 * @param j0 The first x index in the labels (included).
 * @param j1 The last x index in the labels (included).
 * @param ROI_basic_array Input / output features.
 * @param S_min Minimum morphological threshold.
 * @param S_max Maximum morphological threshold.
 * @see ROI_basic_t for more explanations about the features.
 */
void features_merge_CCL_HI_v2(const uint32_t** in_labels, const uint8_t** img_HI, uint32_t** out_labels, const int i0,
                              const int i1, const int j0, const int j1, ROI_basic_t* ROI_basic_array,
                              const uint32_t S_min, const uint32_t S_max);

/**
 * Shrink features. Remove features when feature identifier is set to 0.
 * Source features (`ROI_src_X`) are copied into destination features (`ROI_dst_X`) if `ROI_src_id` > 0.
 * @param ROI_src_id Input array of ROI unique identifiers.
 * @param ROI_src_xmin Input array of minimum \f$x\f$ values of the bounding box.
 * @param ROI_src_xmax Input array of maximum \f$x\f$ values of the bounding box.
 * @param ROI_src_ymin Input array of minimum \f$y\f$ values of the bounding box.
 * @param ROI_src_ymax Input array of maximum \f$y\f$ values of the bounding box.
 * @param ROI_src_S Input array of ROI surfaces.
 * @param ROI_src_Sx Input array of sums of \f$x\f$ properties.
 * @param ROI_src_Sy Input array of sums of \f$y\f$ properties.
 * @param ROI_src_x Input array of centroid abscissas.
 * @param ROI_src_y Input array of centroid ordinates.
 * @param ROI_dst_id Output array of ROI unique identifiers.
 * @param ROI_dst_xmin Output array of minimum \f$x\f$ values of the bounding box.
 * @param ROI_dst_xmax Output array of maximum \f$x\f$ values of the bounding box.
 * @param ROI_dst_ymin Output array of minimum \f$y\f$ values of the bounding box.
 * @param ROI_dst_ymax Output array of maximum \f$y\f$ values of the bounding box.
 * @param ROI_dst_S Output array of ROI surfaces.
 * @param ROI_dst_Sx Output array of sums of \f$x\f$ properties.
 * @param ROI_dst_Sy Output array of sums of \f$y\f$ properties.
 * @param ROI_dst_x Output array of centroid abscissas.
 * @param ROI_dst_y Output array of centroid ordinates.
 * @return The number of regions of interest (ROIs) after the data shrink.
 * @see features_merge_CCL_HI_v2 for more explanations about why some identifiers can be set to 0.
 * @see ROI_basic_t for more explanations about the features.
 */
size_t _features_shrink_ROI_array(const uint32_t* ROI_src_id, const uint32_t* ROI_src_xmin,
                                  const uint32_t* ROI_src_xmax, const uint32_t* ROI_src_ymin,
                                  const uint32_t* ROI_src_ymax, const uint32_t* ROI_src_S, const uint32_t* ROI_src_Sx,
                                  const uint32_t* ROI_src_Sy, const float* ROI_src_x, const float* ROI_src_y,
                                  const size_t n_ROI_src, uint32_t* ROI_dst_id, uint32_t* ROI_dst_xmin,
                                  uint32_t* ROI_dst_xmax, uint32_t* ROI_dst_ymin, uint32_t* ROI_dst_ymax,
                                  uint32_t* ROI_dst_S, uint32_t* ROI_dst_Sx, uint32_t* ROI_dst_Sy, float* ROI_dst_x,
                                  float* ROI_dst_y);

/**
 * Shrink features. Remove features when feature identifier is set to 0.
 * Source features (`ROI_basic_array_src`) are copied into destination features (`ROI_basic_array_dst`) if
 * `ROI_src_id` > 0.
 * @param ROI_basic_array_src Input features.
 * @param ROI_basic_array_dst Output features.
 * @return The number of regions of interest (ROIs) after the data shrink.
 * @see features_merge_CCL_HI_v2 for more explanations about why some identifiers can be set to 0.
 * @see ROI_basic_t for more explanations about the features.
 */
void features_shrink_ROI_array(const ROI_basic_t* ROI_basic_array_src, ROI_basic_t* ROI_basic_array_dst);

/**
 * Compute magnitude features. The magnitude represents the brightness of a ROI.
 * In a first time, the sum of the pixels intensities is performed. In a second time, the noise level around the
 * connected-component is subtracted to give a better estimation of the real brightness.
 * The magnitude can be defined as follow:
 * \f$ M = \displaystyle\sum_{p=0} ^{P} i_p - \big((\displaystyle\sum_{n=0} ^{N} i_n) / N\big) \times P\f$,
 * where \f$P\f$ is the the number of pixels in the current CC, \f$i_x\f$ is the brightness of the pixel \f$x\f$
 * and \f$N\f$ is the number of noisy pixels considered.
 * @param img Input image in grayscale (\f$[\texttt{img\_height}][\texttt{img\_width}]\f$, the values of the pixel
 *            range are \f$ [ 0;255 ] \f$).
 * @param img_width Image width.
 * @param img_height Image height.
 * @param labels Input 2D array of labels (\f$[\texttt{img\_height}][\texttt{img\_width}]\f$).
 * @param ROI_xmin Input array of minimum \f$x\f$ values of the bounding box.
 * @param ROI_xmax Input array of maximum \f$x\f$ values of the bounding box.
 * @param ROI_ymin Input array of minimum \f$y\f$ values of the bounding box.
 * @param ROI_ymax Input array of maximum \f$y\f$ values of the bounding box.
 * @param ROI_S Input array of ROI surfaces.
 * @param ROI_magnitude Output array of ROI magnitudes.
 * @param n_ROI Number of connected-components (= ROIs).
 * @see ROI_basic_t for more explanations about the basic features.
 * @see ROI_misc_t for more explanations about the miscellaneous features.
 */
void _features_compute_magnitude(const uint8_t** img, const uint32_t img_width, const uint32_t img_height,
                                 const uint32_t** labels, const uint32_t* ROI_xmin, const uint32_t* ROI_xmax,
                                 const uint32_t* ROI_ymin, const uint32_t* ROI_ymax, const uint32_t* ROI_S,
                                 uint32_t* ROI_magnitude, const size_t n_ROI);

/**
 * Compute magnitude features. The magnitude represents the brightness of a ROI.
 * In a first time, the sum of the pixels intensities is performed. In a second time, the noise level around the
 * connected-component is subtracted to give a better estimation of the real brightness.
 * The magnitude can be defined as follow:
 * \f$ M = \displaystyle\sum_{p=0} ^{P} i_p - \big((\displaystyle\sum_{n=0} ^{N} i_n) / N\big) \times P\f$,
 * where \f$P\f$ is the the number of pixels in the current CC, \f$i_x\f$ is the brightness of the pixel \f$x\f$
 * and \f$N\f$ is the number of noisy pixels considered.
 * @param img Input image in grayscale (\f$[\texttt{img\_height}][\texttt{img\_width}]\f$, the values of the pixel
 *            range are \f$ [ 0;255 ] \f$).
 * @param img_width Image width.
 * @param img_height Image height.
 * @param labels Input 2D array of labels (\f$[\texttt{img\_height}][\texttt{img\_width}]\f$).
 * @param ROI_basic_array Input basic features.
 * @param ROI_misc_array Output miscellaneous features (including the magnitudes).
 * @see ROI_basic_t for more explanations about the basic features.
 * @see ROI_misc_t for more explanations about the miscellaneous features.
 */
void features_compute_magnitude(const uint8_t** img, const uint32_t img_width, const uint32_t img_height,
                                const uint32_t** labels, const ROI_basic_t* ROI_basic_array, ROI_misc_t* ROI_misc_array);