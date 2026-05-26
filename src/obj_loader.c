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
    obj->da_indices = NULL;
    {
        int f_slash_count = -1;

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
            }else if(strcmp(flag, "f") == 0){
                int vert_idx[3] ={0};
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
                        sscanf_res = sscanf(line_p, "%d %d %d", &vert_idx[0], &vert_idx[1], &vert_idx[2]);
                        break;
                    case 1:
                        sscanf_res = sscanf(line_p, "%d/%*d %d/%*d %d/%*d", &vert_idx[0], &vert_idx[1], &vert_idx[2]);
                        break;
                    case 2:
                        sscanf_res = sscanf(line_p, "%d/%*d/%*d %d/%*d/%*d %d/%*d/%*d", &vert_idx[0], &vert_idx[1], &vert_idx[2]);
                        if(sscanf_res <= 0){
                            sscanf_res = sscanf(line_p, "%d//%*d %d//%*d %d//%*d", &vert_idx[0], &vert_idx[1], &vert_idx[2]);
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
                for(int i=0; i < 3; i++){
                    da_push(obj->da_indices, vert_idx[i]);
                }
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
    printf("vertices \n");
    for(int i = 0; (i+2) < da_len(obj->da_verts); i+=3){
        printf("%f, %f, %f\n", obj->da_verts[i], obj->da_verts[i+1], obj->da_verts[i+2]);
    }
    printf("\n");
    printf("indices \n");
    for(int i = 0; (i+2) < da_len(obj->da_indices); i+=3){
        printf("%d, %d, %d\n", obj->da_indices[i], obj->da_indices[i+1], obj->da_indices[i+2]);
    }
}
