#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <memoryapi.h>

#define FALSE 0

void* myMalloc(SIZE_T size);
BOOL myFree(void* ptr);