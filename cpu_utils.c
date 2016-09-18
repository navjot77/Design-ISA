//
// Created by dearm on 9/17/2016.
//
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <mem.h>
#include <math.h>
#include "cpu_utils.h"
#include "opcodes.h"
#include "exec_utils.h"

void loadAndStoreInstrs(char *fileName, char *memory[], EXEC_INFO *info){
    FILE *fp = NULL;
    char buff[250];
    int memLoc = BOOT_SECTOR;

    fp = fopen(fileName, "r");
    if(fp == NULL) {
        printf("Error opening file %s", fileName);
        exit;
    }

    while(fgets(buff, 250, fp)) {
        strcpy(memory[memLoc], convertInstrToBin(buff));
        printf("mem is %s\n", memory[memLoc]);
        memLoc++;

    }

    info->lines = memLoc - BOOT_SECTOR;
}

char *convertInstrToBin(char *instr) {
    char *tokens[5];
    char *token;
    bool hasOffset = false;
    char *binInstr = (char *)malloc(sizeof(char) * 32 + 1);
    unsigned short instructionType = 0; //R type = 0, I type = 1, J type = 2

    char *temp = malloc(strlen(instr) + 1);
    strcpy(temp, instr);

    char *rs = NULL, *rt = NULL, *rd = NULL, *immVal = NULL;

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
        instructionType = 1;
        hasOffset = true;
    }else if(strcmp(tokens[0], "sw") == 0){
        strcpy(binInstr, SW);
        instructionType = 1;
        hasOffset = true;
    }

    //convert the rest
    if(instructionType == 1){

        if(hasOffset) { //has an offset so must convert differently

            rt = convertToBin(atoi(strchr(tokens[1], '$') + 1), false);

            char *leftParens = strchr(tokens[params], '(');
            *leftParens = '\0';
            immVal = convertToBin(atoi(tokens[params]), true);

            char *rightParens = strchr(leftParens + 1, ')');
            *rightParens = '\0';
            leftParens += 2;
            rs = convertToBin(atoi(leftParens), false);
        }

    }else{
        //conversion for the rest here
    }

    //build the converted string here

    strcat(binInstr, rs);
    strcat(binInstr, rt);
    strcat(binInstr, immVal);
    binInstr[32] = '\0';
    printf("rt %s rs %s imm %s\n", rt, rs, immVal);
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
    for(int i = numOfBits - 2; i >= 0; i--){
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
char *ALU(int op, char *opLeft, char *opRight, char *flags, int size) {
    char *left, *right, *result = NULL;

    left = signExtend(opLeft, size);
    right = signExtend(opRight, size);

    switch(op){
        case 0:
            result = addBinary(left, right, flags, size);
            break;
        case 1:
            break;
        case 2:
            break;
    }
    return result;
}

char *addBinary (char *opLeft, char *opRight, char *flags, int size){
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
    if(flags) {
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
EXEC_INFO initCPU(char *PC) {
    EXEC_INFO info;

    PC = BOOT_ADDR;
    info.heap_ptr = HEAP_SEGMENT;
    info.stack_ptr = STACK_SEGMENT - 1;
    info.lines = 0;

    return info;
}

/*The CPU sends PC to the MAR and sends a READ command on the control bus
In response to the read command (with address equal to PC), the memory returns the data stored at the memory location indicated by PC on the databus
        The CPU copies the data from the databus into its MDR (also known as MBR, see section Components above)
A fraction of a second later, the CPU copies the data from the MDR to the Instruction Register (IR)
The PC is incremented so that it points to the following instruction in memory. This step prepares the CPU for the next cycle.*/
//char *ALU(int op, char *opLeft, char *opRight, char *flags, int size) {
void runProgram(char **memory, char *PC, char *memAddr, char *memData, char **regFile, char *flags, EXEC_INFO info){
    char instr[WORD_SIZE + 1];
    strcpy(PC, decimalToBinary(BOOT_SECTOR, PC_SIZE + 1));
    printf("bin %s\n", decimalToBinary(BOOT_SECTOR, PC_SIZE + 1));
    printf("dec %d\n", binaryToDecimal(PC, PC_SIZE));
    int memLoc;

    for(int i = 0; i < info.lines; i++){
        //fetch instr from mem
        memLoc = binaryToDecimal(PC, PC_SIZE);
        printf("in mem %s\n", memory[memLoc]);
        strcpy(instr, memory[memLoc]);

        //store in memData

        //ir (?)

        //incr pc
        strcpy(PC, ALU(0, PC, "1", NULL, 16));

        //exec instr

    }

}