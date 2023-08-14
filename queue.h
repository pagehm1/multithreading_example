#ifndef QUEUE_H
#define QUEUE_H

#include <semaphore.h>
#include <pthread.h>
#include "product_record.h"

#define THREADCOUNT 10000 //number of threads that can be in a queue

struct queue 
{
	int id;     //tells us which location in the array it is
	int front;  //keeps track of the value in the front
	int back;   //keeps track of the value in back
	int size;
	struct product_record productQueue[THREADCOUNT];
};

void enqueue(struct queue* threadHolder, struct product_record record);

void dequeue(struct queue* threadHolder);

void setQueueSize(struct queue* threadHolder);

void getFront(struct queue productHolder, struct product_record* record);

#endif