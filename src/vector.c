#include "vector.h"
#include <math.h>

vec3_t vec3_rotate_x(vec3_t v, float angle) {
    // look at x, counterclockwise, y to z
    vec3_t rotated_vector = {.y = v.y * cos(angle) - v.z * sin(angle),
                             .z = v.y * sin(angle) + v.z * cos(angle),
                             .x = v.x};
    return rotated_vector;
}

vec3_t vec3_rotate_y(vec3_t v, float angle) {
    // look at y, clockwise, x to z, why?
    vec3_t rotated_vector = {.x = v.x * cos(angle) - v.z * sin(angle),
                             .z = v.x * sin(angle) + v.z * cos(angle),
                             .y = v.y};
    return rotated_vector;
}

vec3_t vec3_rotate_z(vec3_t v, float angle) {
    // look at z, counterclockwise, x to y
    vec3_t rotated_vector = {.x = v.x * cos(angle) - v.y * sin(angle),
                             .y = v.x * sin(angle) + v.y * cos(angle),
                             .z = v.z};
    return rotated_vector;
}
