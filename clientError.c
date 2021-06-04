//
// Created by student on 6/4/21.
//

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include "segel.h"
#include "clientError.h"

// enum Result {SEG_FAULT_CREAT_FILES,}; // perhaps will be easier to spot a bug in the test

/*
 * Read the HTTP response and print it out
 */
void clientPrintError(int fd)
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
    char *path;
};

/*
 * Send an HTTP request for the specified file - Error 501 Not Implemented (PUT instead of GET on line 70)
 */
void * clientSend501(void *parameters)
{
    struct thread_arguments *thread_arguments = (struct thread_arguments*)parameters;
    int fd = thread_arguments->fd;

    //printf("thread number = %ld prints connfd = %d\n", pthread_self(),fd);

    char *filename = thread_arguments->filename;


    char buf[MAXLINE];
    char hostname[MAXLINE];

    Gethostname(hostname, MAXLINE);

    /* Form and send the HTTP request */
    sprintf(buf, "PUT %s HTTP/1.1\n", filename);
    sprintf(buf, "%shost: %s\n\r\n", buf, hostname);
    Rio_writen(fd, buf, strlen(buf));
    clientPrintError(fd);
    pthread_exit(0);
    //Close(fd);
}

// file does not exist
void * clientSend404(void *parameters)
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
    clientPrintError(fd);
    pthread_exit(0);
    //Close(fd);
}

// bad permissions for dynamic cgi
void * clientSend403(void *parameters)
{
    struct stat fs;
    int r;

    struct thread_arguments *thread_arguments = (struct thread_arguments*)parameters;
    int fd = thread_arguments->fd;

    //printf("thread number = %ld prints connfd = %d\n", pthread_self(),fd);

    char *filename = thread_arguments->filename;


    r = stat(thread_arguments->path,&fs);
    if( r==-1)
    {
        fprintf(stderr,"Error reading '%s'\n",filename);
        exit(1);
    }

    r = chmod(thread_arguments->path, S_IRUSR );
    if( r!=0)
    {
        fprintf(stderr,"Unable to reset permissions on '%s'\n",filename);
        exit(1);
    }


    char buf[MAXLINE];
    char hostname[MAXLINE];

    Gethostname(hostname, MAXLINE);

    /* Form and send the HTTP request */
    sprintf(buf, "GET %s HTTP/1.1\n", filename);
    sprintf(buf, "%shost: %s\n\r\n", buf, hostname);
    Rio_writen(fd, buf, strlen(buf));
    clientPrintError(fd);
    pthread_exit(0);
    //Close(fd);
}

struct thread_arguments *initThreadArgsError(int fd, char *filename,char *filepath){
    struct thread_arguments *args1 = malloc(sizeof(struct thread_arguments));
    args1->fd = fd;
    args1->filename = filename;
    args1->path = filepath;
    return args1;
}

void destroyThreadArgsError(struct thread_arguments *args) {
    free(args);
}

void testError(char * host, int port) {
    // initialize variables
    int array_size = 3;

    // create connection Fd's
    int ConnFd[array_size];

    for (int i = 0; i < array_size; ++i) {
        ConnFd[i] = Open_clientfd(host, port);
    }

    //prepare argument for each thread
    struct thread_arguments* t0_args = initThreadArgsError(ConnFd[0],"output0.cgi","./public/output0.cgi");
    struct thread_arguments* t1_args = initThreadArgsError(ConnFd[1],"nofile.html",NULL);
    struct thread_arguments* t2_args = initThreadArgsError(ConnFd[2],"home0.html",NULL);

    // create threads
    pthread_t threads[2];

    pthread_create(&threads[2], NULL, clientSend501, (void*)t2_args);
    pthread_create(&threads[1], NULL, clientSend404, (void*)t1_args);
    pthread_create(&threads[0], NULL, clientSend403, (void*)t0_args);

    // wait for threads to finish
    for (unsigned int i=0; i<array_size; i++)
        pthread_join(threads[i], NULL);

    //printf("finished join\n");

    for (int i = 0; i < array_size; ++i) {
        //printf("closing connfd = %d\n", ConnFd[i]);
        Close(ConnFd[i]);
    }
    return;
}

void testError2(char * host, int port) {
    // generate different client threads count
    int NUMBER_OF_TESTS = 3;
    int reqCount[NUMBER_OF_TESTS];
    for(int i=0; i<NUMBER_OF_TESTS; i++) {
        reqCount[i] = i+3;
    }

    for (int j = 0; j < NUMBER_OF_TESTS; ++j) {
        // initialize variables
        int array_size = reqCount[j];
        int filename_size = 50;

        //printf("initialize arguments\n");

        // create file name array
        char* filename_array[array_size];
        char* filepath_array[array_size];
        //printf("died here\n");
        for (int i = 0; i < array_size; ++i) {
            filename_array[i] = malloc(filename_size);
            filepath_array[i] = malloc(filename_size);
        }
        //printf("died here 2\n");

        // create suffix
        char* suffix = malloc(filename_size);
        strcpy(suffix,".cgi");

        char* number_str = malloc(filename_size);

        for (int i = 0; i < array_size; ++i) {
            // convert file name number into a string
            sprintf(number_str, "%d", i);
            //printf("died here 3\n");

            // concatenate all this mess
            strcpy(filename_array[i],"output");
            strcpy(filepath_array[i],"./public/output");
            //printf("died here 4\n");

            strcat(filename_array[i],number_str);
            strcat(filepath_array[i],number_str);
            //printf("died here 5\n");
            strcat(filename_array[i],suffix);
            strcat(filepath_array[i],suffix);
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
            arguments_array[i] = initThreadArgsError(ConnFd[i],filename_array[i], filepath_array[i]);
        }

        //printf("created parameters to function Client Send array\n");

        // create threads
        pthread_t threads[array_size];
        for (unsigned int i=0; i<array_size/3; i++)
            pthread_create(&threads[i], NULL, clientSend403, (void*)arguments_array[i]);

        for (unsigned int i=array_size/3; i<array_size*(2/3); i++)
            pthread_create(&threads[i], NULL, clientSend404, (void*)arguments_array[i]);

        for (unsigned int i=array_size*(2/3); i<array_size/3; i++)
            pthread_create(&threads[i], NULL, clientSend501, (void*)arguments_array[i]);

        //printf("created threads array\n");

        // wait for threads to finish
        for (unsigned int i=0; i<array_size; i++)
            pthread_join(threads[i], NULL);

        //printf("finished join\n");

        for (int i = 0; i < array_size; ++i) {
            //printf("closing connfd = %d\n", ConnFd[i]);
            Close(ConnFd[i]);
        }

        for (int i = 0; i < array_size; ++i) {
            destroyThreadArgsError(arguments_array[i]);
        }

        //printf("finished closing client Fd's\n");
    }
}