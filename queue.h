#include <stdio.h>
#include <stdlib.h>

struct Queue;

struct Queue *initQueue();
void enqueue(struct Queue *queue, int connfd);
int dequeque(struct Queue *queue);
void dequequeById(struct Queue *queue, int id);
void destroyQueue(struct Queue *queue);
void dequequeByIndex(struct Queue *queue, int index);
void printQueue(struct Queue *queue);
int QueueSize(struct Queue *queue);
long getArrivalTime(struct Queue *queue, int connfd);
