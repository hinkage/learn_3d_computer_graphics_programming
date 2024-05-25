#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "texture.h"
#include "upng.h"
#include "vector.h"
#include <stdint.h>

typedef struct {
    int a;
    int b;
    int c;
    text2_t a_uv;
    text2_t b_uv;
    text2_t c_uv;
    uint32_t color;
} face_t;

typedef struct {
    vec4_t points[3];
    text2_t texcoords[3];
    uint32_t color;
    upng_t *texture;
    // float avg_depth; // For Painter's Algorithm
} triangle_t;

vec3_t get_triangle_normal(vec4_t vertices[3]);

void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2,
                   uint32_t color);
void draw_triangle_pixel(int x, int y, uint32_t color, vec4_t point_a,
                         vec4_t point_b, vec4_t point_c);
void draw_filled_triangle(int x0, int y0, float z0, float w0, int x1, int y1,
                          float z1, float w1, int x2, int y2, float z2,
                          float w2, uint32_t color);
void draw_triangle_texel(int x, int y, upng_t *texture, vec4_t point_a,
                         vec4_t point_b, vec4_t point_c, text2_t a_uv,
                         text2_t b_uv, text2_t c_uv);
void draw_textured_triangle(int x0, int y0, float z0, float w0, float u0,
                            float v0, int x1, int y1, float z1, float w1,
                            float u1, float v1, int x2, int y2, float z2,
                            float w2, float u2, float v2, upng_t *texture);
#endif
