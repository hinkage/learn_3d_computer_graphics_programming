#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdint.h>
typedef struct {
    float u;
    float v;
} text2_t;

extern int texture_width;
extern int texture_height;

extern const uint8_t REDBRICK_TEXTURE[];

extern uint32_t *mesh_texture;

void convertRGBAtoARGB(uint8_t *data, size_t length);

#endif
