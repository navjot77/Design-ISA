//
// Created by dearm on 9/17/2016.
//

#ifndef DESIGN_ISA_EXEC_UTILS_H
#define DESIGN_ISA_EXEC_UTILS_H

typedef struct exec_info{
    int heap_ptr;
    int stack_ptr;
    int lines;
}EXEC_INFO;

typedef struct label_info{
    int lineNum;
    int offset;
    char *labelName;
}LABEL_INFO;

#endif //DESIGN_ISA_EXEC_UTILS_H
