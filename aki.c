//
// Created by Legion on 15.11.2021.
//

#include "aki.h"
#include "graphic_dump.h"
#include "constants.h"

const char* AKI_BASE_PATH = "/mnt/c/Users/Legion/CLionProjects/Tree/aki_base.txt";


int main() {
    SET1251;
    TreeConstructor(aki_tree);

    setvbuf(stdout, NULL, _IONBF, 0); // DISABLE BUFFERING

    int is_base_scanned = GetAkiBase(&aki_tree, AKI_BASE_PATH);
    if (!is_base_scanned) {
        DetermineNodeInfo(&aki_tree, aki_tree.root_node, 0); //TODO: make ROOT() macro work not only with pointers
    }

    TreeGraphicDump(&aki_tree, NAME(aki_tree));
    

    while (TRUE) {
        #if (PRINT_DEBUG == 1)
            printf("Вхожу в цикл запуска акинатора в функции main(). \n");
        #endif

        PrintStartingMessage();
        treeInfo cmd_code = 0;

        #if (PRINT_DEBUG == 1)
            printf("Начинаю считывать cmd_code в функции main(). \n");
        #endif

        while (!scanf("%ld", &cmd_code) && !IS_VALID(cmd_code))
            get_command:;
            printf("Please enter valid command number (0-5)! \n");
            FLUSH;

        #if (PRINT_DEBUG == 1)
            printf("Получил код. cmd_code = %ld. \n", cmd_code);
        #endif

        switch (cmd_code) {
            case 0:
                goto exit_program;
                break;

            case 1:
                AkiGame(&aki_tree);
                break;

            case 2:
                AkiCompare(&aki_tree);
                break;

            case 3:
                AkiDescribe(&aki_tree);
                break;

            case 4:
                TreeGraphicDump(&aki_tree, NAME(aki_tree));
                break;

            case 5:
                break;

            default:
                goto get_command;
                break;
        }
    }
    exit_program:;
    /*
     * PUTTING BASE INFO TO THE FILE FOR THE NEXT PROGRAM WORKTIME
     */
    #if (PRINT_DEBUG == 1)
        printf("Закончил цикл работы акинатора. Запускаю PutAkiData() и TreeDestructor. \n");
    #endif

    PutAkiData(&aki_tree);
    TreeDestructor(&aki_tree);
}


int GetAkiBase(Tree* tree, const char* base_path) {
    REPORT(tree     );
    REPORT(base_path);

    #if (PRINT_DEBUG == 1)
        printf("Вхожу в функцию GetAkiBase. tree = %p, base_path = %s. \n", tree, base_path);
    #endif

    GetFileContent(&tree->base, base_path);
    CreateLines(&tree->base);
    CheckLines (&tree->base);

    StackConstructor(nodes_stack); // nodes_stack where nodes will be added
    int is_parsed   = ParseAkiBase(tree, &nodes_stack);
    if (!is_parsed)                   return is_parsed;

    tree->root_node =      ConstructTree(&nodes_stack); // OR setroot() macro can be used

    #if (PRINT_DEBUG == 1)
        printf("Закончил работу функции GetAkiBase(). Вызываю StackDestructor. \n");
    #endif

    StackDestructor(&nodes_stack);

    #if (PRINT_DEBUG == 1)
        printf("StackDestructor закончил работу. GetAkiBase() вернет %i. \n", ROOT(tree) != NULL);
    #endif

    return ROOT(tree) != NULL;
}


