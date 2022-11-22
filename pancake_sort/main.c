#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "stdbool.h"

typedef struct Node {

    int value;
    bool isGuard;
    struct Node* next;
    struct Node* prev;

} Node;

void PrintNode (Node* node) {
    printf("Node (%p)\n", node);
    printf("\tprev: %p\n", node->prev);
    printf("\tnext: %p\n", node->next);
}

Node* AddToList (int value, bool isGuard, Node* list) {
    Node* head = list;

    Node* newNode = malloc(sizeof(Node));
    if (head != NULL) {
        while (head->next != NULL) { head = head->next; }
        head->next = newNode;
    }
    else
        list = newNode;

    newNode->prev = head;
    newNode->next = NULL;
    newNode->isGuard = isGuard;
    newNode->value = value;

    return list;
}

void FreeList (Node* list) {
    Node* head = list;
    while (head != NULL) {
        Node* tmp = head;
        head = head->next;
        free (tmp);
    }
}

void PrintList (Node* list) {
    Node* head = list;
    while (head != NULL) {
        printf("Node (%p) - %d%s\n", head, head->value, head->isGuard ? ", guard" : "");
        // PrintNode(head);
        head = head->next;
    }
}

Node* GetLargest (Node* list) {

    Node* largestNode = list;
    Node* head = list;

    while (head != NULL) {

        if (head->isGuard == false && head->value > largestNode->value)
            largestNode = head;

        head = head->next;
    }

    return largestNode;
}

void SwapRange (Node* startPtr, Node* endPtr) {

    Node* head = startPtr;

    while (head->prev != endPtr) {

        Node* lower = head->prev;
        Node* upper = head->next;

        head->prev = upper;
        head->next = lower;

        head = upper;
    }

    Node* startPtrNext = startPtr->next;
    startPtrNext->next = endPtr;
    startPtr->next = head;
    endPtr->prev = startPtrNext;
    head->prev = startPtr;
}

void PancakeSort (Node* list) {

    Node* guard1 = list;
    Node* guard2 = list;
    while (guard2->next != NULL) guard2 = guard2->next;

    Node* sortedHead = guard1->next;

    while (sortedHead->isGuard == false) {
        Node* largest = GetLargest (sortedHead);
        // printf("------INIT-----\n");
        // printf("sorted head: %d\n", sortedHead->value);
        // printf("largest: %d\n", largest->value);
        // PrintList(list);
        // printf("---------------\n");

        if (largest == sortedHead) {
            sortedHead = sortedHead->next;
            continue;
        }

        // printf("-----SWAP-1---\n");
        // printf("from: %d to: %d\n", largest->value, guard2->prev->value);
        SwapRange(largest, guard2->prev);
        // PrintList(list);
        // printf("---------------\n");
        
        
        // printf("------SWAP-2---\n");
        // printf("from %d to %d\n", sortedHead->value, guard2->prev->value);
        SwapRange(sortedHead, guard2->prev);
        // PrintList(list);
        // printf("---------------\n");
        sortedHead = largest->next;
        // printf("new sorted head: %d\n", sortedHead->value);
        // printf("---------------\n");
    }
}

int main () {

    Node* list = NULL;

    list = AddToList(-1, true, list);
    list = AddToList(0, false, list);
    list = AddToList(8, false, list);
    list = AddToList(4, false, list);
    list = AddToList(7, false, list);
    list = AddToList(5, false, list);
    list = AddToList(1, false, list);
    list = AddToList(2, false, list);
    list = AddToList(6, false, list);
    list = AddToList(3, false, list);
    list = AddToList(9, false, list);
    list = AddToList(-1, true, list);

    printf("original:\n");
    PrintList(list);

    PancakeSort(list);
    
    printf("result:\n");
    PrintList(list);

    FreeList(list);

    return 0;
}