#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void build_index(const char *domain, char *index_url);

int main(void)
{

    return 0;
}

void build_index(const char *domain, char *index_url)
{
    strcpy(index_url, "http://www.");
    strcat(strcat(index_url, domain), "/index.html");
}
