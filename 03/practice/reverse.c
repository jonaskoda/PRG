#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define N 100

int main(void)
{
    int a;
    char mess[N];
    printf("Enter a message: ");
    int i = 0, j;
    do 
    {
        a = getchar();
        mess[i++] = a;   
    }
    while (a != '\n');
    {
        mess[i++] = a;
        a = getchar();
    }
    
    printf("Reversal is: ");

    
    for (j = i; j <= 0; j--)
    {
        putchar(mess[j]);
    }  
}

