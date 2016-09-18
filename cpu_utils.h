//
// Created by dearm on 9/17/2016.
// Utility functions for our CPU such as decoders, converters and the like
//

#ifndef DESIGN_ISA_CPU_UTILS_H
#define DESIGN_ISA_CPU_UTILS_H

#include <stdbool.h>
#include "exec_utils.h"

//instruction breakdown
#define OPCODE_SIZE 6
#define RS_SIZE 5
#define RT_SIZE 5
#define IMM_SIZE 16

//mem split sizes
#define BOOT_SECTOR 512
#define TEXT_SEGMENT 100
#define DATA_SEGMENT 50
#define STACK_SEGMENT 64000 //where the stack begins
#define HEAP_SEGMENT 662

#define WORD_SIZE 32
#define PC_SIZE 16
#define MEM_ROWS 64000 //number of rows for 16b addressing with 32b WORD 64KB total memory
#define NUM_REG 8

#define BOOT_ADDR 0

//flag defines
#define OVERFLOW_FLAG 1
#define ZERO_FLAG 0

EXEC_INFO initCPU(char *PC);
void loadAndStoreInstrs(char *fileName, char *memory[MEM_ROWS], EXEC_INFO *info);
void runProgram(char **memory, char *PC, char *memAddr, char *memData, char **regFile, char *flags, EXEC_INFO info);

char *ALU(int op, char *opLeft, char *opRight, char *flags, int size);
char *addBinary (char *opLeft, char *opRight, char *flags, int size);
char *signExtend(char *value, int size);

char *convertInstrToBin(char *instr);

char *convertToBin(int toConvert, bool isImmVal);
char *decimalToBinary(int toConvert, int numOfBits);
int binaryToDecimal(char *binary, int size);



#endif //DESIGN_ISA_CPU_UTILS_H
