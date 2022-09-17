#include <stdio.h>
#include <stdlib.h>

#define REALLOC_CONST 15

enum 
{
    NO_END_LINE = 0,
    END_LINE = 1,
    END_FILE = 2,
};
enum
{
    FIND = 1,
    NOT_FOUND = 0,
};
// Functions
typedef struct 
{
    char *line;
    int size;
    int capacity; // allocated memory for the line 
} str_line;

void init_line(str_line *my_line);
void resize_line (str_line *my_line);
int read_line(str_line *my_line, FILE *f);
void free_line(str_line *my_line);
void open_file(FILE **f, char *name_file);
void close_file(FILE **f);
int find_pattern(str_line *my_line, char *pattern);
int get_length(char *my_str);

// The main program 
int main(int argc, char *argv[])
{
    str_line line;
    FILE *f;
    int finish_read = NO_END_LINE;
    open_file(&f, argv[2]);
    while (finish_read != END_FILE) 
    {
        init_line(&line);
        finish_read = read_line(&line, f);
        if (find_pattern(&line, argv[1]))
        {
            printf("%s\n", line.line);
        }
        free_line(&line);
    } 

    close_file(&f);
    
  
    return EXIT_SUCCESS;
}

void init_line(str_line *my_line)
{
    my_line -> line = (char*) malloc(sizeof(char));
    my_line -> size = 0;
    my_line -> capacity = 1;
    my_line -> line[0] = '\0';
}

void resize_line (str_line *my_line) // realloc
{
    my_line -> capacity += REALLOC_CONST;
    my_line -> line = realloc(my_line -> line, my_line -> capacity * sizeof(char));
}

void free_line(str_line *my_line)
{
    if (my_line -> line != NULL)
    {
        free(my_line -> line);
    }
}

void open_file(FILE **f, char *name_file)
{
    *f = fopen(name_file, "r");
    if (*f == NULL)
    {
        exit(1);
    }
}

int read_line(str_line *my_line, FILE *f)
{
    int finish_line = NO_END_LINE;
    while(!finish_line)
    {
        char ch = fgetc(f);
        if (my_line -> size + 2 >= my_line -> capacity)
        {
            resize_line(my_line);
        }
        switch (ch)
        {
            case '\n':
            {
                finish_line = END_LINE;
                break;
            }
            case EOF:
            {
                finish_line = END_FILE;
                break;
            }
            default:
            {
                my_line -> line[my_line -> size++] = ch;
                break;
            }
        }
    }
    my_line -> line[my_line -> size++] = '\0';
    return finish_line;
}

void close_file(FILE **f)
{
    if (fclose(*f) == EOF)
    {
        exit(1);
    }
}

int get_length(char *my_str)
{
    int temp = 0;
    while(my_str[temp])
    {
        temp++;
    }
    return temp;
}

int find_pattern(str_line *my_line, char *pattern)
{
    int len = get_length(pattern);
    int i = 0;
    if (len == 0)
    {
        return FIND;
    }
    while (my_line -> line[i])
    {
        if (my_line -> line[i] == pattern[0])
        {
            int j = 1;
            while (j < len && i + j < my_line -> size && my_line -> line[i + j] == pattern[j])
            {
                j++;
            }
            if (j == len)
            {
                return FIND;
            }
        }
        i++;
    }
    return NOT_FOUND;
}
