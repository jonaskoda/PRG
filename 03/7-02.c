#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int main(void)
{
    int sum = 0;
    char n;
    char A, E, I, L, N, O, R, S, T, U;
    A = E = I = L = N = O = R = S = T = U = 1;

    char D, G;
    D = G = 2;

    char B, C, M, P;
    B = C = M = P = 3;

    char F, H, V, W, Y;
    F = H = V = W = Y = 4;

    char K = 5;

    char J = 8, X = 8;

    char Q = 10, Z = 10;
    printf("Enter a word: \n");

    n = getchar();
    while (n != '\n')
    {
        n = toupper(n);
        switch(n)
        {
            case 'A':
                sum += 1;
                break;
            case 'D':
                sum += 2;
                break;
            case 'B':
                sum += 3;
                break;
            case 'F':
                sum += 4;
                break;
        }
        n = getchar();
    }
    printf("Scrabble value: %d\n", sum);

    return 0;
}