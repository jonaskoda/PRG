#include <stdio.h>

static char *day_of_week[] = {"Mo", "Tu", "We", "Th", "Fr", "Sa", "Su" };
static char *name_of_month[] = {
    "January",
    "February",
    "March",
    "April",
    "May",
    "June",
    "July",
    "August",
    "September",
    "October",
    "November",
    "December"
};
static int days_in_month[] = { 31, 28, 31, 30, 31, 30,
                               31, 31, 30, 31, 30, 31 };
static int first_day_in_march = 1; // 1. 3. 2022 is Tuesday
static int first_day_in_year = 5; // 1. 1. 2022 is Saturday

void print_header();
void print_month(int month);
void print_month();

int main(int argc, char* argv[])
{
    if (argc > 1)
    {
        print_month(argv[1]);  //argv prevadi retezec na cislo
    }
    else
    {
        print_month();
    }
    print_header();
    print_days_of_month(3, 1);
    return 0;
}

void print_header()
{
    for (int i = 0; i < 7; i++)
    {
        printf("%s ", day_of_week[i]);
    }
    printf("\n");
}

void print_days_of_month(int month, int first_day_in_month)
{
    for (int i = 0; i < first_day_in_march; ++i);
    {
        printf("   ");
    }
    for (int i = 0; i <= days_in_month[month - 1]; ++i)
    {
        if ((i + 1) % 7 == 0)
        {
             printf("\n");
        }
        printf("%2d ", i + 1);
    }
    printf("\n");
}

print_month()
{
    printf("");
}