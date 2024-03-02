/*!
 * \file
 * \brief *Feature* functions. Connected-Component Analysis (CCA) functions.
 */

#pragma once

#include "fmdt/features/features_struct.h"

/**
 * Allocation of the basic features.
 *
 * @param max_size Maximum capacity of each *feature* field (= maximum number of elements in the arrays).
 * @return Pointer of allocated RoIs.
 */
RoI_basic_t* features_alloc_RoIs_basic(const size_t max_size);

/**
 * Initialization of the basic features. Set all zeros.
 *
 * @param RoIs_basic Pointer of RoIs.
 * @param max_size Maximum capacity of each *feature* field (= maximum number of elements in the arrays).
 */
void features_init_RoIs_basic(RoI_basic_t* RoIs_basic, const size_t max_size);

/**
 * Free the features.
 *
 * @param RoIs_basic Pointer of RoIs.
 */
void features_free_RoIs_basic(RoI_basic_t* RoIs_basic);

/**
 * Allocation of the association features.
 *
 * @param max_size Maximum capacity of each *feature* field (= maximum number of elements in the arrays).
 * @return Pointer of allocated RoIs.
 */
RoI_asso_t* features_alloc_RoIs_asso(const size_t max_size);

/**
 * Initialization of the association features. Set all zeros.
 *
 * @param RoIs_asso Pointer of RoIs.
 * @param max_size Maximum capacity of each *feature* field (= maximum number of elements in the arrays).
 */
void features_init_RoIs_asso(RoI_asso_t* RoIs_asso, const size_t max_size);

/**
 * Free the features.
 *
 * @param RoIs_asso Pointer of RoIs.
 */
void features_free_RoIs_asso(RoI_asso_t* RoIs_asso);

/**
 * Allocation of the motion features.
 *
 * @param max_size Maximum capacity of each *feature* field (= maximum number of elements in the arrays).
 * @return Pointer of allocated RoIs.
 */
RoI_motion_t* features_alloc_RoIs_motion(const size_t max_size);

/**
 * Initialization of the motion features. Set all zeros.
 *
 * @param RoIs_motion Pointer of RoIs.
 * @param max_size Maximum capacity of each *feature* field (= maximum number of elements in the arrays).
 */
void features_init_RoIs_motion(RoI_motion_t* RoIs_motion, const size_t max_size);

/**
 * Free the features.
 *
 * @param RoIs_motion Pointer of RoIs.
 */
void features_free_RoIs_motion(RoI_motion_t* RoIs_motion);

/**
 * Allocation of the magnitude features.
 *
 * @param max_size Maximum capacity of each *feature* field (= maximum number of elements in the arrays).
 * @return Pointer of allocated RoIs.
 */
RoI_magn_t* features_alloc_RoIs_magn(const size_t max_size);

/**
 * Initialization of the magnitude features. Set all zeros.
 *
 * @param RoIs_misc Pointer of RoIs.
 * @param max_size Maximum capacity of each *feature* field (= maximum number of elements in the arrays).
 */
void features_init_RoIs_magn(RoI_magn_t* RoIs_misc, const size_t max_size);

/**
 * Free the features.
 *
 * @param RoIs_misc Pointer of RoIs.
 */
void features_free_RoIs_magn(RoI_magn_t* RoIs_misc);

/**
 * Allocation of the ellipse features.
 *
 * @param max_size Maximum capacity of each *feature* field (= maximum number of elements in the arrays).
 * @return Pointer of allocated RoIs.
 */
RoI_elli_t* features_alloc_RoIs_elli(const size_t max_size);

/**
 * Initialization of the ellipse features. Set all zeros.
 *
 * @param RoIs_misc Pointer of RoIs.
 * @param max_size Maximum capacity of each *feature* field (= maximum number of elements in the arrays).
 */
void features_init_RoIs_elli(RoI_elli_t* RoIs_misc, const size_t max_size);

/**
 * Free the features.
 *
 * @param RoIs_misc Pointer of RoIs.
 */
void features_free_RoIs_elli(RoI_elli_t* RoIs_misc);

/**
 * Allocation of all the features.
 *
 * @param max_size Maximum capacity of each *feature* field (= maximum number of elements in the arrays).
 * @param alloc_asso Allocate association features if set to 1 (set the field to NULL if 0).
 * @param alloc_motion Allocate motion features if set to 1 (set the field to NULL if 0).
 * @param alloc_magn Allocate magnitude features if set to 1 (set the field to NULL if 0).
 * @param alloc_elli Allocate ellipse features if set to 1 (set the field to NULL if 0).
 * @return Pointer of allocated RoIs.
 */
RoIs_t* features_alloc_RoIs(const size_t max_size, const uint8_t alloc_asso, const uint8_t alloc_motion,
                            const uint8_t alloc_magn, const uint8_t alloc_elli);

/**
 * Initialization of the features. Set all zeros.
 *
 * @param RoIs Pointer of RoIs.
 */
void features_init_RoIs(RoIs_t* RoIs);

