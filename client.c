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
struct thread_arguments{
    int fd;
    char *filename;
};

/*
 * Send an HTTP request for the specified file
 */
void * clientSend(void *parameters)
{
    struct thread_arguments *thread_arguments = (struct thread_arguments*)parameters;
    int fd = thread_arguments->fd;

    //printf("thread number = %ld prints connfd = %d\n", pthread_self(),fd);

    char *filename = thread_arguments->filename;


    char buf[MAXLINE];
    char hostname[MAXLINE];

    Gethostname(hostname, MAXLINE);

    /* Form and send the HTTP request */
    sprintf(buf, "GET %s HTTP/1.1\n", filename);
    sprintf(buf, "%shost: %s\n\r\n", buf, hostname);
    Rio_writen(fd, buf, strlen(buf));
    clientPrint(fd);
    pthread_exit(0);
    //Close(fd);
}



struct thread_arguments *initThreadArgs(int fd, char *filename){
    struct thread_arguments *args1 = malloc(sizeof(struct thread_arguments));
    args1->fd = fd;
    args1->filename = filename;
    return args1;
}

void destroyThreadArgs(struct thread_arguments *args) {
    free(args);
}


int main(int argc, char *argv[])
{
  char *host; //, *filename;
  int port;
  //int clientfd;

  if (argc != 4) {
    fprintf(stderr, "Usage: %s <host> <port> <filename>\n", argv[0]);
    exit(1);
  }

  host = argv[1];
  port = atoi(argv[2]);
  //filename = argv[3];

  // initialize variables
  int array_size = 10;
  int filename_size = 50;

  //printf("initialize arguments\n");

  // create file name array
  char* filename_array[array_size];
  //printf("died here\n");
  for (int i = 0; i < array_size; ++i) {
      filename_array[i] = malloc(filename_size);
  }
  //printf("died here 2\n");

  // create suffix
  char* suffix = malloc(filename_size);
  strcpy(suffix,".html");

  char* number_str = malloc(filename_size);

  for (int i = 0; i < array_size; ++i) {
      // convert file name number into a string
      sprintf(number_str, "%d", i);
      //printf("died here 3\n");

      // concatenate all this mess
      strcpy(filename_array[i],"home");
      //printf("died here 4\n");

      strcat(filename_array[i],number_str);
      //printf("died here 5\n");
      strcat(filename_array[i],suffix);
      //printf("died here 6\n");
  }

  //printf("created files array\n");

  // create connection Fd's - should malloc?
  int ConnFd[array_size];

  for (int i = 0; i < array_size; ++i) {
      ConnFd[i] = Open_clientfd(host, port);
  }

  //printf("created Connection Fd's array\n");

  // create parameters for clientSend function - should malloc?
  struct thread_arguments* arguments_array[array_size];
  for (int i = 0; i < array_size; ++i) {
      arguments_array[i] = initThreadArgs(ConnFd[i],filename_array[i]);
  }

  //printf("created parameters to function Client Send array\n");

  // create threads
  pthread_t threads[array_size];
  for (unsigned int i=0; i<array_size; i++)
      pthread_create(&threads[i], NULL, clientSend, (void*)arguments_array[i]);

  //printf("created threads array\n");

  // wait for threads to finish
  for (unsigned int i=0; i<array_size; i++)
      pthread_join(threads[i], NULL);

  //printf("finished join\n");

  for (int i = 0; i < array_size; ++i) {
      //printf("closing connfd = %d\n", ConnFd[i]);
      Close(ConnFd[i]);
  }

  //printf("finished closing client Fd's\n");
  exit(0);
}
