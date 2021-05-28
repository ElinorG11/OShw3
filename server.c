#include "segel.h"
#include "request.h"
#include "queue.h" // we added this

// server.c: A very, very simple web server
//
// To run:
//  ./server <portnum (above 2000)>
//
// Repeatedly handles HTTP requests sent to this port number.
// Most of the work is done within routines written in request.c
//

/* Implementing multi-threaded server */
pthread_cond_t cond;
pthread_mutex_t mutex;

void work(struct Queue *queue) {
    pthread_mutex_lock(&mutex);
    // variable queue_size is updated inside dequeue

}

void createThreadPool(int num_of_threads) {
    for(int i=0; i<num_of_threads; i++) {
        pthread_create()
    }
}

/* Done implementing multi-threaded server */

// HW3: Parse the new arguments too
void getargs(int *port, int argc, char *argv[])
{
    if (argc < 2) {
	fprintf(stderr, "Usage: %s <port>\n", argv[0]);
	exit(1);
    }
    *port = atoi(argv[1]);

}


int main(int argc, char *argv[])
{
    int listenfd, connfd, port, clientlen;
    struct sockaddr_in clientaddr;

    getargs(&port, argc, argv);

    // 
    // HW3: Create some threads...
    //
    /* Start multi-thread implementation */
    //int num_of_threads = argv[];

    // create request queue
    struct Queue *queue = initQueue();

    // initialize condition and mutex
    pthread_cond_init(&cond, NULL);
    pthread_mutex_init(&mutex, NULL);

    // create num_of_threads threads
    createThreadPool(num_of_threads);

    /* Done multi-thread implementation */

    listenfd = Open_listenfd(port);
    while (1) {
	clientlen = sizeof(clientaddr);
	connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *) &clientlen);

	// 
	// HW3: In general, don't handle the request in the main thread.
	// Save the relevant info in a buffer and have one of the worker threads 
	// do the work. 
	// 
	requestHandle(connfd);

	Close(connfd);
    }

}


    


 
