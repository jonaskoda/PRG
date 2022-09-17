#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/* Queue structure which holds all necessary data */
typedef struct queue_t{
    void *data;
    int *first;
    int *last;
    int capacity;
} queue_t;

/* creates a new queue with a given size */
queue_t* create_queue(int capacity);

/* deletes the queue and all allocated memory */
void delete_queue(queue_t *queue);                 // mazat i hodnoty, na ktere pointer ukazuje
                                                   // uvazujeme, že pri mazani je fronta jiz prazdna a neni potreba dealokovat jednotlive elementy
/*
 * inserts a reference to the element into the queue
 * returns: true on success; false otherwise
 */
bool push_to_queue(queue_t *queue, void *data); // ukazatel na void, muzeme vkladat cokoli za datovy typ

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
