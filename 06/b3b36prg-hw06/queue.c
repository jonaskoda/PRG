#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "queue.h"

queue_t* create_queue(int capacity)
{
    queue_t *queue = my_malloc(sizeof(queue_t));    
    queue->first = -1;
    queue->last = -1;
    queue->capacity = capacity;
    queue->size = 0;
    queue->array = my_malloc(capacity * sizeof(item));
    return queue;
}

void *my_malloc(size_t size)       
{                                      
    void *ret = malloc(size);
    if (!ret)
    {
        fprintf(stderr, "Error, malloc\n");
        free(ret);
        exit(MALLOC_ERROR);
    }
    return ret;
}

void my_realloc(int new_capacity, queue_t* queue)
{
    queue->capacity = new_capacity;                    // update the capacity of the array to a given value
    void *temp = (void*) realloc(queue->array,queue->capacity * sizeof(item));  // realloc to the specified size
    if (temp == NULL)
    {
        printf("Realloc failed\n");
        free_completely(queue);
        exit(REALLOC_ERROR);
    }
    queue->array = temp;                               // assign the resized array back to the queue structure 
}

void free_completely(queue_t *queue)
{
    while (get_queue_size(queue))                      // free all the elements inside the array
    {
    void *p = pop_from_queue(queue);
    free(p);
    }
    // free memory
    delete_queue(queue);                               // free the array and structure itself
}

void free_elements(queue_t *queue)
{
    while (get_queue_size(queue))                      // free all the elements inside the array
    {
    void *p = pop_from_queue(queue);
    free(p);
    }
}
 
void delete_queue(queue_t *queue)
{
    free(queue->array);                                // free the array for storing elements
    free(queue);                                       // free the queue
}

void reorganise(queue_t *queue)
{
    queue_t *temp_struct = my_malloc(queue->capacity * sizeof(queue_t));
    temp_struct->array = my_malloc(queue->capacity * sizeof(item));

    for (int i = 0; i < queue->size; i++)              // loop through all elements and move them at the beginning 
    {                                                  // of a new array one by one
        temp_struct->array[i % queue->capacity].value = 
        queue->array[(queue->first + i) % queue->capacity].value;
    }
    free_elements(queue);
    queue->array = temp_struct->array;


    queue->first = 0;                                  // first elements is now at index 0
    queue->last = queue->size-1;                       // last elements is at index size-1
}

bool push_to_queue(queue_t *queue, void *data)
{
    if (queue->size >= queue->capacity)                // check whether the queue is too small
    {
        my_realloc(queue->capacity * 2, queue);        // if so -> reallocate
    }
    if (queue->first == -1)                            // queue is empty
    {
        queue->first = 0;
        queue->last = 0;
    }
    else if (queue->last == (queue->capacity)-1)       
    {
        queue->last = 0;                               // change last to 0
    }
    else
    {
        queue->last = (queue->last + 1) % queue->capacity; 
    }
    queue->array[queue->last].value = data;           // push the element inside the queue
    queue->size++;
    return true;
}

void* pop_from_queue(queue_t *queue)
{
    if (queue->first == -1)
    {
        return false;                                 // queue is empty nothing to pop from
    }
    else
    {
        void* data = queue->array[queue->first].value; // store the value to be poped to data

        if (queue->size < queue->capacity / 3)        // if there is too much unused space 
        {                                             // -> schrink the queue
            reorganise(queue);
            my_realloc(queue->capacity / 3, queue);
        }
        if (queue->first == queue->last)              // queue is now empty
        {
            queue->first = -1;
            queue->last = -1;
        }
        else if (queue->first == (queue->capacity)-1) 
        {
            queue->first = 0;                         // change first to 0
        }
        else 
        {
            queue->first = (queue->first+1) % queue->capacity;
        }
        queue->size--;
        return data;                                  // return the popped element
    }
}

void* get_from_queue(queue_t *queue, int idx)
{
    if (idx < 0 || idx >= queue->capacity || idx > queue->size -1 || queue->first == -1)
    {
        return NULL;   // inexisting index of the queue / index at the pos. where 
    }                  // the queue is empty or the queue is simply totally empty
    else
    {
        void* data = queue->array[(queue->first + idx) % queue->capacity].value;
        return data;   // otherwise return the element at the specified index
    }
}

int get_queue_size(queue_t *queue)
{
   return queue->size;
}

