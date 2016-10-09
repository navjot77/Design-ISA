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
        strcpy(memory[memLoc], convertInstrToBin(buff));
        memLoc++;
    }

    info->lines = memLoc - TEXT_SEGMENT;
}

char *convertInstrToBin(char *instr) {
    char *tokens[MAX_TOKENS];
    char *token = NULL;
    char *binInstr = (char *) malloc(sizeof(char) * WORD_SIZE);
    char *temp = malloc(strlen(instr) + 1);

    mallocErrorCheck(binInstr);

    strcpy(temp, instr);

    //init tokens array so we know how many args for instr
    for (int i = 0; i < MAX_TOKENS; i++)
        tokens[i] = NULL;

    //get opcode
    token = strtok(temp, " ");
    if(token == NULL){
        printf("Error. Expected token\n");
    }
    //change instruction name to lowercase
    for (int i = 0; i < strlen(token); i++)
        token[i] = (char) tolower(token[i]);

    tokens[0] = (char *) malloc(sizeof(char) * strlen(token) + 1);
    mallocErrorCheck(tokens[0]);

    strcpy(tokens[0], token);
    tokens[0][strlen(token)] = '\0';

    //tokenize instr and store in tokens
    for (int walk = 1; (token = strtok(NULL, ",\n ")) != NULL; walk++) {
        tokens[walk] = (char *) malloc(sizeof(char) * strlen(token) + 1);
        mallocErrorCheck(tokens[walk]);
        strcpy(tokens[walk], token);
    }

    if (strcmp(tokens[0], "lw") == 0) {
        strcpy(binInstr, LW);
        strcpy(binInstr + OPCODE_SIZE, genLWSWbinInstr(tokens));
    } else if (strcmp(tokens[0], "sw") == 0) {
        strcpy(binInstr, SW);
        strcpy(binInstr + OPCODE_SIZE, genLWSWbinInstr(tokens));
    } else if (strcmp(tokens[0], "ld") == 0) {
        strcpy(binInstr, LD);
        strcpy(binInstr + OPCODE_SIZE, genLDSTbinInstr(tokens));
    }else if (strcmp(tokens[0], "st") == 0){
        strcpy(binInstr, ST);
        strcpy(binInstr + OPCODE_SIZE, genLDSTbinInstr(tokens));
    }else if(strcmp(tokens[0], "sub") == 0){
        strcpy(binInstr, SUB);
        strcpy(binInstr + OPCODE_SIZE, genRTypeInstr(tokens));
    }else if(strcmp(tokens[0], "add") == 0) {
        strcpy(binInstr, ADD);
        strcpy(binInstr + OPCODE_SIZE, genRTypeInstr(tokens));
    } else if(strcmp(tokens[0], "mul") == 0){
        strcpy(binInstr, MUL);
        strcpy(binInstr + OPCODE_SIZE, genRTypeInstr(tokens));
    }

    binInstr[WORD_SIZE] = '\0';

    return binInstr;
}

char *genRTypeInstr(char **tokens){
    char *binInstr  = (char *)malloc((WORD_SIZE - OPCODE_SIZE) * sizeof(char) + 1);
    char *rd = NULL, *rs = NULL, *rt = NULL;

    mallocErrorCheck(binInstr);

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

    mallocErrorCheck(binInstr);

    dest = decimalToBinary(atoi(tokens[1] + 1), REG_ADDR_SIZE);
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
    char *binInstr = (char *)malloc(sizeof(char) * (WORD_SIZE - OPCODE_SIZE));

    mallocErrorCheck(binInstr);

    rt = decimalToBinary(atoi(strchr(tokens[1], '$') + 1), REG_ADDR_SIZE);

    char *leftParens = strchr(tokens[2], '(');
    *leftParens = '\0';
    immVal = decimalToBinary(atoi(tokens[2]), IMM_SIZE);

    char *rightParens = strchr(leftParens + 1, ')');
    *rightParens = '\0';
    leftParens += 2;
    rs = decimalToBinary(atoi(leftParens), REG_ADDR_SIZE);

    //build the converted string here
    strcpy(binInstr, rs);
    strcat(binInstr, rt);
    strcat(binInstr, immVal);
    binInstr[WORD_SIZE] = '\0';

    return binInstr;
}

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

