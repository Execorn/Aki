//
// Created by Legion on 13.11.2021.
//

#ifndef TREE_GRAPHIC_DUMP_H
#define TREE_GRAPHIC_DUMP_H

#include "binary_tree.h"

#include <fcntl.h>
#include <unistd.h>

void FillDotFile(Tree* tree, FILE* output, const char* tree_name)                       __attribute__((nothrow));
void TreeGraphicDump(Tree* tree, const char* tree_name)                                 __attribute__((nothrow));
void MakeDumpFilePath(char dot_file_name[260], const char* tree_name, const char* folder, const char* extension);


#endif //TREE_GRAPHIC_DUMP_H
