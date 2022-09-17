#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

#define N 1000000
#define ONE 1
#define ERROR -1
#define WRONG_INPUT 100
#define TRUE 1
#define FALSE 0

// functions
long int read_input(void);
int eratosthenes(int *arr);
void make_prim_array(int *arr, int *primes, int len);
void prime_factoring(int *primes,long int num, int prime_count);


// The main program
int main(int argc, char *argv[])
{
    static int arr[N];                    // declaring an array of len 10**6
    long int n;
    int prime_count = eratosthenes(arr);  // finding primes + returning num of primes found
    int primes[prime_count];              // initializing an array of size as num of primes
    make_prim_array(arr, primes, prime_count);                 // putting primes into array

    n = read_input();                     // scanning input arguments
    while (n != 0)
    { 
        if (n == ERROR)
        {
        fprintf(stderr, "Error: Chybny vstup!\n");
        return WRONG_INPUT;
        }   
        printf("Prvociselny rozklad cisla %ld je:\n", n);
        prime_factoring(primes, n, prime_count);
        n = read_input();
    } 
    return EXIT_SUCCESS;
}

long int read_input(void)
{
    long int n;
    int input_count;
    input_count = scanf("%ld\n", &n);
    if ((input_count != ONE) || (input_count == ONE && (n < 0)))
    {
        n = ERROR;   
    }
    return n;
}

int eratosthenes(int *arr)
{
    int prime_counter = 0;
    int square = sqrt(N);
    arr[0] = 1;
    arr[1] = 1;
    for (int i = 2; i <= square; i++)
    {
        if (arr[i] == 0)
        {
            prime_counter++;            //count the prime
            for (int j = i; j*i <= N; j++)
            {
                arr[j*i] = 1;
            }
        }
    }
    for (int j = square+1; j <= N; j++) //complete the counting of primes
    {
        if (arr[j] == 0)       
        {
            prime_counter++;
        }
    }
    return prime_counter;
}   

void make_prim_array(int *arr, int *primes, int len)
{                                      // looping through long array
    int index_count = 0;               // and finding primes
    for (int i = 0; i <= N; i++)   
    {
        if (arr[i] == 0)
        {
            primes[index_count] = i;   //adding primes to new array
            index_count++;  
        }
    }
}

void prime_factoring(int *primes,long int num, int prime_count)
{
    int first = TRUE;
    int i = 0;
    
    while ((primes[i] <= num) && (i < prime_count))
    {
        if (num % primes[i] == 0)
        {
            int counter = 0;
            while (num % primes[i] == 0)
            {
                num = num / primes[i];
                counter++; 
            }
            if (first == FALSE)
            {
                printf(" x ");
            }
            if (counter == 1)
            {
                printf("%d",primes[i]);
            }
            else
            {
                printf("%d^%d",primes[i], counter);
            }
            first = FALSE;
        }
        i++;
    }
    if (num != 1)                    // case for prime as an input
    {
        printf("%ld", num);
    }
    else if ((num == 1) && (i == 0)) // case for 1 as an input
    {
        printf("%ld", num);
    }    
    printf("\n");
} 

