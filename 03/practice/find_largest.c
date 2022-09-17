#include <stdio.h>

#define N 10

int *find_largest(int a[], int n);

int main(void)
{
    int a[N];
    int *p;


    p = find_largest(a, N);
    printf("%d ", *p);

    return 0;
}

int *find_largest(int a[], int n)
{
    static int max;
    max = a[0];

    for (int i = 1; i < n; i++)
    {
        if (a[i] > max)
        {
            max = a[i];
        }
    }
    return (&max);
}