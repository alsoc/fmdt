#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fmdt/CCL/CCL_struct.h"

enum ccl_impl_e CCL_str_to_enum(const char* str) {
    if (strcmp(str, "LSLH") == 0) {
        return LSLH;
    } else if (strcmp(str, "LSLM") == 0) {
        return LSLM;
    } else {
        fprintf(stderr, "(EE) 'CCL_str_to_enum()' failed, unknown input ('%s').\n", str);
        exit(-1);
    }
}
