/*!
 * \file
 * \brief *Feature* functions. Connected-Component Analysis (CCA) functions.
 */

#pragma once

#include "fmdt/features/features_struct.h"

/**
 * Allocation of the basic features.
 * @param max_size Maximum capacity of each *feature* field (= maximum number of elements in the arrays).
 * @param RoI_id Pointer of `max_size` elements to use for the `id` field. If set to NULL, the `id` field is allocated.
 * @return Pointer of allocated RoIs.
 */
RoI_basic_t* features_alloc_RoI_basic(const size_t max_size, uint32_t* RoI_id);

/**
 * Initialization of the basic features. Set all zeros.
 * @param RoI_basic Pointer of RoIs.
 * @param init_id Boolean to initialize or not the `id` field.
 */
void features_init_RoI_basic(RoI_basic_t* RoI_basic, const uint8_t init_id);

/**
 * Free the features.
 * @param RoI_basic Pointer of RoIs.
 * @param free_id Boolean to free or not the `id` field.
 */
void features_free_RoI_basic(RoI_basic_t* RoI_basic, const uint8_t free_id);

/**
 * Allocation of the association features.
 * @param max_size Maximum capacity of each *feature* field (= maximum number of elements in the arrays).
 * @param RoI_id Pointer of `max_size` elements to use for the `id` field. If set to NULL, the `id` field is allocated.
 * @return Pointer of allocated RoIs.
 */
RoI_asso_t* features_alloc_RoI_asso(const size_t max_size, uint32_t* RoI_id);

/**
 * Initialization of the association features. Set all zeros.
 * @param RoI_asso Pointer of RoIs.
 * @param init_id Boolean to initialize or not the `id` field.
 */
void features_init_RoI_asso(RoI_asso_t* RoI_asso, const uint8_t init_id);

/**
 * Free the features.
 * @param RoI_asso Pointer of RoIs.
 * @param free_id Boolean to free or not the `id` field.
 */
void features_free_RoI_asso(RoI_asso_t* RoI_asso, const uint8_t free_id);

/**
 * Allocation of the motion features.
 * @param max_size Maximum capacity of each *feature* field (= maximum number of elements in the arrays).
 * @param RoI_id Pointer of `max_size` elements to use for the `id` field. If set to NULL, the `id` field is allocated.
 * @return Pointer of allocated RoIs.
 */
RoI_motion_t* features_alloc_RoI_motion(const size_t max_size, uint32_t* RoI_id);

/**
 * Initialization of the motion features. Set all zeros.
 * @param RoI_motion Pointer of RoIs.
 * @param init_id Boolean to initialize or not the `id` field.
 */
void features_init_RoI_motion(RoI_motion_t* RoI_motion, const uint8_t init_id);

/**
 * Free the features.
 * @param RoI_motion Pointer of RoIs.
 * @param free_id Boolean to free or not the `id` field.
 */
void features_free_RoI_motion(RoI_motion_t* RoI_motion, const uint8_t free_id);

/**
 * Allocation of the miscellaneous features.
 * @param max_size Maximum capacity of each *feature* field (= maximum number of elements in the arrays).
 * @param RoI_id Pointer of `max_size` elements to use for the `id` field. If set to NULL, the `id` field is allocated.
 * @return Pointer of allocated RoIs.
 */
RoI_misc_t* features_alloc_RoI_misc(const size_t max_size, uint32_t* RoI_id);

/**
 * Initialization of the miscellaneous features. Set all zeros.
 * @param RoI_misc Pointer of RoIs.
 * @param init_id Boolean to initialize or not the `id` field.
 */
void features_init_RoI_misc(RoI_misc_t* RoI_misc, const uint8_t init_id);

/**
 * Free the features.
 * @param RoI_misc Pointer of RoIs.
 * @param free_id Boolean to free or not the `id` field.
 */
void features_free_RoI_misc(RoI_misc_t* RoI_misc, const uint8_t free_id);

/**
 * Allocation of all the features.
 * @param max_size Maximum capacity of each *feature* field (= maximum number of elements in the arrays).
 * @return Pointer of allocated RoIs.
 */
RoI_t* features_alloc_RoI(const size_t max_size);

