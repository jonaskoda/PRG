#include <stdio.h>

int main(int argc, char *argv[])
{
    int rows, rowcnt, symbols;
    printf("Enter the number of rows:");
    scanf("%d", &rows);

    for (rowcnt = 1; rowcnt < rows; rowcnt++)
    {
        printf("*"); 
        for (symbols = 1; symbols <= (rowcnt); symbols++)
        {
            if (symbols == rowcnt-1)
                printf("*");
            else
                printf(" ");
        }
        printf("\n");
    }

    for (int i = 0; i < rows; i++)  // last line
    {
        printf("*");
    }
    return 0;
}
