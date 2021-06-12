#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "queue.h"

struct Node{
    int connfd;
    struct timeval req_arrival_time;
    struct Node *next;
};

struct Queue{
    struct Node *head;
    struct Node *end;
    int queue_size;
    /* For Debugging */
    int handled_requests_count;
    int dropped_requests;
};

struct Queue *initQueue() {
    struct Queue *queue = malloc(sizeof(struct Queue));
    if(queue == NULL) {
        return NULL;
    }
    queue->head = NULL;
    queue->end = queue->head;
    queue->queue_size = 0;

    // For Debugging
    queue->handled_requests_count = 0;
    queue->dropped_requests = 0;

    return queue;
}

// For Debugging
void incHandledRequestsCount(struct Queue *queue){
    if(queue == NULL) return;
    queue->handled_requests_count++;
}

int getHandleRequestsCount(struct Queue *queue) {
    if(queue == NULL) return -1;
    return queue->handled_requests_count;
}

void incDroppedRequestsCount(struct Queue *queue){
    if(queue == NULL) return;
    queue->dropped_requests++;
}

int getDroppedRequestsCount(struct Queue *queue) {
    if(queue == NULL) return -1;
    return queue->dropped_requests;
}

////////////////////////////////

void enqueue(struct Queue *queue, int connfd, struct timeval arrival_time) {
    if(queue == NULL) return;

    //printf("%ld: enqueue element: %d, initial queue size of %d\n", pthread_self(), connfd, queue->queue_size);


    struct Node *node = malloc(sizeof (struct Node));

    if(node == NULL) return;


    //printf("got connfd = %d in enqueue\n", connfd);

    node->connfd = connfd;

    //printf("died after connfd update in enqueue\n");

    node->req_arrival_time = arrival_time;
    node->next = NULL;

    // Empty list
    if(queue->queue_size == 0){
        queue->head = node;
        queue->end = queue->head;
    }
    else {
        queue->end->next = node;
        queue->end = node;
    }

    queue->queue_size++;

    //printf("New list:\n");
    //printQueue(queue);
}

int dequeque(struct Queue *queue) {
    if(queue == NULL) return -1;

    // Empty queue
    if(queue->queue_size == 0) return -1;

    // Remove and return first request
    struct Node *temp = queue->head;
    queue->head = queue->head->next;

    // if we have only one element and we remove it - we need to make sure to update end
    if(queue->head == NULL) {
        queue->end = NULL;
    }
    int connfd = temp->connfd;
    free(temp);

    queue->queue_size--;

    //printf("%ld: after dequeue of element: %d final queue size: %d\n", pthread_self(),connfd,queue->queue_size);
    //printf("New list:\n");
    //printQueue(queue);

    return connfd;
}

void dequequeById(struct Queue *queue, int id) {

    // For Debug
    //printf("%d\n", queue->handled_requests_count);

    if(queue == NULL) return;

    //printf("%ld: looking for id: %d. list before dequequeById:\n",pthread_self(),id);
    //printQueue(queue);

    // Empty queue
    if(queue->queue_size == 0) return;

    struct Node *iterator = queue->head;
    struct Node *prev_iterator = queue->head;
    while (iterator != NULL) {

        if(iterator->connfd == id) {
            // head is the required element
            if(iterator == queue->head) {
                queue->head = iterator->next;

                // if we have only one element and we remove it - we need to make sure to update end
                if(queue->head == NULL){
                    queue->end = queue->head;
                }

                free(iterator);
                queue->queue_size--;

                //printf("%ld: queue size dequeueById: %d\n",pthread_self(), queue->queue_size);
                //printf("New list:\n");
                //printQueue(queue);

                return;
            }

            // removing last element
            if(iterator == queue->end) {
                prev_iterator->next = iterator->next;
                queue->end = prev_iterator;
                free(iterator);
                queue->queue_size--;

                //printf("%ld: queue size dequeueById: %d\n",pthread_self(), queue->queue_size);
                //printf("New list:\n");
                //printQueue(queue);

                return;
            }

            prev_iterator->next = iterator->next;
            free(iterator);
            queue->queue_size--;

            //printf("%ld: queue size dequeueById: %d\n",pthread_self(), queue->queue_size);
            //printf("New list:\n");
            //printQueue(queue);

            return;
        }

        prev_iterator = iterator;
        iterator = iterator->next;
    }

}

int dequequeByIndex(struct Queue *queue, int index) {

    if(queue == NULL) return -1;

    // Empty queue
    if(queue->queue_size == 0) return -1;

    struct Node *iterator = queue->head;
    struct Node *prev_iterator = queue->head;
    int fd;

    for (int i = 0; i < index; ++i) {
        if(iterator == NULL){
            printf("iterator is NULL\n");
            break;
        }
        prev_iterator = iterator;
        iterator = iterator->next;
    }

    if(iterator != NULL) {
        fd = iterator->connfd;

        // head is the required element
        if(iterator == queue->head) {
            queue->head = iterator->next;

            // if we have only one element and we remove it - we need to make sure to update end
            if (queue->head == NULL) {
                queue->end = queue->head;
            }
            free(iterator);
            queue->queue_size--;
            return fd;
        }

        // removing last element
        if(iterator == queue->end) {
            prev_iterator->next = iterator->next;
            queue->end = prev_iterator;
            free(iterator);
            queue->queue_size--;
            return fd;
        }

        else {
            prev_iterator->next = iterator->next;
            free(iterator);
            queue->queue_size--;
        }
    }
    return fd;
}

int QueueSize(struct Queue *queue) {
    if(queue == NULL) return 0;
    return queue->queue_size;
}

void printQueue(struct Queue *queue) {
    if(queue == NULL) {
        printf("queue is null\n");
        return;
    }
    if(queue->queue_size == 0) {
        printf("Queue is empty\n");
        return;
    }

    struct Node *iterator = queue->head;

    while (iterator != NULL) {
        printf("Connection Fd of current node is: %d\n", iterator->connfd);
        iterator = iterator->next;
    }
}

void getArrivalTimeByConnFd(struct Queue *queue, int connfd, struct timeval *arrival_time) {
    if(queue == NULL || queue->queue_size == 0) return;

    struct Node *iterator = queue->head;

    while (iterator != NULL) {
        if(iterator->connfd == connfd) {
            *arrival_time = iterator->req_arrival_time;
        }
        iterator = iterator->next;
    }
}

void getArrivalTimeByIndex(struct Queue *queue, int index, struct timeval *arrival_time) {
    if(queue == NULL || queue->queue_size == 0) return;

    struct Node *iterator = queue->head;

    for (int i = 0; i < index; ++i) {
        iterator = iterator->next;
    }

    *arrival_time = iterator->req_arrival_time;
}

void destroyQueue(struct Queue *queue) {
    struct Node *curr_iterator = queue->head;
    struct Node *prev_iterator = queue->head;
    while (curr_iterator != NULL) {
        prev_iterator = curr_iterator;
        curr_iterator = prev_iterator->next;
        free(prev_iterator);
    }
    free(queue);
}




