#ifndef _PROCESSOR_UTILS_H_
#define _PROCESSOR_UTILS_H_

#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

typedef uint64_t lineInfo;

typedef struct {
    uint8_t* content;
    size_t length;

    lineInfo lSpace_index; // first space index in line
    lineInfo rSpace_index; // last space index in line BUT last BEFORE arguments (not last at all)

    size_t args_length;
} line;

typedef struct {
    line* lines;
    uint8_t* byte_code; // IMPORTANT: never change the order, functions in utils.c use static order of this struct

    size_t total_bytes;
    lineInfo total_lines;
} file_content;

size_t GetFileByteSize(int thread);

void GetFileContent(file_content* current_file, const char* input_file_path);

void GetNumberOfLines(file_content* current_file);

void InitializeLinesInformation(file_content* current_file);

void CreateLines(file_content* current_file);

void CheckLines(file_content* current_file);

void FreeFileContent(file_content* current_file);

uint64_t ScanLine(uint8_t buff[], uint64_t length, FILE* input);

#endif