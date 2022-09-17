#include <stdio.h>

#define N 10

void print_array(int arr[], int n);
void *create_rand_arr(int arr[], int n);

int main()
{
    int arr[N] = {2,3,4,5,1,9,8,4,5,7};
    print_array(arr, N);

    srand(time(NULL));
    int* arr1 = create_rand_array(15);
    print_array(arr1, 15);
    free(arr1);

    return 0;
}

void print_array(int arr[], int n)
{
    for (int i = 0; i < n; i++)
    {
        printf("%d", arr[i]);
    }
    printf("\n");
}

void *create_rand_arr(int arr[], int n)
{
    int* arr = (int*) malloc(n * sizeof(int));
    if (arr = NULL)
    {
        printf("Error");
        exit(100);
    }
    for (int i = 0; i < n; i++)
    {
        arr[i] = rand() % 10;
    }
    return arr;
}
