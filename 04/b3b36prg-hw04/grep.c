#include <stdio.h>
#include <stdlib.h>

#define N 50

enum
{
    FOUND = 1,
    NOT_FOUND = 0,
};

// Functions
void check_file(FILE *fptr);
void open_file(FILE **f, const char *fname);
void return_error();
char *read_line(FILE *f);
char *make_array(int len);
char *resize(char *line, int new_len);
int find_match(const char *pattern, const char *line);
void free_line(char *line);
int get_len(const char *string);

// Main program
int main(int argc, char *argv[])
{
    const char *pattern = argc > 1? argv[1]: NULL; // loading pattern argument, else null ptr
    const char *fname = argc > 2 ? argv[2]: NULL;  // loading [FILE], else null ptr
    int output_cnt = 0;
    
    FILE *f = NULL;
    if (argc == 3)                     // input as a [FILE]
    {
        open_file(&f, fname);
    }    
    else if (argc == 2)                // input as a stdin
    {
        f = stdin;
    }
    else 
    {
        return_error();
    }
    char *line = read_line(f);
    while (line != NULL)               // main loop for finding similarity in each line
    {   
        if (find_match(pattern, line)) // look for a match with a given pattern 
        {
            printf("%s\n", line);      // print the line that matches 
            output_cnt++;
        }
        free_line(line);
        line = read_line(f);
    }
    fclose(f);

    if (output_cnt)
        return EXIT_SUCCESS;
    else
        return EXIT_FAILURE;
}

void check_file(FILE *fptr)
{
    if (fptr == NULL)
    {
        fprintf(stderr, "Error, cannot open file\n");
        exit(101);
    }
}

void open_file(FILE **f, const char *fname)
{
    *f = fopen(fname, "r");  // r for reading a file
    check_file(*f);
}

void return_error()
{
    fprintf(stderr, "Error, input unrecognized\n");
    exit(102);
}

char *read_line(FILE *f)
{
    char *line;
    char ch;
    int len = 0;
    int capacity = N;

    line = make_array(N);                         // initialize an array
    while ((ch = fgetc(f)) != '\n' && ch != EOF)  // load input char after char
    {
        if (len >= capacity)                      // realloc condition
        {
            line = resize(line, (capacity + N));
            capacity += N;
        }
        line[len++] = ch;                   
    }
    if (len == 0 && ch == EOF)
    {
        free(line);
        line = NULL;
    }
    else 
    {
        line[len++] = '\0';                       // create a string
    }
    return line;
}

char *make_array(int len)
{
    char *text;

    text = malloc(len * sizeof(char));
    if (text == NULL)
    {
        fprintf(stderr, "Malloc error");
        exit(-1);
    }
    return text;
}

char *resize(char *line, int new_len)
{
    char *temp;

    temp = realloc(line, new_len);
    if (temp == NULL)
    {
        free(line);
        fprintf(stderr, "Error, cannot realloc\n");
        exit(-1);
    }
    line = temp;

    return line;
}

int find_match(const char *pattern, const char *line)
{
    int pat_len = get_len(pattern);
    int line_len = get_len(line);
    int i = 0;

    if (pat_len == 0)
    {
        return FOUND;
    }
    while (line[i] != '\0')           // comparison loop
    {
        if (pattern[0] == line[i])    // first matching char found
        {
            int j = 0; 
            while (pattern[j] == line[i+j] && j < pat_len && j+i < line_len)  
            {
                j++;                  // continuing the comparison with rest of the line
            }
            if (j == pat_len)         // all chars of pattern match with the ones in the line
            {
                return FOUND;
            }
        }
        i++;
    }
    return NOT_FOUND;
}

void free_line(char *line)
{
    if (line != NULL)
    {
        free(line);
    }
}

int get_len(const char *string)
{
    int counter = 0;
    while (string && string[counter] != '\0')
    {
        counter++;
    }
    return counter;
}

