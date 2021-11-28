#include <assert.h>
#include <string.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h> // for read() and close() functions

#include "utils.h"

void GetFileContent(file_content* current_file, const char* input_file_path) {
    assert("current_file pointer is NULL" && current_file != NULL);
    assert("Input file pointer is NULL" && input_file_path != NULL);

    int file_thread = open(input_file_path, O_RDONLY, 0); // returns a file descriptor, else -1
    assert("Can't open the input file" && file_thread != -1);

    current_file->total_bytes = GetFileByteSize(file_thread);
    current_file->byte_code = (uint8_t*) calloc(current_file->total_bytes + 1, sizeof(current_file->byte_code[0]));
    assert("Byte code pointer is NULL" && current_file->byte_code != NULL);

    read(file_thread, current_file->byte_code, (lineInfo) current_file->total_bytes);
    close(file_thread); // IMPORTANT: do NOT forget to close
}

// HERE IS THE SAME FUNCTION BUT FOR DESCRIPTORS
// using fstat from <sys/stat.h>
// WARNING: fstat in NOT defined in ANSI C standard

size_t GetFileByteSize(int thread) {
    assert("File thread is invalid" && thread > 0);

    struct stat cur_file;
    fstat(thread, &cur_file);
    return cur_file.st_size; //see what it is below
}

/*
 * NOTE:
 * here is the most important struct stat info's
{ _dev_t	st_dev;      Equivalent to drive number 0=A 1=B ...
  _ino_t	st_ino;      Always zero ?
  _mode_t	st_mode;	 See above constants (stdlib lol)
   short 	st_nlink;	 Number of links.
   short 	st_uid; 	 User: Maybe significant on NT ?
   short 	st_gid; 	 Group: Ditto
  _dev_t	st_rdev;	 Seems useless (not even filled in)
  __st_off_t	st_size;	 File size in bytes
  __st_time_t	st_atime;	 Access time (always 00:00 on FAT)
  __st_time_t	st_mtime;	 Modified time
  __st_time_t	st_ctime;	 Creation time
}
 */

void GetNumberOfLines(file_content* current_file) {
    assert("Current file pointer is NULL" && current_file != NULL);

    lineInfo total_strings = 0;

    for (lineInfo current_char_index = 0; current_char_index < (current_file->total_bytes); current_char_index++) {
        if (current_file->byte_code[current_char_index] == '\n' || current_file->byte_code[current_char_index] == '\0') {
            total_strings++;
        }
    }
    current_file->total_lines = total_strings;
    // IMPORTANT: do not forget to check if current_char is NULL-BYTE (line 61)
}

void InitializeLinesInformation(file_content* current_file) {
    assert("Current file pointer is NULL" && current_file != NULL);

    current_file->lines = (line*) calloc(current_file->total_lines + 1, sizeof(current_file->lines[0]));
    // allocating total_lines + 1 because newLine is required for asm to work correct
    assert("Lines pointer is NULL" && current_file->lines != NULL);

    current_file->lines[0].content = &current_file->byte_code[0]; // setting pointer to beginning of byte_code
    for (size_t current_line_index = 1, current_line = 1; current_line_index < current_file->total_bytes; current_line_index++) {
        if (current_file->byte_code[current_line_index - 1] == '\n') { // it means that string is empty
            current_file->lines[current_line].lSpace_index = current_file->lines[current_line].rSpace_index = 0;

            current_file->lines[current_line].content = &current_file->byte_code[current_line_index];
            current_file->lines[current_line - 1].length = current_file->lines[current_line].content -
                                                           current_file->lines[current_line - 1].content - 1;
            // using difference in value of pointers to find actual string length
            current_line++;
        }
    }
    current_file->lines[current_file->total_lines - 1].length = &current_file->byte_code[current_file->total_bytes] -
                                                                 current_file->lines[current_file->total_lines - 1].content - 1;
    // using difference in pointers values again (struct has static order)
}

void CheckLines(file_content* current_file) {
    assert("Current file pointer is NULL" && current_file != NULL);
    lineInfo rLetter_index = 0; // index of last symbol that is NOT empty

    for (size_t current_line = 0; current_line < current_file->total_lines; current_line++) {
        for (lineInfo current_line_index = 0; current_file->lines[current_line].content[current_line_index]; current_line_index++) {
            switch (current_file->lines[current_line].content[current_line_index])
            {
                case '}':
                    current_file->lines[current_line].length = current_line_index + 1;
                    current_file->lines[current_line].content[current_line_index] = '\0';
                    // EXAMPLE:
                    // "push 4;hello world"
                    // actual length is 6 ("push 4") + 1 ('\0') = 7 and index of ; is 7
                case '\n':
                case '\r':
                    current_file->lines[current_line].content[current_line_index] = '\0';
                    current_file->lines[current_line].content[current_line_index + 1] = '\0';
                    break;
                    // changing \n and \r to \0 to FINISH the string
                case ' ': // just ignore it
                    break;
                default:
                    rLetter_index = current_line_index;
                    break;
            }
        }
        size_t entry_checker = 0; // checks if symbol is inside the string
        /* LTRIM AND RTRIM HERE */

        for (lineInfo current_line_index = 0; current_file->lines[current_line].content[current_line_index]; current_line_index++) {
            if (current_file->lines[current_line].content[0] == ' ') { // if string starts with space and empty
                current_file->lines[current_line].content[0] = '\0'; // finishing the string instantly
                current_file->lines[current_line].content++;
                current_file->lines[current_line].length--;
                rLetter_index--;
                current_line_index = 0; // returning cycle to beginning
            }
            else if (current_file->lines[current_line].content[current_line_index] == '"') {
                current_file->lines[current_line].args_length += 1;
                entry_checker = !entry_checker; // true to false, or false to true (1 -> 0, 0 -> 1)
            }
            else if (current_file->lines[current_line].content[current_line_index] == ' ') {
                if (entry_checker) {
                    current_file->lines[current_line].args_length += 1;
                }
                if (current_file->lines[current_line].lSpace_index == 0)
                    current_file->lines[current_line].lSpace_index = current_line_index;
                if ((current_line_index >= current_file->lines[current_line].lSpace_index) &
                    (current_file->lines[current_line].content[current_line_index + 1] != ' ') &
                    (current_file->lines[current_line].rSpace_index == 0))
                    current_file->lines[current_line].rSpace_index = current_line_index;

                if (current_line_index > rLetter_index)
                    current_file->lines[current_line].content[current_line_index] = '\0';
            }
            else if (current_file->lines[current_line].lSpace_index != 0) {
                current_file->lines[current_line].args_length += 1;
            }
            current_file->lines[current_line].length = rLetter_index + 1;
        }
    }
}

uint64_t ScanLine(uint8_t buff[], uint64_t length, FILE* input) {
    assert(buff != NULL);

    uint8_t  current      = 0;
    uint64_t total_cnt    = 0;

    while ((current = (uint8_t) fgetc(input)) != '\n' && total_cnt < length - 1) buff[total_cnt++] = current;
    buff[total_cnt++] = '\0';

    return total_cnt;
}


void CreateLines(file_content* current_file) {
    GetNumberOfLines(current_file); // gets total_lines_number
    InitializeLinesInformation(current_file);
}

void FreeFileContent(file_content* current_file) {
    assert("Current file pointer is NULL" && current_file != NULL);

    free(current_file->byte_code);
    current_file->byte_code = NULL;

    free(current_file->lines);
    current_file->lines = NULL;
}