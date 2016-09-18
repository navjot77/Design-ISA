//
// Created by dearm on 9/17/2016.
//
#include <stdlib.h>
#include <stdio.h>
#include <mem.h>
#include "cpu_utils.h"

char *checkAndGetArg(int argc, char **arg);

void main(int argc, char **argv)
{
    char *sourceCode;

    unsigned char memory[MEM_ROWS][WORD_SIZE]; //64kb mem
    unsigned char regFile[NUM_REG][WORD_SIZE];

    unsigned char PC[PC_SIZE];
    unsigned char memAddr[WORD_SIZE];
    unsigned char memData[WORD_SIZE];
    unsigned char flags[WORD_SIZE];

    //initCPU(PC); //need to init the PC
    //sourceCode = checkAndGetArg(argc, argv);
    //loadAndStoreInstrs(sourceCode, memory);
    //runProgram();

    convertInstrToBin("LW $2, offset 4($1)");

//    printf("note: \n The instruction that you enter is not considered for now, it can be linked from final function \n    Function works on default instruction that is hardcoded \n     I have printed Instruction register(IR) in each stage.    Final IR will be saved into memory once it is set up");
//    char inputIns[256];	//instruction register
//    //inputIns= "ADD r1, r2"
//    printf("enter instruction \n");
//    scanf("%s", &inputIns[0]); //scan input instruction(for now it is dummy)
//    //saveinstruction(inputIns);
}

//will get the file name from argv
char *checkAndGetArg(int argc, char **arg){
    char *file;

    if(argc > 2) {
        printf("Incorrect number of arguments. File is required\n");
        exit;
    }

    file = (char *)malloc(sizeof(char) * strlen(arg[1]) + 1);
    return file;
}
