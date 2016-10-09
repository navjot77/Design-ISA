//
// Created by dearm on 9/17/2016.
//
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "cpu_utils.h"
#include "opcodes.h"

#define MAX_TOKENS 10

void loadAndStoreInstrs(char *fileName, EXEC_INFO *info){
    FILE *fp = NULL;
    char buff[250];
    int memLoc = TEXT_SEGMENT;

    fp = fopen(fileName, "r");
    if(fp == NULL) {
        printf("Error opening file %s", fileName);
        exit;
    }

    while(fgets(buff, 250, fp)) {
       // decodedInstr = convertInstrToBin(buff);
        strcpy(memory[memLoc], convertInstrToBin(buff));
        memLoc++;
    }

    info->lines = memLoc - TEXT_SEGMENT;
}

char *convertInstrToBin(char *instr) {
    char *tokens[MAX_TOKENS];
    char *token;
    char *binInstr = (char *)malloc(sizeof(char) * WORD_SIZE);

    char *temp = malloc(strlen(instr) + 1);
    strcpy(temp, instr);

    //init tokens array so we know how many args for instr
    for(int i = 0; i < MAX_TOKENS; i++)
        tokens[i] = NULL;

    //get opcode
    token = strtok(temp, " ");
    //change instruction name to lowercase
    for(int i = 0; i < strlen(token); i++)
        token[i] = (char)tolower(token[i]);

    tokens[0] = (char *)malloc(sizeof(char) * strlen(token) + 1);
    strcpy(tokens[0], token);
    tokens[0][strlen(token)] = '\0';

    int params = 0; // we want to know how many params we have for easier processing

    //tokenize instr and store in tokens
    for(int walk = 1; (token = strtok(NULL, ",\n ")); walk++){
        tokens[walk] = (char *)malloc(sizeof(char) * strlen(token) + 1);
        strcpy(tokens[walk], token);
        params++;
    }

    //decode opcode first
    if(strcmp(tokens[0], "lw") == 0){
        strcpy(binInstr, LW);
        strcpy(binInstr + OPCODE_SIZE, genLWSWbinInstr(tokens));
    }else if(strcmp(tokens[0], "sw") == 0){
        strcpy(binInstr, SW);
        strcpy(binInstr + OPCODE_SIZE, genLWSWbinInstr(tokens));
    }else if(strcmp(tokens[0], "ld") == 0){
        strcpy(binInstr, LD);
        strcpy(binInstr + OPCODE_SIZE, genLDSTbinInstr(tokens));
    }else if(strcmp(tokens[0], "sub") == 0){
        strcpy(binInstr, SUB);
        strcpy(binInstr + OPCODE_SIZE, genRTypeInstr(tokens));
    }else if(strcmp(tokens[0], "add") == 0) {
        strcpy(binInstr, ADD);
        strcpy(binInstr + OPCODE_SIZE, genRTypeInstr(tokens));
    }

    binInstr[WORD_SIZE] = '\0';
    return binInstr;
}

char *genRTypeInstr(char **tokens){
    char *binInstr  = (char *)malloc((WORD_SIZE - OPCODE_SIZE) * sizeof(char) + 1);
    char *rd, *rs, *rt;

    rd = decimalToBinary(atoi(tokens[1] + 1), 10);
    rs = decimalToBinary(atoi(tokens[2] + 1), 8);
    rt = decimalToBinary(atoi(tokens[3] + 1), 8);

    strcpy(binInstr, rd);
    strcat(binInstr, rs);
    strcat(binInstr, rt);
    binInstr[WORD_SIZE - OPCODE_SIZE] = '\0';
    return binInstr;
}

char *genLDSTbinInstr(char **tokens){
    char *binInstr = (char *)malloc((WORD_SIZE - OPCODE_SIZE + 1) * sizeof(char));
    char *indexReg = NULL, *baseReg = NULL, *distance = NULL, *scale = NULL, *dest = NULL;
    // $reg, distance(base, index, scale)

    dest = decimalToBinary(atoi(tokens[1] + 1), RS_SIZE);
    baseReg = decimalToBinary(atoi(strchr(tokens[2], '$') + 1), 5);
    *strchr(tokens[2], '(') = '\0';

    distance = decimalToBinary(atoi(tokens[2]), 7);
    indexReg = decimalToBinary(atoi(tokens[3] + 1), 5);
    *strchr(tokens[4], ')') = '\0';
    scale = decimalToBinary(atoi(tokens[4]), 4);

    //decode instr here
    strcpy(binInstr, dest);
    strcat(binInstr, distance);
    strcat(binInstr, baseReg);
    strcat(binInstr, indexReg);
    strcat(binInstr, scale);

    binInstr[WORD_SIZE] = '\0';
    return binInstr;
}

