#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define N 10

void find_two_largest(const int *a, int n, int *largest, int *second_largest);

int main(void)
{
    int first, second;
    int a[N];

    printf("Enter %d elements: ", N);
    for (int i = 0; i < N; i++)
    {
        scanf("%d", &a[i]);
    }
    find_two_largest(a, N, &first, &second);

    printf("largest: %d second: %d", first, second);

    return 0;
}

void find_two_largest(const int *a, int n, int *largest, int *second_largest)
{
    const int *p;
    p = a;
    *largest = *p;
    *second_largest = *p;
    
    for (p = a+1; p < a + n; p++)
    {
        if (*p > *largest)
        {
            *largest = *p;
        }
        if ((*second_largest < *p) && (*p < *largest))
        {
            *second_largest = *p;
        }
    }
}

