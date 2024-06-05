#include "matrix.h"
#include <math.h>

mat4_t mat4_identity(void) {
    mat4_t m = {.m = {
                    {1, 0, 0, 0},
                    {0, 1, 0, 0},
                    {0, 0, 1, 0},
                    {0, 0, 0, 1},
                }};
    return m;
}

mat4_t mat4_make_scale(float sx, float sy, float sz) {
    mat4_t m = mat4_identity();
    m.m[0][0] = sx;
    m.m[1][1] = sy;
    m.m[2][2] = sz;
    return m;
}

mat4_t mat4_make_translation(float tx, float ty, float tz) {
    mat4_t m = mat4_identity();
    m.m[0][3] = tx;
    m.m[1][3] = ty;
    m.m[2][3] = tz;
    return m;
}

mat4_t mat4_make_rotation_x(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    /*
     * | 1 0  0 0 |
     * | 0 c -s 0 |
     * | 0 s  c 0 |
     * | 0 0  0 1 |
     */
    mat4_t m = mat4_identity();
    m.m[1][1] = c;
    m.m[1][2] = -s;
    m.m[2][1] = s;
    m.m[2][2] = c;
    return m;
}

mat4_t mat4_make_rotation_y(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    /*
     * Swap sign of s to keep couterclockwise
     * |  c 0 s 0 |
     * |  0 1 0 0 |
     * | -s 0 c 0 |
     * |  0 0 0 1 |
     */
    mat4_t m = mat4_identity();
    m.m[0][0] = c;
    m.m[0][2] = s;
    m.m[2][0] = -s;
    m.m[2][2] = c;
    return m;
}

mat4_t mat4_make_rotation_z(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    /*
     * | c -s 0 0 |
     * | s  c 0 0 |
     * | 0  0 1 0 |
     * | 0  0 0 1 |
     */
    mat4_t m = mat4_identity();
    m.m[0][0] = c;
    m.m[0][1] = -s;
    m.m[1][0] = s;
    m.m[1][1] = c;
    return m;
}

mat4_t mat4_make_perspective(float fovy, float aspecty, float znear,
                             float zfar) {
    // aspecty = height / width,
    mat4_t m = {.m = {{0}}};
    // t = z / x = z / y
    float t = 1 / tan(fovy / 2);
    // x * aspecty = x * (height / width) = x / width * height
    // so x / width = y / height
    // so width of view = height of view, viewport is a square
    // it should be a square to make objects do not distort on x or y
    m.m[0][0] = t * aspecty;
    m.m[1][1] = t;
    m.m[2][2] = zfar / (zfar - znear);
    m.m[2][3] = -znear * zfar / (zfar - znear);
    // For holding the value of z
    m.m[3][2] = 1.0;
    return m;
}

vec4_t mat4_mul_vec4_project(mat4_t mat_proj, vec4_t v) {
    vec4_t result = mat4_mul_vec4(mat_proj, v);
    // Perform perspective divide with original z-value that is now stored in w
    if (result.w != 0.0) {
        result.x /= result.w;
        result.y /= result.w;
        result.z /= result.w;
    }
    return result;
}

vec4_t mat4_mul_vec4(mat4_t m, vec4_t v) {
    vec4_t result;
    result.x =
        m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3] * v.w;
    result.y =
        m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3] * v.w;
    result.z =
        m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3] * v.w;
    result.w =
        m.m[3][0] * v.x + m.m[3][1] * v.y + m.m[3][2] * v.z + m.m[3][3] * v.w;
    return result;
}

mat4_t mat4_mul_mat4(mat4_t a, mat4_t b) {
    mat4_t m;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            m.m[i][j] = a.m[i][0] * b.m[0][j] + a.m[i][1] * b.m[1][j] +
                        a.m[i][2] * b.m[2][j] + a.m[i][3] * b.m[3][j];
        }
    }
    return m;
}

mat4_t mat4_look_at(vec3_t eye, vec3_t target, vec3_t up) {
    // forward (z)
    vec3_t z = vec3_sub(target, eye);
    vec3_normalize(&z);
    // right (x)
    vec3_t x = vec3_cross(up, z);
    vec3_normalize(&x);
    // up (y)
    vec3_t y = vec3_cross(z, x);
    // The dot product between two vectors in 3D space is indeed related
    // to projection, just like in 2D space.
    // Geometrically, the dot product of two vectors in 3D space measures
    // how much one vector projects onto the other. If the two vectors are
    // perpendicular (have a dot product of 0), then one vector does not
    // project onto the other. If they are parallel, then the dot product is
    // maximized, equal to the product of their magnitudes.
    // Dot product of [x.x, x.y, x.z] and [x, y, z] can be explained as
    // projecting [x, y, z] to new x-axis in camera space, and the length
    // of new x-axis is 1, so it is the x component of that point in camera
    // space.
    mat4_t view_matrix = {.m = {
                              {x.x, x.y, x.z, -vec3_dot(x, eye)},
                              {y.x, y.y, y.z, -vec3_dot(y, eye)},
                              {z.x, z.y, z.z, -vec3_dot(z, eye)},
                              {0, 0, 0, 1},
                          }};
    return view_matrix;
}
