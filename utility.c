//
// Created by dearm on 10/31/2016.
//

#include <stdio.h>
#include <malloc.h>
#include <mem.h>
#include <math.h>
#include <stdbool.h>
#include "utility.h"
#include "cpu_utils.h"
#include "opcodes.h"

/**
 * Utility function to convert from decimal to binary string
 * @param toConvert : number to convert
 * @param numOfBits : number of bits
 * @return returns the binary string representation for the decimal
 */
char *decimalToBinary(int toConvert, int numOfBits){
    char *binary = (char *)malloc(sizeof(char) * numOfBits + 1);

    mallocErrorCheck(binary);

    binary[numOfBits] = '\0';
    for(int i = numOfBits - 1; i >= 0; i--){
        toConvert & 1 ? (binary[i] = '1') : (binary[i] = '0');
        toConvert >>= 1;
    }
    return binary;
}

/**
 * Utility function to convert from binary to decimal
 * @param binary : string that contains binary pattern
 * @param size  : number of bits
 * @return returns the string converted to a decimal
 */
int binaryToDecimal(char *binary, int size) {
    int num = 0;

    for(int i = size - 1; i >= 0; i--) {
        if(binary[i] == '1')
            num += pow(2, size - 1 - i);
    }
    return num;
}

int signedBinaryToDecimal(char *binary, int size) {
    int result = 0;
    char *convertedVal = malloc(sizeof(char) * size + 1);
    mallocErrorCheck(convertedVal);
    strcpy(convertedVal, binary);

    bool isNeg = false;

    if(convertedVal[size - 2] == '1'){
        for(int i = size - 1; i >= 0; i--){
            if(convertedVal[i] == '1'){
                convertedVal[i] = '0';
            }else convertedVal[i] = '1';
        }
        convertedVal = ALU(ADD_OP, convertedVal, "1", size, 0);
        isNeg = true;
    }

    result = binaryToDecimal(convertedVal, size);
    free(convertedVal);

    if(isNeg)
        result = result * -1;
    return result;
}

/**
 * Takes a decimal and inverts the bits
 * @param toConvert : number to invert
 * @param numOfBits : number of bits
 * @return returns a string of bits. The result is ~toConvert
 */
char *decimalToComplementBinary(int toConvert, int numOfBits){
    char *binary = (char *)malloc(sizeof(char) * numOfBits + 1);
    mallocErrorCheck(binary);

    binary[numOfBits] = '\0';
    for(int i = numOfBits - 1; i >= 0; i--){
        toConvert & 1 ? (binary[i] = '0') : (binary[i] = '1');
        toConvert >>= 1;
    }
    return binary;
}


/**
 * @param ptr : pointer to the memory to check
 */
void mallocErrorCheck(char *ptr){
    if(ptr == NULL){
        printf("Error on malloc. Exiting\n");
        exit(1);
    }
}

/**
 * Function to simplify printing execution data
 * @param instrNum : the offset of the instruction to print
 */
