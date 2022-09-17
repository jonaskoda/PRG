#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

int main(void)
{
    char array[80];
    int len;
    char n;
    int i = 0;
    
    printf("Enter a sentence: ");
    do 
    {
        n = getchar();
        array[i++] = n;
    }
    while (n != '?' || n != '.' || n != '!');
    {
        n = getchar();
        array[i++] = n;
    } 
    
    return 0;
}
