#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

enum 
{
    MALLOC_ERROR = 100,
    REALLOC_ERROR = 101,
};

typedef struct item
{  
    void* value;        // value of unknown type
} item;

typedef struct queue_t  // Queue structure which holds all necessary data
{
    int first;
    int last;
    int capacity;
    int size;
    item *array;
} queue_t;

/* creates a new queue with a given size */
queue_t* create_queue(int capacity);

/*
 * function for allocating space for a given size
 */
void *my_malloc(size_t size);

/*
 * reallocates the size of an array by a given parameter
 */
void my_realloc(int new_capacity, queue_t* queue);

/*
 * free everything allocated
 */
void free_completely(queue_t *queue);

void free_elements(queue_t *queue);

/* deletes the queue and all allocated memory */
void delete_queue(queue_t *queue);                 

/*
 * move all the elements of an array at the beginning 
 * and move pointers to the start and end of a queue accordingly
 */
void reorganise(queue_t *queue);

/*
 * inserts a reference to the element into the queue
 * returns: true on success; false otherwise
 */
bool push_to_queue(queue_t *queue, void *data); 

/*
 * gets the first element from the queue and removes it from the queue
 * returns: the first element on success; NULL otherwise or false spis
 */
void* pop_from_queue(queue_t *queue);

/*
 * gets idx-th element from the queue, i.e., it returns the element that 
 * will be popped after idx calls of the pop_from_queue()
 * returns: the idx-th element on success; NULL otherwise
 */
void* get_from_queue(queue_t *queue, int idx);

/* gets number of stored elements */
int get_queue_size(queue_t *queue);

#endif /* __QUEUE_H__ */


