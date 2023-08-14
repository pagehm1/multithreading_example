/****************************************
*Name: Hunter Page, pagehm1@etsu.edu
*Class: CSCI 4727-001
*File Name: queue.c
*Date Last Modified: 11/14/21
*
****************************************/

#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <pthread.h>
#include "modifyFile.h"

//adds a number to the queue
void enqueue(struct queue* threadHolder, struct product_record record)
{
	//check if the queue is full
	if(threadHolder->back == threadHolder->size)
	{
		printf("Thread is full. Dequeue.");
	}
	else
	{
		//initialize the front of the queue
		if(threadHolder->front == -1)
		{
			threadHolder->front = 0;
		}

		threadHolder->productQueue[threadHolder->back] = record;

		threadHolder->back++;
	}
}

//Changes the front of the queue so that a number is
//inaccessible, or removed from the queue
void dequeue(struct queue* threadHolder)
{
	if(threadHolder->front == -1 || threadHolder->front > threadHolder->back)
	{
		printf("Thread is empty");
	}
	
	threadHolder->front++;	
}

//grabs the 'first' item in the queue
void getFront(struct queue productHolder, struct product_record* record)
{
	record = &productHolder.productQueue[productHolder.front];
}

//set the amount of records that can be held in the queue
void setQueueSize(struct queue* threadHolder)
{
	threadHolder->size = sizeof(threadHolder->productQueue) / sizeof(int);
}