/**
 *  list.h
 *
 *  Full Name: Lucas Aguiar
 *  Course section: Section M
 *  Description of the program: queue data structure containing the tasks in the system
 *  
 */
 
#include "process.h"
#include <stdbool.h>

typedef struct node {
    Process *process;
    struct node *next;
}Node;

typedef struct queue {
    Node *head;
    Node *tail;
    int size;
}Queue;

// insert and delete operations.
void offer(Queue *queue, Process* process);
Process* poll(Queue *queue);
void toString(Queue *queue);
bool isEmpty(Queue *queue);
