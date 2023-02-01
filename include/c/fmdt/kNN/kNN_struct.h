/*!
 * \file
 * \brief k-Nearest Neighbors (kNN) structures.
 */

#pragma once

#include <stdint.h>

/**
 *  Inner data structure required to compute associations between RoIs.
 */
typedef struct {
    float** distances; /*!< 2D array of euclidean distances (\f$[\texttt{\_max\_size}][\texttt{\_max\_size}]\f$).
                            \f$y\f$ axis represents RoIs at \f$t-1\f$ and \f$x\f$ axis represents RoIs at \f$t\f$.
                            For instance, \f$\texttt{distances}[i][j]\f$ represents the distance between
                            \f$RoI_{t-1}^i\f$ and \f$RoI_{t}^j\f$. Note that sometime, for efficiency reasons, the
                            implementation may choose to store squared euclidean distances instead of euclidean
                            distances. */
    uint32_t** nearest; /*!< 2D array of ranks (\f$[\texttt{\_max\_size}][\texttt{\_max\_size}]\f$).
                             \f$y\f$ axis represents RoIs at \f$t-1\f$ and \f$x\f$ axis represents RoIs at \f$t\f$.
                             For instance, \f$\texttt{nearest}[i][j]\f$ represents the rank of \f$RoI_{t-1}^i\f$ and
                             \f$RoI_{t}^j\f$. Rank = 1 means that \f$i\f$ and \f$j\f$ are the closest possible RoIs
                             association, rank = 2 means that \f$i\f$ and \f$j\f$ are the second closest possible RoIs
                             association, and so on. Rank = 0 means that \f$i\f$ and \f$j\f$ were not associated
                             together (common reason is that they are too far from each others). */
    uint32_t* conflicts; /*!< 1D array of conflicts (\f$[\texttt{\_max\_size}]\f$).
                              A conflict happens when they are more than one \f$RoI_{t-1}\f$ that is the closet to
                              \f$RoI_{t}^j\f$.
                              \f$\texttt{conflicts}[j]\f$ contains 0 if there is no conflict.
                              \f$\texttt{conflicts}[j]\f$ contains more than 0 if there are conflicts.
                              For instance if \f$RoI_{t-1}^{i1}\f$, \f$RoI_{t-1}^{i2}\f$ and \f$RoI_{t-1}^{i3}\f$ are
                              all the closest to \f$RoI_{t}^j\f$, then \f$\texttt{conflicts}[j] = 2\f$.
                              This buffer is allocated only if the FMDT_ENABLE_DEBUG macro is defined. */
    size_t _max_size; /*!< Maximum number of RoIs allocated in the previous fields. */
} kNN_data_t;
