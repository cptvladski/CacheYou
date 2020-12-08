#include "c_hashmap/hashmap.h"
#include <stdio.h>
#include <string.h>
#include "utils.h"
#include <unistd.h>

#define BUFFSIZE 1024

void *workerThread(void *arg);