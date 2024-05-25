#include "array.h"
#include "camera.h"
#include "clipping.h"
#include "display.h"
#include "light.h"
#include "matrix.h"
#include "mesh.h"
#include "texture.h"
#include "upng.h"
#include <SDL_keycode.h>
#include <SDL_pixels.h>
#include <SDL_timer.h>
#include <stdio.h>

#define MAX_TRIANGLES_PER_MESH 10000

// Array of triangles that should be rendered frame by frame
triangle_t triangles_to_render[MAX_TRIANGLES_PER_MESH];
int num_triangles_to_render = 0;

mat4_t world_matrix;
mat4_t proj_matrix;
mat4_t view_matrix;

bool is_running = false;
int previous_frame_time = 0;
float delta_time;

void setup(void) {
    set_render_method(RENDER_WIRE);
    set_cull_method(CULL_BACKFACE);

    // Initialize the perspective projection matrix
    float aspectx = (float)get_window_width() / (float)get_window_height();
    float aspecty = (float)get_window_height() / (float)get_window_width();
    float fovy = M_PI / 3.0f;
    float fovx = atan(tan(fovy / 2.0f) * aspectx) * 2.0f;
    float znear = 0.1f;
    float zfar = 100.0f;
    proj_matrix = mat4_make_perspective(fovy, aspecty, znear, zfar);

    // Initialize frustum planes with a point and a normal
    init_frustum_planes(fovx, fovy, znear, zfar);

    // load_cube_mesh_data();
    load_obj_file_data("./assets/cube.obj");

    load_png_texture_data("./assets/cube.png");
}

void process_input(void) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            is_running = false;
            break;
        case SDL_KEYDOWN:
            SDL_Keycode sym = event.key.keysym.sym;
            if (sym == SDLK_ESCAPE) {
                is_running = false;
                break;
            }
            if (sym == SDLK_1) {
                set_render_method(RENDER_WIRE_VERTEX);
                break;
            }
            if (sym == SDLK_2) {
                set_render_method(RENDER_WIRE);
                break;
            }
            if (sym == SDLK_3) {
                set_render_method(RENDER_FILL_TRIANGLE);
                break;
            }
            if (sym == SDLK_4) {
                set_render_method(RENDER_FILL_TRIANGLE_WIRE);
                break;
            }
            if (sym == SDLK_5) {
                set_render_method(RENDER_TEXTURED);
                break;
            }
            if (sym == SDLK_6) {
                set_render_method(RENDER_TEXTURED_WIRE);
                break;
            }
            if (sym == SDLK_7) {
                set_cull_method(CULL_BACKFACE);
                break;
            }
            if (sym == SDLK_8) {
                set_cull_method(CULL_NONE);
                break;
            }
            if (sym == SDLK_UP) {
                camera.position.y += 3.0f * delta_time;
                break;
            }
            if (sym == SDLK_DOWN) {
                camera.position.y -= 3.0f * delta_time;
                break;
            }
            // Must input capital character to trigger these events, do not know
            // why
            if (sym == SDLK_a) {
                camera.yaw += 1.0f * delta_time;
                break;
            }
            if (sym == SDLK_d) {
                camera.yaw -= 1.0f * delta_time;
                break;
            }
            if (sym == SDLK_w) {
                camera.forward_velocity =
                    vec3_mul(camera.direction, 5.0f * delta_time);
                camera.position =
                    vec3_add(camera.position, camera.forward_velocity);
                break;
            }
            if (sym == SDLK_s) {
                camera.forward_velocity =
                    vec3_mul(camera.direction, 5.0f * delta_time);
                camera.position =
                    vec3_sub(camera.position, camera.forward_velocity);
                break;
            }
            break;
        }
    }
}

