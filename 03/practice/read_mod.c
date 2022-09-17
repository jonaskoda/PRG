#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int read_line(char str[], int n);

int main(void)
{

    return 0;
}

int read_line(char str[], int n)
{
    int ch, i = 0;

    while (isspace(getchar()))
    {
        ;
    }

    while ((isspace(getchar()) == false) && (ch = getchar() != '\n') && (i < n))
    {
         str[i++] = ch;
    }
    if (ch == '\n' && i < n-1)
    {
        str[i++] = '\n';
    }
    
    str[i] = '\0';
    return i;
}
