//
// Created by student on 5/28/21.
//

#ifndef OSHW3_QUEUE_H
#define OSHW3_QUEUE_H

struct Queue;

struct Queue *initQueue();
void enqueue(struct Queue *queue, int connfd);
void dequeque(struct Queue *queue);
int queueSize(struct Queue *queue)

#endif //OSHW3_QUEUE_H
