/*!
 * \file
 * \brief Compute functions related to images.
 */

#pragma once

#include "fmdt/features/features_struct.h"
#include "fmdt/tracking/tracking_struct.h"
#include "fmdt/image/image_struct.h"

/**
 * From a given color, returns the corresponding RBG representation.
 * @param color Color enum value.
 * @return RGB struct.
 */
rgb8_t image_get_color(enum color_e color);

/**
 * Allocate grayscale image data.
 * @param img_width Image width.
 * @param img_height Image height.
 * @return Pointer of image data.
 */
img_data_t* image_gs_alloc(const size_t img_width, const size_t img_height);

/**
 * Convert labels into a black & white image.
 * If the program is linked with the OpenCV library, then the `show_id` boolean can be used to draw the label number
 * on the black & white image.
 * @param img_data Image data.
 * @param labels Labels (2D array of size \f$[\texttt{img\_data->height}][\texttt{img\_data->width}]\f$).
 * @param RoIs_id Array of RoI unique identifiers (useful only if `show_id == 1`).
 * @param RoIs_xmax Array of maximum \f$x\f$ coordinates of the bounding box (useful only if `show_id == 1`).
 * @param RoIs_ymin Array of minimum \f$y\f$ coordinates of the bounding box (useful only if `show_id == 1`).
 * @param RoIs_ymax Array of maximum \f$y\f$ coordinates of the bounding box (useful only if `show_id == 1`).
 * @param n_RoIs Number of connected-components (= number of RoIs) (useful only if `show_id == 1`).
 * @param show_id Boolean to enable display of the label numbers (has no effect if the program has not be linked with
 *                the OpenCV library).
 */
void _image_gs_draw_labels(img_data_t* img_data, const uint32_t** labels, const uint32_t* RoIs_id,
                           const uint32_t* RoIs_xmin, const uint32_t* RoIs_xmax, const uint32_t* RoIs_ymin,
                           const uint32_t* RoIs_ymax, const size_t n_RoIs, const uint8_t show_id);

/**
 * @param img_data Image data.
 * @param labels Labels (2D array of size \f$[\texttt{img\_data->height}][\texttt{img\_data->width}]\f$).
 * @param RoIs_basic Basic features (useful only if `show_id == 1`).
 * @param show_id Boolean to enable display of the label numbers (has no effect if the program has not be linked with
 *                the OpenCV library).
 * @see _image_gs_draw_labels for the explanations about the nature of the processing.
 */
void image_gs_draw_labels(img_data_t* img_data, const uint32_t** labels, const RoIs_basic_t* RoIs_basic,
                          const uint8_t show_id);

/**
 * Return a pixels array of the grayscale image.
 * @param img_data Image data.
 */
uint8_t* image_gs_get_pixels(img_data_t* img_data);

/**
 * Return a 2D pixels array of the grayscale image.
 * @param img_data Image data.
 */
uint8_t** image_gs_get_pixels_2d(img_data_t* img_data);

/**
 * Deallocate grayscale image data.
 * @param img_data Image data.
 */
void image_gs_free(img_data_t* img_data);

/**
 * Allocate color image data.
 * @param img_width Image width.
 * @param img_height Image height.
 * @return Pointer of image data.
 */
img_data_t* image_color_alloc(const size_t img_width, const size_t img_height);

/**
 * Draw bounding boxes (BBs) on a color image.
 * If the program is linked with the OpenCV library, then the `show_id` boolean can be used to draw the ids
 * corresponding to each BB on the color image. Moreover, if the program is linked with OpenCV, this routine add the
 * legend on the top left corner.
 * @param img_data Image data.
 * @param img 2D grayscale image (2D array of size
 *            \f$[\texttt{img\_data->height}][\texttt{img\_data->width}]\f$). This image will be copied in `img_data`.
 * @param BBs List of bounding boxes.
 * @param BBs_color List of colors associated to the bounding boxes.
 * @param n_BBs Number of bounding boxes to draw.
 * @param show_id Boolean to enable display of the BB ids (has no effect if the program has not be linked with the
 *                OpenCV).
 * @param is_gt Boolean to draw the ground truth legend (has no effect is the program has not been linked with OpenCV).
 */
void image_color_draw_BBs(img_data_t* img_data, const uint8_t** img, const BB_t* BBs, const enum color_e* BBs_color,
                          const size_t n_BBs, const uint8_t show_id, const uint8_t is_gt);

/**
 * Return a pixels array of the color image.
 * @param img_data Image data.
 */
rgb8_t* image_color_get_pixels(img_data_t* img_data);

/**
 * Return a 2D pixels array of the color image.
 * @param img_data Image data.
 */
rgb8_t** image_color_get_pixels_2d(img_data_t* img_data);

/**
 * Deallocate color image data.
 * @param img_data Image data.
 */
void image_color_free(img_data_t* img_data);
