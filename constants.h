//
// Created by Legion on 14.11.2021.
//

#ifndef TREE_CONSTANTS_H
#define TREE_CONSTANTS_H 1


enum TreeConstants {
    RECURSION_LIMIT =    1 << 12, // IMPORTANT: NOT REALLY const value, can be changed with setrecursionlimit()
    DEFAULT_DEPTH   =          0,
    DEF_CODE        =         -1,
};

enum PoisonConstants {
    POISON_VALUE    = 0xDEADDEAD,
    POISON_NODE     = 0xABADBABE,
};

enum GlobalConstants {
    TRUE            =          1,
    ERROR           =         -1, // -1 is RESERVED identifier, do not use!
};

enum GraphicDumpConstants {
    /*
        In editions of Windows before Windows 10 version 1607,
        the maximum length for a path is MAX_PATH,
        which is defined as 260 characters.
    */
    MAX_PATH        =        260,
    COMMAND_LINE    =        200,
    EXTRA_SYMBOLS   =        100,
};

extern const char* DUMP_FILE_EXTENSION;
extern const char*    DUMP_FILE_FORMAT;
extern const char*       GV_CMD_FORMAT;
extern const char*         DUMP_FOLDER;
extern const char*       DOT_BEGINNING;
extern const char*         NODE_FORMAT;
extern const char*        EDGES_FORMAT;
extern const char*        IMAGE_FOLDER;


/* Aki constants */

enum AkiConstants {
    CONDITION_LENGTH = 300,
    CMD_ANSWER       = 5,
    NODE_VALUE_SIZE  = 150,
};

extern const char*       AKI_BASE_PATH;


#endif //TREE_CONSTANTS_H