char *genLWSWbinInstr(char **tokens) {
    char *rt = NULL, *immVal = NULL, *rs = NULL;
    char *binInstr = (char *)malloc(sizeof(char) * WORD_SIZE);

    rt = decimalToBinary(atoi(strchr(tokens[1], '$') + 1), RT_SIZE);

    char *leftParens = strchr(tokens[2], '(');
    *leftParens = '\0';
    immVal = decimalToBinary(atoi(tokens[2]), IMM_SIZE);

    char *rightParens = strchr(leftParens + 1, ')');
    *rightParens = '\0';
    leftParens += 2;
    rs = decimalToBinary(atoi(leftParens), RS_SIZE);

    //build the converted string here
    strcpy(binInstr, rs);
    strcat(binInstr, rt);
    strcat(binInstr, immVal);
    binInstr[WORD_SIZE] = '\0';

    return binInstr;
}

char *decimalToBinary(int toConvert, int numOfBits){
    char *binary = (char *)malloc(sizeof(char) * numOfBits + 1);

    binary[numOfBits] = '\0';
    for(int i = numOfBits - 1; i >= 0; i--){
        toConvert & 1 ? (binary[i] = '1') : (binary[i] = '0');
        toConvert >>= 1;
    }
    return binary;
}

int binaryToDecimal(char *binary, int size) {
    int num = 0;

    for(int i = size - 1; i >= 0; i--) {
        if(binary[i] == '1')
            num += pow(2, size - 1 - i);
    }
    return num;
}

//ADD = 0, SUB = 1, MUL = 2, DIV = 3, MOD = 4
//setFlags if 0 -> do not change the flags
char *ALU(int op, char *opLeft, char *opRight, int size, int setFlags) {
    char *left, *right, *result = NULL;

    left = signExtend(opLeft, size);
    right = signExtend(opRight, size);

    switch(op){
        case 0:
            result = addBinary(left, right, size, setFlags);
            break;
        case 1:
            result = subBinary(left, right, size, setFlags);
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
    }
    return result;
}

char *subBinary(char *opLeft, char *opRight, int size, int setFlags){
    char *right, *left;
    // Getting Complement of A Number(The opcode which is on right side)
    right = decimalToComplementBinary(binaryToDecimal(opRight, size), size);

    // Adding 1 to the complement of operand
    right = addBinary(decimalToBinary(1, size), right, size, setFlags);
    
    return addBinary(opLeft, right, size, setFlags);
}

char *addBinary (char *opLeft, char *opRight, int size, int setFlags){
    char carry = '0';

    char *sum = (char *)malloc(sizeof(char) * size + 1);
    if(sum == NULL)
        exit;

    for(int i = 0; i < size; i++){
        sum[i] = '0';
    }
    sum[size] = '\0';

    for(int pos = size - 1; pos >= 0; pos--){
        sum[pos] = (opLeft[pos] ^ opRight[pos] ^ carry);
        carry = (opLeft[pos] & opRight[pos]) | (opRight[pos] & carry) | (opLeft[pos] & carry);
    }

    //don't want to set flags for PC increment
    if(setFlags) {
        if (carry == '1')
            flags[OVERFLOW_FLAG] = '1';
        else flags[OVERFLOW_FLAG] = '0';

        for (int i = 0; i < WORD_SIZE; i++) {
            if (sum[i] == '1') {
                flags[ZERO_FLAG] = '0';
                break;
            }
            flags[ZERO_FLAG] = '1';
        }
    }
    return sum;
}

// This function takes Integer and convert into binary. The binary format is complement.
char *decimalToComplementBinary(int toConvert, int numOfBits){
    char *binary = (char *)malloc(sizeof(char) * numOfBits + 1);
    binary[numOfBits] = '\0';
    for(int i = numOfBits - 1; i >= 0; i--){
           toConvert & 1 ? (binary[i] = '0') : (binary[i] = '1');
           toConvert >>= 1;
    }

    return binary;
}


char *signExtend(char *value, int size){

    if(strlen(value) == size)
        return value;

    char *extVal = (char *)malloc(sizeof(char) * size + 1);
    if(extVal == NULL)
        exit;

    for(int i = 0; i < size; i++)
        extVal[i] = '0';

    extVal[size] = '\0';

    int count = size - 1;
    for(int i = strlen(value) - 1; i >= 0; i--){
        extVal[count] = value[i];
        count--;
    }

    return extVal;
}

//inits the CPU- including inits the PC to initial value
EXEC_INFO initCPU() {
    EXEC_INFO info;

    strcpy(PC, BOOT_ADDR);
    info.heap_ptr = HEAP_SEGMENT;
    info.stack_ptr = STACK_SEGMENT;
    info.lines = 0;

    return info;
}

