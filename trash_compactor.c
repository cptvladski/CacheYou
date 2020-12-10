#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "trash_compactor.h"
int id = 0;
void res_add(lifetime_stack_t *stack, void *res, void (*dtor)(void *)){
    int index = ++stack->top;
    #ifdef DEBUG
    printf("adding %p...\n",res);
    #endif
    stack->resources[index].res = res;
    stack->resources[index].dtor = dtor;
    stack->resources[index].id = id++; 
} 

void lifetime_destroy(lifetime_stack_t *lifetime){
    int i;
    for (i = lifetime->top; i >= 0; i--){
        // #if DEBUG
        // printf("<destroying>%s|%d</destroying>\n",lifetime->resources[i].res,lifetime->resources[i].id);
        // #endif
        #ifdef DEBUG
        printf("destroying %p...\n",lifetime->resources[i].res);
        #endif 
        lifetime->resources[i].dtor(lifetime->resources[i].res);
    }
    free(lifetime->resources);
    free(lifetime);
} 

lifetime_stack_t *create_stack(unsigned int capacity){
    lifetime_stack_t *stack = calloc(1,sizeof(lifetime_stack_t)); 
    if(!stack)
        return NULL;
    stack -> capacity = capacity;
    stack->top = -1; 
    stack->resources = calloc(capacity , sizeof(resource_t));
    if(!stack->resources){
        free(stack);
        return NULL;
    }
    return stack;
}

void join_wrapper(void *thread){
    pthread_t *th = (pthread_t*) thread;
    #ifdef DEBUG
    printf("Joining %lu\n",*th % 100);
    #endif
    pthread_join(*th,NULL);
    #ifdef DEBUG
    printf("Joined %lu\n",*th % 100);
    #endif
}

void fd_wrapper(void *fd){
    close(*(int*)fd);
}