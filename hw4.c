/****************************************
*Name: Hunter Page, pagehm1@etsu.edu
*Class: CSCI 4727-001
*File Name: hw4.c
*Date Last Modified: 11/14/21
*
****************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "product_record.h"
#include <unistd.h>
#include <sys/wait.h>
#include "modifyFile.h"
#include "queue.h"
#include "file_struct.h"
#include <semaphore.h>
#include <pthread.h>

int i; //for loop number; total that calculates a continual total of the orders
double runningTotal; //keeps track of the total cost of all entries
struct queue stationQueues[7]; //holds the queues for each station; Shared Memory**

//initialize the semaphores used in the program
sem_t stationZeroSema, stationOneSema, stationTwoSema, stationThreeSema, stationFourSema, writingStationSema;


//station that reads a record from the input file
void *readingStation(void* input)
{
	//read the entire file passed in
	while(1)
	{
		struct file *userFile = input;
		
		//reads from the file needed
		readFile(userFile->fileNeeded, &userFile->product);		

		//printf("\nReading Station Enqueuing\n"); 
		enqueue(&stationQueues[0], userFile->product);
		
		sem_post(&stationZeroSema); //signal for stationZero to start
		
		//check for the end of the file
		if(userFile->product.idnumber == -1)
		{
			break;
		}	
	}
	
}

//Calculates the tax amount of a product record
void *stationZero(void* unused)
{
	struct product_record record;
	
	while(1)
	{
		sem_wait(&stationZeroSema);
		
		//get the front of the queue
		record = stationQueues[0].productQueue[stationQueues->front];
		
		//check if a special order
		if(record.idnumber <=1000)
		{
			record.tax = record.price * record.number * 0.05;
			record.stations[0] = 1;
		}
		
		//send the record to the next station and dequeue the current station
		enqueue(&stationQueues[1], record);
		dequeue(&stationQueues[0]);
		
		//tell next station that it can run
		sem_post(&stationOneSema);
		
		//check for the end of the file
		if(record.idnumber == -1)
		{
			break;
		}
	}
}


//calculates the shipping and handling
void *stationOne(void* unused)
{
	struct product_record record;
	
	while(1)
	{
		sem_wait(&stationOneSema);
		
		//get the front of the queue
		record = stationQueues[1].productQueue[stationQueues[1].front];
		
		if(record.idnumber != -1)
		{
			record.sANDh = (record.price * record.number * 0.01) + 10;
			
		}
		
		record.stations[1] = 1;
		enqueue(&stationQueues[2], record);
		dequeue(&stationQueues[1]);
		sem_post(&stationTwoSema);
		
		if(record.idnumber == -1)
		{
			break;
		}
	}
}


//calculates the total of the order
void *stationTwo(void* unused)
{
	struct product_record record;
	
	while(1)
	{
		sem_wait(&stationTwoSema);
				
		//get the front of the queue
		record = stationQueues[2].productQueue[stationQueues[2].front];
				
		record.total = (record.price * record.number) + record.tax + record.sANDh;
		record.stations[2] = 1;
				
		enqueue(&stationQueues[3], record);
		dequeue(&stationQueues[2]);
		sem_post(&stationThreeSema);
		
		if(record.idnumber == -1)
		{
			break;
		}
	}
}



//calculates the running total of the orders collected so far
void *stationThree(void* unused)
{
	struct product_record record;
	
	while(1)
	{
		sem_wait(&stationThreeSema);
		
		//get the front of the queue
		record = stationQueues[3].productQueue[stationQueues[3].front];
				
		runningTotal += record.total;
		printf("\n\nRunning total: %.2f\n\n", runningTotal); //print the current running total
		record.stations[3] = 1;
		
		//send the record to the printing and writing stations
		enqueue(&stationQueues[4], record);
		enqueue(&stationQueues[5], record);
		dequeue(&stationQueues[3]);
		
		//open up for the stations to be written and printed
		sem_post(&stationFourSema);
		sem_post(&writingStationSema);
		
		if(record.idnumber == -1)
		{
			break;
		}
	}
}

//prints the product record after all other stages
void *stationFour(void* unused)
{
	struct product_record record;
	
	while(1)
	{
		sem_wait(&stationFourSema);
		
		//get the front of the queue
		record = stationQueues[4].productQueue[stationQueues[4].front];
		record.stations[4] = 1;
		
		printStruct(record);
		dequeue(&stationQueues[4]);

		if(record.idnumber == -1)
		{
			break;
		}
	}
}

//Write the record to the file
void *writingStation(void* output)
{
	while(1)
	{
		sem_wait(&writingStationSema);
		struct file *userFile = output;
		
		userFile->product = stationQueues[5].productQueue[stationQueues[5].front];
		
		//reads from the file needed
		writeFile(userFile->fileNeeded, userFile->product);		
		
		dequeue(&stationQueues[5]);

		if(userFile->product.idnumber == -1)
		{
			break;
		}
	}
}

//adds info needs for reading or writing thread to a struct to pass in
void enterFileInfo(FILE* file, struct file *fileInfo)
{
	fileInfo->fileNeeded = file;
}

/*
 *Creates seven threads that will perform each process of the stations and read/write from files
	
	argv[1] - input file
	argv[2] - output file
*/
int main(int argc, const char* argv[])
{
	struct product_record productInfo; 			//struct that stores the info read in from the file
	struct file input; 							//holds info for the info file
	struct file output; 						//holds info for the output file
    FILE* inputFile = fopen(argv[1], "r"); 		//makes the input file from file passed in
	FILE* outputFile = fopen(argv[2], "w+"); 	//makes the ouput file from the name passed in 
	int rc; 									//holds the result of a thread being created
	pthread_t threadID[7]; 						//thread id holder for the threads being made	
	
	//set size for all queues
	for(i = 0; i < 7; i++)
	{
		setQueueSize(&stationQueues[i]);
	}
	
	//enter the info needed to pass a struct to the input and output threads
	//also for testing passing a struct to a thread to send multiple items
	enterFileInfo(inputFile, &input);
	enterFileInfo(outputFile, &output);
	
	//initialize semaphore to 0 so that it is initially locked
	//and forces stationZero to wait
	sem_init(&stationZeroSema, 0, 0);
	sem_init(&stationOneSema, 0, 0);
	sem_init(&stationTwoSema, 0, 0);
	sem_init(&stationThreeSema, 0, 0);
	sem_init(&stationFourSema, 0, 0);
	sem_init(&writingStationSema, 0, 0);
	
	//create the threads for each station 
	if((rc = pthread_create(&threadID[0], NULL, readingStation, &input)))
	{
		fprintf(stderr, "error: pthread_create on %d.\n", rc);
		return EXIT_FAILURE;
	}
	//create the threads for each station 
	if((rc = pthread_create(&threadID[1], NULL, stationZero, NULL)))
	{
		fprintf(stderr, "error: pthread_create on %d.\n", rc);
		return EXIT_FAILURE;
	}
	
	//create the threads for each station 
	if((rc = pthread_create(&threadID[2], NULL, stationOne, NULL)))
	{
		printf("error: pthread_create, rc: %d\n", rc);
        return EXIT_FAILURE;
	}
	//create the threads for each station 
	if((rc = pthread_create(&threadID[3], NULL, stationTwo, NULL)))
	{
		printf("error: pthread_create, rc: %d\n", rc);
        return EXIT_FAILURE;
	}
	
	//create the threads for each station 
	if((rc = pthread_create(&threadID[4], NULL, stationThree, NULL)))
	{
		printf("error: pthread_create, rc: %d\n", rc);
        return EXIT_FAILURE;
	}
	
	//create the threads for each station 
	if((rc = pthread_create(&threadID[5], NULL, stationFour, NULL)))
	{
		printf("error: pthread_create, rc: %d\n", rc);
        return EXIT_FAILURE;
	}
	
	//create the threads for each station 
	if((rc = pthread_create(&threadID[6], NULL, writingStation, &output)))
	{
		printf("error: pthread_create, rc: %d\n", rc);
        return EXIT_FAILURE;
	}
	
	//join back once the threads are all done
	pthread_join(threadID[0], NULL);
	pthread_join(threadID[1], NULL);
	pthread_join(threadID[2], NULL);
	pthread_join(threadID[3], NULL);
	pthread_join(threadID[4], NULL);
	pthread_join(threadID[5], NULL);
	pthread_join(threadID[6], NULL);
	
	//close the files
	fclose(inputFile);
	fclose(outputFile);
	exit(0);
}
