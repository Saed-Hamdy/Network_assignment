#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include <arpa/inet.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include<pthread.h>
#include "lib.h"

#define NOTFOUND "HTTP/1.0 404 Not Found\r\n"
#define OK_MESSAGE "HTTP/1.0 200 OK\r\n"


char client_message[2000];
char buffer[1024];
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void print_request(struct Request *req );

int send_file(int soc, const char *fn)
{
    char * fBuffer ,*buf;
    FILE *file;
    long fSize;
    int err = 0;
	printf("%s\n", fn);
    if (!(file = fopen(fn, "rb"))) {
        perror("fopen");
        // char str3[] ="HTTP/1.0 404 Not Found\r\n";
        send(soc,NOTFOUND,strlen(NOTFOUND),0);
        return 1;
    }
    fseek(file, 0L, SEEK_END);
    fSize = ftell(file);
    fseek(file, 0L, SEEK_SET);

    /* Consider adding Date + Server here. */
    buf=(char*) malloc (100);
    sprintf(buf,
            "HTTP/1.0 200 OK\r\n"
            "Content-length: %ld\r\n"
            "Content-Type: text/html/image\r\n"
            "\r\n", fSize
    );
    if (strlen(buf) < 0) {
        err = 3;
        printf("Error writing header.\n");
        fclose (file);
		free (fBuffer);
		// @ToDo
		//return error ;
    }
	// allocate memory to contain the whole file:
	fBuffer = (char*) malloc (sizeof(char)*fSize);
	if (fBuffer == NULL) {fputs ("Memory error",stderr);printf("Memory error........................\n"); }

	// copy the file into the fBuffer:
	// unsigned char info[54] ;
	size_t result = fread (fBuffer,sizeof(char),fSize,file);
	// int w = *(int*)&info[18];
	// int h = *(int*)&info[22];
	
	printf("%d----%d---------------------------------------\n",result,fSize );
	if (result != fSize) {fputs ("Reading error",stderr); printf("Reading error........................\n"); }
	int n_size=1 + strlen(buf)+ fSize;
	char * str3 = (char *) malloc( n_size);

	strcpy(str3, buf);
	memcpy(str3+strlen(buf),fBuffer,fSize);
	//strcat(str3, fBuffer);
	 printf("%d  :  %d\n", strlen(str3),n_size);

	/* the whole file is now loaded in the memory fBuffer. */
	// terminate
	fclose (file);
	free (fBuffer);
	pthread_mutex_lock(&lock);
	//sleep(1);
	int send_size =send(soc,str3,n_size,0);
	printf("send size =    %d  ;\n",send_size );
	pthread_mutex_unlock(&lock);
	    /* Debug print. */
	printf("response:\n'%s'\n", str3);
	printf("%d  :  %d\n", strlen(str3),n_size);
}
void * socketThread(void *arg)
{

 printf("in thread \n");
 int newSocket = *((int *)arg);

 // while(1){ 
  if(recv(newSocket, client_message, 2000, 0) < 0)
    {
       printf("Receive failed\n");
    }
    printf("reciveeee..............\n");
    //Print the received message
   printf("Data received: %s\n",client_message);
   struct Request *req = parse_request(client_message);
   print_request(req);
   if (req->method == GET) send_file(newSocket,req->url+1);
   else if(req->method == POST)
   {
   	send(newSocket,OK_MESSAGE,strlen(OK_MESSAGE),0);
   //	memset(client_message, '\0', sizeof client_message);
   	if(recv(newSocket, client_message, 2000, 0) < 0)
    {
       printf("Receive failed\n");
    }
	int len, remain_data, file_size = atoi(client_message);
	FILE *received_file = fopen(req->url+1, "w");
	if (received_file == NULL)
	{
	    printf("Failed to open file foo --> \n");
	    // exit(EXIT_FAILURE);
	}
	remain_data = file_size;
	while (((len = recv(newSocket, buffer, 2000, 0)) > 0) && (remain_data > 0))
	{
	    printf("in\n");
	    fwrite(buffer, sizeof(char), len, received_file);
	    remain_data -= len;
	    printf("Receive %d bytes and we hope :- %d bytes\n", len, remain_data);
	}
	fclose(received_file);
   }
   //memset(client_message, '\0', sizeof client_message);
   free(req);
 // }
  // Send message to the client socket 
  // char *message = (char *)malloc(sizeof(client_message)+20);
  // strcpy(message,"Hello Client : ");
  // strcat(message,client_message);
  // strcat(message,"\n");
  // strcpy(buffer,message);
  // free(message);
  // pthread_mutex_unlock(&lock);
  // sleep(1);
  // send(newSocket,buffer,strlen(buffer),0);
  close(newSocket);
  printf("Exit socketThread \n");
  pthread_exit(NULL);
}


int main(int argc, char *argv[])
{
  int serverSocket, newSocket;
  struct sockaddr_in serverAddr;
  struct sockaddr_storage serverStorage;
  socklen_t addr_size;
  //Create the socket. 
  serverSocket = socket(PF_INET, SOCK_STREAM, 0);
  // Configure settings of the server address struct
  // Address family = Internet 
  serverAddr.sin_family = AF_INET;
  //Set port number, using htons function to use proper byte order 
  if(argc>1)
  	serverAddr.sin_port = htons(atoi(argv[1]));
  else
  serverAddr.sin_port = htons(7799);
  //Set IP address to localhost 
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  //Set all bits of the padding field to 0 
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
  //Bind the address struct to the socket 
  bind(serverSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
  //Listen on the socket, with 40 max connection requests queued 
  	if(listen(serverSocket,50)==0)
    	printf("Listening\n");
  	else printf("Error\n");
    pthread_t tid[60];
    int i = 0;


    while(1)
    {
        //Accept call creates a new socket for the incoming connection
        if( i > 50)
        {
          i = 0;
          while(i < 50)
          {
          	 printf(" A :%d:\n",i);
           	 pthread_join(tid[i++],NULL);
             printf(" B :%d:\n",i);
          }
          i = 0;
        }
        printf("a:   %d\n ",i );
        addr_size = sizeof serverStorage;
        printf("b:  %d\n",i);
        newSocket = accept(serverSocket, (struct sockaddr *) &serverStorage, &addr_size);
        printf("c:  %d\n",i);
        //for each client request creates a thread and assign the client request to it to process
       //so the main thread can entertain next request
        if( pthread_create(&tid[i++], NULL, socketThread, &newSocket) != 0 )
           printf("Failed to create thread\n");
        
    }
  return 0;
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
    }
}