#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "texture.h"
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
    vec2_t points[3];
    text2_t texcoords[3];
    uint32_t color;
    float avg_depth;
} triangle_t;

void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2,
                   uint32_t color);
void draw_filled_triangle(int x0, int y0, int x1, int y1, int x2, int y2,
                          uint32_t color);
void draw_texel(int x, int y, uint32_t *texture, vec2_t point_a, vec2_t point_b,
                vec2_t point_c, float u0, float v0, float u1, float v1,
                float u2, float v2);
void draw_textured_triangle(int x0, int y0, float u0, float v0, int x1, int y1,
                            float u1, float v1, int x2, int y2, float u2,
                            float v2, uint32_t *texture);
#endif
