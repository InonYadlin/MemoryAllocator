#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <memoryapi.h>

#define FALSE 0

typedef struct MemoryBlock
{
	void *_ptr;
	SIZE_T _size;
	struct MemoryBlock *_next;
} MemoryBlock;

extern SIZE_T TOTAL_MEMORY_ALLOCATED;

void printSizes(void);
void* improvedMalloc(SIZE_T size);
BOOL improvedFree(void* ptr);
void* improvedReAlloc(SIZE_T newSize, void* ptr);