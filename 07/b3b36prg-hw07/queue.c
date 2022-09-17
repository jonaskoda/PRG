#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include "queue.h"

enum                         
{
    GET_ENTRY_FAILURE = -1,
    MALLOC_ERROR = 100,
};

typedef struct item              // a linked list node to store a value and adress
{                                // to the next element
    void *value;
    struct item *next;
}item;

typedef struct linked_list_t     // structure for the linked_list
{
    item *first;
    item *last;  
    int capacity;
    int (*compare)(const void *, const void *);
    void (*clear)(void *);
}linked_list_t;


void *my_malloc(size_t size);
item *create_item(void *entry);
_Bool is_empty(void *queue);
void insert_empty_case(linked_list_t **link_list, item *temp1);
void insert_one_case(linked_list_t **link_list, item *temp1);
void insert_multiple_case(linked_list_t **link_list, item *temp1, item *temp2);
void erase_first_case(linked_list_t **link_list, item **temp1, void *entry, int *deleted);
void set_to_null(linked_list_t *link_list);


void *my_malloc(size_t size)       
{                                      
    void *ret = malloc(size);
    if (!ret)
    {
        fprintf(stderr, "Error, malloc\n");
        free(ret);
        ret = NULL;
    }
    return ret;
}


void *create()      // creates and allocates memory for a new queue structure
{
    linked_list_t *queue = my_malloc(sizeof(linked_list_t));
    queue->first = NULL;
    queue->last = NULL;
    queue->capacity = 0;
    queue->compare = NULL;      // save pointers to functions 
    queue->clear = NULL;
    return queue;
}


item *create_item(void *entry)  // creates and allocates memory for a new node structure
{
    item *ret = my_malloc(sizeof(item));
    ret->value = entry;
    ret->next = NULL;
    return ret;
}


_Bool is_empty(void *queue)               // returns true in case the queue is empty
{
    linked_list_t *link_list = (linked_list_t*)queue;

    return (link_list == NULL || link_list->first == NULL);
}


_Bool push(void *queue, void *entry)      // push the element to the tail of the queue
{
    linked_list_t *link_list = (linked_list_t*)queue;

    if (entry < 0 || !link_list)
    {
        return false;
    }
    item *node = create_item(entry);
    if (link_list->last)                  // if queue is not empty
    {
        link_list->last->next = node;
    }
    else                                  // if queue is empty 
    {
        link_list->first = node;
    }
    link_list->last = node;
    link_list->capacity++;
    return true;
}


void* pop(void *queue)                    // pop the element from the front of the queue
{
    linked_list_t *link_list = (linked_list_t*)queue;

    if (!link_list || !link_list->first)
    {
        return (void*) NULL;
    }
    item *temp1 = link_list->first;       // store previous front 
    void* ret = link_list->first->value;  // and move front one node ahead
    link_list->first = link_list->first->next;

    if (link_list->first == NULL)
    {
        link_list->last = NULL;
    }
    link_list->capacity--;
    free(temp1);
    return ret;
}


void insert_empty_case(linked_list_t **link_list, item *temp1)  // insert the node into an empty queue
{
    (*link_list)->last = temp1;
    (*link_list)->first = temp1;
    (*link_list)->capacity++;
}


// (*link_list)->first->value >= temp1->value)
void insert_one_case(linked_list_t **link_list, item *temp1)   // insert the node into a queue of size 1
{
    if ((*link_list)->compare((*link_list)->first->value, temp1->value) >= 0)  // put the node behind the first one
    {
        temp1->next = (*link_list)->first->next;  
        (*link_list)->first->next = temp1;
        (*link_list)->last = temp1;
    }
    else                                                       // put the node in front of the 
    {                                                          // first one
        temp1->next = (*link_list)->first;
        (*link_list)->first = temp1;
    }
    (*link_list)->capacity++;
}



void insert_multiple_case(linked_list_t **link_list, item *temp1, item *temp2)
{                                                              // insert the node into a queue of size
    while (temp2->next != NULL && (*link_list)->compare(temp2->next->value, temp1->value) > 0) // larger than 1
    {
        temp2 = temp2->next;                                   // iterate through nodes intil smaller
    }                                                          // value is found
    if (temp2->next == NULL)
    {
        temp1->next = temp2->next;                             // end up at the end -> insert the node
        temp2->next = temp1;             
        (*link_list)->last = temp1;
    }
    else                                                       // insert the node in front of the 
    {                                                          // node with a smaller value
        temp1->next = temp2->next;  
        temp2->next = temp1;
    }
    (*link_list)->capacity++;
}


