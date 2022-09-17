#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

bool search(int a[], int n, int key);

int main(void)
{
    int n = 10;
    int key;
    int a[] = {1,2,3,4,5,6,7,8,9,10};
    printf("enter a key: ");
    scanf("%d", &key);
    bool result;
    result = search(a, n, key);

    if (result == true)
    {
        printf("True");
    }
    else
    {
        printf("False");
    }
    
}

bool search(int a[], int n, int key)
{
    int *p;
    for (p = a; p < a + n; p++)
    {
        if (*p == key)
        {
            return true;
        }
    }
    if (*p != true)
    {
        return false;
    }
}
