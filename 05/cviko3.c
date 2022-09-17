#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define M 1000

int lenstr(char* str);

char *read_str();

int main()
{
    char input[M];
    char z;
    int idx = 0;
    bool space = false;

    while (scanf("%c", &z) == 1)
    {
        if (z == ' ' || z == '\n' || z == '\t')
        {
            if (!space)
            {
                memset(&input[0], 0, M);
                idx = 0;
                space = true;
            }
            else
            {
                printf("%s %d\n", input, lenstr(input));
                memset(&input[0], 0, M);
                idx = 0;
            }
        }
        else
        {
            input[idx++] = z;
        }
        input[idx++] = z;

    }
    //input[idx] = '\0';

    printf("%s\n", input);

    return 0;
}

int lenstr(char* str)
{
    int idx = 0;
    while (str[idx++])
    {
        ;
    }
    return idx;
}

char *read_str()
{
    int idx = 0;
    char z;
    int lnstr = 10;
    char *str;
    str = (char*) malloc(lnstr);
    while (scanf("%c", &z) == 1)
    {
        str[idx++] = z;
        if (lnstr == idx)
        {
            lnstr *= 2;
            //free(str);
            str = (char*) realloc(str, lnstr);
            if(str == NULL)
            {
                exit(100);
            }
        }
    }
}
