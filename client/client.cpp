#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include<pthread.h>
#include "/lib/fileHandler.h"
#include "/lib/HttpMessage.h"

 char *raw_request = "GET /sample.txt HTTP/1.1\r\n"
            "Host: localhost:8080\r\n"
            "Connection: keep-alive\r\n"
            "Upgrade-Insecure-Requests: 1\r\n"
            "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
            "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_13_3) AppleWebKit/604.5.6 (KHTML, like Gecko) Version/11.0.3 Safari/604.5.6\r\n"
            "Accept-Language: en-us\r\n"
            "DNT: 1\r\n"
            "Accept-Encoding: gzip, deflate\r\n"
            "\r\n"
            "Usually GET requests don\'t have a body\r\n"
            "But I don\'t care in this case :)";


std::vector<char> read_request_file(char *file_name){
    std::vector<char> data = file_read(file_name);
    return data
}

void * client_thread(void *arg){
    printf("In thread\n");
    char message[1000];
    char buffer[1024];
    int clientSocket;
    struct sockaddr_in serverAddr;
    socklen_t addr_size;
    // Create the socket. 
    if ((clientSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
      printf(" socket () failed \n") ;

    //Configure settings of the server address
    // Address family is Internet 
    serverAddr.sin_family = AF_INET;
    //Set port number, using htons function 
    serverAddr.sin_port = htons(7799);
    //Set IP address to localhost
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
    //Connect the socket to the server using the address
    addr_size = sizeof(serverAddr);
    if (connect(clientSocket, (struct sockaddr *) &serverAddr,addr_size) < 0)
        printf("Connection failed !!!!! \n") ;

    printf("Connected..........................................\n") ;
    // connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size);
    strcpy(message, raw_request);
    send(clientSocket, message, strlen(message), 0) ;
    //Read the message from the server into the buffer
    recv(clientSocket, buffer, 1024, 0) ;
    //Print the received message
    printf("Data received:\n %s\n", buffer);
    close(clientSocket);
    printf("Client Socket is closed\n" );
    pthread_exit(NULL);
}


int main(){
    int i = 50;
    //int arg[];
    client_thread(NULL);
    // pthread_t tid[51];
    // while(i< 50)
    // {
    //   if( pthread_create(&tid[i], NULL, cientThread, NULL) != 0 )
    //          printf("Failed to create thread\n");
    //   i++;
    // }
    // sleep(5);
    // // i = 1;
    //  while(i>0)
    //  {
    //   pthread_join(tid[--i],NULL);
    //     printf("%d:\n",i);
    //  }
    return 0;
}
