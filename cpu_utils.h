//
// Created by dearm on 9/17/2016.
// Utility functions for our CPU such as decoders, converters and the like
//

#ifndef DESIGN_ISA_CPU_UTILS_H
#define DESIGN_ISA_CPU_UTILS_H

#include <stdbool.h>

//mem split sizes
#define BOOT_SECTOR 512


#define WORD_SIZE 32
#define PC_SIZE 16
#define MEM_ROWS 64000 //number of rows for 16b addressing with 32b WORD 64KB total memory
#define NUM_REG 8

#define BOOT_ADDR 0

void initCPU(unsigned char *PC);
void loadAndStoreInstrs(char *fileName, unsigned char *memory[MEM_ROWS]);
void runProgram();
unsigned char *ALU(unsigned char *opLeft, unsigned char *opRight);

char *convertInstrToBin(char *instr);
unsigned char *addBinary(unsigned char *opLeft, unsigned char *opRight);

unsigned char *convertToBin(int toConvert, bool isImmVal);
unsigned char *decimalToBinary(int toConvert, int numOfBits);
int binaryToDecimal(char *binary);


#endif //DESIGN_ISA_CPU_UTILS_H
