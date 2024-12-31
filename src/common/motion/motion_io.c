#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "fmdt/motion/motion_struct.h"
#include "fmdt/motion/motion_io.h"

void motion_write(FILE* f, const motion_t* motion_est1, const motion_t* motion_est2, uint8_t hexa_float) {
    char theta1_str[256], tx1_str[256], ty1_str[256], e1_str[256], stddev1_str[256];
    char theta2_str[256], tx2_str[256], ty2_str[256], e2_str[256], stddev2_str[256];

    union { float f; uint32_t u; } theta1_f2u  = { .f = motion_get_theta(&motion_est1->tmat) };
    union { float f; uint32_t u; } tx1_f2u     = { .f = motion_est1->tmat.tx                 };
    union { float f; uint32_t u; } ty1_f2u     = { .f = motion_est1->tmat.ty                 };
    union { float f; uint32_t u; } e1_f2u      = { .f = motion_est1->mean_error              };
    union { float f; uint32_t u; } stddev1_f2u = { .f = motion_est1->std_deviation           };

    union { float f; uint32_t u; } theta2_f2u  = { .f = motion_get_theta(&motion_est2->tmat) };
    union { float f; uint32_t u; } tx2_f2u     = { .f = motion_est2->tmat.tx                 };
    union { float f; uint32_t u; } ty2_f2u     = { .f = motion_est2->tmat.ty                 };
    union { float f; uint32_t u; } e2_f2u      = { .f = motion_est2->mean_error              };
    union { float f; uint32_t u; } stddev2_f2u = { .f = motion_est2->std_deviation           };

    if (hexa_float) {
        snprintf(theta1_str,  sizeof(theta1_str),  "0x%08" PRIx32, theta1_f2u.u );
        snprintf(tx1_str,     sizeof(tx1_str),     "0x%08" PRIx32, tx1_f2u.u    );
        snprintf(ty1_str,     sizeof(ty1_str),     "0x%08" PRIx32, ty1_f2u.u    );
        snprintf(e1_str,      sizeof(e1_str),      "0x%08" PRIx32, e1_f2u.u     );
        snprintf(stddev1_str, sizeof(stddev1_str), "0x%08" PRIx32, stddev1_f2u.u);

        snprintf(theta2_str,  sizeof(theta2_str),  "0x%08" PRIx32, theta2_f2u.u );
        snprintf(tx2_str,     sizeof(tx2_str),     "0x%08" PRIx32, tx2_f2u.u    );
        snprintf(ty2_str,     sizeof(ty2_str),     "0x%08" PRIx32, ty2_f2u.u    );
        snprintf(e2_str,      sizeof(e2_str),      "0x%08" PRIx32, e2_f2u.u     );
        snprintf(stddev2_str, sizeof(stddev2_str), "0x%08" PRIx32, stddev2_f2u.u);
    } else {
        snprintf(theta1_str,  sizeof(theta1_str),  "%10.5f",       theta1_f2u.f );
        snprintf(tx1_str,     sizeof(tx1_str),     "%10.5f",       tx1_f2u.f    );
        snprintf(ty1_str,     sizeof(ty1_str),     "%10.5f",       ty1_f2u.f    );
        snprintf(e1_str,      sizeof(e1_str),      "%10.5f",       e1_f2u.f     );
        snprintf(stddev1_str, sizeof(stddev1_str), "%10.5f",       stddev1_f2u.f);

        snprintf(theta2_str,  sizeof(theta2_str),  "%10.5f",       theta2_f2u.f );
        snprintf(tx2_str,     sizeof(tx2_str),     "%10.5f",       tx2_f2u.f    );
        snprintf(ty2_str,     sizeof(ty2_str),     "%10.5f",       ty2_f2u.f    );
        snprintf(e2_str,      sizeof(e2_str),      "%10.5f",       e2_f2u.f     );
        snprintf(stddev2_str, sizeof(stddev2_str), "%10.5f",       stddev2_f2u.f);
    }

    fprintf(f, "# Motion:\n");
    fprintf(f, "# ----------------------------------------------------------------||----------------------------------------------------------------\n");
    fprintf(f, "#         First motion estimation (with all associated RoIs)      ||          Second motion estimation (exclude moving RoIs)        \n");
    fprintf(f, "# ----------------------------------------------------------------||----------------------------------------------------------------\n");
    fprintf(f, "# ------------|------------|------------|------------|------------||------------|------------|------------|------------|------------\n");
    fprintf(f, "#       theta |         tx |         ty |   mean err |    std dev ||      theta |         tx |         ty |   mean err |    std dev \n");
    fprintf(f, "# ------------|------------|------------|------------|------------||------------|------------|------------|------------|------------\n");
    fprintf(f, "   %s | %s | %s | %s | %s || %s | %s | %s | %s | %s \n",
            theta1_str, tx1_str, ty1_str, e1_str, stddev1_str, theta2_str, tx2_str, ty2_str, e2_str, stddev2_str);
}
