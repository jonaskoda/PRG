#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int main(void)
{
    int seen_count[10] = {0};
    int digit;
    long int n;


    printf("Enter a number: ");
    scanf("%ld", &n);

    while (n > 0)
    {
        digit = n % 10;
        
        seen_count[digit] += 1;
        n = n / 10;
    }
    printf("Digit:       ");
    for (int i = 0; i < 10; i++)
    {
        printf("%d  ", i);
    }
    printf("\n");
    printf("Occurrences: ");
    for (int i = 0; i < 10; i++)
    {
        printf("%d  ", seen_count[i]);
    }
  

    return 0;
}

