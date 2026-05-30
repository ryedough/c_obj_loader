#pragma once
#include <stdbool.h>

typedef struct obj Obj;

struct obj {
    float* da_verts;
    unsigned int* da_indices;
    bool has_uv;
};

Obj* obj_load(const char*);
void obj_free(Obj** const);
void obj_print(Obj*);
