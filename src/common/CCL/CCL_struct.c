#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fmdt/CCL/CCL_struct.h"

enum ccl_impl_e CCL_str_to_enum(const char* str) {
    if (strcmp(str, "LSLH") == 0) {
        return LSLH;
    } else if (strcmp(str, "LSLM") == 0) {
#ifdef FMDT_LSL_LINK
        return LSLM;
#else
        fprintf(stderr, "(EE) 'CCL_str_to_enum()' failed, 'LSLM' implementation requires to link with the LSL library.\n");
        exit(-1);
#endif
    } else {
        fprintf(stderr, "(EE) 'CCL_str_to_enum()' failed, unknown input ('%s').\n", str);
        exit(-1);
    }
}
