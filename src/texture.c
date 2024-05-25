#include "texture.h"

text2_t tex2_clone(text2_t *t) {
    text2_t result = {t->u, t->v};
    return result;
}
