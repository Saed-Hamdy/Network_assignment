#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include<netinet/in.h>
#include <arpa/inet.h>
#include<pthread.h>
#include <string.h>
#include "../lib/HttpMessage.h"
#include "requestHandler.h"

#define DEFAULT_PORT 80
#define LOCAL_HOST "127.0.0.1"
#define MAX_CONNECTIONS 50


void * socket_thread(void *arg)
{
    char client_message[2000];
    int socketID = *((int *)arg);
    // while(true)
    // { 
        if(recv(socketID, client_message, 2000, 0) < 0)
        {
            printf("Failed to Receive Request\n");
            // break;
        }
        struct Request *req = parse_request(client_message);
        printf("................. request recived: %s\n", req->url);
        // print_request(req);
        if(req->method == Method::GET)
            handle_get_request(socketID, req);
        else if(req->method == Method::POST)
            handle_post_request(socketID, req);
        else{
            printf("Unsupported request\n");
        }
        free(req);
    // }
    close(socketID);
    printf("Exit socket thread\n");
    pthread_exit(NULL);
}


int main(int argc, char *argv[])
{
    // handle arguments
    long port_number = DEFAULT_PORT;
    if(argc > 2){
        printf("Too many arguments, at maximum one argument is expected: {port number}\n");
        return 1;
    } else if(argc > 1)
        port_number = atol(argv[1]);
    
    int serverSocket, newSocket;
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size;
    //Create the socket. 
    serverSocket = socket(PF_INET, SOCK_STREAM, 0);
    // Configure settings of the server address struct
    // Address family = Internet 
    serverAddr.sin_family = AF_INET;
    // set port number, using htons function to use proper byte order 
    serverAddr.sin_port = htons(port_number);
    //Set IP address to localhost 
    serverAddr.sin_addr.s_addr = inet_addr(LOCAL_HOST);
    //Set all bits of the padding field to 0 
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
    //Bind the address struct to the socket 
    bind(serverSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
    //Listen on the socket, with N max connection requests queued 
    if(listen(serverSocket, MAX_CONNECTIONS) == 0)
        printf("Listening\n");
    else
        printf("Error, can't listen to port\n");
    
    pthread_t tid[60];
    int i = 0;
    while(true)
    {
        //Accept call creates a new socket for the incoming connection
        if( i > MAX_CONNECTIONS)
        {
          i = 0;
          while(i < MAX_CONNECTIONS)
          {
            pthread_join(tid[i++],NULL);
          }
          i = 0;
        }
        addr_size = sizeof(serverStorage);
        newSocket = accept(serverSocket, (struct sockaddr *) &serverStorage, &addr_size);
        // for each client request, create a thread and assign the client request to it
        // By doing so, the main thread can entertain next requests.
        if(pthread_create(&tid[i++], NULL, socket_thread, &newSocket) != 0 )
           printf("Failed to create thread\n");
        
    }
    return 0;
}
