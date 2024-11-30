#include "Allocator.h"
#include "time.h"

int main(int argc, char** argv)
{
    int* a = (int*)improvedMalloc(sizeof(int));
    int* b = (int*)improvedMalloc(sizeof(int) * 5);
    int* c = (int*)improvedMalloc(sizeof(int) * 3);
    c[0] = 4;
    c[1] = 12;
    c[2] = 5;

    improvedFree(a);

    c = (int*)improvedReAlloc(sizeof(int) * 50, c);

    printf("%d\n%d\n%d\n", c[0], c[1], c[2]);

    improvedFree(b);
    improvedFree(c);
    

    printf("Leaks: %zu\n", TOTAL_MEMORY_ALLOCATED);

    return 0;
}