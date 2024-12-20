#include "Allocator.h"

// Array which stores all allocated memory blocks
MemoryBlock *linkedMemoryList = NULL;

// Variable which tracks all allocated memory
SIZE_T TOTAL_MEMORY_ALLOCATED = 0;

/*
* Function which prints all the sizes of the memory blocks stored in the linked list
* Input: None
* Output: None
*/
void printSizes(void)
{
    MemoryBlock* temp = linkedMemoryList;

    while (temp != NULL)
    {
        printf("Size of memory block at address %p: %zu bytes\n", temp->_ptr, temp->_size);
        temp = temp->_next;
    }

    printf("Current allocated bytes: %zu\n", TOTAL_MEMORY_ALLOCATED);
}

/*
* Function which allocates memory
* Input: Size of block to allocate
* Output: Pointer to allocated memory
*/
void* improvedMalloc(SIZE_T size)
{
    // Checking if the given size is valid
    if (size <= 0)
    {
        printf("Invalid size, please enter a number greater than 0!\n");
        return NULL;
    }

    // Allocate memory for the MemoryBlock metadata (size, pointer, etc.)
    MemoryBlock* node = (MemoryBlock*)VirtualAlloc(
        NULL,                   // Let the system determine the starting address
        sizeof(MemoryBlock),    // Size of the metadata structure
        MEM_COMMIT | MEM_RESERVE,
        PAGE_READWRITE
    );

    if (node == NULL)
    {
        printf("VirtualAlloc failed with error: %lu\n", GetLastError());
        return NULL;
    }

    node->_size = size;

    // Allocate the actual memory block
    node->_ptr = VirtualAlloc(
        NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE
    );

    if (node->_ptr == NULL)
    {
        printf("Failed to allocate memory block.\n");
        VirtualFree(node, 0, MEM_RELEASE);
        return NULL;
    }

    // Track memory allocation
    TOTAL_MEMORY_ALLOCATED += sizeof(MemoryBlock) + size;

    // Insert node into linked list
    node->_next = linkedMemoryList;
    linkedMemoryList = node;

    printf("Allocated %zu bytes at %p\n", size, node->_ptr);
    return node->_ptr;
}

/*
* Function which frees allocated memory
* Input: Pointer to allocated memory
* Output: If the freeing was successful
*/
BOOL improvedFree(void* ptr)
{
    // Check if linkedMemoryList is empty
    if (linkedMemoryList == NULL)
    {
        printf("No memory was allocated\n");
        return FALSE;
    }

    // Check if the pointer is NULL
    if (ptr == NULL)
    {
        printf("Given pointer is NULL\n");
        return FALSE;
    }

    MemoryBlock* temp = linkedMemoryList;
    MemoryBlock* prev = NULL;

    // Find the block to free
    while (temp != NULL)
    {
        SIZE_T size = temp->_size;
        if (temp->_ptr == ptr)
        {
            if (prev != NULL)
            {
                prev->_next = temp->_next;
            }
            else
            {
                linkedMemoryList = temp->_next;
            }

            // Free the allocated memory
            if (!VirtualFree(temp->_ptr, 0, MEM_RELEASE))
            {
                printf("Error freeing memory at %p\n", ptr);
                return FALSE;
            }

            // Free the metadata
            if (!VirtualFree(temp, 0, MEM_RELEASE))
            {
                printf("Error freeing memory block metadata.\n");
                return FALSE;
            }

            // Update total memory allocated
            TOTAL_MEMORY_ALLOCATED -= size + sizeof(MemoryBlock);
            printf("Memory freed successfully!\n");
            return TRUE;
        }

        prev = temp;
        temp = temp->_next;
    }

    printf("Pointer %p not found in allocated memory list.\n", ptr);
    return FALSE;
}

/*
* Function reallocates memory for a pointer while retaining the original values
* Input: Size of the new memory block, Pointer to the previous location
* Output: Pointer to the new memory location
*/
void* improvedReAlloc(SIZE_T newSize, void* ptr)
{
    // Check if pointer is NULL, or newSize is zero
    if (ptr == NULL)
    {
        return improvedMalloc(newSize);  // Allocate new memory
    }
    if (newSize == 0)
    {
        improvedFree(ptr);  // Free the current memory
        return NULL;
    }

    // Find the block in the list
    MemoryBlock* temp = linkedMemoryList;
    while (temp != NULL)
    {
        if (temp->_ptr == ptr)
        {
            // Reallocate memory (allocate a new block)
            void* newPtr = improvedMalloc(newSize);
            if (newPtr == NULL)
            {
                return NULL;
            }

            // Copy the old data into the new block
            SIZE_T copySize = (newSize < temp->_size) ? newSize : temp->_size;
            memcpy(newPtr, ptr, copySize);

            // Free the old block
            improvedFree(ptr);

            return newPtr;
        }

        temp = temp->_next;
    }

    printf("Pointer %p not found.\n", ptr);
    return NULL;
}