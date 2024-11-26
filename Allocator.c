#include "Allocator.h"

/*
* Function which allocates memory 
* Input: Size of block to allocate
* Output: Pointer to allocated memory
*/
void* myMalloc(SIZE_T size)
{
    // Checking if the given size is 0 or less
    if (size <= 0)
    {
        printf("Invalid size, please enter a number greater than 0!\n");
        return NULL;
    }

    // Allocating the memory
    void* allocatedMemory = VirtualAlloc(
        NULL,                // Let the system determine the starting address
        size,                // Size of the memory block
        MEM_COMMIT | MEM_RESERVE, // Reserve and commit the memory
        PAGE_READWRITE       // Allow read and write access
    );

    // Checking if the memory allocated properly
    if (allocatedMemory == NULL)
    {
        printf("VirtualAlloc failed with error : % lu\n", GetLastError());
        return NULL;
    }

    // Print the address of the allocated memory
    printf("Memory successfully allocated at address: %p\n", allocatedMemory);
    return allocatedMemory;
}

/*
* Function which frees allocated memory
* Input: Pointer to allocated memory
* Output: If the freeing was successful
*/
BOOL myFree(void* allocatedPtr)
{
    // Checking if the given pointer is a null to avoid errors
    if (allocatedPtr == NULL)
    {
        printf("Given pointer is a NULL\n");
        return FALSE;
    }

    // Freeing the memory
    BOOL deallocationStatus = VirtualFree(
        allocatedPtr, // Free the given memory
        0, // No need to specify size as we are using the MEM_RELEASE flag
        MEM_RELEASE // Use the MEM_RELEASE flag
    );

    // If the memory was freed, print info message and return TRUE
    if (deallocationStatus != FALSE)
    {
        printf("Deallocation successful!\n");
        return deallocationStatus;
    }

    // Otherwise, return the error message
    printf("Deallocation could not be complete: % lu\n", GetLastError());
    return FALSE;
}

/*
* Function reallocates memory for a pointer while retaining the original values
* Input: Size of the new memory block, Pointer to the previous location
* Output: Pointer to the new memory location
*/
void* myReAlloc(SIZE_T newSize, void* array)
{
    // If the pointer to the given array is null, print an error message and return null
    if (array == NULL)
    {
        printf("Given array is null, aborting\n");
        return NULL;
    }

    // If the new size is 0, simply free the previous array
    if (newSize == 0)
    {
        printf("Size is 0, Freeing the memory\n");
        myFree(array);
        return NULL;
    }

    // Allocating memory for the new array
    void* newArray = myMalloc(newSize);

    // Checking if the allocation was successful
    if (newArray == NULL)
    {
        return NULL;
    }

    // Copying the contents of the old array into the new one
    memcpy(newArray, array, newSize);

    // Freeing the previous array
    myFree(array);

    // Returning the pointer to the new array
    return newArray;
}
