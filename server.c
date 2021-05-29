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

// shared queues of requests
struct Queue *waiting_queue;
struct Queue *currently_executing_queue;

void * thread_workload() {
    // after handling a specific request the thread continues waiting for new ones
    while(1) {
        // executing critical section - accessing to shared queue
        pthread_mutex_lock(&mutex);
        // variable queue_size is updated inside dequeue
        while (waiting_queue->queue_size == 0) {
            pthread_cond_wait(&cond, &mutex);
        }
        // get request from waiting queue
        int connfd = dequeque(waiting_queue);
        enqueue(currently_executing_queue, connfd);
        pthread_mutex_unlock(&mutex);

        // request handling of a thread shouldn't block the others
        requestHandle(connfd);

        // executing critical section - accessing to shared queue
        pthread_mutex_lock(&mutex);
        dequequeById(currently_executing_queue, connfd);
        pthread_mutex_unlock(&mutex);
    }
}

void createThreadPool(int thread_count) {
    pthread_t threads[thread_count];
    for(int i=0; i<thread_count; i++) {
        pthread_create(&threads[i], NULL, thread_workload, NULL);
    }
    // Perhaps should store threads in list ?
}

/* Done implementing multi-threaded server */

// HW3: Parse the new arguments too
void getargs(int *port, int *thread_count, int *max_queue_size, char* sched_alg, int argc, char *argv[])
{
    if (argc < 2) {
	fprintf(stderr, "Usage: %s <port>\n", argv[0]);
	exit(1);
    }
    *port = atoi(argv[1]);
    *thread_count = atoi(argv[2]);
    *max_queue_size = atoi(argv[3]);
    sched_alg = argv[4];
}


int main(int argc, char *argv[])
{
    int listenfd, connfd, port, clientlen, thread_count, max_queue_size;
    char sched_alg;
    struct sockaddr_in clientaddr;

    getargs(&port, &thread_count, &max_queue_size, &sched_alg, argc, argv);

    // 
    // HW3: Create some threads...
    //
    /* Start multi-thread implementation */

    // create request queue
    waiting_queue = initQueue();
    currently_executing_queue = initQueue();

    // initialize condition and mutex
    pthread_cond_init(&cond, NULL);
    pthread_mutex_init(&mutex, NULL);

    // create num_of_threads threads
    createThreadPool(thread_count);

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
    /* Start multi-thread implementation */

	pthread_mutex_lock(&mutex);

	// crititcal section
	// make sure waiting & currently_executing requests are less than queue size specified in cmd
	if(waiting_queue->queue_size + currently_executing_queue->queue_size < max_queue_size) {
	    // waiting queue size will be increased inside enqueue()
        enqueue(waiting_queue, connfd);
        // signal all threads that a request has been added
        pthread_cond_broadcast(&cond);
	}
	else {
	    // part 2.0
	}

    pthread_mutex_unlock(&mutex);

    /* Done multi-thread implementation */

	Close(connfd);
    }

}


    


 
