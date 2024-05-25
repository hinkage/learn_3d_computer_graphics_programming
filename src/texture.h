#ifndef TEXTURE_H
#define TEXTURE_H

typedef struct {
    float u;
    float v;
} text2_t;

text2_t tex2_clone(text2_t *t);

#endif
