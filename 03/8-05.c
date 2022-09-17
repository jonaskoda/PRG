#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

int main(void)
{
    char array[80];
    char n;
    int i = 0;
    int key; 

    printf("Enter message to be encrypted: ");
    do 
    {
    n = getchar();
    array[i++] = n;
    }
    while (n != '\n');
    {
        n = getchar();
        array[i++] = n;
    }
    
    for (int j = 0; j < (int)sizeof(array) / sizeof(array[0]); j++)
    {
        putchar(array[j]);
    }

    printf("Enter shift amount (1-25): ");
    scanf("%d", &key);

    printf("Encrypted message: ");
    for (int j = 0; j < strlen(array); j++)
    {
        n = array[j];
        if (n >= 'A' && n <= 'Z')
        {
            n = ((n - 'A') + key) % 26 + 'A';
            putchar(n);
        }
        else if (n >= 'a' && n <= 'z')
        {
            n = ((n - 'a') + key) % 26 + 'a';
            putchar(n);
        }
        else
        {
            putchar(array[j]);
        }
    }
    return 0;
}
