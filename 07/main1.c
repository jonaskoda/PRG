#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 6. ukol na dodelani

struct student {  // neni pojmenovana
    char name[10];
    int grade;
};
typedef struct queue {  // je pojmenovana 
    struct student* data;
    int first;
    int last;
    int capacity;
} QUEUE;

QUEUE init_queue(int cap) 
{
    QUEUE q;
    q.data = (struct student*) malloc(cap * sizeof(struct student));
    if (q.data == NULL)
    {
        fprintf(stderr, "Error malloc\n");
        exit(1);
    }
    q.capacity = cap;
    q.first = 0;
    q.last = 0;
    return q;

}
void print_queue(QUEUE q) {
    for (int i = q.first; i < q.last; i++)
        printf("%s %d\n", q.data[i].name, q.data[i].grade);
}
void free_queue(QUEUE *q) 
{
    free(q->data);
    q->capacity = 0;
}
void push_queue(QUEUE *q, char* sName, int sGrade) 
{
    strcpy(q->data[q->last].name, sName);
    q->data[q->last].grade = sGrade;
    q->last++;
    if (q->last >= q->capacity) 
    {
        q->capacity *= 2;
        if ((q->data = (struct student*)realloc(q->data, q->capacity * sizeof(struct student))) == NULL) {
            printf("Nedostatek mista\n");
            free_queue(q);
            return;
        }
    }
}
int is_empty(QUEUE q) 
{
    return q.first = q.last;
}
struct student pop_queue(QUEUE *q) 
{
    struct student s;
    if (is_empty(*q))
    {
        s.name[0] = '\0';
        s.grade = 1;
        return s;
    }
    strcpy(s.name, q->data[q->first].name);
    s.grade = q->data[q->first].grade;
    q->first++;
    return s;
}
int main() {
    QUEUE myQueue = init_queue(10);
    print_queue(myQueue);
    push_queue(&myQueue, "Honza", 1);
    push_queue(&myQueue, "Jana", 1);
    push_queue(&myQueue, "Vera", 3);
    push_queue(&myQueue, "Eva", 2);
    push_queue(&myQueue, "Marie", 1);
    print_queue(myQueue);
//    printf("Mazu %s\n", pop_queue(&myQueue).name);
//    printf("Mazu %s\n", pop_queue(&myQueue).name);
//    printf("Mazu %s\n", pop_queue(&myQueue).name);
//    print_queue(myQueue);
    free_queue(&myQueue);
    return 0;
}
