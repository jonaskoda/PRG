#include <stdio.h>
#include <string.h>

int lenstr(char* str);
int cmpstr(char* str1, char* str2);

int main()
{
    char str[] = "Dnes je hezky den";
    printf("Delka retezce je %d\n", lenstr(str));

    printf("%d\n", cmpstr(str, "Dnes je nej den"));

    return 0;
}

int cmpstr(char* str1, char* str2)
{
    if (lenstr(str1) != lenstr(str2))
    {
        return -1;
    }
    for (int i = 0; i < lenstr(str1); i++)
    {
        if (str1[i] != str2[i])
        {
            return -i;
        }
    }    
    return 1;
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
