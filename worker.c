#include "worker.h"
#include <json.h>

extern map_t cache;
extern struct json_tokener *tokener;

void *workerThread(void *arg){
    char buffer[BUFFSIZE];
    int new_socket = (*(args_t*)arg).fd;
    char *self = get_self();
    int read_bytes = 0;
    int bytes = 0;
    while((bytes = read(new_socket+read_bytes, buffer, BUFFSIZE - read_bytes)) > 0){
        printf("[%s]read %d bytes from socket\n",self,bytes);
        read_bytes += bytes;
    }
    printf("[%s]got buffer: %s\n",self,buffer);
    //it's just json
    if(isJson(buffer)){
        printf("[%s]inserting... %s\n",self,buffer);
        buffer[strlen(buffer) - 1] = '\0';
        //parse json
        struct json_object* object = json_tokener_parse_ex(tokener,buffer,strlen(buffer));
        if(object == NULL){
            printf("[%s]bad json\n",self);
            fflush(stdout);
            return NULL;
        }
        char * uuid = randomUUID();
        printf("[%s] generated %s\n",self,uuid);
        char *value = strdup(buffer);
        hashmap_put(cache,uuid,value);
        send(new_socket , uuid , strlen(uuid) , 0 );
        char * obj;
        //validate put
        hashmap_get(cache,uuid,(void**)&obj);
        printf("[%s] inserted %s\n",self,obj);
    }
    //it's just a uuid
    else{
        uuid_t binuuid;
        buffer[36] = '\0';
        printf("[%s]retrieving...[last %d][len %lu]%s => %d\n",self,buffer[strlen(buffer)],strlen(buffer),buffer,uuid_parse(buffer,binuuid));
        //parse uuid
        if(uuid_parse(buffer,binuuid) ==-1){
            printf("[%s]bad uuid\n",self);
            fflush(stdout);
            return NULL;
        }
        printf("[%s]good uuid\n",self);
        fflush(stdout);
        char *uuid = malloc(37);
        uuid_unparse_lower(binuuid,uuid);
        char *obj;
        hashmap_get(cache,uuid,(void**)&obj);
        if(obj == NULL){
            printf("[%s]no entry found\n",self);
            fflush(stdout);
            return NULL;
        }
        send(new_socket , obj , strlen(obj) , 0 );
    }
    fflush(stdout);
    return NULL;
}