#include <stdio.h>

void swap(int *p, int *q);
void split_time(long int total_sec, int *hr, int *min, int *sec);

int main(void)
{
    /*
    int i,j;
    printf("enter i and then j: ");
    scanf("%d %d", &i, &j);

    swap(&i, &j);
    printf("value after swapping: %d %d", i, j);
    */

    long int total_sec;
    int hr, min, sec;

    printf("enter time from midnight in seconds: ");
    scanf("%ld", &total_sec);
    split_time(total_sec, &hr, &min, &sec);
     
    printf("after modifying: hours - %d minutes - %d seconds - %d", hr, min, sec);

    return 0;
}

void swap(int *p, int *q)
{
    int temp;
    temp = *p;
    *p = *q;
    *q = temp;
}

void split_time(long int total_sec, int *hr, int *min, int *sec)
{
    int i = 0;


    *hr = total_sec / 3600;

    total_sec = total_sec % 3600;

    *min = total_sec / 60;

    total_sec = total_sec % 60;

    *sec = total_sec;


}
