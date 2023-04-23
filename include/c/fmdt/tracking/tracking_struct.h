/*!
 * \file
 * \brief Tracking structures.
 */

#pragma once

#include <stdint.h>
#include <stddef.h>

#include "fmdt/motion/motion_struct.h" // motion_t

/**
 *  Enumeration of the different object types (= object classification).
 */
enum obj_e { OBJ_UNKNOWN = 0, /*!< Unknown (= uninitialized). */
             OBJ_METEOR, /*!< Meteor. */
             OBJ_STAR, /*!< Star. */
             OBJ_NOISE, /*!< Noise (generally noise means that it is not a meteor and not a star). */
             N_OBJECTS /*!< Number of objects in the enumeration. */
};

/**
 *  Enumeration of the states in the tracking finite-state machine.
 */
enum state_e { STATE_UNKNOWN = 0, /*!< Unknown (= uninitialized). */
               STATE_UPDATED, /*!< Track has been updated (or created). */
               STATE_LOST, /*!< Track has not been updated, it is lost. */
               STATE_FINISHED, /*!< Track is finished. */
               N_STATES /*!< Number of states in the enumeration. */
};

/**
 *  Enumeration of the possible reasons why an `OBJ_METEOR` has been finally classified as an `OBJ_NOISE`.
 */
enum change_state_reason_e { REASON_UNKNOWN = 0, /*!< Unknown (= uninitialized). */
                             REASON_TOO_BIG_ANGLE, /*!< Angle made by the 3 last positions is to big. */
                             REASON_WRONG_DIRECTION, /*!< Track radically changed its direction. */
                             REASON_TOO_LONG_DURATION, /*!< Track lived a too long time to be a meteor. */
                             N_REASONS /*!< Number of reasons in the enumeration. */
};

/**
 *  Vector of `uint32_t`, to use with C vector lib.
 */
typedef uint32_t* vec_uint32_t;

/**
 *  Bounding box structure. Used to represent the bounding box around a RoI.
 */
typedef struct {
    uint32_t frame_id; /*!< Frame id corresponding to the bounding box. */
    uint32_t track_id; /*!< Track id corresponding to the bounding box. */
    uint32_t bb_x; /*!< Center \f$x\f$ of the bounding box. */
    uint32_t bb_y; /*!< Center \f$y\f$ of the bounding box. */
    uint32_t rx; /*!< Radius \f$x\f$ of the bounding box. */
    uint32_t ry; /*!< Radius \f$y\f$ of the bounding box. */
    int is_extrapolated; /*!< Boolean that defines if the bounding box is a real bounding box (from a
                              connected-component) or if it has been extrapolated in the tracking. */
} BB_t;

/**
 *  Vector of `BB_t`, to use with C vector lib.
 */
typedef BB_t* vec_BB_t;

/**
 *  Features required in the tracking.
 */
typedef struct {
    uint32_t id; /**< RoI unique identifiers. A RoI identifier should starts from 1 while 0 should be reserved for
                      uninitialized structure. */
    uint32_t frame; /**< Frame number of the RoI. */
    uint32_t xmin; /**< Minimum \f$x\f$ coordinates of the bounding box. */
    uint32_t xmax; /**< Maximum \f$x\f$ coordinates of the bounding box. */
    uint32_t ymin; /**< Minimum \f$y\f$ coordinates of the bounding box. */
    uint32_t ymax; /**< Maximum \f$y\f$ coordinates of the bounding box. */
    uint32_t S; /**< Numbers of points/pixels = surfaces of the RoIs. */
    float x; /**< \f$x\f$ coordinates of the centroid (\f$ x = S_x / S \f$). */
    float y; /**< \f$y\f$ coordinates of the centroid (\f$ y = S_y / S \f$). */
    uint32_t prev_id; /**< Previous corresponding RoI identifiers (\f$RoI_{t - 1} \leftrightarrow RoI_{t}\f$). */
    uint32_t next_id; /**< Next corresponding RoI identifiers (\f$ RoI_{t} \leftrightarrow RoI_{t + 1}\f$). */
    float dx; /**< \f$x\f$ components of the distance between centroids at \f$t - 1\f$ and \f$t\f$. */
    float dy; /**< \f$y\f$ components of the distance between centroids at \f$t - 1\f$ and \f$t\f$. */
    float error; /**< Velocity norm / error. \f$ e = \sqrt{dx^2 + dy^2} \f$. */
    uint32_t time; /*!< Number of times the RoI and its predecessors have been associated (non-moving RoI). */
    uint32_t time_motion; /*!< Number of times the RoI and its predecessors have been associated (moving RoI). */
    uint8_t is_extrapolated; /*!< Boolean that defines if this RoI has been extrapolated. It prevents to associate it to
                                  a new track. */
} RoI_t;