/**
 * Free the features.
 *
 * @param RoIs Pointer of RoIs.
 */
void features_free_RoIs(RoIs_t* RoIs);

/**
 * Basic features extraction from a 2D array of `labels`.
 * In other words, this function converts a (sparse ?) 2-dimensional representation of connected-components (CCs) into a
 * list of CCs.
 *
 * @param labels Input 2D array of labels (\f$[i1 - i0 + 1][j1 - j0 + 1]\f$).
 * @param i0 First \f$y\f$ index in the labels (included).
 * @param i1 Last \f$y\f$ index in the labels (included).
 * @param j0 First \f$x\f$ index in the labels (included).
 * @param j1 Last \f$x\f$ index in the labels (included).
 * @param RoIs_basic Basic features.
 * @param n_RoIs Number of connected-components (= number of RoIs) in the 2D array of `labels`.
 *
 * @see RoI_basic_t for more explanations about the features.
 */
void features_extract(const uint32_t** labels, const int i0, const int i1, const int j0, const int j1,
                      RoI_basic_t* RoIs_basic, const size_t n_RoIs);

/**
 * This function performs a surface thresholding as follow: if \f$ S_{min} > S \f$ or \f$ S > S_{max}\f$, then the
 * corresponding `RoIs_id` is set to 0.
 *
 * @param in_labels Input 2D array of labels (\f$[i1 - i0 + 1][j1 - j0 + 1]\f$).
 * @param out_labels Output 2D array of labels (\f$[i1 - i0 + 1][j1 - j0 + 1]\f$). \p out_labels can be NULL, this way
 *                   only the features will be updated. \p out_labels can also be the same pointer as \p in_labels, this
 *                   way the output labels will be computed in place.
 * @param i0 First \f$y\f$ index in the labels (included).
 * @param i1 Last \f$y\f$ index in the labels (included).
 * @param j0 First \f$x\f$ index in the labels (included).
 * @param j1 Last \f$x\f$ index in the labels (included).
 * @param RoIs_basic Features.
 * @param n_RoIs Number of connected-components (= number of RoIs) in the 2D array of `in_labels`.
 * @param S_min Minimum morphological threshold.
 * @param S_max Maximum morphological threshold.
 * @return Number of labels after filtering.
 *
 * @see RoI_basic_t for more explanations about the features.
 */
uint32_t features_filter_surface(const uint32_t** in_labels, uint32_t** out_labels, const int i0, const int i1,
                                 const int j0, const int j1, RoI_basic_t* RoIs_basic, const size_t n_RoIs,
                                 const uint32_t S_min, const uint32_t S_max);

/**
 * Hysteresis re-labeling and morphological thresholding. From a 2D array of labels (`in_label`) and a binary image
 * (`img_HI`), the function generates a new 2D array of labels (`out_labels`). The newly produced labels (`out_labels`)
 * are a sub-set of the "old" labels (`in_labels`). Labels from `in_labels` are kept in `out_labels` only if at least
 * one pixel of the current connected-component exists in the binary image (`img_HI`). Finally, this function performs a
 * morphological thresholding as follow: if \f$ S_{min} > S \f$ or \f$ S > S_{max}\f$ then the corresponding `RoIs_id`
 * is set to 0.
 *
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
 * @param n_RoIs Number of connected-components (= number of RoIs) in the 2D array of `in_labels`.
 * @param S_min Minimum morphological threshold.
 * @param S_max Maximum morphological threshold.
 * @return Number of labels.
 *
 * @see RoI_basic_t for more explanations about the features.
 */
uint32_t features_merge_CCL_HI_v2(const uint32_t** in_labels, const uint8_t** img_HI, uint32_t** out_labels,
                                  const int i0, const int i1, const int j0, const int j1, RoI_basic_t* RoIs_basic,
                                  const size_t n_RoIs, const uint32_t S_min, const uint32_t S_max);

/**
 * Hysteresis re-labeling and morphological thresholding. From a 2D array of labels (`in_label`) and a grayscale image
 * (`img`), the function generates a new 2D array of labels (`out_labels`). The newly produced labels (`out_labels`)
 * are a sub-set of the "old" labels (`in_labels`). Labels from `in_labels` are kept in `out_labels` only if at least
 * one pixel of the current connected-component exists in the binary image (`img`). Finally, this function performs a
 * morphological thresholding as follow: if \f$ S_{min} > S \f$ or \f$ S > S_{max}\f$ then the corresponding `RoIs_id`
 * is set to 0.
 * Note: this function is optimized to be more efficient than to compute the thresholding and to merge the labels
 * separately.
 *
 * @param in_labels Input 2D array of labels (\f$[i1 - i0 + 1][j1 - j0 + 1]\f$).
 * @param img Grayscale image (2D array \f$[i1 - i0 + 1][j1 - j0 + 1]\f$, grayscale is in \f$[0;255]\f$ range).
 * @param out_labels Output 2D array of labels (\f$[i1 - i0 + 1][j1 - j0 + 1]\f$).
 * @param i0 First \f$y\f$ index in the labels (included).
 * @param i1 Last \f$y\f$ index in the labels (included).
 * @param j0 First \f$x\f$ index in the labels (included).
 * @param j1 Last \f$x\f$ index in the labels (included).
 * @param RoIs_basic Features.
 * @param n_RoIs Number of connected-components (= number of RoIs) in the 2D array of `in_labels`.
 * @param S_min Minimum morphological threshold.
 * @param S_max Maximum morphological threshold.
 * @param threshold_high Value (between \f$[0;255]\f$). If the pixel intensity is higher than \p threshold, then the
 *                       pixel is kept for the re-labeling, else the pixel is ignored. \p threshold_high should be
 *                       higher than the threshold value used for \p in_labels.
 * @param no_labels_zeros_init Boolean for optimization purpose. If set to 1, \p out_labels is not initialized in this
 *                             function. Thus, it is up to the developer to properly initialize \p out_labels before
 *                             calling this routine. If you are not sure, prefer to set this boolean to 0.
 * @return Number of labels.
 *
 * @see RoIsbasic_t for more explanations about the features.
 */
