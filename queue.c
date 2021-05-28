#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct Node{
    int connfd;
    struct Node *next;
};

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

    queue->head = malloc(sizeof(struct Node));
    if(queue->head == NULL) {
        return NULL;
    }

    queue->head->next = NULL;
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

    queue->end->next = node;
    queue->end = queue->end->next;

    queue->queue_size++;
}

void dequeque(struct Queue *queue) {
    if(queue == NULL) return;

    // Empty queue
    if(queue->queue_size == 0) return;

    // Remove and return first request
    struct Node *temp = queue->head;
    queue->head = queue->head->next;
    free(temp);

    queue->queue_size--;
}

int queueSize(struct Queue *queue){
    return queue->queue_size;
}

