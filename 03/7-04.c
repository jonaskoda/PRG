#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    int mins, hours;
    char part;

    printf("Enter a 12-hour time: ");
    scanf("%2d:%2d %c", &hours, &mins, &part);

    if (part == 'A')
    {
        printf("Equivalent 24-hour time: %2d:%2d %c", hours, mins, part);
    }
    else if (part == 'P')
    {
        if (hours != 12)
        {
            hours = hours % 12;
        }
        printf("Equivalent 24-hour time: %2d:%2d %c", hours, mins, part);
    }
   
}