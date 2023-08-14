/****************************************
*Name: Hunter Page, pagehm1@etsu.edu
*Class: CSCI 4727-001
*File Name: modifyFile.c
*Date Last Modified: 11/14/21
*
****************************************/

#include "modifyFile.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int i, j;

//prints the values contained in the struct 
void printStruct(struct product_record productInfo)
{
	printf("ID Number: %d\nProduct Name: %s\nPrice: %.2f\nNumber Ordered: %d\nTax: %.2f\nShipping and Handling: %.2f\nTotal Price: %.2f\n", 
											productInfo.idnumber, 
											productInfo.name, 
											productInfo.price, 
											productInfo.number, 
											productInfo.tax, 
											productInfo.sANDh, 
											productInfo.total);
											
	
	
	printf("Stations completed: ");
	//takes the size of the stations array and 
	for(i = 0; i < sizeof(productInfo.stations) / 4; i++)
	{
		printf("%d ",productInfo.stations[i]);
	}
	
	printf("\n\n");
}


/*
 *Reads the file passed in and places all of the information into a passed in struct
*/
void readFile(FILE* file, struct product_record* productInfo)
{
	i = 0;
	
	//scans all items 
    fscanf(file, "%d", &productInfo->idnumber);
	fscanf(file, "%s", productInfo->name);
	fscanf(file, "%lf", &productInfo->price);
	fscanf(file, "%d", &productInfo->number);
	fscanf(file, "%lf", &productInfo->tax);
	fscanf(file, "%lf", &productInfo->sANDh);
	fscanf(file, "%lf", &productInfo->total);
	
	//loops x amount of times based off of stations count
	for(i = 0; i < sizeof(productInfo->stations) / 4; i++)
	{
		fscanf(file, "%d", &productInfo->stations[i]);
		
		//changes number to 0 if not already
		if(productInfo->stations[i] != 0)
		{
			productInfo->stations[i] = 0;
		}
	}
	
	//print to confirm correct info
	//printStruct(*productInfo);
}


/*
 *Writes the product information into a file
*/
void writeFile(FILE* file, struct product_record productInfo)
{
	//prints each struct piece to a file using a scanable and readable format
	fprintf(file, "\n%d\n", productInfo.idnumber);
	fprintf(file, "%s\n", productInfo.name);
	fprintf(file, "%.2lf\n", productInfo.price);
	fprintf(file, "%d\n", productInfo.number);
	fprintf(file, "%.2lf\n", productInfo.tax);
	fprintf(file, "%.2lf\n", productInfo.sANDh);
	fprintf(file, "%.2lf\n", productInfo.total);
	
	//loops x amount of times based off of stations count
	for(i = 0; i < sizeof(productInfo.stations) / 4; i++)
	{
		fprintf(file, "%d", productInfo.stations[i]);
		
		//Place an extra space between each station number except for the very last one
		if(i + 1 != sizeof(productInfo.stations) / 4)
		{
			fprintf(file, "%c", ' ');
		}
	}
	
	//print to confirm correct info
	//printStruct(productInfo);
}