_Bool insert(void *queue, void *entry)                         // insert a node into a queue based on
{                                                              // its value
    linked_list_t *link_list = (linked_list_t*)queue;

    if (entry < 0 || !link_list)                               // case for empty queue
    {
        return false;
    }
    item *temp1 = create_item(entry);                          
    if (!link_list->last)                                      // queue is empty
    {
        insert_empty_case(&link_list, temp1);
    }
    else if (link_list->capacity == 1)                         // case for one element in the queue 
    {
        insert_one_case(&link_list, temp1);
    }
    else 
    { 
        item *temp2 = link_list->first;
        if (link_list->compare(temp1->value, temp2->value) >= 0)  // possible only if temp2 is inserted as the first element
        {
            temp1->next = temp2; 
            link_list->first = temp1;
            link_list->capacity++;
            return true;                                          // termination
        }
        insert_multiple_case(&link_list, temp1, temp2);           // normal case for queue larger than one
    }  
    return true;
}


void erase_first_case(linked_list_t **link_list, item **temp1, void *entry, int *deleted)  
{
    while((*temp1) != NULL && (*link_list)->compare((*temp1)->value, entry) == 0)   // case in which the entry is present as a first
    {                                                                               // element in the queue or even more times in a row
        (*link_list)->first = (*temp1)->next;
        (*link_list)->clear((*temp1)->value);
        free((*temp1));
        *deleted = *deleted + 1;
        (*link_list)->capacity--;
        (*temp1) = (*link_list)->first;
    }
    set_to_null((*link_list));                                // if queue is empty, redirect its pointers
}


void set_to_null(linked_list_t *link_list)                    // queue is empty, redirect its pointers
{
    if (is_empty(link_list))                  
    { 
        link_list->first = NULL;
        link_list->last = NULL;
    }
}


_Bool erase(void *queue, void *entry)                         // get rid of specified values
{
    linked_list_t *link_list = (linked_list_t*)queue;

    int deleted = 0;
    if (!link_list->last)                                     // queue is empty
    {
        return false;
    }
    item *temp1 = link_list->first;
    item *temp2 = NULL;
    erase_first_case(&link_list, &temp1, entry, &deleted);    // case in which the entry is present as a first
                                                              // element in the queue or even more times in a row
    while (temp1 != NULL)
    {    
        while(temp1 != NULL && link_list->compare(temp1->value, entry) != 0)         // loop through nodes
        {
            temp2 = temp1;
            temp1 = temp1->next;
        }
        if (temp1 == NULL)                                    // ended up at the end
            break;
        temp2->next = temp1->next;
        link_list->clear(temp1->value);
        free(temp1);                                          // move pointers and free a node
        temp1 = temp2->next;
        deleted++;
        link_list->capacity--;
    }
    set_to_null(link_list);                                   // if queue is empty, redirect its pointers
    if (deleted == 0)
        return false;
    else 
        return true;
}


void* getEntry(const void *queue, int idx)                    // get the value at the specified index
{
    linked_list_t *link_list = (linked_list_t*)queue;

    void *ret = (void*) GET_ENTRY_FAILURE;                    // base case
    if (link_list->last && link_list->first && idx >= 0 && idx < link_list->capacity)    
    {                                        // queue is not empty and index is both within a queue's range and existant
        item *temp1 = link_list->first;
        for (int i = 0; i <= idx; i++)
        {
            if (idx == i)
            {
                ret = temp1->value;                           // found the value
            }
            temp1 = temp1->next; 
        }
    }
    return ret;
}


int size(const void *queue)
{
    linked_list_t *link_list = (linked_list_t*)queue;

    return link_list->capacity;
}


void clear(void *queue)                                       // clean after yourself
{                                                             // free everything
    linked_list_t *link_list = (linked_list_t*)queue;

    if (link_list->last != NULL)
    {
        item *temp1 = link_list->first;
        item *temp2; 
        while (temp1 != NULL)                                 // queue is not empty
        {
            temp2 = temp1->next;
            link_list->clear(temp1->value);
            free(temp1);
            link_list->capacity--;
            temp1 = temp2;
        }
        link_list->first = NULL;
        link_list->last = NULL;
    }
}


void setCompare(void *queue, int (*compare)(const void *, const void *))
{
    linked_list_t *link_list = (linked_list_t*)queue;

    if (link_list != NULL)
    {
        link_list->compare = compare;
    }
}


void setClear(void *queue, void (*clear)(void *))
{
    linked_list_t *link_list = (linked_list_t*)queue;

    if (link_list != NULL)
    {
        link_list->clear = clear;
    }
}



