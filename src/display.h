#ifndef DISPLAY_H
#define DISPLAY_H
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdint.h>

#define FPS 60
#define FRAME_TARGET_TIME (1000 / FPS)

enum { CULL_NONE, CULL_BACKFACE };

enum {
    RENDER_WIRE,
    RENDER_WIRE_VERTEX,
    RENDER_FILL_TRIANGLE,
    RENDER_FILL_TRIANGLE_WIRE,
    RENDER_TEXTURED,
    RENDER_TEXTURED_WIRE
};

bool initialize_window(void);
void draw_grid(void);
void draw_pixel(int x, int y, uint32_t color);
void draw_line(int x0, int y0, int x1, int y1, uint32_t color);
void draw_rect(int x, int y, int width, int height, uint32_t color);
void render_color_buffer(void);
void clear_color_buffer(uint32_t color);
void clear_z_buffer();
float get_zbuffer_at(int x, int y);
void update_zbuffer_at(int x, int y, float value);
void destroy_window(void);

bool should_cull_backface(void);
bool should_render_filled_triangle(void);
bool should_render_textured_triangle(void);
bool should_render_wireframe(void);
bool should_render_wire_vertex(void);

int get_window_width(void);
int get_window_height(void);
void set_render_method(int method);
void set_cull_method(int method);

#endif
