//
// Created by student on 6/4/21.
//
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include "segel.h"
#include "clientStatic.h"

// define global constants
int NUMBER_OF_THREADS = 50; // PIAZZA: generate around 50-60 client thread at most
int NUMBER_OF_REQUESTS = 1024; // PIAZZA: each client will handle more than one request (upper bound of 1024 concurrent requests)
int MAX_THREAD_REQUESTS = 16; // each thread handles NUMBER_OF_REQUESTS/(NUMBER_OF_THREADS+1)
int FILENAME_SIZE = 50;

// enum Result {SEG_FAULT_CREAT_FILES,}; // perhaps will be easier to spot a bug in the test

/*
 * Read the HTTP response and print it out
 */
void clientPrintStatic(int fd)
{
    printf("send request for client %d\n", fd);
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
    int threadId;
    int fd;
    char **files_names;
};

/*
 * Send an HTTP request for the specified file
 */
void clientSendStatic(int fd, char *filename)
{
    char buf[MAXLINE];
    char hostname[MAXLINE];

    Gethostname(hostname, MAXLINE);

    /* Form and send the HTTP request */
    sprintf(buf, "GET %s HTTP/1.1\n", filename);
    sprintf(buf, "%shost: %s\n\r\n", buf, hostname);
    Rio_writen(fd, buf, strlen(buf));
    clientPrintStatic(fd);
    pthread_exit(0);
    //Close(fd);
}

void *clientSendStaticAux(void *parameters){
    struct thread_arguments *thread_arguments = (struct thread_arguments*)parameters;
    int fd = thread_arguments->fd;
    for (int i = 0; i < MAX_THREAD_REQUESTS; ++i) {
        clientSendStatic(fd,thread_arguments->files_names[i]);
    }
    return NULL;
}

struct thread_arguments *initThreadArgsStatic(int thread_id, int fd, char *filename[]){
    struct thread_arguments *args1 = malloc(sizeof(struct thread_arguments));
    if(args1 == NULL) {
        printf("Error Allocating argument parameters\n");
        return NULL;
    }
    args1->threadId = thread_id;
    args1->fd = fd;
    args1->files_names = malloc(MAX_THREAD_REQUESTS + 1);
    printf("died here\n");

    // each thread is responsible for files starting from (index = 20*thread_id + thread_id) -> (index + 20)
    // where thread_id = {0,1,2,...50}
    int counter = 0;
    int index = (MAX_THREAD_REQUESTS+1)*thread_id;
    for (int i = index; i <= index + MAX_THREAD_REQUESTS; ++i) {
        args1->files_names[counter] = malloc(FILENAME_SIZE);
        strcpy(args1->files_names[counter],filename[i]);
        printf("copying file %s No %d\n",filename[i],i);
        counter++;
    }

    return args1;
}

void destroyThreadArgsStatic(struct thread_arguments *args) {
    for (int i = 0; i < MAX_THREAD_REQUESTS; ++i) {
        free(args->files_names[i]);
    }
    printf("released files\n");
    free(args->files_names);
    printf("released files names array\n");
    free(args);
    printf("released args struct\n");
}

// generate different client threads
void testStatic(char * host, int port) {
    /* STATIC REQUESTS */

    // create file name array
    char* filename_array[NUMBER_OF_REQUESTS];
    //printf("died here\n");
    for (int i = 0; i < NUMBER_OF_REQUESTS; ++i) {
        filename_array[i] = malloc(FILENAME_SIZE);
    }
    //printf("died here 2\n");

    // create suffix
    char* suffix = malloc(FILENAME_SIZE);
    strcpy(suffix,".html");

    char* number_str = malloc(FILENAME_SIZE);

    for (int i = 0; i < NUMBER_OF_REQUESTS; ++i) {
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

    // create connection Fd's
    int ConnFd[NUMBER_OF_THREADS];

    for (int i = 0; i < NUMBER_OF_THREADS; ++i) {
        ConnFd[i] = Open_clientfd(host, port);
    }

    //printf("created Connection Fd's array\n");

    // create parameters for clientSend function
    struct thread_arguments* arguments_array[NUMBER_OF_THREADS];
    // -2 since there are 1200 requests, 50 threads -> each thread handles about 24 req.
    // thread No 47 handles request No. 47*25 + 24 = 1199 => after 1200 we get SIGSEGV
    // problem arises since threads handle disjoint set of requests, and I'm not really good at math so I
    // chose the numbers of the constant above arbitrarily and just made sure I don't touch places in memory which doesn't belong to me
    for (int i = 0; i < NUMBER_OF_THREADS; ++i) {
        arguments_array[i] = initThreadArgsStatic(i,ConnFd[i],filename_array);
    }

    printf("created parameters to function Client Send array\n");

    // create threads
    pthread_t threads[NUMBER_OF_THREADS];
    for (unsigned int i=0; i<NUMBER_OF_THREADS; ++i)
        pthread_create(&threads[i], NULL, clientSendStaticAux, (void*)arguments_array[i]);

    printf("created threads array\n");

    // wait for threads to finish
    for (unsigned int i=0; i<NUMBER_OF_THREADS; i++)
        pthread_join(threads[i], NULL);

    //printf("finished join\n");

    for (int i = 0; i < NUMBER_OF_THREADS; ++i) {
        //printf("closing connfd = %d\n", ConnFd[i]);
        Close(ConnFd[i]);
    }

    printf("closed connections\n");

    // I have double free for some reason, and I can't find it, which is extremely ANNOYING.
    for (int i = 0; i < NUMBER_OF_THREADS; ++i) {
        //destroyThreadArgsStatic(arguments_array[i]);
    }

    //printf("finished closing client Fd's\n");

}
