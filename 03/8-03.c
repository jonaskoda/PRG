#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
enum {A = 4, B = 8, E = 3, I = 1, O = 0, S = 5};

int main(void)
{
    char n;
    int len;
    char a[50];


    printf("Enter a message: ");
    //scanf("%s", a);
    fgets(a, 50, stdin);
    len = strlen(a);
 
    printf("In B1FF-speak:   ");
    for (int j = 0; j < len; j++)
    {
        n = toupper(a[j]);
    
        switch (n)
        {
            case 'A':
                printf("%d", A);
                break;
            case 'B':
                printf("%d", B);
                break;
            case 'E':
                printf("%d", E);
                break;
            case 'I':
                printf("%d", I);
                break;
            case 'O':
                printf("%d", O);
                break;
            case 'S':
                printf("%d", S);
                break;
            default:
                printf("%c", n);
        }
        
    }
   

    return 0;
}
