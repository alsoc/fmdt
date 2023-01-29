#include <stdio.h>
#include <stdlib.h>

#include "fmdt/motion/motion_io.h"

void motion_write(FILE* f, const motion_t* motion_est1, const motion_t* motion_est2) {
    fprintf(f, "# Motion:\n");
    fprintf(f, "# ---------------------------------------------------||---------------------------------------------------\n");
    fprintf(f, "#  First motion estimation (with all associated ROIs)||   Second motion estimation (exclude moving ROIs)  \n");
    fprintf(f, "# ---------------------------------------------------||---------------------------------------------------\n");
    fprintf(f, "# ----------|---------|---------|----------|---------||----------|---------|---------|----------|---------\n");
    fprintf(f, "#     theta |      tx |      ty | mean err | std dev ||    theta |      tx |      ty | mean err | std dev \n");
    fprintf(f, "# ----------|---------|---------|----------|---------||----------|---------|---------|----------|---------\n");
    fprintf(f, "   %8.5f | %7.4f | %7.4f | %8.4f | %7.4f || %8.5f | %7.4f | %7.4f | %8.4f | %7.4f \n",
            motion_est1->theta, motion_est1->tx, motion_est1->ty, motion_est1->mean_error, motion_est1->std_deviation,
            motion_est2->theta, motion_est2->tx, motion_est2->ty, motion_est2->mean_error, motion_est2->std_deviation);
}
