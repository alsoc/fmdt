#pragma once

typedef struct {
    float theta; // rotation angle
    float tx; // translation vector x
    float ty; // translation vector y
    float mean_error;
    float std_deviation;
} motion_t;
