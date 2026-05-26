#include <obj_loader.h>
#include <stdio.h>
#include <stdlib.h>

#define Assert(x, msg) do{\
    if(!(x)){\
        fprintf(stderr, "Fail at : %s\n", (msg));\
        exit(1);\
    }\
    printf("Success : %s\n", (msg));\
}while(0)

int main(){
    Obj *obj = obj_load("./assets/ball.obj");
    obj_print(obj);
    obj_free(&obj);
    Assert(obj == NULL, "obj_free should make the variable null");
}