/**
 * Initialization of the features. Set all zeros.
 * @param RoI Pointer of RoIs.
 */
void features_init_RoI(RoI_t* RoI);

/**
 * Free the features.
 * @param RoI Pointer of RoIs.
 */
void features_free_RoI(RoI_t* RoI);

/**
 * Basic features extraction from a 2D array of `labels`.
 * In other words, this function converts a (sparse ?) 2-dimensional representation of connected-components (CCs) into a
 * list of CCs.
 * @param labels 2D array of labels (\f$[i1 - i0 + 1][j1 - j0 + 1]\f$).
 * @param i0 First \f$y\f$ index in the labels (included).
 * @param i1 Last \f$y\f$ index in the labels (included).
 * @param j0 First \f$x\f$ index in the labels (included).
 * @param j1 Last \f$x\f$ index in the labels (included).
 * @param RoI_id Array of RoI unique identifiers.
 * @param RoI_xmin Array of minimum \f$x\f$ coordinates of the bounding box.
 * @param RoI_xmax Array of maximum \f$x\f$ coordinates of the bounding box.
 * @param RoI_ymin Array of minimum \f$y\f$ coordinates of the bounding box.
 * @param RoI_ymax Array of maximum \f$y\f$ coordinates of the bounding box.
 * @param RoI_S Array of RoI surfaces.
 * @param RoI_Sx Array of sums of \f$x\f$ properties.
 * @param RoI_Sy Array of sums of \f$y\f$ properties.
 * @param RoI_x Array of centroids abscissa.
 * @param RoI_y Array of centroids ordinate.
 * @param n_RoI Number of connected-components (= number of RoIs) in the 2D array of `labels`.
 * @see RoI_basic_t for more explanations about the features.
 */
void _features_extract(const uint32_t** labels, const int i0, const int i1, const int j0, const int j1,
                       uint32_t* RoI_id, uint32_t* RoI_xmin, uint32_t* RoI_xmax, uint32_t* RoI_ymin, uint32_t* RoI_ymax,
                       uint32_t* RoI_S, uint32_t* RoI_Sx, uint32_t* RoI_Sy, float* RoI_x, float* RoI_y,
                       const size_t n_RoI);

/**
 * @param labels Input 2D array of labels (\f$[i1 - i0 + 1][j1 - j0 + 1]\f$).
 * @param i0 First \f$y\f$ index in the labels (included).
 * @param i1 Last \f$y\f$ index in the labels (included).
 * @param j0 First \f$x\f$ index in the labels (included).
 * @param j1 Last \f$x\f$ index in the labels (included).
 * @param n_RoI Number of connected-components (= number of RoIs) in the 2D array of `labels`.
 * @param RoI_basic Basic features.
 * @see _features_extract for the explanations about the nature of the processing.
 * @see RoI_basic_t for more explanations about the features.
 */
void features_extract(const uint32_t** labels, const int i0, const int i1, const int j0, const int j1,
                      const size_t n_RoI, RoI_basic_t* RoI_basic);


/**
 * Hysteresis re-labeling and morphological thresholding. From a 2D array of labels (`in_label`) and a binary image
 * (`img_HI`), the function generates a new 2D array of labels (`out_labels`). The newly produced labels (`out_labels`)
 * are a sub-set of the "old" labels (`in_labels`). Labels from `in_labels` are kept in `out_labels` only if at least
 * one pixel of the current connected-component exists in the binary image (`img_HI`). Finally, this function performs a
 * morphological thresholding as follow: if \f$ S_{min} > S \f$ or \f$ S > S_{max}\f$ then the corresponding `RoI_id` is
 * set to 0.
 * @param in_labels Input 2D array of labels (\f$[i1 - i0 + 1][j1 - j0 + 1]\f$).
 * @param img_HI Binary image (2D array \f$[i1 - i0 + 1][j1 - j0 + 1]\f$, \f$\{0,1\}\f$ has to be coded as
 *               \f$\{0,255\}\f$). This image results from a threshold filter on the original image. This threshold
 *               filter should be higher than the first one used to compute the initial labels (`in_labels`).
 * @param out_labels Output 2D array of labels (\f$[i1 - i0 + 1][j1 - j0 + 1]\f$).
 * @param i0 First \f$y\f$ index in the labels (included).
 * @param i1 Last \f$y\f$ index in the labels (included).
 * @param j0 First \f$x\f$ index in the labels (included).
 * @param j1 Last \f$x\f$ index in the labels (included).
 * @param RoI_id Array of RoI unique identifiers.
 * @param RoI_xmin Array of minimum \f$x\f$ coordinates of the bounding box.
 * @param RoI_xmax Array of maximum \f$x\f$ coordinates of the bounding box.
 * @param RoI_ymin Array of minimum \f$y\f$ coordinates of the bounding box.
 * @param RoI_ymax Array of maximum \f$y\f$ coordinates of the bounding box.
 * @param RoI_S Array of RoI surfaces.
 * @param n_RoI Number of connected-components (= number of RoIs) in the 2D array of `in_labels`.
 * @param S_min Minimum morphological threshold.
 * @param S_max Maximum morphological threshold.
 * @see RoI_basic_t for more explanations about the features.
 */
