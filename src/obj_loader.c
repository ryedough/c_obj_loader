#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dynamic_array.h>
#include <obj_loader.h>
#include <stdbool.h>
#include <hashmap.h>

#define MAX_LINE_BUF 512

typedef struct {
    float x,y;
} UV;

typedef struct {
    float x,y,z;
} Vertex;

Obj* obj_load(const char* path){
    Obj* obj = malloc(sizeof(*obj));
    obj->has_uv = false;

    FILE *obj_src = fopen(path, "r");
    if(!obj_src){
        fprintf(stderr, "Can't open obj file at %s\n", path);
        exit(1);
    }

    obj->da_verts = NULL;
    obj->da_indices = NULL;

    Vertex* da_raw_verts = NULL;
    UV* da_raw_uvs = NULL;
    Hashmap* hmap_found_f = hmap_create(500);
    char** da_found_f_indices = NULL;

    {
        int f_slash_count = -1;

        char line[MAX_LINE_BUF];
        while(fgets(line, MAX_LINE_BUF, obj_src)){
            char flag[20] = {0};

            char *line_p = line;
            char *flag_p = flag;
            while(*line_p != ' '){
               *flag_p++ = *line_p++;
            }
            line_p++;

            if(strcmp(flag, "v") == 0){
                Vertex vert;
                if(sscanf(line_p, "%f %f %f", &vert.x, &vert.y, &vert.z) <= 0){
                    fprintf(stderr, "Invalid vertex position format\n");
                    exit(1);
                }
                da_push(da_raw_verts, vert);
            }
            if(strcmp(flag, "vt") == 0){
                obj->has_uv = true;
                UV uv;
                if (sscanf(line_p, "%f %f", &uv.x, &uv.y) <= 0) {
                    fprintf(stderr, "Invalid vertex UV format\n");
                    exit(1);
                }
                da_push(da_raw_uvs, uv);
            } else if(strcmp(flag, "f") == 0){
                unsigned int idx[3] ={0};
                unsigned int uv_idx[3]={0};

                int sscanf_res = -1;
get_f_format:
                switch (f_slash_count) {
                    case -1:
                        f_slash_count = 0;
                        char* slash_tester = line_p;
                        while(*slash_tester != ' ') {
                            if(*slash_tester == '/'){
                                f_slash_count++;
                            }
                            slash_tester++;
                        }
                        goto get_f_format;
                    case 0:
                        if(obj->has_uv) break;
                        sscanf_res = sscanf(line_p, "%u %u %u", &idx[0], &idx[1], &idx[2]);
                        break;
                    case 1:
                        sscanf_res = sscanf(line_p, "%u/%u %u/%u %u/%u", &idx[0], &uv_idx[0], &idx[1], &uv_idx[1], &idx[2], &uv_idx[2]);
                        break;
                    case 2:
                        sscanf_res = sscanf(line_p, "%u/%u/%*u %u/%u/%*u %u/%u/%*u", &idx[0], &uv_idx[0], &idx[1], &uv_idx[1], &idx[2], &uv_idx[2]);
                        if(sscanf_res <= 0 && !obj->has_uv){
                            sscanf_res = sscanf(line_p, "%u//%*u %u//%*u %u//%*u", &idx[0], &idx[1], &idx[2]);
                            break;
                        }
                        break;
                    default:
                        fprintf(stderr, "Invalid obj f format");
                        exit(1);
                        break;
                }
                if(sscanf_res <= 0){
                    fprintf(stderr, "Invalid obj f format");
                    exit(1);
                }
                if(obj->has_uv){
                    for(int i=0; i<3; i++){
                        size_t c_idx = idx[i]-1;
                        size_t c_uv_idx = uv_idx[i]-1;
                        char key[20];
                        snprintf(key, sizeof(key), "%td,%td", c_idx, c_uv_idx);

                        HashmapElement* found_index = hmap_get(hmap_found_f, key);
                        if(found_index){
                            da_push(obj->da_indices, (size_t)found_index->value);
                        }else {
                            Vertex raw_verts = da_raw_verts[c_idx];
                            UV raw_uv = da_raw_uvs[c_idx];
                            da_push(obj->da_verts, raw_verts.x);
                            da_push(obj->da_verts, raw_verts.y);
                            da_push(obj->da_verts, raw_verts.z);
                            da_push(obj->da_verts, raw_uv.x);
                            da_push(obj->da_verts, raw_uv.y);
                            size_t indices = (da_len(obj->da_verts)/5)-1;
                            da_push(obj->da_indices, indices);
                            hmap_insert(hmap_found_f, key, (void*)indices);
                        }
                    }
                }else {
                    for(int i=0; i < 3; i++){
                        da_push(obj->da_indices, idx[i]-1);
                    }
                }
            }
        }
    }
    // flattening da_raw_verts
    if(!obj->has_uv){
        for(int i=0; i<da_len(da_raw_verts);i++){
            da_push(obj->da_verts, da_raw_verts[i].x);
            da_push(obj->da_verts, da_raw_verts[i].y);
            da_push(obj->da_verts, da_raw_verts[i].z);
        }
    }
    hmap_free(&hmap_found_f, NULL);
    da_free(da_found_f_indices);
    da_free(da_raw_verts);
    da_free(da_raw_uvs);
    fclose(obj_src);
    return obj;
}

void obj_free(Obj ** const obj){
    da_free((*obj)->da_verts);
    da_free((*obj)->da_indices);
    free(*obj);
    *obj = NULL;
}

void obj_print(Obj* obj){
    printf("da_verts length: %td\nda_indices length : %td\nhas_uv : %s\n", da_len(obj->da_verts), da_len(obj->da_indices), obj->has_uv ? "true":"false");
}