void runProgram(EXEC_INFO info){
    char instr[WORD_SIZE + 1];
    char rd[11];
    char rs[11];
    char rt[11];
    char imm[IMM_SIZE + 1];

    imm[IMM_SIZE] = '\0';
    rd[RS_SIZE] = '\0';
    rs[RS_SIZE] = '\0';
    rt[RT_SIZE] = '\0';

    strcpy(PC, decimalToBinary(TEXT_SEGMENT, PC_SIZE));
    int memLoc;

    for(int i = 0; i < info.lines; i++){
	//fetch instr from mem
        memLoc = binaryToDecimal(PC, PC_SIZE);
        strcpy(instr, memory[memLoc]);

        //store in memData opcode:6 | rs 5 $2 | rt 5 $1 | im16 = 622 (lw $t, 622($rs)) -->  $t = MEM[$s + offset]
        if(strncmp(LW, instr, OPCODE_SIZE) == 0){
            strncpy(rs, instr + OPCODE_SIZE, RS_SIZE);
            strncpy(rt, instr + OPCODE_SIZE + RS_SIZE, RT_SIZE);
            strncpy(imm, instr + OPCODE_SIZE + RS_SIZE + RT_SIZE, IMM_SIZE);

            memLoc = binaryToDecimal(regFile[binaryToDecimal(rs, RS_SIZE)], RS_SIZE) + binaryToDecimal(imm, IMM_SIZE);
            strcpy(memAddr, decimalToBinary(memLoc, WORD_SIZE));
            strcpy(memData, memory[binaryToDecimal(memAddr, WORD_SIZE)]);

            strcpy(regFile[binaryToDecimal(rt, RT_SIZE)], memData);
            printf("LW storing data %d from Memory Address %d to Register $%d\n", binaryToDecimal(memData, WORD_SIZE), binaryToDecimal(memAddr, WORD_SIZE), binaryToDecimal(rt, RT_SIZE));
            printf("Data stored in Register $%d is %d\n",  binaryToDecimal(rt, RT_SIZE), binaryToDecimal(regFile[binaryToDecimal(rt, RT_SIZE)], WORD_SIZE));
        }else if(strncmp(SW, instr, OPCODE_SIZE) == 0){ //MEM[$s + offset] = $t = sw $t, offset($s)

            strncpy(rs, instr + OPCODE_SIZE, RS_SIZE);
            strncpy(rt, instr + OPCODE_SIZE + RS_SIZE, RT_SIZE);
            strncpy(imm, instr + OPCODE_SIZE + RS_SIZE + RT_SIZE, IMM_SIZE);

            memLoc = binaryToDecimal(regFile[binaryToDecimal(rs, RS_SIZE)], RS_SIZE) + binaryToDecimal(imm, IMM_SIZE);
            strcpy(memAddr, decimalToBinary(memLoc, WORD_SIZE));         //store memory addr to store data in
            strcpy(memData, regFile[binaryToDecimal(rt, RT_SIZE)] );      //grab data to transfer and store in memData
            strcpy(memory[binaryToDecimal(memAddr, WORD_SIZE)], memData);//store in memory location
            printf("SW storing data %d from Register $%d to Memory Address %d\n", binaryToDecimal(memData, WORD_SIZE), binaryToDecimal(rt, RT_SIZE), binaryToDecimal(memAddr, WORD_SIZE));
            printf("Data stored in Memory Address %d is %d\n",  binaryToDecimal(memAddr, WORD_SIZE), binaryToDecimal(memory[binaryToDecimal(memAddr, WORD_SIZE)], WORD_SIZE));

        }else if(strncmp(LD, instr, OPCODE_SIZE) == 0) {
            printf("here");
        }else if(strncmp(SUB, instr, OPCODE_SIZE) == 0) { //MEM[$s + offset] = $t = sw $t, offset($s)
            char *result;

            strncpy(rd, instr + OPCODE_SIZE, 10);
            strncpy(rs, instr + OPCODE_SIZE + 10, 8);
            strncpy(rt, instr + OPCODE_SIZE + 10 + 8, 8);

            result = ALU(1, regFile[binaryToDecimal(rs, 8)], regFile[binaryToDecimal(rt, 8)], WORD_SIZE, 1);
            strcpy(regFile[binaryToDecimal(rd, 10)], result);
        }else if((strncmp(ADD, instr, OPCODE_SIZE) == 0)) {
            char *result;
            strncpy(rd, instr + OPCODE_SIZE, 10);
            strncpy(rs, instr + OPCODE_SIZE + 10, 8);
            strncpy(rt, instr + OPCODE_SIZE + 10 + 8, 8);

            result = ALU(0, regFile[binaryToDecimal(rs, 8)], regFile[binaryToDecimal(rt, 8)], WORD_SIZE, 1);
            strcpy(regFile[binaryToDecimal(rd, 10)], result);
        }

        strcpy(PC, ALU(0, PC, "1", 16, 0)); //move to next instruction
        printExecutionData(i);
    }
}


