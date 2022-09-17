#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define LARGEST 20

void compare(char word[],char *largest, char *smallest);

int main(void)
{
    char smallest[LARGEST+1];
    char largest[LARGEST+1];
    char word[LARGEST+1];

 

    printf("Enter word: ");
    scanf("%20s", word);
    strcpy(smallest, word);
    strcpy(largest, word);
    while (strlen(word) != 4)
    {
        compare(word, largest, smallest);
        scanf("%20s", word);
    }

    printf("smallest: %s\n", smallest);
    printf("largest: %s\n", largest);


    return 0;
}

void compare(char word[], char *largest, char *smallest)
{
    if (strcmp(word, smallest) < 0)
    {
        strcpy(smallest, word);
    }
    else if (strcmp(word, largest) > 0)
    {
        strcpy(largest, word);
    }
}