Node* ConstructTree(Stack_t* stack) {
    REPORT(stack);

    #if (PRINT_DEBUG == 1)
        printf("Вхожу в функцию ConstructTree. stack = %p. \n", stack);
    #endif

    if (stack->current_index <= 0) {
        ERR("Stack has no members!");
    }


    StackConstructor(tree_constructor);
    Node* actual_node      = 0;
    stackElement next_node = 0;
    #define POPPED_NODE (Node*) StackPop(&tree_constructor)

    while (stack->current_index > 0) {
        #if (PRINT_DEBUG == 1)
            printf("Вхожу в цикл stack->current_index > 0. stack->current_index = %ld. \n", stack->current_index);
        #endif

        nodeValue new_value = StackPop(stack);

        Node* node_raw = MakeRawNode(new_value, NULL, NULL);

        StackPush(&tree_constructor, (stackElement) node_raw);

        // do NOT remove current_index check IT IS IMPORTANT after StackPop
        if (!stack->current_index) break;
        next_node = StackPop(stack);

        if (next_node) {
            /*
             * Making a parent node (question) and two branches
             */
            actual_node        = POPPED_NODE;

            LEFT (actual_node) = POPPED_NODE;

            RIGHT(actual_node) = POPPED_NODE;

            StackPush(&tree_constructor, (stackElement) actual_node);

            actual_node        = POPPED_NODE;

            StackPush(&tree_constructor, (stackElement) actual_node);

            StackPush(stack, next_node);
        }
    }

    #if (PRINT_DEBUG == 1)
        printf("Вышел из цикла stack->current_index > 0. stack->current_index = %ld. \n", stack->current_index);
    #endif

    actual_node        = POPPED_NODE;
    LEFT (actual_node) = POPPED_NODE;
    RIGHT(actual_node) = POPPED_NODE;

    StackPush(&tree_constructor, (stackElement) actual_node);

    actual_node        = POPPED_NODE;
    StackDestructor(&tree_constructor);

    #undef POPPED_NODE


    #if (PRINT_DEBUG == 1)
        printf("Закончил работу функции ConstructTree(). ConstructTree() вернет %p. \n", actual_node);
    #endif

    return actual_node;
}

void PrintStartingMessage() {
    printf("%s \n",
           "Please select the game mode: \n"
           "0) Exit program :(           \n"
           "1) Play with akinator!       \n"
           "2) Play comparison game!    \n"
           "3) Play description game!     \n"
           "4) Make a graphic dump!      \n");
}


void AkiGame(Tree* aki_tree) {
    REPORT(aki_tree); // checks if aki_tree is NULL

    #if (PRINT_DEBUG == 1)
        printf("Вхожу в функцию AkiGame. aki_tree = %p. \n", aki_tree);
    #endif

    /*
     * Stack for backtrace: all nodes (questions) where player didn't answer, will be popped here
     * After the game ends, if user says that we got wrong guess, we will pop elements and try again from the
     * right and left_node branches (we will go RIGHT first)
     */
    StackConstructor(backtrace);

    /*
     * Stack to remember previous node path: all nodes that we go through will be pushed here!
     */
    
    StackConstructor(path_nodes);


    ConstructAnswerStack(ROOT(aki_tree), &backtrace, &path_nodes);
    Node* answerNode = (Node*) StackPop(&path_nodes);


    uint8_t ans[CMD_ANSWER];
    ScanOnlyYesOrNoAnswer(ans);

    #if (PRINT_DEBUG == 1)
        printf("Получил ans, ans = %s. \n", ans);
    #endif

    switch (*ans) {
        case 'Y':
        case 'y':
            positive_answer:;
            printf("Alright! Let's go to the next question! \n");
            break;

        case 'N':
        case 'n':
            while (backtrace.current_index > 0) {
                ConstructAnswerStack(RIGHT(((Node*) StackPop(&backtrace))), &backtrace, &path_nodes);
                answerNode = (Node*) StackPop(&path_nodes);

                printf("Is your person %s? \n", answerNode->node_value);

                ScanOnlyYesOrNoAnswer(ans);
                if (*ans == 'Y' || *ans == 'y') {
                    goto positive_answer;
                }
            }
            printf("Can't determine your guess :( \n");
            DetermineNodeInfo(aki_tree, answerNode, (Node*) StackPop(&path_nodes));
            break;

        default:
            printf("Unknown answer, please type valid answer! \n");
            break;
    }

    #if (PRINT_DEBUG == 1)
        printf("Закончил функцию AkiGame. \n");
    #endif

    StackDestructor(&path_nodes);
    StackDestructor(&backtrace);
}

