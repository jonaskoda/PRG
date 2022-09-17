#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


int main(int argc, char *argv[])
{
    int sum = 0;

    for (int i = 1; i <(argc-1); i++)
    {
        sum += atoi(argv[i]);
    }
    printf("%d",sum);
    return 0;
}


