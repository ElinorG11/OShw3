#include <stdio.h>
#include <stdlib.h>

struct Queue;

struct Queue *initQueue();
void enqueue(struct Queue *queue, int connfd, struct timeval arrival_time);
int dequeque(struct Queue *queue);
void dequequeById(struct Queue *queue, int id);
void destroyQueue(struct Queue *queue);
int dequequeByIndex(struct Queue *queue, int index);
void printQueue(struct Queue *queue);
int QueueSize(struct Queue *queue);
void getArrivalTimeByConnFd(struct Queue *queue, int connfd, struct timeval *arrival_time);
void getArrivalTimeByIndex(struct Queue *queue, int index, struct timeval *arrival_time);

// For Debug
void incHandledRequestsCount(struct Queue *queue);
int getHandleRequestsCount(struct Queue *queue);
void incDroppedRequestsCount(struct Queue *queue);
int getDroppedRequestsCount(struct Queue *queue);