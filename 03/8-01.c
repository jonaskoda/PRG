#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int main(void)
{
    bool digit_seen[10] = {false};
    bool digit_added[10] = {false};
    int seen[10] = {0};
    int digit;
    long int n;
    int index = 0;
    bool repeated = false;

    printf("Enter a number: ");
    scanf("%ld", &n);

    while (n > 0)
    {
        digit = n % 10;
        if (digit_seen[digit])
        {
            if (digit_added[digit] == false)  
            {  
            seen[index++] = digit;
            digit_added[digit] = true;
            repeated = true;
            }
        }
        digit_seen[digit] = true;
        n = n / 10;
    }
    if (repeated)
    {
        printf("Repeated digit(s): ");
        for (int j = 0; j < index; j++)
        {
            printf("%d ", seen[j]);
        }
    }
    else
    {
        printf("No repeated digit/s\n");
    }

    return 0;
}

