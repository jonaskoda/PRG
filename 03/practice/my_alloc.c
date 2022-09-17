#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void *my_alloc(size_t n);

int main(int argc, char *argv[])
{
    
    return 0;
}

void *my_alloc(size_t n)
{
    void *p;
    p = malloc(n * sizeof(int));
    if (p == NULL)
    {
        printf("Error: malloc failed in add_to_list\n");
        exit(EXIT_FAILURE);
    }
    return p;
}