/**
 *  Description of a track.
 */
typedef struct {
    uint32_t id; /**< Track unique identifiers. A track identifier should starts from 1 while 0 should be reserved for
                      uninitialized structure. */
    RoI_t begin; /**< First RoI corresponding to this track. */
    RoI_t end; /**< Last RoI corresponding to this track. */
    float extrapol_x; /**< Last \f$x\f$ position of the extrapolated track (used only if `state` == `STATE_LOST`). */
    float extrapol_y; /**< Last \f$y\f$ position of the extrapolated track (used only if `state` == `STATE_LOST`). */
    float extrapol_u; /**< Velocity \f$x\f$ estimation of the track for extrapolation (used only if `state` ==
                           `STATE_LOST`). */
    float extrapol_v; /**< Velocity \f$y\f$ estimation of the track for extrapolation (used only if `state` ==
                           `STATE_LOST`). */
    uint8_t extrapol_order; /**< Number of times this track has been extrapolated (used only if `state` ==
                                 `STATE_LOST`). */
    enum state_e state; /**< State of the track. */
    enum obj_e obj_type; /**< Object type (classification). */
    enum change_state_reason_e change_state_reason; /**< Reason of the noise type classification. */
    vec_uint32_t RoIs_id; /**< Vector of the RoI ids history of this track. */
} track_t;

/**
 *  Vector of `track_t`, to use with C vector lib.
 */
typedef track_t* vec_track_t;

/**
 *  History of the previous RoI features.
 *  This structure allows to access RoI in the past frames.
 *  RoIs at \f$t\f$ are stored in the first array element while RoIs at \f$t-\texttt{\_size}\f$ are store in the
 *  \f$\texttt{\_size} - 1\f$ element.
 *  The memory layout is a Structure of Arrays (SoA), each field is an array of `_max_size` capacity (except for
 * `_max_size` itself and `_size` fields that are both scalar values).
 */
typedef struct {
    RoI_t** array; /**< 2D array of RoIs, the first dimension is the time and the second dimension is the RoIs at a
                        given time. */
    motion_t* motion; /**< Array of motion estimations. */
    uint32_t* n_RoIs; /**< Array of numbers of RoIs. */
    uint32_t _max_n_RoIs; /**< Maximum number of RoIs. */
    size_t _size; /**< Current size/utilization of the fields. */
    size_t _max_size; /**< Maximum capacity of data that can be contained in the fields. */
} RoIs_history_t;

/**
 *  Inner data used by the tracking.
 */
typedef struct {
    vec_track_t tracks; /**< Vector of tracks. */
    RoIs_history_t* RoIs_history; /**< RoIs history. */
    RoI_t* RoIs_list; /**< List of RoIs. This is a temporary array used to group all the RoIs belonging to a same
                           track. */
} tracking_data_t;

/**
 * Compute the duration of a track.
 * @param track_begin First RoI of the track.
 * @param track_end Last RoI of the track.
 * @return The elapsed time (in number of frames).
 */
size_t _tracking_get_track_time(const RoI_t track_begin, const RoI_t track_end);

/**
 * Compute the duration of a track.
 * @param tracks A vector of tracks.
 * @param t The position of one track in the tracks array.
 * @return The elapsed time (in number of frames).
 */
size_t tracking_get_track_time(const vec_track_t tracks, const size_t t);

/**
 * Counts the number of tracks in a vector of tracks.
 * @param tracks A vector of tracks.
 * @param n_stars Write the number of tracks that have been classified as star.
 * @param n_meteors Write the number of tracks that have been classified as meteor.
 * @param n_noise Write the number of tracks that have been classified as noise.
 * @return The real number of tracks (may be less than the \p tracks vector size).
 */
size_t tracking_count_objects(const vec_track_t tracks, unsigned* n_stars, unsigned* n_meteors, unsigned* n_noise);
