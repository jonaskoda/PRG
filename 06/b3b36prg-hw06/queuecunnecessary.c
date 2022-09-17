#include <stdio.h>
#include<stdlib.h>
#include<assert.h>
#include "queue.h"

enum 
{
    NO_FILE = 100,
    NOT_OPENED = 101,
    MALLOC_ERROR = 102,
    WRONG_SIZE = 103,
    QUEUE_ERROR = 104,
    WRONG_COMMAND = 105,
};

// functions
FILE *open_file(const char *fname); 
int *my_malloc(size_t size);
int check_file(FILE *f);
int read_queue_size(FILE *f);
char read_queue_action(FILE *f, int *v);   // second command is a pointer to an inr number
queue_t* create_queue(int capacity);
bool push_to_queue(queue_t *queue, void *data);
void* pop_from_queue(queue_t *queue);
void* get_from_queue(queue_t *queue, int idx);
int get_queue_size(queue_t *queue);




// the main program
int main(int argc, char *argv[])
{
    const char *fname = argc > 1 ? argv[1] : NULL;
    FILE *f = open_file(fname);
    if (f)
    {
        check_file(f);
    }

    // process file
    if (fclose(f) == 0)     // if the file has been closed successfuly 
    {                       // continue to standard exit return
        return EXIT_SUCCESS;
    }
}

FILE *open_file(const char *fname)
{
    FILE *f = NULL;
    if (fname == NULL)
    {
        fprintf(stderr, "no file on the input\n");
        exit(NO_FILE);
    }
    if ((f = fopen(fname, "r")) == NULL)  // f stands for reading
    {
        fprintf(stderr, "File not opened - fail\n");
        exit(NOT_OPENED);
    }
    return f;
}

int *my_malloc(size_t size)
{
    int *p = malloc(sizeof(int));
    if (p == NULL)
    {
        fprintf(stderr, "Error, malloc failed\n");
        exit(MALLOC_ERROR);
    }
    return p;
}

int check_file(FILE *f)
{
    int ret = 0;
    assert(f);  
    int size = read_queue_size(f);
    if (size < 0)
    {
        fprintf(stderr, "Size cannot be negative\n");
        exit(WRONG_SIZE);
    }
    queue_t* queue = create_queue(size);
    if (!queue)
    {
        fprintf(stderr, "Error, not able to create queue\n");
        exit(QUEUE_ERROR);
    }
    // processing commands
    char action;
    int *p = my_malloc(sizeof(int));
    while ((action = read_queue_action(f, p)) != 0)
    {
        switch (action)
        {
            case 'a':
               // fprintf("%d\n", *p);
                push_to_queue(queue, p);   
                p = my_malloc(sizeof(int));
                break;
            case 'r':
                {       
                int *temp_p = pop_from_queue(queue); // pak je to potreba uvolnit
                if (temp_p)     // if there is any value to be removed
                {
                    //fprintf("%d\n", *p);
                    free(temp_p);
                }
                break;
                }
            default:
                fprintf(stderr, "undefined command input\n");
                delete_queue(queue); 
                free(p);
                exit(WRONG_COMMAND);  // popremyslet, jestli nebude lepsi jen vatit hodnotu a podle ni pripadne dealokovat neco
        }
    }
    free(p);
    delete_queue(queue); 

    return ret;      
}

int read_queue_size(FILE *f)
{
    assert(f);
    int size;
    if (fscanf(f, "%d", &size) != 1)
    {
        size = -1;
    }
    return size;
}

char read_queue_action(FILE *f, int *v) // v = value
{
    assert(f && v);
    char action = 0;
    int r = fscanf(f, "%c %d\n", &action, v);
    if (r == 1)  
    {
        action = (action != 'r'? 0 : action);
    }
    else if (r == 2)
    {
        action = (action == 'a'? action : 0);
    }
    return action;
}

