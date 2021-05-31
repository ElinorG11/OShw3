/*
 * client.c: A very, very primitive HTTP client.
 * 
 * To run, try: 
 *      ./client www.cs.technion.ac.il 80 /
 *
 * Sends one HTTP request to the specified HTTP server.
 * Prints out the HTTP response.
 *
 * HW3: For testing your server, you will want to modify this client.  
 * For example:
 * 
 * You may want to make this multi-threaded so that you can 
 * send many requests simultaneously to the server.
 *
 * You may also want to be able to request different URIs; 
 * you may want to get more URIs from the command line 
 * or read the list from a file. 
 *
 * When we test your server, we will be using modifications to this client.
 *
 */
#include <pthread.h>
#include <string.h>
#include "segel.h"


/*
 * Read the HTTP response and print it out
 */
void clientPrint(int fd)
{
  rio_t rio;
  char buf[MAXBUF];  
  int length = 0;
  int n;
  
  Rio_readinitb(&rio, fd);

  /* Read and display the HTTP Header */
  n = Rio_readlineb(&rio, buf, MAXBUF);
  while (strcmp(buf, "\r\n") && (n > 0)) {
    printf("Header: %s", buf);
    n = Rio_readlineb(&rio, buf, MAXBUF);

    /* If you want to look for certain HTTP tags... */
    if (sscanf(buf, "Content-Length: %d ", &length) == 1) {
      printf("Length = %d\n", length);
    }
  }

  /* Read and display the HTTP Body */
  n = Rio_readlineb(&rio, buf, MAXBUF);
  while (n > 0) {
    printf("%s", buf);
    n = Rio_readlineb(&rio, buf, MAXBUF);
  }
}

/*
 * Send an HTTP request for the specified file
 */
void * clientSend(void *parameters)
{
    int fd = *(int*)(parameters);
    printf("fd = %d\n",fd);
    char *filename = (char*)(parameters+sizeof(int));
    printf("filename = %s\n",filename);
    char buf[MAXLINE];
    char hostname[MAXLINE];

    Gethostname(hostname, MAXLINE);

    /* Form and send the HTTP request */
    sprintf(buf, "GET %s HTTP/1.1\n", filename);
    sprintf(buf, "%shost: %s\n\r\n", buf, hostname);
    Rio_writen(fd, buf, strlen(buf));
    clientPrint(fd);
    Close(fd);
}

struct args{
    int fd;
    char *filename;
};

struct args *initArgs(int fd, char *filename){
    struct args *args1 = malloc(sizeof(struct args));
    args1->fd = fd;
    args1->filename = filename;
    return args1;
}

void destroy(struct args *args) {
    free(args);
}

int main(int argc, char *argv[])
{
  char *host, *filename;
  int port;
  int clientfd;

  if (argc != 4) {
    fprintf(stderr, "Usage: %s <host> <port> <filename>\n", argv[0]);
    exit(1);
  }

  host = argv[1];
  port = atoi(argv[2]);
  filename = argv[3];
  int N = 30;
  char* filename2 = malloc(N);
    strcpy(filename2,"home2.html");
    char* filename3 = malloc(N);
    strcpy(filename3,"home3.html");

    char* filename4 = malloc(N);
    strcpy(filename4,"home4.html");

    char* filename5 = malloc(N);
    strcpy(filename5,"home5.html");

    char* filename6 = malloc(N);
    strcpy(filename6,"home6.html");

    char* filename7 = malloc(N);
    strcpy(filename7,"home7.html");

    char* filename8 = malloc(N);
    strcpy(filename8,"home8.html");

  int thread_count = 8;
  pthread_t threads[thread_count];

    /* Open a single connection to the specified host and port */
    clientfd = Open_clientfd(host, port);
    int clientfd1 = Open_clientfd(host, port);
    int clientfd2 = Open_clientfd(host, port);
    int clientfd3 = Open_clientfd(host, port);
    int clientfd4 = Open_clientfd(host, port);
    int clientfd5 = Open_clientfd(host, port);
    int clientfd6 = Open_clientfd(host, port);
    int clientfd7 = Open_clientfd(host, port);

    struct args* params = initArgs(clientfd,filename);
    struct args* params1 = initArgs(clientfd1,filename2);
    struct args* params2 = initArgs(clientfd2,filename3);
    struct args* params3 = initArgs(clientfd3,filename4);
    struct args* params4 = initArgs(clientfd4,filename5);
    struct args* params5 = initArgs(clientfd5,filename6);
    struct args* params6 = initArgs(clientfd6,filename7);
    struct args* params7 = initArgs(clientfd7,filename8);

    pthread_create(&threads[0], NULL, clientSend, (void*)params);
    pthread_create(&threads[1], NULL, clientSend, (void*)params1);
    pthread_create(&threads[2], NULL, clientSend, (void*)params2);
    pthread_create(&threads[3], NULL, clientSend, (void*)params3);
    pthread_create(&threads[4], NULL, clientSend, (void*)params4);
    pthread_create(&threads[5], NULL, clientSend, (void*)params5);
    pthread_create(&threads[6], NULL, clientSend, (void*)params6);
    pthread_create(&threads[7], NULL, clientSend, (void*)params7);

  for (unsigned int i=0; i<thread_count; i++)
      pthread_join(threads[i], NULL);

  exit(0);
}
