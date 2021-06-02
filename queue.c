#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

struct Node{
    int connfd;
    struct Node *next;
};
/*
int getConnfd(struct Node *node){
    return node->connfd;
}

struct Node * getNext(struct Node *node) {
    return node->next;
}
*/

struct Queue{
    struct Node *head;
    struct Node *end;
    int queue_size;
};

struct Queue *initQueue() {
    struct Queue *queue = malloc(sizeof(struct Queue));
    if(queue == NULL) {
        return NULL;
    }

    queue->end = queue->head;
    queue->queue_size = 0;
    return queue;
}

void enqueue(struct Queue *queue, int connfd) {
    if(queue == NULL) return;

    struct Node *node = malloc(sizeof (struct Node));
    if(node == NULL) return;

    node->connfd = connfd;
    node->next = NULL;

    // Empty list
    if(queue->queue_size == 0){
        queue->head = node;
        queue->end = queue->head;
    }
    else {
        queue->end->next = node;
        queue->end = queue->end->next;
    }

    queue->queue_size++;
}

int dequeque(struct Queue *queue) {
    if(queue == NULL) return -1;

    // Empty queue
    if(queue->queue_size == 0) return -1;

    // Remove and return first request
    struct Node *temp = queue->head;
    queue->head = queue->head->next;
    int connfd = temp->connfd;
    free(temp);

    queue->queue_size--;
    return connfd;
}

void dequequeById(struct Queue *queue, int id) {
    if(queue == NULL) return;

    // Empty queue
    if(queue->queue_size == 0) return;

    struct Node *iterator = queue->head;
    struct Node *prev_iterator = queue->head;
    while (iterator != NULL) {

        if(iterator->connfd == id) {
            // head is the required element
            if(iterator == queue->head) {
                queue->head = iterator->next;
                free(iterator);
                queue->queue_size--;
                break;
            }

            prev_iterator->next = iterator->next;
            free(iterator);
            queue->queue_size--;
            break;
        }

        prev_iterator = iterator;
        iterator = iterator->next;
    }

}

void dequequeByIndex(struct Queue *queue, int index) {
    if(queue == NULL) return;

    // Empty queue
    if(queue->queue_size == 0) return;

    struct Node *iterator = queue->head;
    struct Node *prev_iterator = queue->head;

    for (int i = 0; i < index; ++i) {
        prev_iterator = iterator;
        iterator = iterator->next;
    }

    // head is the required element
    if(iterator == queue->head) {
        queue->head = iterator->next;
        free(iterator);
        queue->queue_size--;
    } else {
        prev_iterator->next = iterator->next;
        free(iterator);
        queue->queue_size--;
    }
}

int QueueSize(struct Queue *queue) {
    if(queue == NULL) return 0;
    return queue->queue_size;
}

void printQueue(struct Queue *queue) {
    if(queue == NULL) return;
    if(queue->queue_size == 0) {
        printf("Queue is empty\n");
        return;
    }

    struct Node *iterator = queue->head;

    while (iterator != queue->end) {
        printf("Connection Fd of current node is: %d\n", iterator->connfd);
        iterator = iterator->next;
    }
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


