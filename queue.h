#include <stdio.h>
#include <stdlib.h>

struct Queue{
    struct Node *head;
    struct Node *end;
    int queue_size;
};

struct Queue *initQueue();
void enqueue(struct Queue *queue, int connfd);
int dequeque(struct Queue *queue);
void dequequeById(struct Queue *queue, int id);
int getConnfd(struct Node *node);
struct Node * getNext(struct Node *node);
void destroyQueue(struct Queue *queue);
