//
// Created by dearm on 9/17/2016.
//
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "cpu_utils.h"
#include "opcodes.h"

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
        strcpy(memory[memLoc], convertInstrToBin(buff));
        memLoc++;

    }

    info->lines = memLoc - TEXT_SEGMENT;
}

char *convertInstrToBin(char *instr) {
    char *tokens[5];
    char *token;
    bool hasOffset = false;
    char *binInstr = (char *)malloc(sizeof(char) * WORD_SIZE);
    unsigned short instructionType = 0; //R type = 0, I type = 1, J type = 2

    char *temp = malloc(strlen(instr) + 1);
    strcpy(temp, instr);

    //init tokens array so we know how many args for instr
    for(int i = 0; i < 5; i++)
        tokens[i] = NULL;

    //get opcode
    token = strtok(temp, " ");

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
        strcpy(binInstr + OPCODE_SIZE, genLWSWbinInstr(tokens, params));
    }else if(strcmp(tokens[0], "sw") == 0){
        strcpy(binInstr, SW);
        strcpy(binInstr + OPCODE_SIZE, genLWSWbinInstr(tokens, params));
    }

    binInstr[WORD_SIZE] = '\0';
    printf("%s\n", binInstr);
    return binInstr;
}

char *genLWSWbinInstr(char **tokens, int params) {
    char *rt = NULL, *immVal = NULL, *rs = NULL;
    char *binInstr = (char *)malloc(sizeof(char) * WORD_SIZE);

    rt= convertToBin(atoi(strchr(tokens[1], '$') + 1), false);

    char *leftParens = strchr(tokens[params], '(');
    *leftParens = '\0';
    immVal = convertToBin(atoi(tokens[params]), true);

    char *rightParens = strchr(leftParens + 1, ')');
    *rightParens = '\0';
    leftParens += 2;
    rs = convertToBin(atoi(leftParens), false);

    //build the converted string here
    strcpy(binInstr, rs);
    strcat(binInstr, rt);
    strcat(binInstr, immVal);
    binInstr[WORD_SIZE] = '\0';
    return binInstr;
}

// converts number to binary rep. if not imm then it is an address so only 5 bits needed
char *convertToBin(int toConvert, bool isImmVal) {

    if(!isImmVal){ //not imm so it is an addr
        return decimalToBinary(toConvert, 5);
    }else{
        return decimalToBinary(toConvert, 16);
    }
    return NULL;
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

//ADD = 0, MUL = 1, DIV = 2
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
            break;
        case 2:
            break;
    }
    return result;
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
        if (carry)
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
    char rs[RS_SIZE + 1];
    char rt[RT_SIZE + 1];
    char imm[IMM_SIZE + 1];

    imm[IMM_SIZE] = '\0';
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

        }
        strcpy(PC, ALU(0, PC, "1", 16, 0)); //move to next instruction
        printExecutionData(i);
    }
}


void printExecutionData(int instrNum){
    char instrBuilder[250];
    char *instrFromMem = memory[TEXT_SEGMENT + instrNum];
    char rs[RS_SIZE + 1], rt[RT_SIZE + 1], imm[IMM_SIZE + 1];
    rs[RS_SIZE] = '\0';
    rt[RT_SIZE] = '\0';
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