void ConstructAnswerStack(Node* question, Stack_t* backtrack, Stack_t* path_nodes) {
    REPORT(backtrack ); // the stack CAN be empty, but it's struct at least should not be NULL
    REPORT(path_nodes);

    #if (PRINT_DEBUG == 1)
        printf("Вхожу в функцию ConstructAnswerStack. "
               "question = %p, backtrack = %p, path_nodes = %p. \n", question, backtrack, path_nodes);
    #endif

    StackPush(path_nodes, (stackElement) question);

    if (!LEFT(question)) {
        return;
    }

    uint8_t ans[CMD_ANSWER]; // ans buffer
    ScanAnswer(ans);

    #if (PRINT_DEBUG == 1)
        printf("Получил ans. ans = %s. \n", ans);
    #endif

    /*
     * switch chooses where to go
     * if 'YES' - we go to the left question
     * if 'NO' - we go to the right question
     */

    switch (*ans) {
        case 'Y':
        case 'y':
            ConstructAnswerStack(LEFT(question), backtrack,  path_nodes);
            break;

        case 'N':
        case 'n':
            ConstructAnswerStack(RIGHT(question), backtrack, path_nodes);
            break;

        case '?':
        case 'i':
        case 'I':
            StackPush(backtrack, (stackElement) question);
            ConstructAnswerStack(LEFT(question), backtrack,  path_nodes);
            break;

        default:
            ERR("Unknown answer, please type it correctly \n");
            break;
    }

    #if (PRINT_DEBUG == 1)
        printf("Выхожу из функциим ConstructAnswerStack. \n");
    #endif

}

void ScanAnswer(uint8_t* ans) {
    REPORT(ans);

    #if (PRINT_DEBUG == 1)
        printf("Вхожу в функцию ScanAnswer(). ans = %s. \n", ans);
    #endif

    while (TRUE) {

        #if (PRINT_DEBUG == 1)
                printf("Вхожу в цикл функции ScanAnswer(). \n");
        #endif

        int checker = scanf("%[NYynIi?]", ans);
        if (checker) break;
        printf("Please enter valid answer: yes, no, Yes, No, I, idk, Idk, or ? \n");
        //FLUSH;

        #if (PRINT_DEBUG == 1)
                printf("Выхожу из цикла ScanAnswer(). ans = %s. \n", ans);
        #endif
    }

    #if (PRINT_DEBUG == 1)
        printf("Выхожу из функции ScanAnswer(). ans = %s. \n", ans);
    #endif
}

void ScanOnlyYesOrNoAnswer(uint8_t* ans) {
    REPORT(ans);

    #if (PRINT_DEBUG == 1)
        printf("Вхожу в функцию ScanOnlyYesOrNoAnswer(). ans = %s. \n", ans);
    #endif

    while (TRUE) {

        #if (PRINT_DEBUG == 1)
            printf("Вхожу в цикл ScanOnlyYesOrNoAnswer(). \n");
        #endif

        int checker = scanf("%[NYyn]", ans);
        if (checker) break;
        printf("Please enter valid answer: yes, no, Yes or No");
        //FLUSH;

        #if (PRINT_DEBUG == 1)
            printf("Выхожу из цикла ScanOnlyYesOrNoAnswer(). ans = %s. \n", ans);
        #endif
    }
    #if (PRINT_DEBUG == 1)
        printf("Выхожу из функции ScanOnlyYesOrNoAnswer(). ans = %s. \n", ans);
    #endif
}

