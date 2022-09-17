#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#define SIZE 10

void print_array(double arr[], int n);
double average(double* arr, int n);
double deviation(double *arr, int n);
int read_array(double arr[]);

int main()
{
    double arr[] = {23.32, 43.65, 23.54, 43.64, 23.54, 23.64, 65.32, 23.53, 23.64, 34.54};
    print_array(arr, SIZE);
    printf("Prumer prvku %.2lf je \n", average(arr,SIZE));
    printf("Smerodatna odchylka je: %.2lf\n", deviation(arr, SIZE));
    double *new_arr = malloc(SIZE *sizeof(double));
    if (new_arr == NULL)
    {
        printf("Error");
        exit(100);
    }
    int n = read_array(&new_arr);
    print_array(new_arr, n);

    

    free(new_arr);

    
    return 0;
}

void print_array(double arr[], int n)
{
    for (int i = 0; i < n; i++)
    {
        printf("%5.2lf", arr[i]);
    }
    printf("\n");
}

double average(double* arr, int n)
{
    double sum = 0.0;
    for (int i = 0; i < n; i++)
    {
        sum += arr[i];
    }
    return sum;
}

double deviation(double *arr, int n)
{
    double avg = average(arr, n);
    double sum = 0.0;
    for (int i = 0; i < n; i++)
    {
        sum += (arr[i] - avg) * (arr[i] - avg);
    }
    return sqrt(sum / (n - 1));
}

int read_array(double arr[])
{
    int r, i = 0;
    while ((r = scanf("%lf", &arr[i++])) == 1 && 1 < SIZE)
    {
        i++;
        
    }
    return i;

}