int binaryToDecimal(char *binary, int size) {
    int num = 0;

    for(int i = size - 1; i >= 0; i--) {
        if(binary[i] == '1')
            num += pow(2, size - 1 - i);
    }
    return num;
}

//ADD = 0, SUB = 1, MUL = 4, DIV = 2, MOD = 3
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
            result = modBinary(left, right, size, setFlags);
            break;
        case 4:
	        result = mulBinary(left, right, size, setFlags);
            break;
    }
    return result;
}

char *subBinary(char *opLeft, char *opRight, int size, int setFlags){
    char *right = NULL;
    // Getting Complement of A Number(The opcode which is on right side)
    right = decimalToComplementBinary(binaryToDecimal(opRight, size), size);

    // Adding 1 to the complement of operand
    right = addBinary(decimalToBinary(1, size), right, size, setFlags);
    
    return addBinary(opLeft, right, size, setFlags);
}

char *addBinary (char *opLeft, char *opRight, int size, int setFlags){
    char carry = '0';
    char *sum = (char *)malloc(sizeof(char) * size + 1);
    mallocErrorCheck(sum);

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

        if(sum[0] == '1')
            flags[SIGN_FLAG] = '1';
        else flags[SIGN_FLAG] = '0';
    }
    return sum;
}

// This function takes Integer and convert into binary. The binary format is complement.
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


