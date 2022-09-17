#include <stdio.h>
#include<stdlib.h>
#include <string.h>
#include<assert.h>
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
        exit(MALLOC_ERROR);
    }
    return ret;
}

void delete_queue(queue_t *queue)
{
    free(queue->array);            // free the array for storing elements
    free(queue);                   // free the queue
}

bool push_to_queue(queue_t *queue, void *data)
{
    //printf("beginning push %d %d\n", queue->first, queue->last);
    if ((queue->first == (queue->last)+1) || 
        (queue->first == 0 && queue->last == (queue->capacity)-1 ))
    {
        return false;
        free(queue->array);
    }
    else
    {
        if (queue->first == -1)
        {
            queue->first = 0;
            queue->last = 0;
        }
        else if (queue->last == (queue->capacity)-1)
        {
            queue->last = 0;
        }
        else
        {
            queue->last = (queue->last + 1) % queue->capacity;
        }
        //printf("end push%d %d\n", queue->first, queue->last);
        queue->array[queue->last].value = data;
        queue->size++;
        //printf("size %d\n", get_queue_size(queue));
    }
    return true;
}

void* pop_from_queue(queue_t *queue)
{
    //printf("beginning pop %d %d\n", queue->first, queue->last);
    if (queue->first == -1)
    {
        return false;  // queue is empty nothing to pop from
    }
    else
    {
        void* data = queue->array[queue->first].value;
        if (queue->first == queue->last)
        {
            queue->first = -1;
            queue->last = -1;
        }
        else if (queue->first == (queue->capacity)-1)
        {
            queue->first = 0;
        }
        else 
        {
            queue->first = (queue->first+1) % queue->capacity;
        }
        //printf("end pop %d %d\n", queue->first, queue->last);
        queue->size--;
        return data;
    }
}

void* get_from_queue(queue_t *queue, int idx)
{
    if (idx < 0 || idx >= queue->capacity || idx > queue->last || queue->first == -1)
    {
        return NULL;   // inexisting index of the queue or the queue is empty
    }
    else
    {
        void* data = queue->array[queue->first + idx].value;
        return data;
    }
}

int get_queue_size(queue_t *queue)
{
   return queue->size;
}
