#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "list.h"

typedef struct Node Node;

struct Node {
    void * data;
    Node * next;
    Node * prev;
};

struct List {
    Node * head;
    Node * tail;
    Node * current;
};

typedef List List;

Node * createNode(void * data) {
    Node * new = (Node *)malloc(sizeof(Node));
    assert(new != NULL);
    new->data = data;
    new->prev = NULL;
    new->next = NULL;
    return new;
}

List * createList() {
    List* list= (List *) malloc(sizeof(List));
    list->head=NULL;
    list->tail=NULL;
    list->current=NULL;
    return list;
}

void * firstList(List * list) {
    if(!list->head) return NULL;
    list->current=list->head;
    return (list->head->data);
}

void * nextList(List * list) {
    if (list->current != NULL && list->current->next!=NULL){
        list->current = list->current->next;
        return list->current->data;
    }
    return NULL;
}

void * lastList(List * list) {
    if(!list->tail) return NULL;
    list->current=list->tail;
    return (list->tail->data);
}

void * prevList(List * list) {
    if (list->current != NULL && list->current->prev!=NULL){
        list->current = list->current->prev;
        return list->current->data;
    }
    return NULL;
}

void pushFront(List * list, void * data) {
   Node* n =createNode(data);
   n->next = list->head;
   list->tail=n;
   if (list->head) list->head->prev = n;
   list->head = n;
   n->prev = NULL;

}

void pushBack(List * list, void * data){
    list->current = list->tail;
    pushCurrent(list,data);
}


// inserta delante del current
void pushCurrent(List * list, void * data) {
    Node* aux = createNode(data);
    if(list->current->next != NULL){
        list->current->next->prev = aux;
        list->current->next = aux->next;
    }
    list->current->next = aux;
    aux->prev = list->current;

    if (aux->next == NULL) list->tail = aux;
}

void * popFront(List * list) {
    list->current = list->head;
    return popCurrent(list);
}

void * popBack(List * list) {
    list->current = list->tail;
    return popCurrent(list);
}

void * popCurrent(List * list) {
    if (list->current->prev != NULL) list->current->prev->next = list->current->next;
    else list->head = list->current->next;

    if (list->current->next != NULL) list->current->next->prev = list->current->prev;
    else{
        list->current->prev->next = NULL;
        list->tail = list->current->prev;
    } 

    return list->current->data;
}

void cleanList(List * list) {
    while (list->head != NULL) {
        popFront(list);
    }
}