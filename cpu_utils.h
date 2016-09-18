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

//memory start addresses
#define TEXT_SEGMENT 512    //where text segment begins
#define DATA_SEGMENT 600
#define STACK_SEGMENT 63999 //where the stack begins
#define HEAP_SEGMENT 661    //where heap begins

//memory segment sizes
#define TEXT_SEGMENT_SIZE DATA_SEGMENT-TEXT_SEGMENT
#define DATA_SEGMENT_SIZE HEAP_SEGMENT-DATA_SEGMENT

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

void printExecutionData(char **memory, char *PC, char *memAddr, char *memData, char **regFile, char *flags, char *instruction, int instrNum);
#endif //DESIGN_ISA_CPU_UTILS_H
