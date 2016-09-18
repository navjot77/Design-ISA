//
// Created by dearm on 9/17/2016.
//
#include <stdlib.h>
#include <stdio.h>
#include <mem.h>
#include "cpu_utils.h"
#include "exec_utils.h"

char *checkAndGetArg(int argc, char **arg);
void init(char *PC, char *memAddr, char *memData, char *instrReg, char *flags);

void main(int argc, char **argv)
{
    char *sourceCode;

    //WORD_SIZE + 1 for /0 for debugging and printing
    char *memory[MEM_ROWS]; //64kb mem
    char regFile[NUM_REG][WORD_SIZE + 1];

    char PC[PC_SIZE + 1];
    char memAddr[WORD_SIZE + 1];
    char memData[WORD_SIZE + 1];
    char instrReg[WORD_SIZE + 1];
    char flags[WORD_SIZE + 1];

    //must malloc separately since we can't alloc more than 64kb in one go
    for(int i = 0; i < MEM_ROWS; i++){
        memory[i] = malloc(sizeof(char) * WORD_SIZE + 1);
    }

    init(PC, memAddr, memData, instrReg, flags);

    //for testing
    memory[HEAP_SEGMENT] = "00000000000000000000000000000010";

    EXEC_INFO info = initCPU(PC); //need to init the PC
    sourceCode = checkAndGetArg(argc, argv);
    loadAndStoreInstrs(sourceCode, memory, &info);
    runProgram(PC, memAddr, memData, regFile, flags, info);

}

//will get the file name from arg
char *checkAndGetArg(int argc, char **arg){
    char *file;

    if(argc > 2) {
        printf("Incorrect number of arguments. File is required\n");
        exit;
    }

    file = (char *)malloc(sizeof(char) * strlen(arg[1]) + 1);
    strcpy(file, arg[1]);
    return file;
}

void init(char *PC, char *memAddr, char *memData, char *instrReg, char *flags) {

    flags[WORD_SIZE] = '\0';
    PC[WORD_SIZE] = '\0';
    memAddr[WORD_SIZE] = '\0';
    memData[WORD_SIZE] = '\0';
    instrReg[WORD_SIZE] = '\0';

    for(int i = 0; i < WORD_SIZE; i++) {
        flags[i] = '0';
        memAddr[i] = '0';
        memData[i] = '0';
        instrReg[i] = '0';
    }
}
