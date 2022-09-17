#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include "linked_list.h"

enum
{
    POP_FAILURE = -1,
    MALLOC_ERROR = 100,
};

typedef struct item            // a linked list node to store a value and adress
{                              // to the next element
    int value;
    struct item *next;
}item;

typedef struct linked_list_t   // structure for the linked_list
{
    item *first;
    item *last;  
    int capacity;
}linked_list_t;


void *my_malloc(size_t size);
linked_list_t *create_LL(void);
item *create_item(int entry);
void delete(linked_list_t **link_list);
_Bool is_empty();
void insert_empty_case(linked_list_t **link_list, item *temp1);
void insert_one_case(linked_list_t **link_list, item *temp1);
void insert_multiple_case(linked_list_t **link_list, item *temp1, item *temp2);
void erase_first_case(linked_list_t **link_list, item **temp1, int entry, int *deleted);
void set_to_null(linked_list_t *link_list);


linked_list_t ptr = {NULL, NULL, 0};   // create a global variable of a linked list 
static linked_list_t *link_list = &ptr;


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


item *create_item(int entry)
{
    item *ret = my_malloc(sizeof(item));
    ret->value = entry;
    ret->next = NULL;
    return ret;
}


void delete(linked_list_t **link_list)           // deallocate node memory
{
    if (link_list && *link_list)
    {
        linked_list_t *list = *link_list;
        item *cursor = list->first;
        while (cursor != NULL)                   // loop through all nodes
        {
            item *prev = cursor;
            cursor = cursor->next;
            free (prev);
        }
        free(list);
        *link_list = NULL; // nebo 0
    }
}


_Bool is_empty()
{
    return (link_list == NULL || link_list->first == NULL);
}


_Bool push(int entry)                   // push the element to the tail of the queue
{
    if (entry < 0 || !link_list)
    {
        return false;
    }
    item *node = create_item(entry);
    if (link_list->last)                // if queue is not empty
    {
        link_list->last->next = node;
    }
    else                                // if queue is empty 
    {
        link_list->first = node;
    }
    link_list->last = node;
    link_list->capacity++;
    return true;
}


int pop(void)                           // pop the element from the front of the queue
{
    if (!link_list || !link_list->first)
    {
        return POP_FAILURE;
    }
    item *temp1 = link_list->first;     // store previous front 
    int ret = link_list->first->value;  // and move front one node ahead
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


void insert_one_case(linked_list_t **link_list, item *temp1)   // insert the node into a queue of size 1
{
    if ((*link_list)->first->value >= temp1->value)            // put the node behind the first one
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
    while (temp2->next != NULL && temp2->next->value > temp1->value)  // larger than 1
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


_Bool insert(int entry)                                       // insert a node into a queue based on 
{                                                             // its value
    if (entry < 0 || !link_list)                              // case for empty queue
    {
        return false;
    }
    item *temp1 = create_item(entry);
    if (!link_list->last)                                     // queue is empty
    {
        insert_empty_case(&link_list, temp1);
    }
    else if (link_list->capacity == 1)                        // case for one element in the queue 
    {
        insert_one_case(&link_list, temp1);
    }
    else 
    { 
        item *temp2 = link_list->first;
        if (temp2->value <= temp1->value)  // possible only if temp2 is inserted as the first element
        {
            temp1->next = temp2; 
            link_list->first = temp1;
            link_list->capacity++;
            return true;                                      // termination
        }
        insert_multiple_case(&link_list, temp1, temp2);       // normal case for queue larger than one
    }  
    return true;
}


void erase_first_case(linked_list_t **link_list, item **temp1, int entry, int *deleted)
{
    while((*temp1) != NULL && (*temp1)->value == entry)   // case in which the entry is present as a first
    {                                                     // element in the queue or even more times in a row
        (*link_list)->first = (*temp1)->next;
        free((*temp1));
        *deleted = *deleted + 1;
        (*link_list)->capacity--;
        (*temp1) = (*link_list)->first;
    }
}


void set_to_null(linked_list_t *link_list)                // queue is empty, redirect its pointers
{
    if (is_empty())                   
    { 
        link_list->first = NULL;
        link_list->last = NULL;
    }
}


_Bool erase(int entry)                                    // get rid of specified values
{
    int deleted = 0;
    if (!link_list->last)                                 // queue is empty
    {
        return false;
    }
    item *temp1 = link_list->first;
    item *temp2 = NULL;
    erase_first_case(&link_list, &temp1, entry, &deleted); // case in which the entry is present as a first
    while (temp1 != NULL)                                  // element in the queue or even more times in a row
    {
        while(temp1 != NULL && temp1->value != entry)      // loop through nodes
        {
            temp2 = temp1;
            temp1 = temp1->next;
        }
        if (temp1 == NULL)                                 // ended up at the end
            break;
        temp2->next = temp1->next;
        free(temp1);                                       // move pointers and free a node
        temp1 = temp2->next;
        deleted++;
        link_list->capacity--;
    }
    set_to_null(link_list);                                // if queue is empty, redirect its pointers
    
    if (deleted == 0)
        return false;
    else 
        return true;
}


int getEntry(int idx)                                      // get the value at the specified index
{
    int ret = -1;
    if (link_list->last && link_list->first && idx >= 0 && idx < link_list->capacity)    
    {                                        // queue is not empty and index is both within a queue's range and existant
        item *temp1 = link_list->first;
        for (int i = 0; i <= idx; i++)
        {
            if (idx == i)
            {
                ret = temp1->value;                        // found the value
            }
            temp1 = temp1->next; 
        }
    }
    return ret;
}


int size(void)
{
    return link_list->capacity;
}


void clear()                                               // clean after yourself
{                                                          // free the nodes
    item *temp1 = link_list->first;
    item *temp2; 
    while (temp1 != NULL)                                  // queue is not empty
    {
        temp2 = temp1->next;
        free(temp1);
        link_list->capacity--;
        temp1 = temp2;
    }
    link_list->first = NULL;
    link_list->last = NULL;
}



