#ifndef FILE_STRUCT
#define FILE_STRUCT

#include "product_record.h"

struct file 
{
	struct product_record product;
	FILE* fileNeeded;
};

#endif