void printExecutionData(int instrNum){
    char instrBuilder[250];
    char *instrFromMem = memory[TEXT_SEGMENT + instrNum];
    char rs[11], rt[11], rd[11], imm[IMM_SIZE + 1];

    rs[10] = '\0';
    rt[10] = '\0';
    rd[10] = '\0';
    imm[IMM_SIZE] = '\0';

    //opcode:6 | rs 5 $2 | rt 5 $1 | im16 = 622 (lw $t, 622($rs))
    if(strncmp(LW, instrFromMem, OPCODE_SIZE) == 0) {
        strncpy(rs, instrFromMem + OPCODE_SIZE, RS_SIZE);
        strncpy(rt, instrFromMem + OPCODE_SIZE + RS_SIZE, RT_SIZE);
        strncpy(imm, instrFromMem + OPCODE_SIZE + RS_SIZE + RT_SIZE, IMM_SIZE);
        sprintf(instrBuilder, "%s $%d, %d($%d)", "LW", binaryToDecimal(rt, RT_SIZE), binaryToDecimal(imm, IMM_SIZE), binaryToDecimal(rs, RS_SIZE));

    }else if (strncmp(SW, instrFromMem, OPCODE_SIZE) == 0){
        strncpy(rs, instrFromMem + OPCODE_SIZE, RS_SIZE);
        strncpy(rt, instrFromMem + OPCODE_SIZE + RS_SIZE, RT_SIZE);
        strncpy(imm, instrFromMem + OPCODE_SIZE + RS_SIZE + RT_SIZE, IMM_SIZE);
        sprintf(instrBuilder, "%s $%d, %d($%d)", "SW", binaryToDecimal(rt, RT_SIZE), binaryToDecimal(imm, IMM_SIZE), binaryToDecimal(rs, RS_SIZE));
    }else if (strncmp(LD, instrFromMem, OPCODE_SIZE) == 0) {
        char indexReg[RS_SIZE + 1], baseReg[RS_SIZE + 1], distance[8], scale[4], dest[RS_SIZE + 1];
        // $reg, distance(base, index, scale)
        indexReg[RS_SIZE] = '\0';
        baseReg[RS_SIZE] = '\0';
        distance[7] = '\0';
        scale[3] = '\0';
        dest[RS_SIZE] = '\0';

        strncpy(dest, instrFromMem + OPCODE_SIZE, RS_SIZE);
        strncpy(distance, instrFromMem + OPCODE_SIZE + RS_SIZE, 7);
        strncpy(baseReg, instrFromMem + OPCODE_SIZE + RS_SIZE + 7, RS_SIZE);
        strncpy(indexReg, instrFromMem + OPCODE_SIZE + RS_SIZE + 7 + RS_SIZE, RS_SIZE);
        strncpy(scale, instrFromMem + OPCODE_SIZE + RS_SIZE + 7 + RS_SIZE + RS_SIZE, 3);
        sprintf(instrBuilder, "%s $%d, %d($%d, $%d, %d)", "LD", binaryToDecimal(dest, RS_SIZE), binaryToDecimal(distance, 7), binaryToDecimal(baseReg, RS_SIZE),
        binaryToDecimal(indexReg, RS_SIZE), binaryToDecimal(scale, 4));
    }else if(strncmp(ADD, instrFromMem, OPCODE_SIZE) == 0) {
        strncpy(rd, instrFromMem + OPCODE_SIZE, 10);
        strncpy(rs, instrFromMem + OPCODE_SIZE + 10, 8);
        strncpy(rt, instrFromMem + OPCODE_SIZE + 10 + 8, 8);

        sprintf(instrBuilder, "%s $%d, $%d, $%d", "ADD", binaryToDecimal(rd, 10), binaryToDecimal(rs, 8), binaryToDecimal(rt, 8));

    }else if(strncmp(SUB, instrFromMem, OPCODE_SIZE) == 0) {
        strncpy(rd, instrFromMem + OPCODE_SIZE, 10);
        strncpy(rs, instrFromMem + OPCODE_SIZE + 10, 8);
        strncpy(rt, instrFromMem + OPCODE_SIZE + 10 + 8, 8);

        sprintf(instrBuilder, "%s $%d, $%d, $%d", "SUB", binaryToDecimal(rd, 10), binaryToDecimal(rs, 8), binaryToDecimal(rt, 8));
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