void DetermineNodeInfo(Tree* aki_tree, Node* actual, Node* prev) {
    REPORT(aki_tree);
    REPORT(actual  );

    #if (PRINT_DEBUG == 1)
        printf("Вхожу в функцию DetermineNodeInfo. "
               "aki_tree = %p, actual = %p, prev = %p. \n", aki_tree, actual, prev);
    #endif

    ROOT(aki_tree) == actual ? printf("case if aki_tree root is 'actual' node\n") : printf("other case \n");

    Node* last_element = 0;
    if (ROOT(aki_tree) != actual) {
        last_element = actual;
        actual = MakeRawNode((nodeValue)(aki_tree->qbyte_code + aki_tree->bytes_count), NULL, NULL);
    }

    //FLUSH;

    LEFT(actual)     = MakeRawNode((nodeValue)(aki_tree->qbyte_code + aki_tree->bytes_count), NULL, NULL);
    aki_tree->bytes_count  += ScanLine(aki_tree->qbyte_code + aki_tree->bytes_count, NODE_VALUE_SIZE, stdin);

    printf("Left branch value is %s. Last element value is %s\n", VALUE(LEFT(actual)), VALUE(last_element));

    //FLUSH;

    VALUE(actual)           = aki_tree->qbyte_code + aki_tree->bytes_count;
    aki_tree->bytes_count  += ScanLine(aki_tree->qbyte_code + aki_tree->bytes_count, NODE_VALUE_SIZE, stdin);

    if (ROOT(aki_tree)   == actual) {
        RIGHT(actual)    = MakeRawNode((nodeValue)(aki_tree->qbyte_code + aki_tree->bytes_count), NULL, NULL);
        aki_tree->bytes_count  += ScanLine(aki_tree->qbyte_code + aki_tree->bytes_count, NODE_VALUE_SIZE, stdin);
    } else {
        RIGHT(actual) = last_element;
    }
    if (ROOT(aki_tree) != actual) {
        if (LEFT(prev) == last_element) {
            LEFT(prev) = actual;
        }
        if (RIGHT(prev) == last_element) {
            RIGHT(prev) = actual;
        }
    }

    #if (PRINT_DEBUG == 1)
        printf("Выхожу из функции DetermineNodeInfo(). \n");
    #endif
}

int ParseAkiBase(Tree* tree, Stack_t* nodes_stack) {
    REPORT(tree       );
    REPORT(nodes_stack);

    #if (PRINT_DEBUG == 1)
        printf("Вхожу в функцию ParseAkiBase. "
               "tree = %p, nodes_stack = %p \n", tree, nodes_stack);
    #endif

    uint8_t extra_symbols[EXTRA_SYMBOLS] = "";

    for (size_t current_string = 0; current_string < tree->base.total_lines; current_string++) {
        if (tree->base.lines[current_string].length == 1) {
            PASS;
        } else if (sscanf((const char*)tree->base.lines[current_string].content,
                          LEFT_BRACKET NO_BRACKETS RIGHT_BRACKET, extra_symbols)) {

            /* MakeParseable() changes last '}' to ' ' and returns amount of brackets
             *
             *
            int sub_brackets = MakeParseable(tree->base.lines[current_string].content, '}', ' ');
            if (sub_brackets > 1) {
                ERR("Brackets in node content!");
            }
            */

            #if (PRINT_DEBUG == 1)
                printf("Записываю в nodes_stack: 0, \"%s\" \n", tree->base.lines[current_string].content + 1);
            #endif

            StackPush(nodes_stack, 0);
            StackPush(nodes_stack, tree->base.lines[current_string].content + 1); // shifting the first one '{'
        } else {
            #if (PRINT_DEBUG == 1)
                printf("Записываю в nodes_stack: \"%s\" \n", tree->base.lines[current_string].content);
            #endif

            StackPush(nodes_stack, tree->base.lines[current_string].content);
        }
    }
    #if (PRINT_DEBUG == 1)
        printf("Выхожу из функции ParseAkiBase(). ParseAkiBase() вернет 1. \n");
    #endif
    return 1;
}

