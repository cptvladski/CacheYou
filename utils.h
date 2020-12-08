#include <uuid/uuid.h>
#include <stdlib.h>

typedef struct thread_arg{
    int fd;
}args_t;

char *randomUUID();

char *get_self();

int isJson(char *str);