char *signExtend(char *value, int size){

    if(strlen(value) == size)
        return value;

    char *extVal = (char *)malloc(sizeof(char) * size + 1);
    mallocErrorCheck(extVal);

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
    char rd[RTYPE_RD_SIZE + 1];
    char rs[RTYPE_ADDR_SIZE + 1];
    char rt[RTYPE_ADDR_SIZE + 1];
    char imm[IMM_SIZE + 1];

    imm[IMM_SIZE] = '\0';
    rd[REG_ADDR_SIZE] = '\0';
    rs[REG_ADDR_SIZE] = '\0';
    rt[REG_ADDR_SIZE] = '\0';

    strcpy(PC, decimalToBinary(TEXT_SEGMENT, PC_SIZE));
    int memLoc = 0, rdOffset = 0, rsOffset = 0, rtOffset = 0, immOffset = 0;

    for(int i = 0; i < info.lines; i++){
	//fetch instr from mem
        memLoc = binaryToDecimal(PC, PC_SIZE);
        strcpy(instr, memory[memLoc]);

        //store in memData opcode:6 | rs 5 $2 | rt 5 $1 | im16 = 622 (lw $t, 622($rs)) -->  $t = MEM[$s + offset]
        if(strncmp(LW, instr, OPCODE_SIZE) == 0){
            rsOffset = OPCODE_SIZE;
            rtOffset = OPCODE_SIZE + REG_ADDR_SIZE;
            immOffset = rtOffset + REG_ADDR_SIZE;

            strncpy(rs, instr + rsOffset, REG_ADDR_SIZE);
            strncpy(rt, instr + rtOffset, REG_ADDR_SIZE);
            strncpy(imm, instr + immOffset, IMM_SIZE);

            memLoc = binaryToDecimal(regFile[binaryToDecimal(rs, REG_ADDR_SIZE)], REG_ADDR_SIZE) + binaryToDecimal(imm, IMM_SIZE);
            strcpy(memAddr, decimalToBinary(memLoc, WORD_SIZE));
            strcpy(memData, memory[binaryToDecimal(memAddr, WORD_SIZE)]);

            strcpy(regFile[binaryToDecimal(rt, REG_ADDR_SIZE)], memData);
            printf("LW storing data %d from Memory Address %d to Register $%d\n", binaryToDecimal(memData, WORD_SIZE), binaryToDecimal(memAddr, WORD_SIZE), binaryToDecimal(rt, REG_ADDR_SIZE));
            printf("Data stored in Register $%d is %d\n",  binaryToDecimal(rt, REG_ADDR_SIZE), binaryToDecimal(regFile[binaryToDecimal(rt, REG_ADDR_SIZE)], WORD_SIZE));

        }else if(strncmp(SW, instr, OPCODE_SIZE) == 0){ //MEM[$s + offset] = $t = sw $t, offset($s)

            rsOffset = OPCODE_SIZE;
            rtOffset = OPCODE_SIZE + REG_ADDR_SIZE;
            immOffset = rtOffset + REG_ADDR_SIZE;

            strncpy(rs, instr + rsOffset, REG_ADDR_SIZE);
            strncpy(rt, instr + rtOffset, REG_ADDR_SIZE);
            strncpy(imm, instr + immOffset, IMM_SIZE);

            memLoc = binaryToDecimal(regFile[binaryToDecimal(rs, REG_ADDR_SIZE)], REG_ADDR_SIZE) + binaryToDecimal(imm, IMM_SIZE);
            strcpy(memAddr, decimalToBinary(memLoc, WORD_SIZE));         //store memory addr to store data in
            strcpy(memData, regFile[binaryToDecimal(rt, REG_ADDR_SIZE)] );      //grab data to transfer and store in memData
            strcpy(memory[binaryToDecimal(memAddr, WORD_SIZE)], memData);//store in memory location
            printf("SW storing data %d from Register $%d to Memory Address %d\n", binaryToDecimal(memData, WORD_SIZE), binaryToDecimal(rt, REG_ADDR_SIZE), binaryToDecimal(memAddr, WORD_SIZE));
            printf("Data stored in Memory Address %d is %d\n",  binaryToDecimal(memAddr, WORD_SIZE), binaryToDecimal(memory[binaryToDecimal(memAddr, WORD_SIZE)], WORD_SIZE));

        }else if(strncmp(LD, instr, OPCODE_SIZE) == 0) {
            // $reg, distance(base, index, scale) 5 7 5 5 4
            int rdOffset = OPCODE_SIZE;
            int distanceOffset = OPCODE_SIZE + REG_ADDR_SIZE;
            int baseOffset = distanceOffset + DIST_SIZE;
            int indexOffset = baseOffset + REG_ADDR_SIZE;
            int scaleOffset = indexOffset + REG_ADDR_SIZE;

            char dist[8];
            char base[REG_ADDR_SIZE + 1];
            char index[REG_ADDR_SIZE + 1];
            char scale[5];

            dist[DIST_SIZE] = '\0';
            base[REG_ADDR_SIZE] = '\0';
            index[REG_ADDR_SIZE] = '\0';
            scale[SCALE_SIZE] = '\0';
            rd[REG_ADDR_SIZE] = '\0';

            strncpy(rd, instr + rdOffset, REG_ADDR_SIZE);
            strncpy(dist, instr + distanceOffset, DIST_SIZE);
            strncpy(base, instr + baseOffset, REG_ADDR_SIZE);
            strncpy(index, instr + indexOffset, REG_ADDR_SIZE);
            strncpy(scale, instr + scaleOffset, SCALE_SIZE);

            //calculate address
            int distVal = binaryToDecimal(dist, DIST_SIZE);
            int baseVal = binaryToDecimal(regFile[binaryToDecimal(base, REG_ADDR_SIZE)], 32);
            int indexVal = binaryToDecimal(regFile[binaryToDecimal(index, REG_ADDR_SIZE)], 32);
            int scaleVal = binaryToDecimal(scale, SCALE_SIZE);

            memLoc = distVal + baseVal + (indexVal * scaleVal);
            //update memAddr and memData SPR
            strcpy(memAddr, decimalToBinary(memLoc, WORD_SIZE));
            strcpy(memData, memory[memLoc]);

            //store in destination reg
            strcpy(regFile[binaryToDecimal(rd, REG_ADDR_SIZE)], memData);

        }else if (strncmp(ST, instr, OPCODE_SIZE) == 0) {
            // $reg, distance(base, index, scale) 5 7 5 5 4
            int rdOffset = OPCODE_SIZE;
            int distanceOffset = OPCODE_SIZE + REG_ADDR_SIZE;
            int baseOffset = distanceOffset + 7;
            int indexOffset = baseOffset + REG_ADDR_SIZE;
            int scaleOffset = indexOffset + REG_ADDR_SIZE;

            char dist[8];
            char base[REG_ADDR_SIZE + 1];
            char index[REG_ADDR_SIZE + 1];
            char scale[5];

            dist[7] = '\0';
            base[REG_ADDR_SIZE] = '\0';
            index[REG_ADDR_SIZE] = '\0';
            scale[4] = '\0';
            rd[REG_ADDR_SIZE] = '\0';

            strncpy(rd, instr + rdOffset, REG_ADDR_SIZE);
            strncpy(dist, instr + distanceOffset, DIST_SIZE);
            strncpy(base, instr + baseOffset, REG_ADDR_SIZE);
            strncpy(index, instr + indexOffset, REG_ADDR_SIZE);
            strncpy(scale, instr + scaleOffset, SCALE_SIZE);

            //calculate address
            int distVal = binaryToDecimal(dist, 7);
            int baseVal = binaryToDecimal(regFile[binaryToDecimal(base, REG_ADDR_SIZE)], 32);
            int indexVal = binaryToDecimal(regFile[binaryToDecimal(index, REG_ADDR_SIZE)], 32);
            int scaleVal = binaryToDecimal(scale, 4);

            memLoc = distVal + baseVal + (indexVal * scaleVal);
            strcpy(memAddr, decimalToBinary(memLoc, WORD_SIZE));
            strcpy(memData, regFile[binaryToDecimal(rd, REG_ADDR_SIZE)]);
            strcpy(memory[memLoc], memData);
        }else if(strncmp(SUB, instr, OPCODE_SIZE) == 0) { //MEM[$s + offset] = $t = sw $t, offset($s)
            char *result;

            rdOffset = OPCODE_SIZE;
            rsOffset = rdOffset + RTYPE_RD_SIZE;
            rtOffset = rsOffset + 8;

            strncpy(rd, instr + rdOffset, RTYPE_RD_SIZE);
            strncpy(rs, instr + rsOffset, 8);
            strncpy(rt, instr + rtOffset, 8);

            result = ALU(1, regFile[binaryToDecimal(rs, 8)], regFile[binaryToDecimal(rt, 8)], WORD_SIZE, 1);
            strcpy(regFile[binaryToDecimal(rd, RTYPE_RD_SIZE)], result);

        }else if((strncmp(ADD, instr, OPCODE_SIZE) == 0)) {
            char *result;

            rdOffset = OPCODE_SIZE;
            rsOffset = rdOffset + RTYPE_RD_SIZE;
            rtOffset = rsOffset + 8;

            strncpy(rd, instr + rdOffset, RTYPE_RD_SIZE);
            strncpy(rs, instr + rsOffset, 8);
            strncpy(rt, instr + rtOffset, 8);

            result = ALU(0, regFile[binaryToDecimal(rs, 8)], regFile[binaryToDecimal(rt, 8)], WORD_SIZE, 1);
            strcpy(regFile[binaryToDecimal(rd, 10)], result);
        }else if(strncmp(MUL, instr, OPCODE_SIZE) == 0) { //MEM[$s + offset] = $t = sw $t, offset($s)
            char *result;

            strncpy(rd, instr + OPCODE_SIZE, 10);
            strncpy(rs, instr + OPCODE_SIZE + 10, 8);
            strncpy(rt, instr + OPCODE_SIZE + 10 + 8, 8);

            result = ALU(4, regFile[binaryToDecimal(rs, 8)], regFile[binaryToDecimal(rt, 8)], WORD_SIZE, 1);
            strcpy(regFile[binaryToDecimal(rd, 10)], result);
        }else if(strncmp(MOD, instr, OPCODE_SIZE) == 0){
            char *result;
            strncpy(rd, instr + OPCODE_SIZE, 10);
            strncpy(rs, instr + OPCODE_SIZE + 10, 8);
            strncpy(rt, instr + OPCODE_SIZE + 10 + 8, 8);

            result = ALU(3, regFile[binaryToDecimal(rs, 8)], regFile[binaryToDecimal(rt, 8)], WORD_SIZE, 1);
            strcpy(regFile[binaryToDecimal(rd, 10)], result);

        }

        strcpy(PC, ALU(0, PC, "1", 16, 0)); //move to next instruction
        printExecutionData(i);
    }
}

