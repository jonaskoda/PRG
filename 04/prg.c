#include <limits.h>
#include <stdio.h>
 
static int a(int i);
static int b(int i);
static int c(int i);
static int d(int i);
 
static int a(int i)
{
    int shift = sizeof(int) * 8 - 1;
 
    if ((i & (0x1 << shift)) == INT_MIN){
        return i + a(0);
    }else if (i % 2 == 0){
        return a(i + 1) - 1;
    }else{
        return i + b(i) / 2;
    }
}
 
static int b(int i)
{
    if ((i & 0x1) == 0){
        return d(i);
    }else{
        return c(i - 1);
    }
}
 
static int c(int i)
{
    int j = i / 2;
 
    if (i == 0){
        return 2;
    }else{
        return b(j);
    }
}
 
static int d(int i)
{
    int ip = i + 1;
    int im = i - 1;
 
    if (i == 1)
        return ip;
    return b(im);
}
 
int main(int argc, char *argv[])
{
    return 0;
}