int MakeParseable(uint8_t* str, uint8_t orig, uint8_t rep) {
    uint8_t* ix = str;
    int n = 0;
    while ((ix = (uint8_t*) strchr((char*) ix, orig)) != NULL) {
        *ix++ = rep;
        n++;
    }
    return n;
}
void PutAkiData(Tree* aki_tree) {
    REPORT(aki_tree);

    FILE* output = fopen("aki.txt", "w");
    REPORT(output); // CHECK if output is NULL

    SaveNodeInfoToTheBase(ROOT(aki_tree), output);
    fclose(output);
}

void SaveNodeInfoToTheBase(Node* root_node, FILE* output) {
    REPORT(root_node)

    if (LEFT(root_node)) {
        fprintf(output, "{\n%s\n", VALUE(root_node));
        /*
         * Recursively calling SaveNodeInfoToTheBase() and printing next branch part of a node just like it was
         */
        SaveNodeInfoToTheBase(LEFT(root_node) , output);
        SaveNodeInfoToTheBase(RIGHT(root_node), output);
        fprintf(output, "}\n");
    } else {
        fprintf(output, "{%s}\n", VALUE(root_node));
    }
}

void AkiDescribe(Tree* aki_tree) {
    REPORT(aki_tree);

    printf("Playing the descriptor game!");
    StackConstructor(desc_path_stack);

    uint8_t member[NODE_VALUE_SIZE] = "";
    FLUSH; // flushing buffer
    int is_successful_describer = 0;

    ScanLine(member, NODE_VALUE_SIZE, stdin);

    PutDescInfo(ROOT(aki_tree), member, &desc_path_stack, &is_successful_describer);

    !is_successful_describer ?
    printf("Can't describe this member. Please try again! \n") :
    PrintDescInfo(&desc_path_stack, member);

    StackDestructor(&desc_path_stack);
}

void PutDescInfo(Node* root_node, uint8_t curmemb[], Stack_t* desc_path_stack, int *is_success) {
    REPORT(root_node);
    REPORT(curmemb);
    REPORT(desc_path_stack);
    /*
     * Recursively going from the root_node and popping from the desc_stack
     */

    if (*is_success) {
        return;
    } else {
        StackPush(desc_path_stack, (stackElement) root_node);
    }

    if (!strcmp((const char*) VALUE(root_node), (const char*) curmemb)) {
        SET_VALUE(is_success, 1); return;
    }

    if (LEFT(root_node) && RIGHT(root_node)) {
        /*
         * Recursive call chain
         */
        //TODO: make it not recursive
        PutDescInfo(LEFT(root_node),  curmemb, desc_path_stack, is_success);
        PutDescInfo(RIGHT(root_node), curmemb, desc_path_stack, is_success);
    }
    if (*is_success == 0) StackPop(desc_path_stack);
}

void PrintDescInfo (Stack_t* stack, uint8_t person[]) {
    REPORT(stack );
    REPORT(person);

    printf("This is your person (%s) path: ", person);
    for (stackInfo current_node = 0; current_node < stack->current_index - 1; current_node++) {
        Node* currentNode = (Node*) PopElementByIndex(stack, current_node);
        Node* nextNode    = (Node*) PopElementByIndex(stack, current_node + 1);

        if (LEFT(currentNode) == nextNode) {
            printf("%s; ", VALUE(currentNode));
        } else {
            printf("value is %s; ", VALUE(currentNode));
        }
        PushElementByIndex(stack, (stackElement) nextNode, current_node + 1);
    }
    stack->current_index = 0;
    MAKE_NEWLINE;
}