void _features_merge_CCL_HI_v2(const uint32_t** in_labels, const uint8_t** img_HI, uint32_t** out_labels, const int i0,
                               const int i1, const int j0, const int j1, uint32_t* RoI_id, const uint32_t* RoI_xmin,
                               const uint32_t* RoI_xmax, const uint32_t* RoI_ymin, const uint32_t* RoI_ymax,
                               const uint32_t* RoI_S, const size_t n_RoI, const uint32_t S_min, const uint32_t S_max);

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
 * @param RoI_basic Features.
 * @param S_min Minimum morphological threshold.
 * @param S_max Maximum morphological threshold.
 * @see _features_merge_CCL_HI_v2 for the explanations about the nature of the processing.
 * @see RoI_basic_t for more explanations about the features.
 */
void features_merge_CCL_HI_v2(const uint32_t** in_labels, const uint8_t** img_HI, uint32_t** out_labels, const int i0,
                              const int i1, const int j0, const int j1, RoI_basic_t* RoI_basic,
                              const uint32_t S_min, const uint32_t S_max);

/**
 * Shrink features. Remove features when feature identifier value is 0.
 * Source features (`RoI_src_X`) are copied into destination features (`RoI_dst_X`) if `RoI_src_id` > 0.
 * @param RoI_src_id Source array of RoI unique identifiers.
 * @param RoI_src_xmin Source array of minimum \f$x\f$ coordinates of the bounding box.
 * @param RoI_src_xmax Source array of maximum \f$x\f$ coordinates of the bounding box.
 * @param RoI_src_ymin Source array of minimum \f$y\f$ coordinates of the bounding box.
 * @param RoI_src_ymax Source array of maximum \f$y\f$ coordinates of the bounding box.
 * @param RoI_src_S Source array of RoI surfaces.
 * @param RoI_src_Sx Source array of sums of \f$x\f$ properties.
 * @param RoI_src_Sy Source array of sums of \f$y\f$ properties.
 * @param RoI_src_x Source array of centroids abscissas.
 * @param RoI_src_y Source array of centroids ordinates.
 * @param n_RoI_src Number of RoIs in the previous arrays.
 * @param RoI_dst_id Destination array of RoI unique identifiers.
 * @param RoI_dst_xmin Destination array of minimum \f$x\f$ coordinates of the bounding box.
 * @param RoI_dst_xmax Destination array of maximum \f$x\f$ coordinates of the bounding box.
 * @param RoI_dst_ymin Destination array of minimum \f$y\f$ coordinates of the bounding box.
 * @param RoI_dst_ymax Destination array of maximum \f$y\f$ coordinates of the bounding box.
 * @param RoI_dst_S Destination array of RoI surfaces.
 * @param RoI_dst_Sx Destination array of sums of \f$x\f$ properties.
 * @param RoI_dst_Sy Destination array of sums of \f$y\f$ properties.
 * @param RoI_dst_x Destination array of centroids abscissas.
 * @param RoI_dst_y Destination array of centroids ordinates.
 * @return Number of regions of interest (RoIs) after the data shrink.
 * @see features_merge_CCL_HI_v2 for more explanations about why some identifiers can be set to 0.
 * @see RoI_basic_t for more explanations about the features.
 */