void update(void) {
    int time_to_wait =
        FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);
    if (0 < time_to_wait && time_to_wait <= FRAME_TARGET_TIME) {
        SDL_Delay(time_to_wait);
    }
    // Get a delta time factor counterted to senconds to be used to update our
    // game objects
    delta_time = (SDL_GetTicks() - previous_frame_time) / 1000.0f;
    previous_frame_time = SDL_GetTicks();

    // Initialize the couter of triangles to render for current frame
    num_triangles_to_render = 0;

    mesh.rotation.x += 0.6f * delta_time;
    // mesh.rotation.y += 0.9f * delta_time;
    // mesh.rotation.z += 0.2f * delta_time;
    // mesh.scale.x += 0.02f * delta_time;
    // mesh.scale.y += 0.01f * delta_time;
    // mesh.translation.x += 0.1f * delta_time;
    mesh.translation.z = 5.0f;

    // Create view matrix
    vec3_t up_direction = {0, 1, 0};
    vec3_t target = {0, 0, 1};
    mat4_t camera_yaw_rotation = mat4_make_rotation_y(camera.yaw);
    camera.direction = vec3_from_vec4(
        mat4_mul_vec4(camera_yaw_rotation, vec4_from_vec3(target)));
    target = vec3_add(camera.position, camera.direction);
    view_matrix = mat4_look_at(camera.position, target, up_direction);

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
            world_matrix = mat4_identity();
            world_matrix = mat4_mul_mat4(scale_matrix, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_z, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_y, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_x, world_matrix);
            world_matrix = mat4_mul_mat4(translation_matrix, world_matrix);

            // World space
            transformed_vertex =
                mat4_mul_vec4(world_matrix, transformed_vertex);

            // To camera space
            transformed_vertex = mat4_mul_vec4(view_matrix, transformed_vertex);

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
        vec3_t origin = {0, 0, 0};
        vec3_t camera_ray = vec3_sub(origin, vector_a);
        // Use dot product to calculate how aligned the camera ray is with
        // the face normal
        float dot_normal_camera = vec3_dot(normal, camera_ray);
        // Bypass the triangles that are looking away from the camera
        if (is_cull_backface()) {
            if (dot_normal_camera < 0) {
                continue;
            }
        }

        // Clipping
        // Create a polygon from the original transformed triangle to be clipped
        polygon_t polygon = create_polygon_from_triangle(
            vec3_from_vec4(transformed_vertices[0]),
            vec3_from_vec4(transformed_vertices[1]),
            vec3_from_vec4(transformed_vertices[2]), mesh_face.a_uv,
            mesh_face.b_uv, mesh_face.c_uv);
        // Clip the polygon and returns a new polygon with potential new
        // vertices
        clip_polygon(&polygon);
        // Break the clipped polygon apart back into individual triangles
        triangle_t triangles_after_clipping[MAX_NUM_POLY_TRIANGLES];
        int num_triangles_after_clipping = 0;
        triangles_from_polygon(&polygon, triangles_after_clipping,
                               &num_triangles_after_clipping);

        // Loops all the assembled triangles after clipping
        for (int t = 0; t < num_triangles_after_clipping; t++) {
            triangle_t triangle_after_clipping = triangles_after_clipping[t];
            // Points on screen also need to hold w to do depth interpolation
            // and comparison. Holding z is not necessary.
            vec4_t projected_points[3];
            for (int j = 0; j < 3; j++) {
                // Project the current vertex
                projected_points[j] = mat4_mul_vec4_project(
                    proj_matrix, triangle_after_clipping.points[j]);
                // Scale into the view
                projected_points[j].x *= (get_window_width() / 2.0);
                projected_points[j].y *= (get_window_height() / 2.0);
                // Invert the y values to account for flipped screen y
                // coordinate, because y axis in model is heading up, but we
                // render buffer from top to bottom
                projected_points[j].y *= -1.0;
                // Translate to center
                projected_points[j].x += (get_window_width() / 2.0);
                projected_points[j].y += (get_window_height() / 2.0);
            }

            // Color
            float light_intensity_factor = -vec3_dot(normal, light.direction);
            uint32_t triangle_color =
                light_apply_intensity(mesh_face.color, light_intensity_factor);

            triangle_t triangle_to_render = {
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
                        {
                            triangle_after_clipping.texcoords[0].u,
                            triangle_after_clipping.texcoords[0].v,
                        },

                        {
                            triangle_after_clipping.texcoords[1].u,
                            triangle_after_clipping.texcoords[1].v,
                        },

                        {
                            triangle_after_clipping.texcoords[2].u,
                            triangle_after_clipping.texcoords[2].v,
                        },
                    },
                .color = triangle_color};
            // Save
            if (num_triangles_to_render < MAX_TRIANGLES_PER_MESH) {
                triangles_to_render[num_triangles_to_render] =
                    triangle_to_render;
                num_triangles_to_render++;
            }
        }
    }

    // Painter's Algorithm
    // Can only sort the triangles,
    // like SVG can only sort the path elements,
    // this can not compare depth on pixel level.
    // Bubble sort bigger avg_depth to the front of array
    /* int num_triangles = array_length(triangles_to_render);
    for (int i = 0; i < num_triangles; i++) {
        for (int j = i + 1; j < num_triangles; j++) {
            if (triangles_to_render[i].avg_depth <
                triangles_to_render[j].avg_depth) {
                triangle_t t = triangles_to_render[i];
                triangles_to_render[i] = triangles_to_render[j];
                triangles_to_render[j] = t;
            }
        }
    } */
}

void render(void) {
    clear_color_buffer(0xFF000000);
    clear_z_buffer();
    draw_grid();

    // Draw triangles on screen
    for (int i = 0; i < num_triangles_to_render; i++) {
        triangle_t triangle = triangles_to_render[i];
        if (should_render_filled_triangles()) {
            draw_filled_triangle(triangle.points[0].x, triangle.points[0].y,
                                 triangle.points[0].z, triangle.points[0].w,
                                 triangle.points[1].x, triangle.points[1].y,
                                 triangle.points[1].z, triangle.points[1].w,
                                 triangle.points[2].x, triangle.points[2].y,
                                 triangle.points[2].z, triangle.points[2].w,
                                 triangle.color);
        }
        if (should_render_textured_triangles()) {
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
        if (should_render_wireframe()) {
            draw_triangle(triangle.points[0].x, triangle.points[0].y,
                          triangle.points[1].x, triangle.points[1].y,
                          triangle.points[2].x, triangle.points[2].y,
                          0xFF00FF00);
        }
        if (should_render_wire_vertex()) {
            // Draw vertices
            for (int j = 0; j < 3; j++) {
                draw_rect(triangle.points[j].x - 3, triangle.points[j].y - 3, 6,
                          6, 0xFF0000FF);
            }
        }
    }

    render_color_buffer();
}

void free_resources(void) {
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
