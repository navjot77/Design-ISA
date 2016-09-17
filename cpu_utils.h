//
// Created by dearm on 9/17/2016.
// Utility functions for our CPU such as decoders, converters and the like
//

#ifndef DESIGN_ISA_CPU_UTILS_H
#define DESIGN_ISA_CPU_UTILS_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "opcodes.h"

#define BOOT_SECTOR 512

#define WORD_SIZE 32
#define PC_SIZE 16
#define MEM_ROWS 64000 //number of rows for 16b addressing with 32b WORD 64KB total memory
#define NUM_REG 8

void initCPU(unsigned char *PC);

void loadAndStoreInstrs(char *fileName);
#endif //DESIGN_ISA_CPU_UTILS_H
