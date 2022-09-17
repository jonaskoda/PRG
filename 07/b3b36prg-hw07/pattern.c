#include <stdio.h>

int main(int argc, char *argv[])
{
    int rows, rowcnt, spaces, symbols;
    printf("Enter the number of rows:");
    scanf("%d", &rows);

    for (rowcnt = 1; rowcnt <= rows; rowcnt++)
    {
        for (spaces = 1; spaces <= (rows - rowcnt); spaces++)
            printf(" ");
        for (symbols = 1; symbols <= ((rowcnt*2)-1); symbols++)
            printf("*");


        printf("\n");
    }
    return 0;
}
