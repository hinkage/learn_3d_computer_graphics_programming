#include "mesh.h"
#include "array.h"
#include "triangle.h"
#include <stdio.h>
#include <string.h>

mesh_t mesh = {.vertices = NULL,
               .faces = NULL,
               .rotation = {0, 0, 0},
               .scale = {1.0, 1.0, 1.0},
               .translation = {0, 0, 0}};

vec3_t cube_vertices[N_CUBE_VERTICES] = {
    {-1, -1, -1}, // 1
    {-1, 1, -1},  // 2
    {1, 1, -1},   {1, -1, -1}, {1, 1, 1}, {1, -1, 1}, {-1, 1, 1}, {-1, -1, 1},
};

face_t cube_faces[N_CUBE_FACES] = {
    // front
    {1, 2, 3, 0xFFFF0000},
    {1, 3, 4, 0xFFFF0000},
    // right
    {4, 3, 5, 0xFF00FF00},
    {4, 5, 6, 0xFF00FF00},
    // back
    {6, 5, 7, 0xFF0000FF},
    {6, 7, 8, 0xFF0000FF},
    // left
    {8, 7, 2, 0xFFFFFF00},
    {8, 2, 1, 0xFFFFFF00},
    // top
    {2, 7, 5, 0xFFFF00FF},
    {2, 5, 3, 0xFFFF00FF},
    // bottom
    {6, 8, 1, 0xFF00FFFF},
    {6, 1, 4, 0xFF00FFFF}};

void load_cube_mesh_data(void) {
    for (int i = 0; i < N_CUBE_VERTICES; i++) {
        vec3_t cube_vertex = cube_vertices[i];
        array_push(mesh.vertices, cube_vertex);
    }
    for (int i = 0; i < N_CUBE_FACES; i++) {
        face_t cube_face = cube_faces[i];
        array_push(mesh.faces, cube_face);
    }
}

void load_obj_file_data(char *filename) {
    FILE *file;
    file = fopen(filename, "r");
    char line[1024];
    while (fgets(line, 1024, file)) {
        // Vertex
        if (0 == strncmp(line, "v ", 2)) {
            vec3_t vertex;
            sscanf(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
            array_push(mesh.vertices, vertex);
        }
        // Face
        if (0 == strncmp(line, "f ", 2)) {
            int vertex_indices[3];
            int texture_indices[3];
            int normal_indices[3];
            if (strstr(line, "/") != NULL) {
                sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d", &vertex_indices[0],
                       &texture_indices[0], &normal_indices[0],
                       &vertex_indices[1], &texture_indices[1],
                       &normal_indices[1], &vertex_indices[2],
                       &texture_indices[2], &normal_indices[2]);
            } else {
                sscanf(line, "f %d %d %d", &vertex_indices[0],
                       &vertex_indices[1], &vertex_indices[2]);
            }
            face_t face = {vertex_indices[0], vertex_indices[1],
                           vertex_indices[2], .color = 0xFF333333};
            array_push(mesh.faces, face)
        }
    }
    fclose(file);
}
