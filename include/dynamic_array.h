#pragma once
// LINT_C_FILE
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#define INIT_CAP 8

typedef struct {
    size_t capacity;
    size_t length;
} Header;

#define da_free(arr) do{    \
    if(!(arr)) break;       \
    free((Header*)(arr)-1); \
    (arr)=NULL;             \
} while(0)

#define da_len(arr) ((arr) ? ((Header*)(arr)-1)->length : 0)

#define da_pop(arr, res) do{     \
    Header* h = (Header*)(arr)-1;\
    if(h->length == 0){          \
        res = NULL;              \
        continue;                \
    }                            \
    h->length -=1;               \
    res=&(arr)[h->length];       \
}while(0)

#define da_create(arr, values, n, cap) do{\
    unsigned int capacity = (cap) > (n) ? (cap) : (n);\
    Header *h = malloc(sizeof(Header) + (capacity * sizeof(typeof(*(values)))));\
    h->capacity = capacity;\
    h->length = (n);\
    (arr) = (typeof(arr))(h+1);\
    for(unsigned int i=0; i<(n); i++){\
        (arr)[i] = (values)[i];\
    }\
} while(0)

#define da_push(arr, value) do{                                                  \
    if(!arr){                                                                    \
        Header *h = malloc(sizeof(*h) + (sizeof(value) * INIT_CAP));             \
        h->capacity=INIT_CAP;                                                    \
        h->length=0;                                                             \
        (arr) = (typeof(arr)) (h+1);                                             \
    }                                                                            \
    Header *h = (Header *)(arr)-1;                                               \
    if(h->capacity < h->length + 1) {                                            \
        size_t new_cap = h->capacity * 2;                                        \
        Header *new_h = realloc(h, sizeof(*h) + (sizeof(value) * new_cap));     \
        if(!new_h){                                                              \
            fprintf(stderr,"%s","cant reallocate array\n");                      \
            exit(1);                                                             \
        }                                                                        \
        h = new_h;                                                               \
        (arr) = (typeof(arr))(h+1);                                              \
        h->capacity = new_cap;                                                   \
    }                                                                            \
    (arr)[h->length] = (value);                                                  \
                                                                                 \
    h->length += 1;                                                              \
} while(0)
