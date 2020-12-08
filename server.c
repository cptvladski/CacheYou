#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <stdlib.h>
#include <memory.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <stdarg.h>
#include <json.h>
#include <uuid/uuid.h>
#include <pthread.h>
#include "c_hashmap/hashmap.h"
#define PORT 1234

map_t cache;
struct json_tokener *tokener;

typedef struct argP{
    char *buffer;
    int fd;
}args_t;

//worker thread for adding entries to cache
void *putWorkerThread(void *arg){
    args_t args = *(args_t*)arg;
    int new_socket = args.fd;
    char *buffer = strdup(args.buffer);
    printf("inserting... %s\n",buffer);
    buffer[strlen(buffer) - 1] = '\0';
    //parse json
    struct json_object* object = json_tokener_parse_ex(tokener,buffer,strlen(buffer));
    if(object == NULL){
        printf("bad json");
        fflush(stdout);
        return NULL;
    }
    uuid_t binuuid;
    uuid_generate_random(binuuid);
    char * uuid = malloc(37);
    uuid_unparse_lower(binuuid,uuid);
    printf("%s\n",uuid);
    char *value = strdup(buffer);
    hashmap_put(cache,uuid,value);
    send(new_socket , uuid , strlen(uuid) , 0 );
    char * obj;
    //validate put
    hashmap_get(cache,uuid,(void**)&obj);
    printf("%s\n",obj);
    return NULL;
}

//worker thread for retrieving entries from cache
void *getWorkerThread(void *arg){
    args_t args = *(args_t*)arg;
    int new_socket = args.fd;
    char *buffer = strdup(args.buffer);
    uuid_t binuuid;
    buffer[36] = '\0';
    printf("retrieving...[last %d][len %lu]%s => %d\n",buffer[strlen(buffer)],strlen(buffer),buffer,uuid_parse(buffer,binuuid));
    //parse uuid
    if(uuid_parse(buffer,binuuid) ==-1){
        printf("bad uuid\n");
        fflush(stdout);
        return NULL;
    }
    printf("good uuid\n");
    fflush(stdout);
    char *uuid = malloc(37);
    uuid_unparse_lower(binuuid,uuid);
    char *obj;
    hashmap_get(cache,uuid,(void**)&obj);
    if(obj == NULL){
        printf("no entry found\n");
        fflush(stdout);
        return NULL;
    }
    send(new_socket , obj , strlen(obj) , 0 );
    return NULL;
}
int main(int argc,char **argv){
    cache = hashmap_new();
    tokener = json_tokener_new(); 
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;
    int new_socket;
    int addrlen = sizeof(struct sockaddr_in);
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 
       
    // Forcefully attaching socket to the port 1234
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons( PORT ); 
    //bind socket to port
    if (bind(server_fd, (struct sockaddr *)&address,sizeof(address))<0) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
    //listen for connection on port
    if (listen(server_fd, 1) < 0) 
    { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    } 
    
    
    while(1){
        char buffer[1024];
        // accept new connection
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,(socklen_t*)&addrlen))<0) 
        { 
            perror("accept"); 
            exit(EXIT_FAILURE); 
        }
        int read_bytes = 0;
        int bytes = 0;
        //read from the socket
        while((bytes = read(new_socket+read_bytes, buffer, 1024)) > 0){
            printf("%d\n",bytes);
            read_bytes += bytes;
        }
        printf("got buffer: %s\n",buffer);
        pthread_t tid;
        args_t arg;
        arg.fd = new_socket;
        arg.buffer = buffer;
        //it's just json
        if(*buffer == '{'){
            pthread_create(&tid,NULL,putWorkerThread,(void*) &arg);
            // printf("inserting...\n");
            // buffer[strlen(buffer) - 1] = '\0';
            // //parse json
            // struct json_object* object = json_tokener_parse_ex(tokener,buffer,strlen(buffer));
            // if(object == NULL){
            //     printf("bad json");
            //     fflush(stdout);
            //     continue;
            // }
            // uuid_t binuuid;
            // uuid_generate_random(binuuid);
            // char * uuid = malloc(37);
            // uuid_unparse_lower(binuuid,uuid);
            // printf("%s\n",uuid);
            // char *value = strdup(buffer);
            // hashmap_put(cache,uuid,value);
            // send(new_socket , uuid , strlen(uuid) , 0 );
            // char * obj;
            // //validate put
            // hashmap_get(cache,uuid,(void**)&obj);
            // printf("%s\n",obj);
        }
        //it's just a uuid
        else{
            pthread_create(&tid,NULL,getWorkerThread,(void*) &arg);
            // uuid_t binuuid;
            // buffer[36] = '\0';
            // printf("retrieving...[last %d][len %d]%s => %d\n",buffer[strlen(buffer)],strlen(buffer),buffer,uuid_parse(buffer,binuuid));
            // //parse uuid
            // if(uuid_parse(buffer,binuuid) ==-1){
            //     printf("bad uuid\n");
            //     fflush(stdout);
            //     continue;
            // }
            // printf("good uuid\n");
            // fflush(stdout);
            // char *uuid = malloc(37);
            // uuid_unparse_lower(binuuid,uuid);
            // char *obj;
            // hashmap_get(cache,uuid,(void**)&obj);
            // if(obj == NULL){
            //     printf("no entry found\n");
            //     fflush(stdout);
            //     continue;
            // }
            // send(new_socket , obj , strlen(obj) , 0 );
            
        }
    }
    json_tokener_free(tokener);
    hashmap_free(cache);
    return 0;
}