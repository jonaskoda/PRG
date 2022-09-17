#include <stdio.h>

#define N 10

void find_two_largest(int a[], int n, int *largest, int *second_largest);

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

void find_two_largest(int a[], int n, int *largest, int *second_largest)
{
    *largest = a[0];
    *second_largest = a[0];

    for (int i = 1; i < N; i++)
    {
        if (a[i] > *largest)
        {
            *largest = a[i];   
        }
        if ((*second_largest < a[i]) && (a[i] < *largest))
        {
            *second_largest = a[i];
        }
    }

}
