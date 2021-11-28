//
// Created by Legion on 8.11.2021.
//

#include "binary_tree.h"
#include "constants.h"
#include "graphic_dump.h"

/*
int main() {
    TreeConstructor(test_tree);

    Node* root = SetRoot(test_tree, 100, NULL, NULL);


    Node* node_120 = AddNode(&test_tree, LEFT(root), 120, NULL, NULL);

    Node* node_80  = AddNode(&test_tree, RIGHT(root), 80, NULL, NULL);

    AddNode(&test_tree, RIGHT(node_80), 600, NULL, NULL);
    AddNode(&test_tree, LEFT(node_80), 500, NULL, NULL);

    AddNode(&test_tree, RIGHT(node_120), -10, NULL, NULL);
    AddNode(&test_tree, LEFT(node_120), -40, NULL, NULL);

    printf("%ld %ld \n", VALUE(node_120), VALUE(node_80));
    TreeGraphicDump(&test_tree, NAME(test_tree));

    TreeDestructor(test_tree);

    return 0;
}
*/

void MakeTree(Tree* tree) {
    REPORT(tree);

    tree->root_node  = (Node*) calloc(1, sizeof (Node)); // allocating memory for root node
    // tree->tree_depth = DEFAULT_DEPTH; // setting depth to zero because our tree still doesn't have any branches
}

void FreeTree(Tree* tree, Node* root_node) {
    REPORT(tree     );
    REPORT(root_node);

    /* Recursively poisoning left and right branches */
    root_node->left_node  != NULL ? FreeTree(tree, root_node->left_node)  : PASS;
    root_node->right_node != NULL ? FreeTree(tree, root_node->right_node) : PASS;

    root_node->left_node  = (Node*)      POISON_NODE;
    root_node->node_value = (nodeValue) POISON_VALUE;
    root_node->node_code  = (uint64_t)  POISON_VALUE;
    root_node->right_node = (Node*)      POISON_NODE;

}

treeInfo CheckTree(Node* root_node) {
    REPORT(root_node);

    /* Recursively checking left and right branches */
    if (root_node->node_value == (nodeValue) POISON_VALUE || root_node->node_code == POISON_VALUE) {
        return ERROR;
    }
    if (root_node->left_node == (Node*) POISON_NODE || root_node->right_node == (Node*) POISON_NODE) {
        return ERROR;
    }

    if (root_node->left_node != NULL) {
        return CheckTree(root_node->left_node);
    }
    if (root_node->right_node != NULL) {
        return CheckTree(root_node->right_node);
    }

    return 1;
}

Node* MakeRawNode(nodeValue value, Node* left, Node* right) {
    Node* new_node = calloc(1, sizeof (Node));
    new_node->right_node = right;
    new_node->left_node  =  left;
    new_node->node_value = value;

    new_node->node_code  =    -1;  // default code is -1
    return new_node;
}

Node* MakeNode(Tree* tree, Node* parent_node, nodeValue value, Node* left_node, Node* right_node) {
    REPORT(tree       );
    REPORT(parent_node); // IMPORTANT: parent_node CAN be NULL so insert the REPORT (or segfault is coming)

    #if (_TREE_DEBUG_ == 1)
        TreeAssert(tree);
    #endif

    parent_node->node_value =      value;
    parent_node->left_node  =  left_node;
    parent_node->right_node = right_node;

    parent_node->node_code  =   DEF_CODE;

    #if (_TREE_DEBUG_ == 1)
        TreeAssert(tree);
    #endif

    return parent_node;
}

Node* MakeRoot(Node* root, nodeValue value, Node* left_node, Node* right_node) {
    REPORT(root);

    root->node_value =      value;
    root->left_node  =  left_node;
    root->right_node = right_node;

    return root;
}