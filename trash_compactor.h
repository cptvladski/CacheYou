#define BUFSIZE 256


typedef struct resource {
    void *res;
    void (*dtor)(void *);
    int id;
} resource_t;

typedef struct stack {
    int top;
    unsigned int capacity;
    resource_t *resources;
} lifetime_stack_t;

void res_add(lifetime_stack_t *stack, void *res, void (*dtor)(void *));

lifetime_stack_t *create_stack(unsigned int capacity);


void lifetime_destroy(lifetime_stack_t *lifetime);

void join_wrapper(void *thread);

void fd_wrapper(void *fd);