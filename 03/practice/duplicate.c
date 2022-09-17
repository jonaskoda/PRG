#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

char *duplicate(char str[]);

int main(int argc, char *argv[])
{
    
    return 0;
}

char *duplicate(char str[])
{
    char *p;
    p = malloc(strlen(str)+1 * sizeof(char));
    if (p == NULL)
    {
        printf("Error: malloc failed\n");
        return NULL;
    }
    strcpy(p, str);
    return p;
}