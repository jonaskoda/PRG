#include <stdio.h>
#include <stdlib.h>

int read_input();

int main(void)
{
    char n;
    int A = 2;
    int B = 2;
    int C = 2;
    int D = 3, E = 3;
    
    printf("Enter phone number: \n");
    n = getchar();
    while (n != '\n')
    {
        switch(n)
        {
            case 'A':
                printf("%d", A);
                break;
            case 'B':
                printf("%d", B);
                break;
            case 'C':
                printf("%d", C);
                break;
            case 'D':
                printf("%d", D);
                break;
            case 'E':
                printf("%d", E);
                break;
        }
        n = getchar();

    }
    return 0;
}

