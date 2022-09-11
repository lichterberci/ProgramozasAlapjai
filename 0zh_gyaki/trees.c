#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

struct Node {
    int id;
    struct Node* left;
    struct Node* right;
};

int max (int a, int b) {
    if (a >= b) return a;
    return b;
}

void PrintTree (struct Node* root, int depth) {

    if (root == NULL)
        return;

    PrintTree (root->right, depth + 1);

    for (int i = 0; i < depth; i++)
        printf ("\t");

    printf ("%d\n", root->id);

    PrintTree (root->left, depth + 1);

};

void FreeTree (struct Node* root) {

    if (root == NULL)
        return;

    FreeTree (root->left);
    FreeTree (root->right);

    free (root);

};

int GetDepth (struct Node* root, int depth) {

    if (root == NULL)
        return 0;

    return max(GetDepth(root->left, depth + 1), GetDepth(root->right, depth + 1)) + 1;

};

bool IsBalanced (struct Node* root) {

    if (root->left != NULL && root->right != NULL)
        return IsBalanced (root->left) && IsBalanced (root->right);

    return abs(GetDepth (root->left, 0) - GetDepth (root->right, 0)) <= 1;
};


int main (void) {

    // make the nodes

    struct Node* root = malloc (sizeof (struct Node));
    root->id = 0;

    struct Node* node1 = malloc (sizeof (struct Node));
    node1->id = 1;

    struct Node* node2 = malloc (sizeof (struct Node));
    node2->id = 2;

    struct Node* node3 = malloc (sizeof (struct Node));
    node3->id = 3;

    struct Node* node4 = malloc (sizeof (struct Node));
    node4->id = 4;

    struct Node* node5 = malloc (sizeof (struct Node));
    node5->id = 5;

    // connect the nodes

    root->left = node1;
    root->right = node2;

    node1->left = node3;
    node1->right = NULL;

    node2->left = NULL;
    node2->right = node4;

    node3->left = NULL;
    node3->right = NULL;

    node4->right = node5;
    node4->left = NULL;

    node5->left = NULL;
    node5->right = NULL;

    printf("Tree:\n");

    PrintTree (root, 0);

    printf ("\n\n");

    if (IsBalanced(root))
        printf ("This tree is balanced!\n");
    else
        printf ("This tree is not balanced!\n");

    FreeTree (root);

    return 0;
}