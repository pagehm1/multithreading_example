#ifndef FILE_MODIFYING_H
#define FILE_MODIFYING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "product_record.h"


void readFile(FILE* file, struct product_record* productInfo);

void writeFile(FILE* file, struct product_record productInfo);

void printStruct(struct product_record productInfo);

#endif