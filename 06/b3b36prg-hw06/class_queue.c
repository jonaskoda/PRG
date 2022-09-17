#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct student {
    char name[10];
    int priority;
};
typedef struct queue {
    struct student data;
    struct queue* next;
} QUEUE;

QUEUE* init_queue() 
{
    return NULL;

}
void print_queue(QUEUE *q) {
    QUEUE *tmp = q;
    while (tmp != NULL) {
        printf("%s %d\n", tmp->data.name, tmp->data.priority);
        tmp = tmp->next;
    }
}
void free_queue(QUEUE **q) 
{
    QUEUE *tmp = *q;
    while (tmp != NULL)
    {
        tmp = tmp->next;
        free(*q);
        *q = tmp;
    }
}
int is_empty(QUEUE *q) {
    return q == NULL;
}
void push_queue(QUEUE **q, char *sName, int prior) 
{
    QUEUE *new_q = (QUEUE*) malloc(sizeof(QUEUE));
    if (new_q == NULL)
    {
        fprintf(stderr, "Error malloc");
        exit(1);
    }
    strcpy(new_q->data.name, sName);
    new_q->data.priority = prior;
    new_q->next = NULL;
    if (is_empty(*q))
    {
        *q = new_q;
        return;
    }
    else if (prior < (*q) -> data.priority)
    {
        new_q->next = *q;
        *q = new_q;
    } else
    {
        QUEUE *tmp = *q;
        while (tmp->next != NULL || tmp->next->data.priority >= prior)
        {
            tmp = tmp->next;
        }
        if (tmp->next == NULL)
        {
            tmp->next = new_q;
        }
        else  // insert at the end
        {
            new_q->next = tmp->next;
            tmp->next = new_q;
        }
    }
}

struct student pop_queue(QUEUE **q) {
    struct student s;
    if (is_empty(*q)) {
        printf("Fronta je prazdna\n");
        s.name[0] = '\0';
        s.priority = 1;
        return s;
    }
    strcpy(s.name, (*q)->data.name);
    s.priority = (*q)->data.priority;
    QUEUE *tmp = *q;
    (*q) = (*q)->next;
    free(tmp);

    return s;
}
int main() {
    QUEUE *myQueue = init_queue();
    print_queue(myQueue);
    push_queue(&myQueue, "Honza", 1);
    push_queue(&myQueue, "Jana", 1);
    push_queue(&myQueue, "Vera", 3);
    push_queue(&myQueue, "Eva", 2);
    push_queue(&myQueue, "Marie", 1);
    print_queue(myQueue);
    printf("Mazu %s\n", pop_queue(&myQueue).name);
    printf("Mazu %s\n", pop_queue(&myQueue).name);
    printf("Mazu %s\n", pop_queue(&myQueue).name);
    print_queue(myQueue);
    free_queue(&myQueue);
    return 0;
}
