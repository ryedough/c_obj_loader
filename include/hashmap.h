#pragma once
// LINT_C_FILE
#include <stdio.h>
#include <stdbool.h>
#define MAX_HMAP_KEY_LEN 20

typedef struct hmap_el{
    struct hmap_el* next;
    char key[MAX_HMAP_KEY_LEN];
    void* value;
} HashmapElement;

typedef struct hmap{
    size_t len;
    size_t cap;
    size_t* da_has_value_idx;
    struct hmap_el** elements;
} Hashmap;

Hashmap *hmap_create(size_t cap);

bool hmap_insert(Hashmap* hmap, char* key, void* value);
HashmapElement* hmap_get(Hashmap* hmap, char* key);
void hmap_free(Hashmap** const hmap, void (*free_value)(void*));
HashmapElement** hmap_to_da(Hashmap* hmap);

void hmap_free_value_fn_just_free(void* value);