void printExecutionData(int instrNum){
    char instrBuilder[250];
    char *instrFromMem = memory[TEXT_SEGMENT + instrNum];
    char rs[RTYPE_ADDR_SIZE + 1], rt[RTYPE_ADDR_SIZE + 1], rd[RTYPE_RD_SIZE + 1], imm[IMM_SIZE + 1];

    char *freeHandle = NULL;

    rs[RTYPE_ADDR_SIZE] = '\0';
    rt[RTYPE_ADDR_SIZE] = '\0';
    rd[RTYPE_RD_SIZE] = '\0';
    imm[IMM_SIZE] = '\0';

    if(strncmp(LW, instrFromMem, OPCODE_SIZE) == 0) {
        strncpy(rs, instrFromMem + OPCODE_SIZE, REG_ADDR_SIZE);
        strncpy(rt, instrFromMem + OPCODE_SIZE + REG_ADDR_SIZE, REG_ADDR_SIZE);
        strncpy(imm, instrFromMem + OPCODE_SIZE + REG_ADDR_SIZE + REG_ADDR_SIZE, IMM_SIZE);
        sprintf(instrBuilder, "%s $%d, %d($%d)", "LW", binaryToDecimal(rt, REG_ADDR_SIZE), binaryToDecimal(imm, IMM_SIZE), binaryToDecimal(rs, REG_ADDR_SIZE));

    }else if (strncmp(SW, instrFromMem, OPCODE_SIZE) == 0){
        strncpy(rs, instrFromMem + OPCODE_SIZE, REG_ADDR_SIZE);
        strncpy(rt, instrFromMem + OPCODE_SIZE + REG_ADDR_SIZE, REG_ADDR_SIZE);
        strncpy(imm, instrFromMem + OPCODE_SIZE + REG_ADDR_SIZE + REG_ADDR_SIZE, IMM_SIZE);
        sprintf(instrBuilder, "%s $%d, %d($%d)", "SW", binaryToDecimal(rt, REG_ADDR_SIZE), binaryToDecimal(imm, IMM_SIZE), binaryToDecimal(rs, REG_ADDR_SIZE));

    }else if (strncmp(LD, instrFromMem, OPCODE_SIZE) == 0) {
        char indexReg[REG_ADDR_SIZE + 1], baseReg[REG_ADDR_SIZE + 1], distance[DIST_SIZE + 1], scale[SCALE_SIZE + 1], dest[REG_ADDR_SIZE + 1];

        indexReg[REG_ADDR_SIZE] = '\0', baseReg[REG_ADDR_SIZE] = '\0', distance[7] = '\0', scale[3] = '\0', dest[REG_ADDR_SIZE] = '\0'; //null terminate our strings

        // $reg, distance(base, index, scale)
        strncpy(dest, instrFromMem + OPCODE_SIZE, REG_ADDR_SIZE);
        strncpy(distance, instrFromMem + OPCODE_SIZE + REG_ADDR_SIZE, DIST_SIZE);
        strncpy(baseReg, instrFromMem + OPCODE_SIZE + REG_ADDR_SIZE + DIST_SIZE, REG_ADDR_SIZE);
        strncpy(indexReg, instrFromMem + OPCODE_SIZE + REG_ADDR_SIZE + DIST_SIZE + REG_ADDR_SIZE, REG_ADDR_SIZE);
        strncpy(scale, instrFromMem + OPCODE_SIZE + REG_ADDR_SIZE + 7 + REG_ADDR_SIZE + REG_ADDR_SIZE, 3);
        sprintf(instrBuilder, "%s $%d, %d($%d, $%d, %d)", "LD", binaryToDecimal(dest, REG_ADDR_SIZE), binaryToDecimal(distance, 7), binaryToDecimal(baseReg, REG_ADDR_SIZE),
                binaryToDecimal(indexReg, REG_ADDR_SIZE), binaryToDecimal(scale, 4));

    }else if (strncmp(ST, instrFromMem, OPCODE_SIZE) == 0) {
        char indexReg[REG_ADDR_SIZE + 1], baseReg[REG_ADDR_SIZE + 1], distance[8], scale[4], dest[REG_ADDR_SIZE + 1];

        indexReg[REG_ADDR_SIZE] = '\0', baseReg[REG_ADDR_SIZE] = '\0', distance[7] = '\0', scale[3] = '\0', dest[REG_ADDR_SIZE] = '\0'; //null terminate our strings

        // $reg, distance(base, index, scale)
        strncpy(dest, instrFromMem + OPCODE_SIZE, REG_ADDR_SIZE);
        strncpy(distance, instrFromMem + OPCODE_SIZE + REG_ADDR_SIZE, DIST_SIZE);
        strncpy(baseReg, instrFromMem + OPCODE_SIZE + REG_ADDR_SIZE + DIST_SIZE, REG_ADDR_SIZE);
        strncpy(indexReg, instrFromMem + OPCODE_SIZE + REG_ADDR_SIZE + DIST_SIZE + REG_ADDR_SIZE, REG_ADDR_SIZE);
        strncpy(scale, instrFromMem + OPCODE_SIZE + REG_ADDR_SIZE + 7 + REG_ADDR_SIZE + REG_ADDR_SIZE, SCALE_SIZE);
        sprintf(instrBuilder, "%s $%d, %d($%d, $%d, %d)", "ST", binaryToDecimal(dest, REG_ADDR_SIZE), binaryToDecimal(distance, 7), binaryToDecimal(baseReg, REG_ADDR_SIZE),
                binaryToDecimal(indexReg, REG_ADDR_SIZE), binaryToDecimal(scale, 4));

    }else if(strncmp(ADD, instrFromMem, OPCODE_SIZE) == 0) {
        strcpy(instrBuilder, freeHandle = buildInstrForRTypePrint(instrFromMem, "ADD"));
        free(freeHandle);

    }else if(strncmp(SUB, instrFromMem, OPCODE_SIZE) == 0) {
        strcpy(instrBuilder, freeHandle = buildInstrForRTypePrint(instrFromMem, "SUB"));
        free(freeHandle);

    } else if(strncmp(MUL, instrFromMem, OPCODE_SIZE) == 0) {
        strcpy(instrBuilder, freeHandle = buildInstrForRTypePrint(instrFromMem, "MUL"));
        free(freeHandle);

    }else if(strncmp(MOD, instrFromMem, OPCODE_SIZE) == 0) {
        strcpy(instrBuilder, freeHandle = buildInstrForRTypePrint(instrFromMem, "MOD"));
        free(freeHandle);
    }else if(strncmp(DIV, instrFromMem, OPCODE_SIZE) == 0) {
        strcpy(instrBuilder, freeHandle = buildInstrForRTypePrint(instrFromMem, "DIV"));
        free(freeHandle);
    }else if(strncmp(ADDI, instrFromMem, OPCODE_SIZE) == 0) {
        strcpy(instrBuilder, freeHandle = buildInstrForITypePrint(instrFromMem, "ADDI"));
        free(freeHandle);
    }else if(strncmp(J, instrFromMem, OPCODE_SIZE) == 0) {
        strcpy(instrBuilder, "J ");
        printf("instr from mem %s\n", instrFromMem);
        for(int i = 0; i < 5; i++){

            if((labels[i].labelName != NULL) && (labels[i].offset == binaryToDecimal(instrFromMem + OPCODE_SIZE, JTA_SIZE))) {
                strcat(instrBuilder, labels[i].labelName);
                break;
            }
        }
    }else if(strncmp(BLT, instrFromMem, OPCODE_SIZE) == 0) {
        char left[REG_ADDR_SIZE + 1], right[REG_ADDR_SIZE + 1], label[IMM_SIZE + 1];
        left[REG_ADDR_SIZE] = '\0';
        right[REG_ADDR_SIZE] = '\0';
        label[IMM_SIZE] = '\0';

        strncpy(left, instrFromMem + OPCODE_SIZE, REG_ADDR_SIZE);
        strncpy(right, instrFromMem + OPCODE_SIZE + REG_ADDR_SIZE, REG_ADDR_SIZE);
        printf("instr from mem %s\n", instrFromMem);

        for(int i = 0; i < 5; i++){
            if((labels[i].labelName != NULL) && (labels[i].offset == signedBinaryToDecimal(instrFromMem + OPCODE_SIZE + REG_ADDR_SIZE + REG_ADDR_SIZE, IMM_SIZE))) {
                strcpy(label, labels[i].labelName);
                printf("label %s\n", label);
                break;
            }
        }

        sprintf(instrBuilder, "%s $%d, $%d, %s", "BLT", binaryToDecimal(left, REG_ADDR_SIZE), binaryToDecimal(right, REG_ADDR_SIZE), label);
    }

    printf("%-30s %-40s %-30s\n", "Instruction", "Binary representation", "Program Counter");
    printf("%-30s %-40s %-30s\n", instrBuilder, instrFromMem, PC);
    printf("%-30s %-40s %-40s\n", "Memory Address", "Memory Data", "Flags");
    printf("%-5d                       %5d                                       %-30s\n\n", binaryToDecimal(memAddr, WORD_SIZE), binaryToDecimal(memData, WORD_SIZE), flags);

    printf("\n %-30s      %-30s\n", "Register name", "Data");
    for(int i = 0 ; i < NUM_REG; i++) {
        printf("\t$%d \t\t%-20s\n", i, regFile[i]);
    }
    printf("********************************************************************************************\n");
}

