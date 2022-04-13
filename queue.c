/**
 *  list.c
 *
 *  Full Name:
 *  Course section:
 *  Description of the program: Various queue operations
 *  
 */
 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "queue.h"
#include "process.h"


// adds a new node (to the tail)
void offer(Queue *queue, Process *process){
    // add the new process to the list 
    struct node *newNode = malloc(sizeof(struct node));

    newNode->process = process;
    //Checks if the tail exists, if it does point the tail to the newly added node
    //If the tail doesn't exist, then the head also doesn't exist and set the head to the newly added node
    if(queue->tail!=NULL) {
        queue->tail->next = newNode;   
    } 
    else {
        queue->head = newNode;
    }
    queue->size++;
    //Sets the tail to newly added node
    queue->tail = newNode;
}

// removes a new node (from the head) 
Process* poll(Queue *queue){
    Process *node = queue->head->process;
    queue->head = queue->head->next;
    queue->size--;
    if(queue->size == 0) {
        queue->tail = NULL;
        queue->head = NULL;
    }
    return node;
}

//Shows the queue (mainly for debugging purposes)
void toString(Queue *queue){
    Node *temp = queue->head;
    printf("Size %d ", queue->size);
    while(temp!=NULL){
        printf("node %d ", temp->process->pid);
        temp = temp->next;
    }
    printf("\n");
}

//Returns true if the queue is empty, false otherwise
bool isEmpty(Queue *queue){
    if(queue->size==0) return true;
    else return false;
}
