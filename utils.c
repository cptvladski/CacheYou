#include "utils.h"

char *randomUUID(){
    uuid_t binuuid;
    uuid_generate_random(binuuid);
    char * uuid = malloc(37);
    uuid_unparse_lower(binuuid,uuid);
    return uuid;
}

char *get_self() {
    char *name = randomUUID();
    name[5] = '\0';
    return name;
}

int isJson(char *str){
    return *str == '{';
}