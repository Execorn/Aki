//
// Created by Legion on 8.11.2021.
//

#ifndef TREE_BINARY_TREE_H
#define TREE_BINARY_TREE_H 1

#ifndef _TREE_DEBUG_
    #define _TREE_DEBUG_ 1 // _TREE_DEBUG_ SHOULD BE EITHER 1 or 0 (to complete check or not)
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "utils.h"


#define  NAME(var)  #var
#define  LEFT(node) node->left_node
#define RIGHT(node) node->right_node
#define  ROOT(tree) tree->root_node
#define VALUE(node) node->node_value

#define PASS (void*)0

#define REPORT(var)                                                                                                 \
    if ((var) == NULL) {                                                                                            \
        fprintf(stderr, "ERROR in function %s: %s is %p. \n", __FUNCTION__, NAME(var), var);                        \
        abort();                                                                                                    \
    }

#define TreeConstructor(name)                                                                                       \
    Tree name;                                                                                                      \
    MakeTree(&(name))

#define TreeDestructor(tree)                                                                                        \
    FreeTree(tree, (tree)->root_node)

#define TreeAssert(name)                                                                                            \
    if (CheckTree((name)->root_node) == ERROR) {                                                                    \
        TreeGraphicDump(name, #name);                                                                               \
        fprintf(stderr, "Error on line %d. \n", __LINE__);                                                          \
        abort();                                                                                                    \
    }

#define SetRoot(tree, value, left, right) MakeRoot((tree).root_node, value, left, right)

#define AddNode(tree, node, value, left, right)                                                                     \
    node = (Node*) calloc(1, sizeof (Node));                                                                        \
    REPORT(node);                                                                                                   \
    MakeNode(tree, node, value, left, right)


typedef struct __binarynode Node;
typedef struct __binarytree Tree;

typedef uint8_t* nodeValue; // IMPORTANT: change the type here to add functionality to the tree
typedef int64_t  treeInfo;

struct __binarynode {
    Node*       left_node; // pointer to Node in left branch

    nodeValue  node_value; // value of a node
    uint64_t    node_code; // code that is used for a graphic dump, it is useless

    Node*      right_node; // pointer to Node in right branch
};

struct  __binarytree {
    file_content     base; // base and strings contains here

    Node*       root_node; // root of the tree (main point)

    nodeValue qbyte_code;  // questions byte-code

    uint64_t bytes_count;  // len in bytes of bytes_code
};

void MakeTree(Tree* tree)                                                __attribute__((nothrow));
void FreeTree(Tree* tree, Node* root_node)                               __attribute__((nothrow));
treeInfo CheckTree(Node* root_node)                                      __attribute__((nothrow));

Node* MakeNode(Tree* tree, Node* parent_node, nodeValue value, Node* left_node, Node* right_node);
Node* MakeRoot(Node* root,                    nodeValue value, Node* left_node, Node* right_node);
Node* MakeRawNode(nodeValue value, Node* left, Node* right);



#endif //TREE_BINARY_TREE_H