/**
 * Utility function for printing for R type instructions
 */
char *buildInstrForRTypePrint(char *instr, char *instrName){
    char rs[RTYPE_ADDR_SIZE + 1], rt[RTYPE_ADDR_SIZE + 1], rd[RTYPE_RD_SIZE + 1], imm[IMM_SIZE + 1];
    char instrBuilder[250];

    rs[RTYPE_ADDR_SIZE] = '\0';
    rt[RTYPE_ADDR_SIZE] = '\0';
    rd[RTYPE_ADDR_SIZE] = '\0';
    imm[IMM_SIZE] = '\0';

    strncpy(rd, instr + OPCODE_SIZE, 10);
    strncpy(rs, instr + OPCODE_SIZE + 10, 8);
    strncpy(rt, instr + OPCODE_SIZE + 10 + 8, 8);

    sprintf(instrBuilder, "%s $%d, $%d, $%d", instrName, binaryToDecimal(rd, 10), binaryToDecimal(rs, 8), binaryToDecimal(rt, 8));
    char *rebuiltInstr = malloc(strlen(instrBuilder) + 1);
    mallocErrorCheck(rebuiltInstr);
    strcpy(rebuiltInstr, instrBuilder);

    return rebuiltInstr;
}

char *buildInstrForITypePrint(char *instr, char *instrName){
    char rd[REG_ADDR_SIZE +1], rs[REG_ADDR_SIZE + 1], imm[IMM_SIZE + 1];
    char instrBuilder[250];

    rd[REG_ADDR_SIZE] = '\0';
    rs[REG_ADDR_SIZE] = '\0';
    imm[IMM_SIZE] = '\0';

    strncpy(rd, instr + OPCODE_SIZE, REG_ADDR_SIZE);
    strncpy(rs, instr + OPCODE_SIZE + REG_ADDR_SIZE, REG_ADDR_SIZE);
    strncpy(imm, instr + OPCODE_SIZE + REG_ADDR_SIZE + REG_ADDR_SIZE, IMM_SIZE);

    sprintf(instrBuilder, "%s $%d, $%d, %d", instrName, binaryToDecimal(rd, REG_ADDR_SIZE), binaryToDecimal(rs, REG_ADDR_SIZE), binaryToDecimal(imm, IMM_SIZE));
    char *rebuiltInstr = malloc(strlen(instrBuilder) + 1);
    mallocErrorCheck(rebuiltInstr);
    strcpy(rebuiltInstr, instrBuilder);

    return rebuiltInstr;
}
