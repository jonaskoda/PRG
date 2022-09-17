#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int *create_array(int n, int initial_value);

int main(int argc, char *argv[])
{
    int number, value;
    number = 10;
    value = 2;
    int *q, i;

    q = create_array(number, value);

    for ( i = *q; i < *q + number; i++)
    {
        printf("%d", q[i]);
    }

    return 0;
}

int *create_array(int n, int initial_value)
{
    int *p, *a;
    p = malloc(n * sizeof(int));
    if (p == NULL)
    {
        printf("Error: malloc failed\n");
        return NULL;
    }
    for (a = p; a < p + n; p++)
    {
        *a = initial_value;
    }
    return p;
}