void printExecutionData(int instrNum){
    char instrBuilder[250];
    char *instrFromMem = memory[TEXT_SEGMENT + instrNum];
    char rs[RTYPE_ADDR_SIZE + 1], rt[RTYPE_ADDR_SIZE + 1], rd[RTYPE_RD_SIZE + 1], imm[IMM_SIZE + 1];

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
        strcpy(instrBuilder, buildInstrForRTypePrint(instrFromMem, "ADD"));

    }else if(strncmp(SUB, instrFromMem, OPCODE_SIZE) == 0) {
        strcpy(instrBuilder, buildInstrForRTypePrint(instrFromMem, "SUB"));

    } else if(strncmp(MUL, instrFromMem, OPCODE_SIZE) == 0) {
        strcpy(instrBuilder, buildInstrForRTypePrint(instrFromMem, "MUL"));

    }else if(strncmp(MOD, instrFromMem, OPCODE_SIZE) == 0) {
        char *result;
        strncpy(rd, instrFromMem + OPCODE_SIZE, 10);
        strncpy(rs, instrFromMem + OPCODE_SIZE + 10, 8);
        strncpy(rt, instrFromMem + OPCODE_SIZE + 10 + 8, 8);

        sprintf(instrBuilder, "%s $%d, $%d, $%d", "MUL", binaryToDecimal(rd, 10), binaryToDecimal(rs, 8), binaryToDecimal(rt, 8));

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


char* mulBinary(char* left, char* right, int size, int setFlags)
{
    char *mul = (char *)malloc(sizeof(char) * size + 1);
    mul[size] = '\0';
    int op1 = binaryToDecimal(left, size);
    int op2 = binaryToDecimal(right, size);

    int result = 0;

    while(op2 != 0) {
        if(op2 & 01) {
            result = binaryToDecimal(addBinary(decimalToBinary(result, WORD_SIZE), decimalToBinary(op1, WORD_SIZE), WORD_SIZE, 0), WORD_SIZE);
        }
        op1 = op1 << 1;
        op2 = op2 >> 1;
    }
    strcpy(mul, decimalToBinary(result, WORD_SIZE));
    return mul;
}

char* leftShift(char* input, int size)
{
    char *result=NULL;
    int num= binaryToDecimal(input, size);
    num = num << 1;
    result= decimalToBinary(num, size);
    return result;
}

char* rightShift(char* input, int size)
{
    char *result=NULL;
    int num= binaryToDecimal(input, size);
    num = num >> 1;
    result= decimalToBinary(num, size);
    return result;
    
}

char* modBinary(char* left, char* right, int size, int setFlags)
{
    char qt[WORD_SIZE + 1];  //quotient and result
    qt[WORD_SIZE] = '\0';

    char *result = malloc(WORD_SIZE + 1);
    mallocErrorCheck(result);
    result[WORD_SIZE] = '\0';

    strcpy(qt, divBinary(left, right, size, setFlags)); //to get quotient when  -> left/right
    result= mulBinary(qt, right, size, setFlags);   //multiply quotient with right  -> (left/right)*right
    result= subBinary(left, result, size, setFlags);    //subtract the result we got from left  -> left-((left/right)*right)
    return result;
}

void mallocErrorCheck(char *ptr){
    if(ptr == NULL){
        printf("Error on malloc. Exiting\n");
        exit(1);
    }
}

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
    char *rebuiltInstr = malloc(strlen(instrBuilder + 1));
    mallocErrorCheck(rebuiltInstr);
    strcpy(rebuiltInstr, instrBuilder);

    return rebuiltInstr;
}
