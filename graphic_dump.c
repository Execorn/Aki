//
// Created by Legion on 13.11.2021.
//

#include "graphic_dump.h"
#include "constants.h"
#include "stack.h"

const char* DUMP_FILE_EXTENSION =                                                               ".dot";
const char* DUMP_FILE_FORMAT    =                                                        "%s%s(%ld)%s";
const char* DUMP_FOLDER         =                      "/mnt/c/Users/Legion/CLionProjects/Tree/dumps/";
const char* DOT_BEGINNING       =                 "digraph %s {\n\tnode[shape = record, height=.1];\n";
const char* NODE_FORMAT         =               "\tnode%ld[label = \"<f0> l | <f1> %s | <f2> r \"];\n";
const char* EDGES_FORMAT        =               "\t\"node%ld\":f%ld -> \"node%ld\":f1 [color = %s];\n";
const char* IMAGE_FOLDER        =              "/mnt/c/Users/Legion/CLionProjects/Tree/graphic_dumps/";
const char* GV_CMD_FORMAT       =                                                 "dot -Tpng %s -o %s";

void FillEdgesInfo(Node* root, FILE* output) {
    REPORT(root  );
    REPORT(output);

    if (root->left_node  != NULL) {
        fprintf(output, EDGES_FORMAT, root->node_code, 0,  root->left_node->node_code, "blue");
    }
    if (root->right_node != NULL) {
        fprintf(output, EDGES_FORMAT, root->node_code, 2, root->right_node->node_code, "red" );
    }

    root->left_node  != NULL ? FillEdgesInfo(root->left_node , output) : NULL;
    root->right_node != NULL ? FillEdgesInfo(root->right_node, output) : NULL;
}

void FillNodesInfo(Node* root, FILE* output, treeInfo* nodes_cnt) {
    REPORT(root  );
    REPORT(output);

    root->node_code = *nodes_cnt; // putting code (number) of a current node, it will be used in func FillEdgesInfo
    fprintf(output, NODE_FORMAT, (*nodes_cnt)++, root->node_value);

    root->left_node  != NULL ? FillNodesInfo(root->left_node , output, nodes_cnt) : NULL;
    root->right_node != NULL ? FillNodesInfo(root->right_node, output, nodes_cnt) : NULL;
}

void FillDotFile(Tree* tree, FILE* output, const char* tree_name) {
    REPORT(tree           );
    REPORT(tree->root_node);
    REPORT(output         );

    char beginning_line[EXTRA_SYMBOLS] = "";
    sprintf(beginning_line, DOT_BEGINNING, tree_name);

    fprintf(output, "%s", beginning_line);

    treeInfo nodes_count = 0;
    FillNodesInfo(tree->root_node, output, &nodes_count);
    FillEdgesInfo(tree->root_node, output);

    fprintf(output, "%s", "}\n");
}

void MakeDumpFilePath(char dot_file_name[MAX_PATH], const char* tree_name, const char* folder, const char* extension) {
    /*
     * .DOT FILE PATH TEMPLATE:
     * "%folder%%tree_name% dump(%version%): %date% | %time%%extension%"
    */

    REPORT(dot_file_name);
    REPORT(tree_name    );
    REPORT(folder       ); // IMPORTANT: folder can be empty -> ""
    REPORT(extension    );

    treeInfo version           =  1; // default version, if current version is taken, version will be increased
    treeInfo output_descriptor =  1; // default descriptor, means that file is not opened yet

    while (TRUE) {
        sprintf(dot_file_name, DUMP_FILE_FORMAT, folder, tree_name, version, extension);
        output_descriptor = open(dot_file_name, 00); // opening as O_RDONLY
        if (output_descriptor == -1) {
            return; // breaking cycle, because we found a version number that is available
        } else {
            version++;
            close((int) output_descriptor); // IMPORTANT: do NOT forget to close files
        }
    }
}

void TreeGraphicDump(Tree* tree, const char* tree_name) {
    REPORT(tree     );
    REPORT(tree_name);

    #if (PRINT_DEBUG == 1)
        printf("Вхожу в функцию TreeGraphicDump. tree = %p, tree_name = %s.\n", tree, tree_name);
    #endif

    char dot_file_path[MAX_PATH] = "";
    MakeDumpFilePath(dot_file_path, tree_name, DUMP_FOLDER, DUMP_FILE_EXTENSION);

    FILE* dump_file = fopen(dot_file_path, "w");
    REPORT(dump_file);

    FillDotFile(tree, dump_file, tree_name);

    fclose(dump_file);
    /*
    char command[MAX_PATH] = "";

    sprintf(command, "cd %s && dot -Tpng %s -o output.png", DUMP_FOLDER, dot_file_path);

    system(command);
    */
    // here should be a command calls
}

