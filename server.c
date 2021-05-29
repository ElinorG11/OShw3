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
#define MAX_SCHED_ALG_SIZE 100

pthread_cond_t consumer_cond,producer_cond;
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
            pthread_cond_wait(&consumer_cond, &mutex);
        }
        // get request from waiting queue
        int connfd = dequeque(waiting_queue);
        enqueue(currently_executing_queue, connfd);
        pthread_mutex_unlock(&mutex);
        printf("before handling req %d\n",connfd);
        // request handling of a thread shouldn't block the others
        requestHandle(connfd);
        printf("thread finished handling req\n");

        // executing critical section - accessing to shared queue
        pthread_mutex_lock(&mutex);
        dequequeById(currently_executing_queue, connfd);
        // send signal to producer in case que
        pthread_cond_signal(&producer_cond);
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
    if (argc < 5) {
	fprintf(stderr, "Usage: %s <port>\n", argv[0]);
	exit(1);
    }
    *port = atoi(argv[1]);
    *thread_count = atoi(argv[2]);
    *max_queue_size = atoi(argv[3]);
    // should check argument errors
    if(strlen(argv[4]) < 100){
        sched_alg = argv[4];
    }
}

int getSchedAlgNum(char *sched_alg) {
    if(strcmp(sched_alg,"block") == 0) {
        return 0;
    } else if(strcmp(sched_alg,"dt") == 0){
        return 1;
    } else if(strcmp(sched_alg,"dh") == 0) {
        return 2;
    } else if(strcmp(sched_alg,"random") == 0) {
        return 3;
    }
}


int main(int argc, char *argv[])
{
    int listenfd, connfd, port, clientlen, thread_count, max_queue_size, drop_percentage;
    char *sched_alg = malloc(MAX_SCHED_ALG_SIZE);
    struct sockaddr_in clientaddr;

    getargs(&port, &thread_count, &max_queue_size, sched_alg, argc, argv);

    int sched_alg_num = getSchedAlgNum(sched_alg);

    // 
    // HW3: Create some threads...
    //
    /* Start multi-thread implementation */

    // create request queue
    waiting_queue = initQueue();
    currently_executing_queue = initQueue();



    // initialize condition and mutex
    pthread_cond_init(&producer_cond, NULL);
    pthread_cond_init(&consumer_cond, NULL);
    pthread_mutex_init(&mutex, NULL);

    // create num_of_threads threads
    createThreadPool(thread_count);

    /* Done multi-thread implementation */

    listenfd = Open_listenfd(port);
    while (1) {
	clientlen = sizeof(clientaddr);
	connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *) &clientlen);
    printf("got connfd, %d\n", connfd);
	// 
	// HW3: In general, don't handle the request in the main thread.
	// Save the relevant info in a buffer and have one of the worker threads 
	// do the work. 
	// 
    /* Start multi-thread implementation */

	pthread_mutex_lock(&mutex);

    // crititcal section

    switch(sched_alg_num) {
        case 0:
            // make sure waiting & currently_executing requests are less than queue size specified in cmd
            while (waiting_queue->queue_size + currently_executing_queue->queue_size >= max_queue_size) {
                pthread_cond_wait(&producer_cond, &mutex);
            }
        case 1:
            if(waiting_queue->queue_size + currently_executing_queue->queue_size >= max_queue_size){
                Close(connfd);
                pthread_mutex_unlock(&mutex);
                continue;
            }
        case 2:
            if(waiting_queue->queue_size + currently_executing_queue->queue_size >= max_queue_size){
                dequeque(waiting_queue);
            }
        case 3:
            drop_percentage = floor(waiting_queue->queue_size / 4);
            for (int i = 0; i < drop_percentage; ++i) {
                int index = rand() % drop_percentage;
                dequequeByIndex(waiting_queue,index);
            }
    }

    // waiting queue size will be increased inside enqueue()
    enqueue(waiting_queue, connfd);
    // signal all threads that a request has been added
    pthread_cond_broadcast(&consumer_cond);

    pthread_mutex_unlock(&mutex);
    printf("finished handling req\n");

    /* Done multi-thread implementation */

	Close(connfd);
    }

    free(sched_alg);
    destroyQueue(waiting_queue);
    destroyQueue(currently_executing_queue);
}


    


 
