#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dynamic_array.h>
#include <obj_loader.h>

Obj* obj_load(const char* path){
    Obj* obj = malloc(sizeof(*obj));

    FILE *obj_src = fopen(path, "r");
    if(!obj_src){
        fprintf(stderr, "Can't open obj file at %s\n", path);
        exit(1);
    }

    obj->da_verts = NULL;
    {

        char line[512];
        while(fgets(line, 512, obj_src)){
            char flag[20] = {0};

            char *line_p = line;
            char *flag_p = flag;
            while(*line_p != ' '){
               *flag_p++ = *line_p++;
            }
            line_p++;

            if(strcmp(flag, "v") == 0){
                float x, y, z;
                if(sscanf(line_p, "%f %f %f", &x, &y, &z) <= 0){
                    fprintf(stderr, "Invalid vertex format\n");
                    exit(1);
                }
                da_push(obj->da_verts, x);
                da_push(obj->da_verts, y);
                da_push(obj->da_verts, z);
            }
        }
    }
    fclose(obj_src);
    return obj;
}

void obj_free(Obj ** const obj){
    da_free((*obj)->da_verts);
    free(*obj);
    *obj = NULL;
}

void obj_print(Obj* obj){
    for(int i = 0; (i+2) < da_len(obj->da_verts); i+=3){
        printf("%f, %f, %f\n", obj->da_verts[i], obj->da_verts[i+1], obj->da_verts[i+3]);
    }
}