uint32_t features_merge_CCL_HI_v3(const uint32_t** in_labels, const uint8_t** img, uint32_t** out_labels, const int i0,
                                  const int i1, const int j0, const int j1, RoI_basic_t* RoIs_basic,
                                  const size_t n_RoIs, const uint32_t S_min, const uint32_t S_max,
                                  const uint8_t threshold_high, const uint8_t no_labels_zeros_init);

/**
 * Shrink features. Remove features when feature identifier value is 0.
 * Source features (`RoIs_X_src`) are copied into destination features (`RoIs_X_dst`) if `RoIs_basic[Y].id` > 0.
 *
 * @param RoIs_basic_src Source features.
 * @param RoIs_magn_src Source features (can be NULL).
 * @param RoIs_elli_src Source features (can be NULL).
 * @param n_RoIs_src Number of RoIs in the previous arrays.
 * @param RoIs_basic_dst Destination features.
 * @param RoIs_magn_dst Destination features (can be NULL).
 * @param RoIs_elli_dst Destination features (can be NULL).
 * @return Number of regions of interest (RoIs) after the data shrink.
 *
 * @see RoI_basic_t for more explanations about the features.
 * @see RoI_magn_t for more explanations about the features.
 * @see RoI_elli_t for more explanations about the features.
 */
size_t features_shrink(const RoI_basic_t* RoIs_basic_src, const RoI_magn_t* RoIs_magn_src,
                       const RoI_elli_t* RoIs_elli_src, const size_t n_RoIs_src, RoI_basic_t* RoIs_basic_dst,
                       RoI_magn_t* RoIs_magn_dst, RoI_elli_t* RoIs_elli_dst);

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
 *
 * @param img Image in grayscale (\f$[i1 - i0 + 1][j1 - j0 + 1]\f$, the values of the pixel
 *            range are \f$ [ 0;255 ] \f$).
 * @param i0 First \f$y\f$ index in the image (included).
 * @param i1 Last \f$y\f$ index in the image (included).
 * @param j0 First \f$x\f$ index in the image (included).
 * @param j1 Last \f$x\f$ index in the image (included).
 * @param labels 2D array of labels (\f$[i1 - i0 + 1][j1 - j0 + 1]\f$).
 * @param RoIs_basic Basic features.
 * @param RoIs_magn Magnitude features (including the magnitudes).
 * @param n_RoIs Number of connected-components (= number of RoIs).
 *
 * @see RoI_basic_t for more explanations about the basic features.
 * @see RoI_magn_t for more explanations about the miscellaneous features.
 */
void features_compute_magnitude(const uint8_t** img, const int i0, const int i1, const int j0, const int j1,
                                const uint32_t** labels, const RoI_basic_t* RoIs_basic, RoI_magn_t* RoIs_magn,
                                const size_t n_RoIs);

/**
 * Compute the semi-major and the semi-minor axes of RoIs.
 *
 * @param RoIs_basic Basic features.
 * @param RoIs_elli Ellipse features (including the `a` and `b` features).
 * @param n_RoIs Number of connected-components (= number of RoIs).
 *
 * @see RoI_basic_t for more explanations about the basic features.
 * @see RoI_elli_t for more explanations about the features.
 */
void features_compute_ellipse(const RoI_basic_t* RoIs_basic, RoI_elli_t* RoIs_elli, const size_t n_RoIs);

/**
 * Initialize labels to zero value depending on bounding boxes.
 *
 * @param RoIs_basic Basic features (contains the bounding boxes).
 * @param n_RoIs Number of connected-components (= number of RoIs).
 * @param labels 2D array of labels (\f$[\texttt{img\_height}][\texttt{img\_width}]\f$).
 *
 * @see RoI_basic_t for more explanations about the basic features.
 */
void features_labels_zero_init(const RoI_basic_t* RoIs_basic, const size_t n_RoIs, uint32_t** labels);
