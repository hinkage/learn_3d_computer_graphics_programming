#ifndef GEOMETRY_CUBE_H
#define GEOMETRY_CUBE_H

#include "triangle.h"
#include "vector.h"

#define N_CUBE_VERTICES 8
#define N_CUBE_FACES (6 * 2)

extern vec3_t cube_vertices[N_CUBE_VERTICES];
extern face_t cube_faces[N_CUBE_FACES];

void load_cube_mesh_data(vec3_t *vertices, face_t *faces);

#endif
