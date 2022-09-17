#include <stdio.h>
#include <stdlib.h>


#define ERROR_INPUT 100;
#define ERROR_RANGE 101;
#define MIN_VALUE 1
#ifndef MAX_VALUE
#define MAX_VALUE 10
#endif


void print_triangle(char c, int n);
void print_line(char c, int n_line);

int main(void)
{
    char* ss = (char*)malloc(15); //pro kontrolu, jestli je vstup fakt jen int a ne treba float
    int n;
    printf("Enter number of lines in the range 1-10: ");
    int r = scanf("%d%s", &n, ss);
    int ret = EXIT_SUCCESS;
    

    if (r != 1)
    {
        ret = ERROR_INPUT;
    }
    else if(n < MIN_VALUE || n > MAX_VALUE)
    {
        ret = ERROR_RANGE;
    }
    else
    {
       print_triangle('*', n);
    }
    switch (ret)
    {
        case ERROR_INPUT: 
            fprintf(stderr, "Error, cannot read integer value from the standard input \n");
            break;
        case ERROR_RANGE:
            fprintf(stderr, "Error, entered value %d is not in range %d-%d\n", n, MIN_VALUE, MAX_VALUE);
            break;
    }
    return ret;
}

void print_triangle(char c, int n);
{
    putchar('\n');
    for (int i = 0; i < n; ++i)
    {
        print_line(c, n - i);
    }
}
void print_line(char c, int n_line);
{
    for (int j = n_line; j > 0; --j)
    {
        putchar('C');
    }
}