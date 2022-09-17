#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define COL 24

void search(int temperatures[][COL], int row, int key);

int main(void)
{
    int row = 7;
    int key;
    int temperatures[row][COL];
    printf("enter a key: ");
    scanf("%d", &key);

    search(temperatures, row, key);
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < COL; j++)
        {
            printf("%d", temperatures[i][j]);
            if (j == 23)
            {
                printf("\n");
            }
        }
    }
}

void search(int temperatures[][COL], int row, int key)
{
    int *p, i, j;
    
    for (p = &temperatures[0][0];p < &temperatures[row-1][COL-1]; p++)
    {
        *p = 1;
    }

}



