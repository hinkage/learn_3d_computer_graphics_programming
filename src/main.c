#include "display.h"
#include "mesh.h"
#include "vector.h"
#include <SDL_timer.h>
#include <stdio.h>

triangle_t triangle_to_render[N_MESH_FACES];

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

    for (int i = 0; i < N_MESH_FACES; i++) {
        face_t mesh_face = mesh_faces[i];
        vec3_t face_vertices[3];
        face_vertices[0] = mesh_vertices[mesh_face.a - 1];
        face_vertices[1] = mesh_vertices[mesh_face.b - 1];
        face_vertices[2] = mesh_vertices[mesh_face.c - 1];

        triangle_t projected_triangle;

        for (int j = 0; j < 3; j++) {
            vec3_t transformed_vertex = face_vertices[j];
            transformed_vertex =
                vec3_rotate_x(transformed_vertex, cube_rotation.x);
            transformed_vertex =
                vec3_rotate_y(transformed_vertex, cube_rotation.y);
            transformed_vertex =
                vec3_rotate_z(transformed_vertex, cube_rotation.z);
            transformed_vertex.z -= camera_position.z;
            vec2_t projected_point = project(transformed_vertex);
            // translate
            projected_point.x += (window_width / 2.0);
            projected_point.y += (window_height / 2.0);
            projected_triangle.points[j] = projected_point;
        }

        triangle_to_render[i] = projected_triangle;
    }
}

void render(void) {
    for (int i = 0; i < N_MESH_FACES; i++) {
        triangle_t triangle = triangle_to_render[i];
        for (int j = 0; j < 3; j++) {
            draw_rect(triangle.points[j].x, triangle.points[j].y, 3, 3,
                      0xFFFFFF00);
        }
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
