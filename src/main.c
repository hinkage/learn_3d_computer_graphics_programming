#include "display.h"
#include "vector.h"
#include <SDL_timer.h>
#include <stdio.h>

const int N_POINTS = 9 * 9 * 9;
vec3_t cube_points[N_POINTS];
vec2_t projected_points[N_POINTS];

vec3_t camera_position = {0, 0, -5};
vec3_t cube_rotation = {0, 0, 0};

float fov_factor = 628;

bool is_running = false;
int previous_frame_time = 0;

void setup(void) {
    color_buffer =
        (uint32_t *)malloc(sizeof(uint32_t) * window_width * window_height);
    color_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                             SDL_TEXTUREACCESS_STREAMING,
                                             window_width, window_height);
    int point_count = 0;
    for (float x = -1; x <= 1; x += 0.25) {
        for (float y = -1; y <= 1; y += 0.25) {
            for (float z = -1; z <= 1; z += 0.25) {
                vec3_t new_point = {x, y, z};
                cube_points[point_count++] = new_point;
            }
        }
    }
}

void process_input(void) {
    SDL_Event event;
    SDL_PollEvent(&event);
    switch (event.type) {
    case SDL_QUIT:
        is_running = false;
        break;
    case SDL_KEYDOWN:
        if (event.key.keysym.sym == SDLK_ESCAPE) {
            is_running = false;
        }
        break;
    }
}

vec2_t project(vec3_t point) {
    vec2_t projected_point = {point.x * fov_factor / point.z,
                              point.y * fov_factor / point.z};
    return projected_point;
}

void update(void) {
    int time_to_wait =
        FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);
    if (0 < time_to_wait && time_to_wait <= FRAME_TARGET_TIME) {
        SDL_Delay(time_to_wait);
    }
    previous_frame_time = SDL_GetTicks();

    cube_rotation.x += 0.01;
    cube_rotation.y += 0.01;
    cube_rotation.z += 0.01;

    for (int i = 0; i < N_POINTS; i++) {
        vec3_t point = cube_points[i];

        vec3_t transformed_point = vec3_rotate_x(point, cube_rotation.x);
        transformed_point = vec3_rotate_y(transformed_point, cube_rotation.y);
        transformed_point = vec3_rotate_z(transformed_point, cube_rotation.z);

        // Translate
        transformed_point.z -= camera_position.z;

        // Project
        vec2_t projected_point = project(transformed_point);

        // Save
        projected_points[i] = projected_point;
    }
}

void render(void) {
    // draw_grid();
    // draw_pixel(300, 300, 0xFFFF0000);
    // draw_rect(100, 100, 200, 100, 0xFF333333);
    for (int i = 0; i < N_POINTS; i++) {
        vec2_t projected_point = projected_points[i];
        draw_rect(projected_point.x + window_width / 2,
                  projected_point.y + window_height / 2, 4, 4, 0xFFFFFF00);
    }
    render_color_buffer();
    clear_color_buffer(0xFF000000);
    SDL_RenderPresent(renderer);
}

int main(int argv, char **args) {
    is_running = initialize_window();

    setup();

    while (is_running) {
        process_input();
        update();
        render();
    }

    destroy_window();

    return 0;
}