void AkiCompare(Tree* aki_tree) {
    REPORT(aki_tree);

    printf("I start comparing objects!");

    StackConstructor(desc_path_first_iter);
    StackConstructor(desc_path_second_iter);

    FLUSH;

    int success_memb1 = 0, success_memb2 = 0; // FLAGS-CHECKER TO SEE IF WE SUCCESSFULLY DESCRIBED
    uint8_t first_memb[NODE_VALUE_SIZE]  = "";
    uint8_t second_memb[NODE_VALUE_SIZE] = "";

    ScanLine(first_memb, NODE_VALUE_SIZE, stdin); // SCANNING ONE LINE
    PutDescInfo(ROOT(aki_tree), first_memb, &desc_path_first_iter, &success_memb1);

    FLUSH;

    ScanLine(second_memb, NODE_VALUE_SIZE, stdin);
    PutDescInfo(ROOT(aki_tree), second_memb, &desc_path_second_iter, &success_memb2);

    if (!success_memb1 || !success_memb2) {
        printf("Something wrong happened, can't compare!! \n");
    }
    else {
        PrintComparedPaths(&desc_path_first_iter, &desc_path_second_iter, first_memb, second_memb);
    }

    /*
     * DO NOT FORGET TO FREE STACK!!!
     */

    StackDestructor(&desc_path_first_iter);
    StackDestructor(&desc_path_second_iter);
}

void PrintComparedPaths(Stack_t* desc_first, Stack_t* desc_second, uint8_t first_memb[], uint8_t* second_memb) {
    REPORT(desc_first );
    REPORT(desc_second);
    REPORT(first_memb );
    REPORT(second_memb);

    printf("Printing paths for %s and %s! \n", first_memb, second_memb);
    /*
     * FIRST CYCLE IS WORKING UNTIL THE PATHS ARE THE SAME
     */
    for (treeInfo pcur = 0; pcur < desc_first->current_index - 1 && pcur < desc_second->current_index - 1;
         pcur++) {
        SET_NODES;

        if (cur_path_node == second_path_node && next_path_node == second_next_node) {
            if (LEFT(cur_path_node) == next_path_node) {
                printf("left_parent: %s; ", VALUE(cur_path_node));
            } else {
                printf("right_parent %s; ", VALUE(cur_path_node));
            }
        } else {
            if (LEFT(cur_path_node) == next_path_node) {
                printf("%s %s, but ", first_memb, VALUE(cur_path_node));
            } else {
                printf("%s is %s, but ", first_memb, VALUE(cur_path_node));
            }

            if (LEFT(second_path_node) == second_next_node) {
                printf("%s %s; ", second_memb, VALUE(second_path_node));
            } else {
                printf("%s is %s; ", second_memb, VALUE(second_path_node));
            }
        }
        PushElementByIndex(desc_first,  (stackElement)   next_path_node, pcur + 1);
        PushElementByIndex(desc_second, (stackElement) second_next_node, pcur + 1);
    }
    
    MAKE_NEWLINE;
    if (desc_first->current_index != desc_second->current_index) {
        Stack_t* long_d  = 0;
        Stack_t* short_d = 0;

        if (desc_first->current_index > desc_second->current_index) {
            long_d  = desc_first;
            short_d = desc_second;
            printf("first member (%s) path: ", first_memb);
        } else {
            long_d  = desc_second;
            short_d = desc_first;
            printf("second member (%s) path: ", second_memb);
        }
        
        for (treeInfo pcur = short_d->current_index - 1; pcur < long_d->current_index - 1; pcur++) {
            Node* currentNode = (Node*) PopElementByIndex(long_d, pcur);
            Node* nextNode    = (Node*) PopElementByIndex(long_d, pcur + 1);

            if (LEFT(currentNode) == nextNode) {
                printf("%s; ", VALUE(currentNode));
            } else {
                printf("next is %s; ", VALUE(currentNode));
            }

            PushElementByIndex(long_d, (stackElement) nextNode, pcur + 1);
        }

        desc_first->current_index = desc_second->current_index = 0; // NOW IT's zero, do not forget to set it for ASSERTION
        MAKE_NEWLINE;
    }
}