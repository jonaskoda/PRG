#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define N 100

int read_line(char str[], int n);
void capitalize(char str[]);

int main(void)
{
    char str[N]; 
    int len;

    printf("Enter something: ");
    len = read_line(str, N);

    capitalize(str);
    int i = 0;
    while (str[i++] != '\0')
    {
        putchar(str[i]);
    }

    return 0;
}

int read_line(char str[], int n)
{
    int ch, i = 0;

    while ((ch = getchar()) != '\n')
    {
        if (i < n)
        {
            str[i++] = ch;
        }
    }
    str[i] = '\0';
    return i;
}

void capitalize(char str[])
{
    int i = 0;
    while (str[i] != '\0')
    {
        if (isalpha(str[i]))
        {
            toupper(str[i]);
        }
        i++;
    }
}