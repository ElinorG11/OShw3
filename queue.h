//
// Created by student on 5/28/21.
//

#ifndef OSHW3_QUEUE_H
#define OSHW3_QUEUE_H

struct Queue{
    struct Node *head;
    struct Node *end;
    int queue_size;
};

struct Queue *initQueue();
void enqueue(struct Queue *queue, int connfd);
int dequeque(struct Queue *queue);
void dequequeById(struct Queue *queue, int id);


#endif //OSHW3_QUEUE_H
