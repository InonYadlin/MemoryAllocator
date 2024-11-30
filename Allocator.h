#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <memoryapi.h>

#define FALSE 0

typedef struct MemoryBlock
{
	void *_ptr;
	size_t _size;
	struct MemoryBlock *_next;
} MemoryBlock;

void printSizes(void);
void* improvedMalloc(SIZE_T size);
BOOL improvedFree(void* ptr);
void improvedReAlloc(SIZE_T newSize, void* ptr);
void* basicMalloc(SIZE_T size);
BOOL basicFree(void* ptr);
void* basicReAlloc(SIZE_T newSize, void* ptr);