#include "array.h"
#include "display.h"
#include "light.h"
#include "matrix.h"
#include "mesh.h"
#include "texture.h"
#include "upng.h"
#include <SDL_timer.h>
#include <stdio.h>

// Array of triangles that should be rendered frame by frame
triangle_t *triangles_to_render = NULL;

vec3_t camera_position = {0, 0, 0};

mat4_t proj_matrix;

bool is_running = false;
int previous_frame_time = 0;

void setup(void) {
    render_method = RENDER_WIRE;
    cull_method = CULL_BACKFACE;

    color_buffer =
        (uint32_t *)malloc(sizeof(uint32_t) * window_width * window_height);
    color_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32,
                                             SDL_TEXTUREACCESS_STREAMING,
                                             window_width, window_height);
    // Initialize the perspective projection matrix
    float fov = M_PI / 3;
    float aspect = window_height / (float)window_width;
    float znear = 0.1;
    float zfar = 100.0;
    proj_matrix = mat4_make_perspective(fov, aspect, znear, zfar);

    // load_cube_mesh_data();
    load_obj_file_data("./assets/crab.obj");

    load_png_texture_data("./assets/crab.png");
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
        if (event.key.keysym.sym == SDLK_1) {
            render_method = RENDER_WIRE_VERTEX;
        }
        if (event.key.keysym.sym == SDLK_2) {
            render_method = RENDER_WIRE;
        }
        if (event.key.keysym.sym == SDLK_3) {
            render_method = RENDER_FILL_TRIANGLE;
        }
        if (event.key.keysym.sym == SDLK_4) {
            render_method = RENDER_FILL_TRIANGLE_WIRE;
        }
        if (event.key.keysym.sym == SDLK_5) {
            render_method = RENDER_TEXTURED;
        }
        if (event.key.keysym.sym == SDLK_6) {
            render_method = RENDER_TEXTURED_WIRE;
        }
        if (event.key.keysym.sym == SDLK_7) {
            cull_method = CULL_BACKFACE;
        }
        if (event.key.keysym.sym == SDLK_8) {
            cull_method = CULL_NONE;
        }
        break;
    }
}

void update(void) {
    int time_to_wait =
        FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);
    if (0 < time_to_wait && time_to_wait <= FRAME_TARGET_TIME) {
        SDL_Delay(time_to_wait);
    }
    previous_frame_time = SDL_GetTicks();

    // Initialize the array of triangles to render
    triangles_to_render = NULL;

    // mesh.rotation.x += 0.01;
    mesh.rotation.y += 0.01;
    // mesh.rotation.z += 0.01;
    // mesh.scale.x += 0.002;
    // mesh.scale.y += 0.001;
    // mesh.translation.x += 0.01;
    mesh.translation.z = 5;

    // Scale matrix
    mat4_t scale_matrix =
        mat4_make_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);
    mat4_t translation_matrix = mat4_make_translation(
        mesh.translation.x, mesh.translation.y, mesh.translation.z);
    mat4_t rotation_matrix_x = mat4_make_rotation_x(mesh.rotation.x);
    mat4_t rotation_matrix_y = mat4_make_rotation_y(mesh.rotation.y);
    mat4_t rotation_matrix_z = mat4_make_rotation_z(mesh.rotation.z);

    // Loop all faces
    int num_faces = array_length(mesh.faces);
    for (int i = 0; i < num_faces; i++) {
        face_t mesh_face = mesh.faces[i];

        vec3_t face_vertices[3];
        face_vertices[0] = mesh.vertices[mesh_face.a];
        face_vertices[1] = mesh.vertices[mesh_face.b];
        face_vertices[2] = mesh.vertices[mesh_face.c];

        vec4_t transformed_vertices[3];

        for (int j = 0; j < 3; j++) {
            vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);

            // Create a World Matrix combining scale, rotation and translation
            // matrices
            mat4_t world_matrix = mat4_identity();
            world_matrix = mat4_mul_mat4(scale_matrix, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_z, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_y, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_x, world_matrix);
            world_matrix = mat4_mul_mat4(translation_matrix, world_matrix);

            transformed_vertex =
                mat4_mul_vec4(world_matrix, transformed_vertex);

            // Save transformed vertices
            transformed_vertices[j] = transformed_vertex;
        }

        // Back-face culling
        vec3_t vector_a = vec3_from_vec4(transformed_vertices[0]);
        vec3_t vector_b = vec3_from_vec4(transformed_vertices[1]);
        vec3_t vector_c = vec3_from_vec4(transformed_vertices[2]);
        // B-A, C-A
        vec3_t vector_ab = vec3_sub(vector_b, vector_a);
        vec3_t vector_ac = vec3_sub(vector_c, vector_a);
        vec3_normalize(&vector_ab);
        vec3_normalize(&vector_ac);
        // Use cross product to find perpendicular
        vec3_t normal = vec3_cross(vector_ab, vector_ac);
        vec3_normalize(&normal);
        // Camera ray
        vec3_t camera_ray = vec3_sub(camera_position, vector_a);
        // Use dot product to calculate how aligned the camera ray is with
        // the face normal
        float dot_normal_camera = vec3_dot(normal, camera_ray);
        // Bypass the triangles that are looking away from the camera
        if (cull_method == CULL_BACKFACE) {
            if (dot_normal_camera < 0) {
                continue;
            }
        }

        vec4_t projected_points[3];
        for (int j = 0; j < 3; j++) {
            // Project the current vertex
            projected_points[j] =
                mat4_mul_vec4_project(proj_matrix, transformed_vertices[j]);
            // Scale into the view
            projected_points[j].x *= (window_width / 2.0);
            projected_points[j].y *= (window_height / 2.0);
            // Invert the y values to account for flipped screen y coordinate,
            // because y axis in model is heading up, but we render buffer from
            // top to bottom
            projected_points[j].y *= -1.0;
            // Translate to center
            projected_points[j].x += (window_width / 2.0);
            projected_points[j].y += (window_height / 2.0);
        }
        float avg_depth =
            (transformed_vertices[0].z + transformed_vertices[1].z +
             transformed_vertices[2].z) /
            3.0;

        // Color
        float light_intensity_factor = -vec3_dot(normal, light.direction);
        uint32_t triangle_color =
            light_apply_intensity(mesh_face.color, light_intensity_factor);

        triangle_t projected_triangle = {
            .points = {{
                           projected_points[0].x,
                           projected_points[0].y,
                           projected_points[0].z,
                           projected_points[0].w,
                       },
                       {
                           projected_points[1].x,
                           projected_points[1].y,
                           projected_points[1].z,
                           projected_points[1].w,
                       },
                       {
                           projected_points[2].x,
                           projected_points[2].y,
                           projected_points[2].z,
                           projected_points[2].w,
                       }},
            .texcoords =
                {
                    {mesh_face.a_uv.u, mesh_face.a_uv.v},
                    {mesh_face.b_uv.u, mesh_face.b_uv.v},
                    {mesh_face.c_uv.u, mesh_face.c_uv.v},
                },
            .color = triangle_color,
            .avg_depth = avg_depth,
        };
        // Save
        array_push(triangles_to_render, projected_triangle);
    }

    // Bubble sort big avg_depth to the front of array
    int num_triangles = array_length(triangles_to_render);
    for (int i = 0; i < num_triangles; i++) {
        for (int j = i + 1; j < num_triangles; j++) {
            if (triangles_to_render[i].avg_depth <
                triangles_to_render[j].avg_depth) {
                triangle_t t = triangles_to_render[i];
                triangles_to_render[i] = triangles_to_render[j];
                triangles_to_render[j] = t;
            }
        }
    }
}

