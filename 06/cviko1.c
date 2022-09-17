#include <stdio.h>
#include <string.h>

int main()
{
    char str[100];
    char new_str[100];
    char orig[] = 'aeiou';
    char rewrite[] = '12345';
    char ch;
    int len;


    printf("Enter a string: ");
    fgets(str ,100,stdin);

    len = strlen(str);
    for (int i = 0; i < len; i++)
    {
        char* swap = strchr(orig, str[i]);
        if (swap == NULL)
        {
            new_str[i] == str[i];
        }
        else
        {
            new_str[i] = rewrite[swap - orig];
        }

    }
    printf("%s\n", new_str);

    return 0;
}