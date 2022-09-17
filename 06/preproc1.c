#include <stdio.h>
#include <stdlib.h>

#define CUBE(x) (x*x)
#define REMAINDER(n) (n%4)
#define PRODUCT(x,y) ((x+y)< 100 ? 1 else 0)

int main()
{
    printf("%d\n", CUBE(10));
    printf("%d\n", REMAINDER(10));
    printf("%d\n", PRODUCT(10,10));
    return 0;
}
