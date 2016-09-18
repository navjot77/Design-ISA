//
// Created by dearm on 9/17/2016.
//
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <mem.h>
#include "cpu_utils.h"
#include "opcodes.h"

void loadAndStoreInstrs(char *fileName, unsigned char **memory){
    FILE *fp = NULL;
    char buff[250];

    //unsigned char instr[32];

    fp = fopen(fileName, "r");
    if(fp == NULL) {
        printf("Error opening file %s", fileName);
        exit;
    }

    while(fgets(buff, 250, fp)) {

    }
}

unsigned char *convertInstrToBin(char *instr) {
    char *tokens[5];
    char *token;
    bool hasOffset = false;
    unsigned char *binInstr = (char *)malloc(sizeof(char) * 32);
    unsigned short instructionType; //R type = 0, I type = 1, J type = 2

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

    printf("token is %s\n", tokens[0]);
    int params = 0; // we want to know how many params we have for easier processing

    //tokenize instr and store in tokens
    for(int walk = 1; token = strtok(NULL, ",\n "); walk++){
        tokens[walk] = (char *)malloc(sizeof(char) * strlen(token) + 1);
        strcpy(tokens[walk], token);
        params++;
    }

    //decode opcode first
    if(strcmp(tokens[0], "lw") == 0){
        strcpy(binInstr, LW);
        printf("opcode instr %s\n", binInstr);
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

            rt = convertToBin(strchr(tokens[1], '$') + 1, false);

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
    strcat(binInstr, rt);
    strcat(binInstr, rs);
    strcat(binInstr, immVal);

    return binInstr;
}

// converts number to binary rep. if not imm then it is an address so only 5 bits needed
unsigned char *convertToBin(int toConvert, bool isImmVal) {

    if(!isImmVal){ //not imm so it is an addr
        unsigned char *addr = (char *)malloc(sizeof(char) * 6);
        for(int i = 4; i >= 0; i--){
            toConvert & 1 ? (addr[i] = (unsigned char)'1') : (addr[i] = (unsigned char)'0');
            toConvert >>= 1;
        }
        addr[5] = '\0';
        return addr;
    }else{
        unsigned char *imm = (char *)malloc(sizeof(char) * 17);
        for(int i = 15; i >= 0; i--){
            toConvert & 1 ? (imm[i] = '1') : (imm[i] = '0');
            toConvert >>= 1;
        }
        imm[16] = '\0';
        return imm;
    }
    return NULL;
}

//inits the CPU- including inits the PC to initial value
void initCPU(unsigned char *PC) {
    PC = BOOT_ADDR;
}

void runProgram(){

}