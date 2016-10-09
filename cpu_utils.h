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

#define BOOT_ADDR "0000000000000000"

//flag defines
#define OVERFLOW_FLAG 1
#define ZERO_FLAG 0

//WORD_SIZE + 1 for /0 for debugging and printing
extern char *memory[MEM_ROWS]; //64kb mem
extern char *regFile[NUM_REG];

extern char PC[PC_SIZE + 1];
extern char memAddr[WORD_SIZE + 1];
extern char memData[WORD_SIZE + 1];
extern char instrReg[WORD_SIZE + 1];
extern char flags[WORD_SIZE + 1];

EXEC_INFO initCPU();
void loadAndStoreInstrs(char *fileName, EXEC_INFO *info);
void runProgram(EXEC_INFO info);

char *ALU(int op, char *opLeft, char *opRight, int size, int setFlags);
char *addBinary (char *opLeft, char *opRight, int size, int setFlags);
char *subBinary (char *opLeft, char *opRight, int size, int setFlags);
char *signExtend(char *value, int size);
char *convertInstrToBin(char *instr);

//functions for decoding instructions
char *genLWSWbinInstr(char **tokens);
char *genLDSTbinInstr(char **tokens);
char *genRTypeInstr(char **tokens);

//char *convertToBin(int toConvert, bool isImmVal);
char *decimalToBinary(int toConvert, int numOfBits);
char *decimalToComplementBinary(int toConvert, int numOfBits);
int binaryToDecimal(char *binary, int size);

void printExecutionData(int instrNum);

char* mulBinary(char* left, char* right, int size, int setFlags);
char* leftShift(char* input, int size);
char* rightShift(char* input, int size);
#endif //DESIGN_ISA_CPU_UTILS_H
