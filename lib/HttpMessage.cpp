#include <string.h>

#include "HttpMessage.h"
#define S_GET "GET" 
#define S_POST "POST" 
#define EOF "\r\n" 
#define SEP " " 


char *read_request_line(struct Request *req, const char *raw){
    // Method
    size_t meth_len = strcspn(raw, SEP);
    if (memcmp(raw, S_GET, strlen(S_GET)) == 0){
        req->method = GET;
    } else if (memcmp(raw, S_POST, strlen(S_POST)) == 0){
        req->method = POST;
    } else {
        req->method = UNSUPPORTED;
    }
    raw += meth_len + 1; // move past <SP>

    // Request-URI
    size_t url_len = strcspn(raw, SEP);
    req->url = (char*) malloc(url_len + 1);
    if (!req->url) {
        return free_request(req);
    }
    memcpy(req->url, raw, url_len);
    req->url[url_len] = '\0';
    raw += url_len + 1; // move past <SP>

    // HTTP-Version
    size_t ver_len = strcspn(raw, EOF);
    req->version = (char*) malloc(ver_len + 1);
    if (!req->version){
        return free_request(req);
    }
    memcpy(req->version, raw, ver_len);
    req->version[ver_len] = '\0';
    raw += ver_len + 2; // move past <CR><LF>

    return raw
}

char *read_headers(struct Request *req, const char *raw){
    struct Header *header = NULL, *last = NULL;
    while (raw[0]!='\r' && raw[1]!='\n'){
        last = header;
        header = (Header*) malloc(sizeof(Header));
        if (!header) {
            return free_request(req);
        }

        // name
        size_t name_len = strcspn(raw, ":");
        header->name = (char*) malloc(name_len + 1);
        if (!header->name) {
            return free_request(req);
        }
        memcpy(header->name, raw, name_len);
        header->name[name_len] = '\0';
        raw += name_len + 1; // move past :
        // skips spaces
        while(isspace((unsigned char)*raw)) raw++;

        // value
        size_t value_len = strcspn(raw, EOF);
        header->value = (char*) malloc(value_len + 1);
        if (!header->value) {
            return free_request(req);
        }
        memcpy(header->value, raw, value_len);
        header->value[value_len] = '\0';
        raw += value_len + 2; // move past <CR><LF>

        // next
        header->next = last;
        // skips spaces
        while(isspace((unsigned char)*raw)) raw++;

    }
    req->headers = header;
    raw += 2; // move past <CR><LF>
}

struct Request *parse_request(const char *raw){
    struct Request *req = NULL;
    req = (Request*)malloc(sizeof(struct Request));
    if (!req) {
        return free_request(req);     
    }
    memset(req, 0, sizeof(struct Request));

    raw = read_request_line(req, raw);
    raw = read_headers(req, raw)
    // read body
    size_t body_len = strlen(raw);
    req->body =(char*)  malloc(body_len + 1);
    if (!req->body) {
        return free_request(req);
    }
    memcpy(req->body, raw, body_len);
    req->body[body_len] = '\0';

    return req;
}


void free_header(struct Header *h){
    if (h) {
        free(h->name);
        free(h->value);
        free_header(h->next);
        free(h);
    }
}


struct Request * free_request(struct Request *req) {
    free(req->url);
    free(req->version);
    free_header(req->headers);
    free(req->body);
    free(req);
    return NULL ;
}

struct Reply * free_reply(struct Reply *rep) {
    free(rep->status);
    free(rep->httpVersion);
    free_header(rep->headers);
    free(req->body);
    free(rep);
    return NULL ;
}

void print_request(struct Request *req ) {
   // struct Request *req = parse_request(request);
    if (req) {
        printf("Method: %d\n", req->method);
        printf("Request-URI: %s\n", req->url);
        printf("HTTP-Version: %s\n", req->version);
        puts("Headers:");
        struct Header *h;
        for (h=req->headers; h; h=h->next) {
            printf("%32s: %s\n", h->name, h->value);
        }
        puts("message-body:");
        if(req->method != POST)
            puts(req->body);
        else
            puts("None");
    }
}