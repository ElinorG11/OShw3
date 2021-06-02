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

pthread_cond_t consumer_cond, producer_cond;
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
        while (waiting_queue == NULL || waiting_queue->queue_size == 0) {
            pthread_cond_wait(&consumer_cond, &mutex);
        }
        // get request from waiting queue
        int connfd = dequeque(waiting_queue);
        //printf("starting thread pid: %ld with connfd: %d\n", pthread_self(), connfd);
        enqueue(currently_executing_queue, connfd);

        /*
        printf("Print waiting requests connection Fd's\n");
        printQueue(waiting_queue);
        printf("Print currently executing requests connection Fd's\n");
        printQueue(currently_executing_queue);
        */

        pthread_mutex_unlock(&mutex);

        // request handling of a thread shouldn't block the others
        requestHandle(connfd);

        // executing critical section - accessing to shared queue
        pthread_mutex_lock(&mutex);
        dequequeById(currently_executing_queue, connfd);

        Close(connfd);
        // send signal to producer in case que
        pthread_cond_signal(&producer_cond);

        //printf("Finished thread pid: %ld with connfd: %d\n", pthread_self(), connfd);

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
    // From Piazza - no need to check for errors. Thank god this is no MATAM
    if (argc < 5) {
	fprintf(stderr, "Usage: %s <port>\n", argv[0]);
	exit(1);
    }
    *port = atoi(argv[1]);
    *thread_count = atoi(argv[2]);
    *max_queue_size = atoi(argv[3]);
    strcpy(sched_alg,argv[4]);
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
    return -1;
}


int main(int argc, char *argv[])
{
    int listenfd, connfd, port, clientlen, thread_count, max_queue_size, drop_percentage;
    char *sched_alg = malloc(MAX_SCHED_ALG_SIZE);
    struct sockaddr_in clientaddr;

    getargs(&port, &thread_count, &max_queue_size, sched_alg, argc, argv);

    // we assume there won't be any error
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
    //printf("got connfd, %d\n", connfd);
	// 
	// HW3: In general, don't handle the request in the main thread.
	// Save the relevant info in a buffer and have one of the worker threads 
	// do the work. 
	// 
    /* Start multi-thread implementation */

    // crititcal section
	pthread_mutex_lock(&mutex);

	// part 2.0: check how to handle too many requests (more than specfied by max_queue_size in the command line
    switch(sched_alg_num) {
        case 0:
            // make sure waiting & currently_executing requests are less than queue size specified in cmd
            while (waiting_queue->queue_size + currently_executing_queue->queue_size >= max_queue_size) {
                pthread_cond_wait(&producer_cond, &mutex);
            }
            // waiting queue size will be increased inside enqueue()
            enqueue(waiting_queue, connfd);
            // signal all threads that a request has been added
            pthread_cond_broadcast(&consumer_cond);
            /* For debugging */
            /*
             printf("Print waiting requests connection Fd's\n");
             printQueue(waiting_queue);
             printf("Print currently executing requests connection Fd's\n");
             printQueue(currently_executing_queue);
            */
            pthread_mutex_unlock(&mutex);
            break;
        case 1:
            if(waiting_queue->queue_size + currently_executing_queue->queue_size >= max_queue_size){
                Close(connfd);
                pthread_mutex_unlock(&mutex);
                break; // was continue; changed to break; to fit general structure
            }
        case 2:
            // waiting queue size will be increased inside enqueue()
            enqueue(waiting_queue, connfd);
            if(waiting_queue->queue_size + currently_executing_queue->queue_size >= max_queue_size){
            // either check that waiting_queue->queue_size != 0 (but then, in case it is 0 we need to unlock() + continue so we won't
            // add this request in line 176. or, we can always add, and discard the head -> in this manner we will keep the apropriate size
            // of both queues.
                // dequeue request from head of the waiting list in case both queues are full
                dequeque(waiting_queue);
                // continue if waiting queue is empty
                if(waiting_queue->queue_size == 0){
                    continue;
                }
            }
            // now we're sure there's a new request in the waiting queue - let all the threads know
            // signal all threads that a request has been added
            pthread_cond_broadcast(&consumer_cond);
            pthread_mutex_unlock(&mutex);
            break;
        case 3:
            // we need to check if waiting_queue->queue_size + currently_executing_queue->queue_size >= max_queue_size
            // also consider corner case where waiting_queue->queue_size == 0

            // waiting queue size will be increased inside enqueue()
            enqueue(waiting_queue, connfd);

            if(waiting_queue->queue_size + currently_executing_queue->queue_size >= max_queue_size){
                // decrease 1 from queue size since we added the new request
                drop_percentage = ceil((waiting_queue->queue_size - 1) / 4);
                for (int i = 0; i < drop_percentage; ++i) {
                    int index = rand() % drop_percentage;
                    dequequeByIndex(waiting_queue,index);
                }
            }

            /* not sure whether we need to check this or not, but I think we can do it
             * just to make sure we're not waking up threads when queue is empty
             * */
            if(waiting_queue->queue_size == 0) {
                continue;
            }

            // now we're sure there's new waiting request
            // signal all threads that a request has been added
            pthread_cond_broadcast(&consumer_cond);
            pthread_mutex_unlock(&mutex);
            break;
        default:
            break;
    }

    // No need to enqueque & broadcast here, handled it in all the different cases.
    // waiting queue size will be increased inside enqueue()
    //enqueue(waiting_queue, connfd);
    // signal all threads that a request has been added
    //pthread_cond_broadcast(&consumer_cond);

    /* got some problems since we need to use break; inside switch case (and we didn't)
     * so to fit general structure each case will handle all of the actions required
     * for a specific scheduling algorithm
     * */
    //pthread_mutex_unlock(&mutex);
    /* Done multi-thread implementation */

    }
    // Close(listenfd);
    pthread_mutex_destroy(&mutex);
    free(sched_alg);
    destroyQueue(waiting_queue);
    destroyQueue(currently_executing_queue);
}


    


 
