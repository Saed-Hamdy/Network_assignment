#include<stdlib.h>
#include<sys/stat.h>
#include "requestHandler.h"
#include "/lib/fileHandler.h"

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;


struct Reply *handle_post_request(int socketID, struct Request* req){
    print_request(req);
    struct Reply *rep = NULL;
    rep = (Reply*) malloc(sizeof(struct Reply));
    if (!rep) {
        return free_reply(rep);     
    }
    memset(rep, 0, sizeof(struct Reply));
    rep->httpVersion = req->version
    rep->status = Status.OK;

}

struct Reply * handle_get_request(int socketID, struct Request* req){
    print_request(req);
    struct Reply *rep = NULL;
    rep = (Reply*) malloc(sizeof(struct Reply));
    if (!rep) {
        return free_reply(rep);     
    }
    memset(rep, 0, sizeof(struct Reply));
    rep->httpVersion = req->version

    if(!file_exists(req->url)){
        rep->status = Status.NOTFOUND;
        return rep;
    }
    rep->status = Status.OK;
    // send required file
    send_file(socketID, req->url)

}



int send_file(int socket, const char *fn)
{
    char * fBuffer *buf;
    FILE *file;
    long fSize;
    int err = 0;
    printf("%s\n", fn);
    if (!(file = fopen(fn, "r"))) {
        perror("fopen");
        return 1;
    }
    fseek(file, 0L, SEEK_END);
    fSize = ftell(file);
    fseek(file, 0L, SEEK_SET);

    /* Consider adding Date + Server here. */
    char buffer[1024];
    buf = (char*) malloc (100);
    sprintf(buf,
            "HTTP/1.1 200 OK\r\n"
            "Content-length: %ld\r\n"
            "Content-Type: text/html\r\n"
            "\r\n", fSize
    );
    len = fSize
    if (len < 0) {
        err = 3;
        printf("Error writing header.\n");
        fclose (file);
        free (fBuffer);
        // @ToDo
        //return error ;
    }
    // allocate memory to contain the whole file:
    fBuffer = (char*) malloc (sizeof(char)*fSize);
    if (fBuffer == NULL) {fputs ("Memory error",stderr); exit (2);}

    // copy the file into the fBuffer:
    size_t result = fread (fBuffer,1,fSize,file);
    if (result != fSize) {fputs ("Reading error",stderr); exit (3);}
    int n_size=1 + strlen(buf)+ strlen(fBuffer);
    char * str3 = (char *) malloc( n_size);
    strcpy(str3, buf);
    strcat(str3, fBuffer);
    printf("%d  :  %d\n", strlen(str3),n_size);

    /* the whole file is now loaded in the memory fBuffer. */
    // terminate
    fclose (file);
    free (fBuffer);
    pthread_mutex_lock(&lock);
    send(soc,str3,n_size,0);
    pthread_mutex_unlock(&lock);
        /* Debug print. */
    printf("response:\n'%s'\n", str3);
    printf("%d  :  %d\n", strlen(str3),n_size);
}
