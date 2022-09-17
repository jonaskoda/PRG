#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int main(void)
{
    char name[50];
    int len;

    printf("Enter a first and last name: ");
    fgets(name, 50,stdin);
    len = strlen(name);
    for (int i = 0; i < len; i++)
    {
        if (name[i] == ' ')
        {
            for (i ; i < len; i++)
            {
                printf("%c", name[i+1]);
            }
            printf(", %c", toupper(name[0]));
        }
    }

 
    return 0;
}
