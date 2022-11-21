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
        head = head->next;
    }
}

void SwapRange (Node* startPtr, Node* endPtr) {
    
    printf("Swap range from %p to %p\n", startPtr, endPtr);

    Node* head = startPtr->next;

    while (head->prev != endPtr) {
        
        /*
        
        start:
            +---+
            |   |  <--- lowerLower
            +---+
            A   |
            |   |
            |   V
            +---+
            | A |  <--- lower
            +---+
            A   |
            |   |
            |   V
            +---+
            | B |  <--- upper, head
            +---+
            A   |
            |   |
            |   V
            +---+
            |   |   <--- upperUpper
            +---+
        finish:
            +---+
            |   |   <--- lowerLower
            +---+
            A   |
            |   |
            |   V
            +---+
            | B |   <--- upper
            +---+
            A   |
            |   |
            |   V
            +---+
            | A |   <--- lower
            +---+
            A   |
            |   |
            |   V
            +---+
            |   |   <--- upperUpper, head
            +---+
        */

        Node* lower = head->prev;
        Node* upper = head;
        head = head->next;

        printf("lower:\n");
        PrintNode(lower);
        printf("upper:\n");
        PrintNode(upper);

        Node* lowerLower = lower->prev;
        Node* upperUpper = upper->next;

        lower->next = lowerLower;
        lower->prev = upper;

        upper->next = lower;
        upper->prev = upperUpper;
    }

    Node* startPtrNext = startPtr->next;
    startPtr->next = head;
    endPtr->prev = startPtrNext;

}

int main () {

    Node* list = NULL;

    list = AddToList(-1, true, list);
    list = AddToList(0, false, list);
    list = AddToList(1, false, list);
    list = AddToList(2, false, list);
    list = AddToList(3, false, list);
    list = AddToList(4, false, list);
    list = AddToList(-1, true, list);

    PrintList(list);

    SwapRange(list->next, list->next->next->next);

    PrintList(list);

    FreeList(list);

    return 0;
}