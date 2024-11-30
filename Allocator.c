#include "Allocator.h"

// Array which stores all allocated memory blocks
MemoryBlock *linkedMemoryList = NULL;

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
        printf("Size of memory block at address %p: %d bytes\n", temp->_ptr, temp->_size);
        temp = temp->_next;
    }
}

/*
* Function which allocates memory
* Input: Size of block to allocate
* Output: Pointer to allocated memory
*/
void* improvedMalloc(SIZE_T size)
{
    // Checking if the given size is 0 or less
    if (size <= 0)
    {
        printf("Invalid size, please enter a number greater than 0!\n");
        return NULL;
    }

    // Allocating memory for the new node
    MemoryBlock* node = (MemoryBlock*)VirtualAlloc(
        NULL,                   // Let the system determine the starting address
        sizeof(MemoryBlock),    // Size of the Node
        MEM_COMMIT | MEM_RESERVE, // Reserve and commit the memory
        PAGE_READWRITE       // Allow read and write access
    );
    
    // Placing the size in the node
    node->_size = size;

    if (node == NULL)
    {
        printf("VirtualAlloc failed with error : % lu\n", GetLastError());
        return NULL;
    }

    // Allocating the memory
    node->_ptr = VirtualAlloc(
        NULL,                // Let the system determine the starting address
        size,                // Size of the memory block
        MEM_COMMIT | MEM_RESERVE, // Reserve and commit the memory
        PAGE_READWRITE       // Allow read and write access
    );

    printf("memory address: %p\n", node->_ptr);

    // Checking if the memory allocated properly
    if (node->_ptr == NULL)
    {
        printf("VirtualAlloc failed with error : % lu\n", GetLastError());
        return NULL;
    }

    // If the list is empty, set the new node as the head and return its pointer
    if (linkedMemoryList == NULL)
    {
        linkedMemoryList = node;
        node->_next = NULL; // Ensure the new node's next pointer is NULL
        return node->_ptr;
    }

    // Traverse to the end of the list
    MemoryBlock* current = linkedMemoryList;

    while (current->_next != NULL)
    {
        current = current->_next;
    }

    // Append the new node
    current->_next = node;
    node->_next = NULL; // Ensure the new node's next pointer is NULL

    return node->_ptr;
}

/*
* Function which frees allocated memory
* Input: Pointer to allocated memory
* Output: If the freeing was successful
*/
BOOL improvedFree(void* ptr)
{
    // Checking if the list of memory blocks is null
    if (linkedMemoryList == NULL)
    {
        printf("No memory was allocated\n");
        return FALSE;
    }

    // Checking if the given pointer is null
    if (ptr == NULL)
    {
        printf("Given pointer is a NULL\n");
        return FALSE;
    }

    // Handle the case where the head node is the one to remove
    if (linkedMemoryList->_ptr == ptr) 
    {
        MemoryBlock* removedNode = linkedMemoryList;

        // Update the head pointer
        linkedMemoryList = linkedMemoryList->_next; 

        // Free the memory associated with the node
        if (removedNode->_ptr) 
        {
            // Free the allocated memory
            if (!VirtualFree(removedNode->_ptr, 0, MEM_RELEASE))
            {
                printf("Error freeing memory\n");
                return FALSE;
            }
        }

        // Free the memory block node
        if (VirtualFree(removedNode, 0, MEM_RELEASE))
        {
            return !FALSE;
        }

        printf("Error freeing memory\n");
        return FALSE;
    }

    MemoryBlock* temp = linkedMemoryList;

    // Traverse the list to find the node to remove
    while (temp->_next != NULL) {
        if (temp->_next->_ptr == ptr) {
            // Node found, remove it
            MemoryBlock* removedNode = temp->_next;

            // Update the link to skip the removed node
            temp->_next = temp->_next->_next; 

            // Free the memory associated with the node
            if (removedNode->_ptr) 
            {
                // Free the allocated memory
                if (!VirtualFree(removedNode->_ptr, 0, MEM_RELEASE))
                {
                    printf("Error freeing memory\n");
                    return FALSE;
                }
            }
            // Free the memory block node
            if (VirtualFree(removedNode, 0, MEM_RELEASE))
            {
                return !FALSE;
            }

            printf("Error freeing memory\n");
            return FALSE;
        }

        // Move to the next node
        temp = temp->_next; 
    }

    return FALSE;
}

/*
* Function reallocates memory for a pointer while retaining the original values
* Input: Size of the new memory block, Pointer to the previous location
* Output: Pointer to the new memory location
*/
void improvedReAlloc(SIZE_T newSize, void* ptr)
{
    // If the pointer to the given array is null, print an error message and return null
    if (ptr == NULL)
    {
        printf("Given array is null, aborting\n");
        return NULL;
    }

    // If the new size is 0, simply free the previous array
    if (newSize == 0)
    {
        printf("Size is 0, Freeing the memory\n");
        improvedFree(ptr);
        return NULL;
    }

    // Creating the new array
    void* newPtr = basicMalloc(newSize);

    // Placing the new array in the linked list
    MemoryBlock* temp = linkedMemoryList;
    while (temp != NULL)
    {
        // If the pointer is equal to the given, place the new one into the link
        if (temp->_ptr == ptr)
        {
            temp->_ptr = newPtr;
            temp->_size = newSize;
            break;
        }

        // Iterate to the next link
        temp = temp->_next;
    }

    // Copying the contents of the old array into the new one
    memcpy(newPtr, ptr, newSize);

    // Freeing the previous array
    improvedFree(ptr);

    // Returning the pointer to the new array
    return newPtr;
}

/*
* Function which allocates memory
* Input: Size of block to allocate
* Output: Pointer to allocated memory
*/
void* basicMalloc(SIZE_T size)
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
BOOL basicFree(void* allocatedPtr)
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
void* basicReAlloc(SIZE_T newSize, void* ptr)
{
    // If the pointer to the given array is null, print an error message and return null
    if (ptr == NULL)
    {
        printf("Given array is null, aborting\n");
        return NULL;
    }

    // If the new size is 0, simply free the previous array
    if (newSize == 0)
    {
        printf("Size is 0, Freeing the memory\n");
        basicFree(ptr);
        return NULL;
    }

    // Allocating memory for the new array
    void* newPtr = basicMalloc(newSize);

    // Checking if the allocation was successful
    if (newPtr == NULL)
    {
        return NULL;
    }

    // Copying the contents of the old array into the new one
    memcpy(newPtr, ptr, newSize);

    // Freeing the previous array
    basicFree(ptr);

    // Returning the pointer to the new array
    return newPtr;
}