size_t _features_shrink(const uint32_t* RoI_src_id, const uint32_t* RoI_src_xmin,
                        const uint32_t* RoI_src_xmax, const uint32_t* RoI_src_ymin,
                        const uint32_t* RoI_src_ymax, const uint32_t* RoI_src_S, const uint32_t* RoI_src_Sx,
                        const uint32_t* RoI_src_Sy, const float* RoI_src_x, const float* RoI_src_y,
                        const size_t n_RoI_src, uint32_t* RoI_dst_id, uint32_t* RoI_dst_xmin,
                        uint32_t* RoI_dst_xmax, uint32_t* RoI_dst_ymin, uint32_t* RoI_dst_ymax,
                        uint32_t* RoI_dst_S, uint32_t* RoI_dst_Sx, uint32_t* RoI_dst_Sy, float* RoI_dst_x,
                        float* RoI_dst_y);

/**
 * @param RoI_basic_src Source features.
 * @param RoI_basic_dst Destination features.
 * @see _features_shrink for the explanations about the nature of the processing.
 * @see RoI_basic_t for more explanations about the features.
 */
void features_shrink(const RoI_basic_t* RoI_basic_src, RoI_basic_t* RoI_basic_dst);

/**
 * Compute magnitude features. The magnitude represents the brightness of a RoI.
 * In a first time, the sum of the pixels intensities is performed. In a second time, the noise level around the
 * connected-component is subtracted to give a better estimation of the real brightness.
 * The magnitude can be defined as follow:
 * \f$ M = \displaystyle\sum_{p=0} ^{P} i_p - \big((\displaystyle\sum_{n=0} ^{N} i_n) / N\big) \times P\f$,
 * where \f$P\f$ is the the number of pixels in the current CC, \f$i_x\f$ is the brightness of the pixel \f$x\f$
 * and \f$N\f$ is the number of noisy pixels considered.
 * @param img Image in grayscale (\f$[\texttt{img\_height}][\texttt{img\_width}]\f$, the values of the pixel
 *            range are \f$ [ 0;255 ] \f$).
 * @param img_width Image width.
 * @param img_height Image height.
 * @param labels 2D array of labels (\f$[\texttt{img\_height}][\texttt{img\_width}]\f$).
 * @param RoI_xmin Array of minimum \f$x\f$ coordinates of the bounding box.
 * @param RoI_xmax Array of maximum \f$x\f$ coordinates of the bounding box.
 * @param RoI_ymin Array of minimum \f$y\f$ coordinates of the bounding box.
 * @param RoI_ymax Array of maximum \f$y\f$ coordinates of the bounding box.
 * @param RoI_S Array of RoI surfaces.
 * @param RoI_magnitude Array of RoI magnitudes.
 * @param n_RoI Number of connected-components (= number of RoIs).
 * @see RoI_basic_t for more explanations about the basic features.
 * @see RoI_misc_t for more explanations about the miscellaneous features.
 */
void _features_compute_magnitude(const uint8_t** img, const uint32_t img_width, const uint32_t img_height,
                                 const uint32_t** labels, const uint32_t* RoI_xmin, const uint32_t* RoI_xmax,
                                 const uint32_t* RoI_ymin, const uint32_t* RoI_ymax, const uint32_t* RoI_S,
                                 uint32_t* RoI_magnitude, const size_t n_RoI);

/**
 * @param img Image in grayscale (\f$[\texttt{img\_height}][\texttt{img\_width}]\f$, the values of the pixel
 *            range are \f$ [ 0;255 ] \f$).
 * @param img_width Image width.
 * @param img_height Image height.
 * @param labels 2D array of labels (\f$[\texttt{img\_height}][\texttt{img\_width}]\f$).
 * @param RoI_basic Basic features.
 * @param RoI_misc Miscellaneous features (including the magnitudes).
 * @see _features_compute_magnitude for the explanations about the nature of the processing.
 * @see RoI_basic_t for more explanations about the basic features.
 * @see RoI_misc_t for more explanations about the miscellaneous features.
 */
void features_compute_magnitude(const uint8_t** img, const uint32_t img_width, const uint32_t img_height,
                                const uint32_t** labels, const RoI_basic_t* RoI_basic, RoI_misc_t* RoI_misc);
