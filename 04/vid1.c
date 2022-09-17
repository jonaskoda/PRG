#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#ifndef SIZE 
#define SIZE 10
#endif

unsigned long int my_str_len(const char *str);
// char *read_string(FILE *fid, char *str);
char *read_line(FILE *fid);
int find_match(FILE *fid, const char *pattern);
_Bool my_str_cmp(const char *str1, const char *str2);
int string_match(const char *pattern, const char *str);


int main(int argc, char *argv[])
{
    const char *pattern = argc > 1? argv[1]: NULL;
    const char *fname = argc > 2 ? argv[2]: NULL;

    if (!pattern || !fname)
    {
        fprintf(stderr, "Add pattern and filename as program argument\n");
        return 101;
    }
    FILE *fid = fopen(fname, "r");  // r for reading
    if (fid == NULL)
    {
        fprintf(stderr, "Error, cannot open file\n");
        return 102;
    }

    int ln = 1;
    char *line = read_line(fid);
    while (line != NULL)
    {
        int m = string_match(pattern, line);
        if (m >= 0)
        {
            printf("line %03d - %s\n", ln, line);
            for (int i = 0; i < 11 + m; ++i)
            {
                putchar(' ');
            }
            for (int i = 0; i < my_str_len(pattern); ++i)
            {
                putchar ('^');
            }
            putchar('\n');
        }
        ln++;
        free(line);
        line = read_line(fid);

    }

    /*
    int ln = 1;
    while (!feof(fid) && !ferror(fid))
    {
        int m = find_match(fid, pattern);
        if (m >= 0)
        {
            printf("Line %d / pattern match starts at %d\n", ln, m);
        }
        ln++;
    }
    
    char str1[SIZE+1];
    if (read_string(fid, str1) == NULL)
    {
        fprintf(stderr, "Error: reading from scanf\n");
        return 100;
    }
    printf("pattern length: %lu\n", my_str_len(pattern));
    printf("str1 length: %lu\n", my_str_len(str1));
    printf("pattern == str1 = %d\n", my_str_cmp(pattern, str1));
    */

    fclose(fid);

    return EXIT_SUCCESS;
}

unsigned long int my_str_len(const char *str)
{
    unsigned long int return_val = 0;

    while (str && str[return_val] != '\0')
    {
        return_val++;
    }

    return return_val;
}
/*
char *read_string(FILE *fid, char *str)
{
    int r = fscanf(fid, "%10s", str);
    return (r == 1) ? str: NULL;
}
*/

_Bool my_str_cmp(const char *str1, const char *str2)
{
    int i1 = 0;
    int i2 = 0;
    if (!str1 || !str2)
    {
        return str1 == str2;
    }
    while (str1[i1] != '\0' && str2[i2] != '\0' && str1[i1] == str2[i2])
    {
        i1++;
        i2++;
    }
    return str1[i1] == str2[i2];
    /*
    while (str1 && str2 && (*str1) != '\0' && (*str2) != '\0' && (*str1) == (*str2))
    {
        str1++;
        str2++;
    }

    return *str1 == *str2;
    */
}

int find_match(FILE *fid, const char *pattern)
{
    int ret = -1;
    int ch;
    int s = -1;
    int i = 0;
    int counter = 0; //number of processed characters 

    while ((ch = getc(fid)) != EOF && ch != '\n')
    {
        if (ret == -1 && pattern)
        {
            if (ch !=  pattern[i])
            {
                i = 0;
                s = -1;
            }
            if (ch == pattern[i])
            {
                if (i == 0)
                {
                    s = counter;
                }
                i++;
            }
            if (pattern[i] == '\0')
            {
                ret = s;
            }
        }
        counter++;
    }

    return ret;
}

char *read_line(FILE *fid)
{
    int capacity = SIZE;
    char *ret; 
    int ch;
    int len = 0;
    ret = malloc(capacity+1 * sizeof(int));
    if (ret == NULL)
    {
        printf("Error");
        exit(-1);
    }
    if (!ret)
    {
        fprintf(stderr, "Error, cannot allocate memory\n");
    }
    while ((ch = getc(fid)) && ch != EOF && ch != '\n')
    {
        if (len == capacity)
        {
            char *temp = realloc(ret, capacity + SIZE +1);
            if (temp == NULL)
            {
                free(ret);
                fprintf(stderr, "Error, cannot realloc\n");
                exit(-1);
            }
            ret = temp;
            capacity += SIZE;
        }
        ret[len++] = ch;
    } // end of reading chars
    if (ch == EOF && len == 0)
    {
        free(ret);
        ret = NULL;
    }
    else
    {
        ret[len] = '\0';
    }
    return ret;
}

int string_match(const char *pattern, const char *str)
{
    int ret = -1;

    if (pattern && str)
    {
        int match_start = 0;
        int i1 = 0;
        int i2 = 0;
        while (pattern[i1] != '\0' && str[i2] != '\0')
        {
            if (str[i2] != pattern[i1])
            {
                i1 = 0;
            }
            if (str[i2] == pattern[i1])
            {
                if (i1 == 0)
                {
                    match_start = i2;
                }
                i1++;
            }
            i2++;
        }
        if (pattern[i1] == '\0')
        {
            ret = match_start;
        }
    }
    return ret;
}