void render(void) {
    draw_grid();

    // Draw triangles on screen
    int num_triangles = array_length(triangles_to_render);
    for (int i = 0; i < num_triangles; i++) {
        triangle_t triangle = triangles_to_render[i];
        if (render_method == RENDER_FILL_TRIANGLE ||
            render_method == RENDER_FILL_TRIANGLE_WIRE) {
            draw_filled_triangle(triangle.points[0].x, triangle.points[0].y,
                                 triangle.points[1].x, triangle.points[1].y,
                                 triangle.points[2].x, triangle.points[2].y,
                                 triangle.color);
        }
        if (render_method == RENDER_TEXTURED ||
            render_method == RENDER_TEXTURED_WIRE) {
            draw_textured_triangle(
                triangle.points[0].x, triangle.points[0].y,
                triangle.points[0].z, triangle.points[0].w,
                triangle.texcoords[0].u, triangle.texcoords[0].v,
                triangle.points[1].x, triangle.points[1].y,
                triangle.points[1].z, triangle.points[1].w,
                triangle.texcoords[1].u, triangle.texcoords[1].v,
                triangle.points[2].x, triangle.points[2].y,
                triangle.points[2].z, triangle.points[2].w,
                triangle.texcoords[2].u, triangle.texcoords[2].v, mesh_texture);
        }
        if (render_method == RENDER_WIRE ||
            render_method == RENDER_WIRE_VERTEX ||
            render_method == RENDER_FILL_TRIANGLE_WIRE ||
            render_method == RENDER_TEXTURED_WIRE) {
            draw_triangle(triangle.points[0].x, triangle.points[0].y,
                          triangle.points[1].x, triangle.points[1].y,
                          triangle.points[2].x, triangle.points[2].y,
                          0xFF00FF00);
        }
        if (render_method == RENDER_WIRE_VERTEX) {
            // Draw vertices
            for (int j = 0; j < 3; j++) {
                draw_rect(triangle.points[j].x - 3, triangle.points[j].y - 3, 6,
                          6, 0xFFFF0000);
            }
        }
    }

    // Clear triangles
    array_free(triangles_to_render);

    render_color_buffer();
    clear_color_buffer(0xFF000000);
    SDL_RenderPresent(renderer);
}

void free_resources(void) {
    free(color_buffer);
    upng_free(png_texture);
    array_free(mesh.faces);
    array_free(mesh.vertices);
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
    free_resources();

    return 0;
}
