//
// Created by dearm on 9/17/2016.
//
#include <stdlib.h>
#include <ctype.h>
#include "cpu_utils.h"

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

unsigned char *convertInstrToBin(unsigned char *instr) {
    char *tokens[4];
    char *token;
    unsigned char binInstr[32];
    unsigned short instructionType; //R type = 0, I type = 1, J type = 2

    //init tokens array so we know how many args for instr
    for(int i = 0; i < 4; i++)
        tokens[i] = NULL;

    //get opcode
    token = strtok(instr, " ");
    for(int i = 0; i < strlen(token); i++)
        token[i] = (char)tolower(token[i]);
    tokens[0] = (char *)malloc(sizeof(char) * strlen(token) + 1);
    strcpy(tokens[0], token);

    int params = 0; // we want to know how many params we have for easier processing
    //tokenize instr and store in tokens
    for(int walk = 1; token = strtok(NULL, ",\n"); walk++){
        tokens[walk] = (char *)malloc(sizeof(char) * strlen(token) + 1);
        strcpy(tokens[walk], token);
        params++;
    }

    //decode opcode first
    if(strcmp(tokens[0], "lw") == 0){
        strcpy(binInstr, LW);
        instructionType = 1;
    }else if(strcmp(tokens[0], "sw") == 0){
        strcpy(binInstr, SW);
        instructionType = 1;
    }

    //convert the rest
    if(instructionType == 1){
        for(int i = 0; i < params - 1; i++){
            strcat(binInstr, convertToBin(atoi((const char *) tokens[i + 1][1]), false));
        }

        //convert immediate value
    }else{

    }
    return binInstr;
}

// converts number to binary rep. if not imm then it is an address so only 5 bits needed
unsigned char *convertToBin(int toConvert, bool isImmVal) {
    unsigned char converted[16]; //since largest size we will have is 16b imm

    if(!isImmVal){ //not imm so it is an addr
        for(int i = 4; i >= 0; i--){
            if(toConvert & 1)
                converted[i] = (unsigned char)"1";
            else
                converted[i] = (unsigned char)"0";
        }
    }else{


    }
}

//inits the CPU- including inits the PC to initial value
void initCPU(unsigned char *PC) {
    PC = BOOT_ADDR;
}

void runProgram(){

}