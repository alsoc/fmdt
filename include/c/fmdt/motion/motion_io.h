#pragma once

#include <stdio.h>

#include "fmdt/motion/motion_struct.h"

void motion_write(FILE* f, const motion_t* motion_est1, const motion_t* motion_est2);
