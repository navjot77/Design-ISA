//
// Created by dearm on 9/17/2016.
//
#include <stdlib.h>
#include <stdio.h>
#include <mem.h>
#include "cpu_utils.h"
#include "exec_utils.h"

char *checkAndGetArg(int argc, char **arg);
void init(char *PC, char *memAddr, char *memData, char *instrReg, char *flags, char **memory, char **regFile);

void main(int argc, char **argv)
{
    char *sourceCode;

    //WORD_SIZE + 1 for /0 for debugging and printing
    char *memory[MEM_ROWS]; //64kb mem
    char *regFile[NUM_REG];

    char PC[PC_SIZE + 1];
    char memAddr[WORD_SIZE + 1];
    char memData[WORD_SIZE + 1];
    char instrReg[WORD_SIZE + 1];
    char flags[WORD_SIZE + 1];

    //must malloc separately since we can't alloc more than 64kb in one go
    for(int i = 0; i < MEM_ROWS; i++){
        memory[i] = malloc(sizeof(char) * WORD_SIZE + 1);
        if(memory[i] == NULL)
            exit;
    }

    for(int i = 0; i < NUM_REG; i++) {
        regFile[i] = (char *)malloc(sizeof(char) * WORD_SIZE + 1);
        if(regFile[i] == NULL)
            exit;
    }

    init(PC, memAddr, memData, instrReg, flags, memory, regFile);

    memory[662][WORD_SIZE] = '\0';
    if(memory[5][WORD_SIZE] == '\0')
        printf("its there!");
    for(int i = 0; i < WORD_SIZE + 1; i++)
        printf("%d %c\n", i, memory[662][i]);

    //for testing
    memory[HEAP_SEGMENT] = "00000000000000000000000000000010";
    regFile[6] = "00000000000000000000000000000111";

    EXEC_INFO info = initCPU(PC); //need to init the PC
    sourceCode = checkAndGetArg(argc, argv);
    loadAndStoreInstrs(sourceCode, memory, &info);
    printf("in main %s\n", memory[BOOT_SECTOR]);
    runProgram(memory, PC, memAddr, memData, regFile, flags, info);

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

void init(char *PC, char *memAddr, char *memData, char *instrReg, char *flags, char **memory, char **regFile) {

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

    for(int row = 0; row < MEM_ROWS; row++){
        for(int col = 0; col < WORD_SIZE; col++){
            memory[row][col] = '0';
        }
        memory[row][WORD_SIZE] = '\0';
    }

    for(int row = 0; row < NUM_REG; row++){
        for(int col = 0; col < WORD_SIZE; col++){
            regFile[row][col] = '0';
        }
        regFile[row][WORD_SIZE] = '\0';
    }

}
