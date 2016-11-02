//
// Created by dearm on 9/17/2016.
//
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include "cpu_utils.h"
#include "opcodes.h"

#define MAX_TOKENS 10

void preprocessFile(FILE *fp) {
    if(fp == NULL) exit(1); //error

    char buff[250];
    char *label, *labelName;
    int relativeAddr = 0;

    while(fgets(buff, 250, fp)){
        label = strchr(buff, ':');
        if(label){
            *label = '\0';
            labelName = malloc(strlen(buff) + 1);
            mallocErrorCheck(labelName);
            strcpy(labelName, buff);

            int i = 0;
            while(labels[i]. labelName != NULL)
                i++;

            labels[i].labelName = labelName;
            labels[i].lineNum = relativeAddr;
        }
        relativeAddr++;
    }
}

/**
 * Inits the CPU with bootup values
 * @return returns a struct containing information about start up addresses etc
 */
EXEC_INFO initCPU() {
    EXEC_INFO info;

    strcpy(PC, BOOT_ADDR);
    info.heap_ptr = HEAP_SEGMENT;
    info.stack_ptr = STACK_SEGMENT;
    info.lines = 0;

    return info;
}

/**
 * loads instructions from file, encodes each instruction and stores in text section of memory
 * @param fileName : file that contains our instructions
 * @param info : struct containing information for execution
 */
void loadAndStoreInstrs(char *fileName, EXEC_INFO *info){
    FILE *fp = NULL;
    char buff[250];
    int memLoc = TEXT_SEGMENT;

    fp = fopen(fileName, "r");
    if(fp == NULL) {
        printf("Error opening file %s", fileName);
        exit;
    }

    preprocessFile(fp);
    char *freeHandle;

    rewind(fp);
    while(fgets(buff, 250, fp)) {

        if(!strchr(buff, ':')) {
            strcpy(memory[memLoc], freeHandle = convertInstrToBin(buff, memLoc));
            free(freeHandle);
        }
        memLoc++;
    }

    free(fileName);
    info->lines = memLoc - TEXT_SEGMENT;

}

/**
 * Decodes all instructions in the text file
 * @param instr : instruction from file to encode
 * @return :
 */
char *convertInstrToBin(char *instr, int currMemLoc) {
    char *tokens[MAX_TOKENS];
    char *token = NULL, *freeHandle = NULL;
    char *binInstr = (char *) malloc(sizeof(char) * WORD_SIZE + 1);
    char *temp = malloc(strlen(instr) + 1);

    int params = 0;

    mallocErrorCheck(binInstr);

    strcpy(temp, instr);

    //init tokens array so we know how many args for instr
    for (int i = 0; i < MAX_TOKENS; i++)
        tokens[i] = NULL;

    //get opcode
    token = strtok(temp, " ");
    if (token == NULL) {
        printf("Error. Expected token\n");
    } else {
        //change instruction name to lowercase
        for (int i = 0; i < strlen(token); i++)
            token[i] = (char) tolower(token[i]);
    }

    tokens[0] = (char *) malloc(sizeof(char) * strlen(token) + 1);
    mallocErrorCheck(tokens[0]);

    strcpy(tokens[0], token);
    tokens[0][strlen(token)] = '\0';

    //tokenize instr and store in tokens
    for (int walk = 1; (token = strtok(NULL, ",\n ")) != NULL; walk++) {
        tokens[walk] = (char *) malloc(sizeof(char) * strlen(token) + 1);
        mallocErrorCheck(tokens[walk]);
        strcpy(tokens[walk], token);
        params++;
    }

//    printf("token %s\n", tokens[0]);

    if (strcmp(tokens[0], "lw") == 0) {
        strcpy(binInstr, LW);
        strcpy(binInstr + OPCODE_SIZE, freeHandle = genLWSWbinInstr(tokens));
    } else if (strcmp(tokens[0], "sw") == 0) {
        strcpy(binInstr, SW);
        strcpy(binInstr + OPCODE_SIZE, freeHandle = genLWSWbinInstr(tokens));
    } else if (strcmp(tokens[0], "ld") == 0) {
        strcpy(binInstr, LD);
        strcpy(binInstr + OPCODE_SIZE, freeHandle = genLDSTbinInstr(tokens));
    } else if (strcmp(tokens[0], "st") == 0) {
        strcpy(binInstr, ST);
        strcpy(binInstr + OPCODE_SIZE, freeHandle = genLDSTbinInstr(tokens));
    } else if (strcmp(tokens[0], "sub") == 0) {
        strcpy(binInstr, SUB);
        strcpy(binInstr + OPCODE_SIZE, freeHandle = genRTypeInstr(tokens));
    } else if (strcmp(tokens[0], "add") == 0) {
        strcpy(binInstr, ADD);
        strcpy(binInstr + OPCODE_SIZE, freeHandle = genRTypeInstr(tokens));
    } else if (strcmp(tokens[0], "mul") == 0) {
        strcpy(binInstr, MUL);
        strcpy(binInstr + OPCODE_SIZE, freeHandle = genRTypeInstr(tokens));
    } else if (strcmp(tokens[0], "div") == 0) {
        strcpy(binInstr, DIV);
        strcpy(binInstr + OPCODE_SIZE, freeHandle = genRTypeInstr(tokens));
    } else if (strcmp(tokens[0], "mod") == 0) {
        strcpy(binInstr, MOD);
        strcpy(binInstr + OPCODE_SIZE, freeHandle = genRTypeInstr(tokens));
    } else if (strcmp(tokens[0], "addi") == 0) {
        strcpy(binInstr, ADDI);
        strcpy(binInstr + OPCODE_SIZE, freeHandle = genITypeInstr(tokens));
    } else if (strcmp(tokens[0], "j") == 0) {
        strcpy(binInstr, J);
        strcpy(binInstr + OPCODE_SIZE, freeHandle = genJTypeInstr(tokens, currMemLoc));
        printf("%s\n", binInstr);
    } else if (strcmp(tokens[0], "blt") == 0) {
        strcpy(binInstr, BLT);
        strcpy(binInstr + OPCODE_SIZE, freeHandle = genBranchTypeInstr(tokens, currMemLoc));
    } else if (strcmp(tokens[0], "bne") == 0) {
        strcpy(binInstr, BNE);
        strcpy(binInstr + OPCODE_SIZE, freeHandle = genBranchTypeInstr(tokens, currMemLoc));
    } else if (strcmp(tokens[0], "beq") == 0) {
        strcpy(binInstr, BEQ);
        strcpy(binInstr + OPCODE_SIZE, freeHandle = genBranchTypeInstr(tokens, currMemLoc));
    }else if (strcmp(tokens[0], "mov") == 0) {
        strcpy(binInstr, MOV);
        strcpy(binInstr + OPCODE_SIZE, freeHandle = genMovTypeInstr(tokens));
    }else if (strcmp(tokens[0], "lea") == 0) {
        strcpy(binInstr, LEA);
        strcpy(binInstr + OPCODE_SIZE, freeHandle = genLEATypeInstr(tokens));
    }

    binInstr[WORD_SIZE] = '\0';


    for (int i = 0; i < params + 1; i++)
        free(tokens[i]);

    free(freeHandle);

    return binInstr;
}

// Opcode 6 bits, followed by Destination register 5 bits and Source Reg.
char *genMovTypeInstr(char **tokens){

    char *binInstr  = (char *)malloc((WORD_SIZE - OPCODE_SIZE) * sizeof(char) + 1);
    mallocErrorCheck(binInstr);

    char *dest = NULL, *src = NULL;

    dest = decimalToBinary(atoi(tokens[1] + 1), REG_ADDR_SIZE);
    src = decimalToBinary(atoi(tokens[2] + 1), REG_ADDR_SIZE);

    strcpy(binInstr, dest);
    strcat(binInstr, src);

    free(dest);
    free(src);

    binInstr[WORD_SIZE - OPCODE_SIZE] = '\0';
    return binInstr;
}

char *genLEATypeInstr(char **tokens){
    char *rt = NULL, *immVal = NULL, *rs = NULL;
    char *binInstr = (char *)malloc(sizeof(char) * (WORD_SIZE - OPCODE_SIZE) + 1);

    mallocErrorCheck(binInstr);

    rt = decimalToBinary(atoi(strchr(tokens[1], '$') + 1), REG_ADDR_SIZE);

    char *leftParens = strchr(tokens[2], '[');

    char *rightParen = strchr(tokens[2], ']');
    *rightParen='\0';
    rs=decimalToBinary(atoi(leftParens+1),16);
    strcpy(binInstr, rt);
    strcat(binInstr, rs);
    binInstr[WORD_SIZE - OPCODE_SIZE] = '\0';

    free(rs);
    free(rt);

    return binInstr;
}






char *genBranchTypeInstr(char **tokens, int currMemLoc) {
    char *encodedInstr  = (char *)malloc((WORD_SIZE - OPCODE_SIZE) * sizeof(char) + 1);
    mallocErrorCheck(encodedInstr);

    char *leftOp = NULL, *rightOp = NULL, *targetAddr = NULL;

    leftOp = decimalToBinary(atoi(tokens[1] + 1), REG_ADDR_SIZE);
    rightOp = decimalToBinary(atoi(tokens[2] + 1), REG_ADDR_SIZE);

    for(int i = 0; i < 5; i++) {
        if(strcmp(labels[i].labelName, tokens[3]) == 0) {
            int addr;
            if (labels[i].lineNum < currMemLoc) {
                addr = labels[i].lineNum - (currMemLoc - TEXT_SEGMENT);
            } else {
                addr = labels[i].lineNum + (currMemLoc - TEXT_SEGMENT);
            }
            labels[i].offset = addr;
            printf("offset %d\n", addr);
            targetAddr = decimalToBinary(addr, IMM_SIZE);
            break;
        }
    }
    strcpy(encodedInstr, leftOp);
    strcat(encodedInstr, rightOp);
    strcat(encodedInstr, targetAddr);

    free(leftOp);
    free(rightOp);
    free(targetAddr);

    return encodedInstr;
}

char *genJTypeInstr(char **tokens, int currMemLoc) {
    char *jumpTargetAddr  = (char *)malloc((WORD_SIZE - OPCODE_SIZE) * sizeof(char) + 1);
    mallocErrorCheck(jumpTargetAddr);

    char *freeHandle = NULL;


    for(int i = 0; i < 5; i++){
        if(strcmp(labels[i].labelName, tokens[1]) == 0){

            int addr;
            if(labels[i].lineNum < currMemLoc) {
                addr = labels[i].lineNum - (currMemLoc - TEXT_SEGMENT);
            }else {
                addr = labels[i].lineNum + (currMemLoc - TEXT_SEGMENT);
            }
            labels[i].offset = addr;
            printf("FOFSET IS %d\n", labels[i].offset);
            freeHandle = decimalToBinary(addr, JTA_SIZE);
            strcpy(jumpTargetAddr, freeHandle);
            free(freeHandle);
            break;
        }
    }
    return jumpTargetAddr;
}

char *genITypeInstr(char **tokens){
    char *binInstr  = (char *)malloc((WORD_SIZE - OPCODE_SIZE) * sizeof(char) + 1);
    mallocErrorCheck(binInstr);

    char *dest = NULL, *src = NULL, *imm = NULL;

    dest = decimalToBinary(atoi(tokens[1] + 1), REG_ADDR_SIZE);
    src = decimalToBinary(atoi(tokens[2] + 1), REG_ADDR_SIZE);
    imm = decimalToBinary(atoi(tokens[3]), IMM_SIZE);

    strcpy(binInstr, dest);
    strcat(binInstr, src);
    strcat(binInstr, imm);

    free(dest);
    free(src);
    free(imm);

    binInstr[WORD_SIZE - OPCODE_SIZE] = '\0';
    return binInstr;
}

/**
 * Utility function to encode instruction for basic R type functions
 * @param tokens : 2d array containing all parsed tokens from instruction
 * @return returns the encoded instruction excluding the opcode
 */
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

    free(rd);
    free(rs);
    free(rt);

    binInstr[WORD_SIZE - OPCODE_SIZE] = '\0';
    return binInstr;
}

/**
 * Utility function to generate the encoded instruction for LD and ST (load and store)
 * These are our general address mode instructions
 * @param tokens : 2d array containing all tokens from parsed instruction
 * @return returns the encoded instruction (excluding the opcode)
 */
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

    free(dest);
    free(distance);
    free(baseReg);
    free(indexReg);
    free(scale);

    binInstr[WORD_SIZE - OPCODE_SIZE] = '\0';
    return binInstr;
}

/**
 * Utility function to decode LW and SW instructions
 * @param tokens : 2d array that contains all parsed tokens from the expression
 * @return : returns the decoded instruction (excluding the opcode)
 */
char *genLWSWbinInstr(char **tokens) {
    char *rt = NULL, *immVal = NULL, *rs = NULL;
    char *binInstr = (char *)malloc(sizeof(char) * (WORD_SIZE - OPCODE_SIZE) + 1);

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
    binInstr[WORD_SIZE - OPCODE_SIZE] = '\0';

    free(rs);
    free(rt);
    free(immVal);

    return binInstr;
}

/**
 *
 * @param op : operation to perform
 * @param opLeft : left hand operand
 * @param opRight : right hand operand
 * @param size : number of bits
 * @param setFlags : should be 0 if we do not want it to modify flags. 2 if the instruction is subtraction
 * @return return the evaluated expression
 */
char *ALU(int op, char *opLeft, char *opRight, int size, int setFlags) {
    char *left = NULL, *right = NULL, *result = NULL;

    left = signExtend(opLeft, size);
    right = signExtend(opRight, size);

    switch(op){
        case ADD_OP:
            result = addBinary(left, right, size, setFlags);
            break;
        case SUB_OP:
            result = subBinary(left, right, size, setFlags);
            break;
        case DIV_OP:
            result = divBinary(left, right, NULL, size, setFlags);
            break;
        case MOD_OP:
            result = modBinary(left, right, size, setFlags);
            break;
        case MUL_OP:
	        result = mulBinary(left, right, size, setFlags);
            break;
        case ADDI_OP:
            result = addImmBinary(left, right, size, setFlags);
            break;
        default:
            printf("Error. No matching operation.\n");
            break;
    }

    return result;
}

/**
 * Adds immediate value with a value from a register
 * @param opLeft
 * @param opRight
 * @param size
 * @param setFlags
 * @return
 */
char *addImmBinary(char *opLeft, char *opRight, int size, int setFlags){
    return addBinary(opLeft, opRight, size, setFlags);
}

/**
 * opLeft - opRight
 * @param opLeft : left hand operand
 * @param opRight : right hand operand
 * @param size : number of bits
 * @param setFlags : setFlags should be 0 if we do not want it to change flags
 * @return : returns a string result of opLeft - opRight
 */
char *subBinary(char *opLeft, char *opRight, int size, int setFlags){
    char *right = NULL;
    // Getting Complement of A Number(The opcode which is on right side)
    right = decimalToComplementBinary(binaryToDecimal(opRight, size), size);

    // Adding 1 to the complement of operand
    right = addBinary(decimalToBinary(1, size), right, size, 0);
    
    return addBinary(opLeft, right, size, setFlags);
}

/**
 * Function to add in binary. opLeft + opRight
 * @param opLeft : left hand operand
 * @param opRight : right hand operand
 * @param size : number of bits
 * @param setFlags : setFlags should be 0 if we should not change flags (ie for PC)
 * @return
 */
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
        if (carry == '1' && setFlags != 2) {
            flags[OVERFLOW_FLAG] = '1';
            printf("OVERFLOW ON INSTRUCTION\n");
        }else flags[OVERFLOW_FLAG] = '0';

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

/**
 * left MOD right
 * @param left : left hand side of operand
 * @param right : right and side of operand
 * @param size : number of bits
 * @param setFlags : specify whether you want flags changed. 0: don't change
 * @return returns the modulus of the expression
 */
char* modBinary(char* left, char* right, int size, int setFlags)
{
    char *remainder = (char *)malloc(sizeof(char) * size + 1);
    mallocErrorCheck(remainder);
    remainder[size] = '\0';

    char *freeHandle = divBinary(left, right, &remainder, size, setFlags);
    free(freeHandle);
    return remainder;
}


/**
 * left MUL right
 * @param left : left hand side operand
 * @param right : right hand side operand
 * @param size : number of bits
 * @param setFlags : setFlags shoud be 0 if flags should not be changed
 * @return : return the result of left MUL right
 */
char* mulBinary(char* left, char* right, int size, int setFlags)
{
    char *mul = (char *)malloc(sizeof(char) * size + 1), *freeHandle = NULL, *freeHandle2 = NULL;
    mul[size] = '\0';
    int op1 = binaryToDecimal(left, size);
    int op2 = binaryToDecimal(right, size);

    long result = 0;

    if((op1 == INT_MIN && (op2 != 0 && op2 != 1)) || (op2 == INT_MIN && (op1 != 0 && op1 != 1)) && setFlags) { //very small # is a very large unsigned #
        flags[OVERFLOW_FLAG] = '1';
        printf("OVERFLOW ON INSTRUCTION\n");
    }else {
        flags[OVERFLOW_FLAG] = '0';

        while (op2 != 0) {
            if (op2 & 01) { //if odd
                result = binaryToDecimal(addBinary(freeHandle = decimalToBinary(result, WORD_SIZE), freeHandle2 = decimalToBinary(op1, WORD_SIZE), WORD_SIZE,
                                  setFlags), WORD_SIZE);
                free(freeHandle);
                free(freeHandle2);
            }
            op1 = op1 << 1;
            op2 = op2 >> 1;
        }

    }
    strcpy(mul, freeHandle = decimalToBinary(result, WORD_SIZE));
    free(freeHandle);
    return mul;
}

/**
 *
 * @param left : left hand side operand
 * @param right : right hand side operand
 * @param remainder : if mod, pass remainder by reference to store the remainder in. Otherwise, pass NULL
 * @param size : number of bits
 * @param setFlags : specify whether flags are to be modified
 * @return return the quotient
 */
char *divBinary(char *left, char *right, char **remainder, int size, int setFlags){
    char *result = malloc(WORD_SIZE + 1), *freeHandle = NULL;
    mallocErrorCheck(result);
    int curr = 1, res = 0;

    int dividend = binaryToDecimal(left, WORD_SIZE);
    int divisor = binaryToDecimal(right, WORD_SIZE);

    if(divisor == 0){
        printf("Error. Divide by 0.\n");
        exit(1);
    }else {

        int denom = divisor;

        while (denom <= dividend) {
            denom <<= 1;
            curr <<= 1;
        }

        denom >>= 1;
        curr >>= 1;

        while (curr != 0) {
            if (dividend >= denom) {
                dividend -= denom;
                res |= curr;
            }
            curr >>= 1;
            denom >>= 1;
        }

        strcpy(result, freeHandle = decimalToBinary(res, WORD_SIZE));
        free(freeHandle);

        //set flags
        if (remainder != NULL) { //this is not the modulus function so no need to save remainder
            strcpy(*remainder, freeHandle = decimalToBinary(dividend, WORD_SIZE));
            free(freeHandle);
            if (dividend == 0)
                flags[ZERO_FLAG] = '1';
            else flags[ZERO_FLAG] = '0';
        } else {
            if (res == 0)
                flags[ZERO_FLAG] = '1';
            else flags[ZERO_FLAG] = '0';
        }
    }

    return result;
}


/**
 * Sign extension unit to ensure data is the same size when performing operations
 * @param value : value to extend
 * @param size : maximum number of bits
 * @return : returns a string to the extended value
 */
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

/**
 * Executes the given program in the text section of memory
 * @param info : struct that contains execution information to load from memory
 */
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

    char *freeHandle = NULL;
    bool jumpOrBra = false;

    strcpy(PC, freeHandle = decimalToBinary(TEXT_SEGMENT, PC_SIZE));
    free(freeHandle);

    int memLoc = 0, rdOffset = 0, rsOffset = 0, rtOffset = 0, immOffset = 0, addr = 0;
    char *result;

    for(int i = 0; i < info.lines; i++){
        result = NULL;
        jumpOrBra = false;
        addr = 0;
        //clear out memAddr and memData for instructions not using memory
        strcpy(memAddr, "00000000000000000000000000000000");
        strcpy(memData, "00000000000000000000000000000000");

        //fetch instr from mem
        memLoc = binaryToDecimal(PC, PC_SIZE);
        strcpy(instr, memory[memLoc]);

        // $t = MEM[$s + offset]
        if(strncmp(LW, instr, OPCODE_SIZE) == 0){ //LOAD WORD

            rsOffset = OPCODE_SIZE;
            rtOffset = OPCODE_SIZE + REG_ADDR_SIZE;
            immOffset = rtOffset + REG_ADDR_SIZE;

            //get register addresses and offset value from instruction memory
            strncpy(rs, instr + rsOffset, REG_ADDR_SIZE);
            strncpy(rt, instr + rtOffset, REG_ADDR_SIZE);
            strncpy(imm, instr + immOffset, IMM_SIZE);

            //calculate memory address and exec instruction
            memLoc = binaryToDecimal(regFile[binaryToDecimal(rs, REG_ADDR_SIZE)], REG_ADDR_SIZE) + binaryToDecimal(imm, IMM_SIZE);
            strcpy(memAddr, freeHandle = decimalToBinary(memLoc, WORD_SIZE));
            free(freeHandle);

            strcpy(memData, memory[binaryToDecimal(memAddr, WORD_SIZE)]);
            strcpy(regFile[binaryToDecimal(rt, REG_ADDR_SIZE)], memData);

        }else if(strncmp(SW, instr, OPCODE_SIZE) == 0){ //MEM[$s + offset] = $t STORE WORD

            rsOffset = OPCODE_SIZE;
            rtOffset = OPCODE_SIZE + REG_ADDR_SIZE;
            immOffset = rtOffset + REG_ADDR_SIZE;

            //get register addresses and offset value from instruction memory
            strncpy(rs, instr + rsOffset, REG_ADDR_SIZE);
            strncpy(rt, instr + rtOffset, REG_ADDR_SIZE);
            strncpy(imm, instr + immOffset, IMM_SIZE);

            //calculate memory address and exec instruction
            memLoc = binaryToDecimal(regFile[binaryToDecimal(rs, REG_ADDR_SIZE)], REG_ADDR_SIZE) + binaryToDecimal(imm, IMM_SIZE);
            strcpy(memAddr, freeHandle = decimalToBinary(memLoc, WORD_SIZE));         //store memory addr to store data in
            free(freeHandle);
            strcpy(memData, regFile[binaryToDecimal(rt, REG_ADDR_SIZE)] );      //grab data to transfer and store in memData
            strcpy(memory[binaryToDecimal(memAddr, WORD_SIZE)], memData);//store in memory location

        }else if(strncmp(LD, instr, OPCODE_SIZE) == 0) { //LOAD using general address mode

            // $reg, distance(base, index, scale) 5 7 5 5 4
            rdOffset = OPCODE_SIZE;
            int distanceOffset = OPCODE_SIZE + REG_ADDR_SIZE;
            int baseOffset = distanceOffset + DIST_SIZE;
            int indexOffset = baseOffset + REG_ADDR_SIZE;
            int scaleOffset = indexOffset + REG_ADDR_SIZE;

            char dist[DIST_SIZE + 1];
            char base[REG_ADDR_SIZE + 1];
            char index[REG_ADDR_SIZE + 1];
            char scale[SCALE_SIZE + 1];

            dist[DIST_SIZE] = '\0';
            base[REG_ADDR_SIZE] = '\0';
            index[REG_ADDR_SIZE] = '\0';
            scale[SCALE_SIZE] = '\0';
            rd[REG_ADDR_SIZE] = '\0';

            //get register addresses and offset value from instruction memory
            strncpy(rd, instr + rdOffset, REG_ADDR_SIZE);
            strncpy(dist, instr + distanceOffset, DIST_SIZE);
            strncpy(base, instr + baseOffset, REG_ADDR_SIZE);
            strncpy(index, instr + indexOffset, REG_ADDR_SIZE);
            strncpy(scale, instr + scaleOffset, SCALE_SIZE);

            //calculate address
            int distVal = binaryToDecimal(dist, DIST_SIZE);
            int baseVal = binaryToDecimal(regFile[binaryToDecimal(base, REG_ADDR_SIZE)], WORD_SIZE);
            int indexVal = binaryToDecimal(regFile[binaryToDecimal(index, REG_ADDR_SIZE)], WORD_SIZE);
            int scaleVal = binaryToDecimal(scale, SCALE_SIZE);

            memLoc = distVal + baseVal + (indexVal * scaleVal);
            //update memAddr and memData SPR
            strcpy(memAddr, freeHandle = decimalToBinary(memLoc, WORD_SIZE));
            free(freeHandle);
            strcpy(memData, memory[memLoc]);

            //store in destination reg
            strcpy(regFile[binaryToDecimal(rd, REG_ADDR_SIZE)], memData);

        }else if (strncmp(ST, instr, OPCODE_SIZE) == 0) { //STORE word using general address mode

            // $reg, distance(base, index, scale) 5 7 5 5 4
            int rdOffset = OPCODE_SIZE;
            int distanceOffset = OPCODE_SIZE + REG_ADDR_SIZE;
            int baseOffset = distanceOffset + DIST_SIZE;
            int indexOffset = baseOffset + REG_ADDR_SIZE;
            int scaleOffset = indexOffset + REG_ADDR_SIZE;

            char dist[DIST_SIZE + 1];
            char base[REG_ADDR_SIZE + 1];
            char index[REG_ADDR_SIZE + 1];
            char scale[SCALE_SIZE + 1];

            dist[DIST_SIZE] = '\0';
            base[REG_ADDR_SIZE] = '\0';
            index[REG_ADDR_SIZE] = '\0';
            scale[SCALE_SIZE] = '\0';
            rd[REG_ADDR_SIZE] = '\0';

            //get register addresses and offset value from instruction memory
            strncpy(rd, instr + rdOffset, REG_ADDR_SIZE);
            strncpy(dist, instr + distanceOffset, DIST_SIZE);
            strncpy(base, instr + baseOffset, REG_ADDR_SIZE);
            strncpy(index, instr + indexOffset, REG_ADDR_SIZE);
            strncpy(scale, instr + scaleOffset, SCALE_SIZE);

            //calculate address
            int distVal = binaryToDecimal(dist, DIST_SIZE);
            int baseVal = binaryToDecimal(regFile[binaryToDecimal(base, REG_ADDR_SIZE)], WORD_SIZE);
            int indexVal = binaryToDecimal(regFile[binaryToDecimal(index, REG_ADDR_SIZE)], WORD_SIZE);
            int scaleVal = binaryToDecimal(scale, SCALE_SIZE);

            memLoc = distVal + baseVal + (indexVal * scaleVal);
            strcpy(memAddr, freeHandle = decimalToBinary(memLoc, WORD_SIZE));
            free(freeHandle);
            strcpy(memData, regFile[binaryToDecimal(rd, REG_ADDR_SIZE)]);
            strcpy(memory[memLoc], memData);

        }else if(strncmp(SUB, instr, OPCODE_SIZE) == 0) { //SUBTRACT INSTRUCTION

            rdOffset = OPCODE_SIZE;
            rsOffset = rdOffset + RTYPE_RD_SIZE;
            rtOffset = rsOffset + RTYPE_ADDR_SIZE;

            //get register addresses and offset value from instruction memory
            strncpy(rd, instr + rdOffset, RTYPE_RD_SIZE);
            strncpy(rs, instr + rsOffset, RTYPE_ADDR_SIZE);
            strncpy(rt, instr + rtOffset, RTYPE_ADDR_SIZE);

            result = ALU(SUB_OP, regFile[binaryToDecimal(rs, RTYPE_ADDR_SIZE)], regFile[binaryToDecimal(rt, RTYPE_ADDR_SIZE)], WORD_SIZE, 2); //execute instruction
            strcpy(regFile[binaryToDecimal(rd, RTYPE_RD_SIZE)], result);

        }else if((strncmp(ADD, instr, OPCODE_SIZE) == 0)) { //ADD INSTRUCTION

            rdOffset = OPCODE_SIZE;
            rsOffset = rdOffset + RTYPE_RD_SIZE;
            rtOffset = rsOffset + RTYPE_ADDR_SIZE;

            //get register addresses and offset value from instruction memory
            strncpy(rd, instr + rdOffset, RTYPE_RD_SIZE);
            strncpy(rs, instr + rsOffset, RTYPE_ADDR_SIZE);
            strncpy(rt, instr + rtOffset, RTYPE_ADDR_SIZE);

            result = ALU(ADD_OP, regFile[binaryToDecimal(rs, RTYPE_ADDR_SIZE)], regFile[binaryToDecimal(rt, RTYPE_ADDR_SIZE)], WORD_SIZE, 1); //execute instruction
            strcpy(regFile[binaryToDecimal(rd, RTYPE_RD_SIZE)], result);

        }else if(strncmp(MUL, instr, OPCODE_SIZE) == 0) { //MUL INSTRUCTION

            //get register addresses and offset value from instruction memory
            strncpy(rd, instr + OPCODE_SIZE, RTYPE_RD_SIZE);
            strncpy(rs, instr + OPCODE_SIZE + RTYPE_RD_SIZE, RTYPE_ADDR_SIZE);
            strncpy(rt, instr + OPCODE_SIZE + RTYPE_RD_SIZE + RTYPE_ADDR_SIZE, RTYPE_ADDR_SIZE);

            result = ALU(MUL_OP, regFile[binaryToDecimal(rs, RTYPE_ADDR_SIZE)], regFile[binaryToDecimal(rt, 8)], WORD_SIZE, 1);
            strcpy(regFile[binaryToDecimal(rd, RTYPE_RD_SIZE)], result);

        }else if(strncmp(MOD, instr, OPCODE_SIZE) == 0){ //MOD INSTRUCTION

            //get register addresses and offset value from instruction memory
            strncpy(rd, instr + OPCODE_SIZE, RTYPE_RD_SIZE);
            strncpy(rs, instr + OPCODE_SIZE + RTYPE_RD_SIZE, RTYPE_ADDR_SIZE);
            strncpy(rt, instr + OPCODE_SIZE + RTYPE_RD_SIZE + RTYPE_ADDR_SIZE, RTYPE_ADDR_SIZE);

            result = ALU(MOD_OP, regFile[binaryToDecimal(rs, RTYPE_ADDR_SIZE)], regFile[binaryToDecimal(rt, RTYPE_ADDR_SIZE)], WORD_SIZE, 1);
            strcpy(regFile[binaryToDecimal(rd, RTYPE_RD_SIZE)], result);

        }else if(strncmp(DIV, instr, OPCODE_SIZE) == 0) { //DIV INSTRUCTION

            //get register addresses and offset value from instruction memory
            strncpy(rd, instr + OPCODE_SIZE, RTYPE_RD_SIZE);
            strncpy(rs, instr + OPCODE_SIZE + RTYPE_RD_SIZE, RTYPE_ADDR_SIZE);
            strncpy(rt, instr + OPCODE_SIZE + RTYPE_RD_SIZE + RTYPE_ADDR_SIZE, RTYPE_ADDR_SIZE);

            result = ALU(DIV_OP, regFile[binaryToDecimal(rs, RTYPE_ADDR_SIZE)], regFile[binaryToDecimal(rt, RTYPE_ADDR_SIZE)], WORD_SIZE, 1);
            strcpy(regFile[binaryToDecimal(rd, RTYPE_RD_SIZE)], result);
        }else if(strncmp(ADDI, instr, OPCODE_SIZE) == 0) {
            strncpy(rd, instr + OPCODE_SIZE, REG_ADDR_SIZE);
            strncpy(rs, instr + OPCODE_SIZE + REG_ADDR_SIZE, REG_ADDR_SIZE);
            strncpy(imm, instr + OPCODE_SIZE + REG_ADDR_SIZE + REG_ADDR_SIZE, IMM_SIZE);

            result = ALU(ADDI_OP, regFile[binaryToDecimal(rs, REG_ADDR_SIZE)], imm, WORD_SIZE, 1);
            strcpy(regFile[binaryToDecimal(rd, REG_ADDR_SIZE)], result);
        }else if(strncmp(J, instr, OPCODE_SIZE) == 0) {
            //load address to jump to here
            jumpOrBra = true;
            addr = binaryToDecimal(instr + OPCODE_SIZE, JTA_SIZE)+1;
            printf("offest is %d %s\n", addr, memory[addr]);
            //i += addr;
            int currPC = binaryToDecimal(PC, PC_SIZE) + addr;
            strcpy(PC, freeHandle = decimalToBinary(currPC, PC_SIZE)); //move to next instruction
            free(freeHandle);
        }else if(strncmp(BLT, instr, OPCODE_SIZE) == 0) {
            jumpOrBra = true;
            int leftReg = binaryToDecimal(instr + OPCODE_SIZE, REG_ADDR_SIZE);
            int rightReg = binaryToDecimal(instr + OPCODE_SIZE + REG_ADDR_SIZE, REG_ADDR_SIZE);

            int leftVal = binaryToDecimal(regFile[leftReg], WORD_SIZE);
            int rightVal = binaryToDecimal(regFile[rightReg], WORD_SIZE);

            printf("found branch instr\n");
            if(leftVal < rightVal) {
                printf("branch taken\n");
                addr = signedBinaryToDecimal(instr + OPCODE_SIZE + REG_ADDR_SIZE + REG_ADDR_SIZE , IMM_SIZE)+1;
                int currPC = binaryToDecimal(PC, PC_SIZE) + addr;
                strcpy(PC, freeHandle = decimalToBinary(currPC, PC_SIZE)); //move to next instruction
                free(freeHandle);
            }else{
                jumpOrBra = false; //branch was not taken
            }
        }else if(strncmp(BNE, instr, OPCODE_SIZE) == 0) {
            jumpOrBra = true;
            int leftReg = binaryToDecimal(instr + OPCODE_SIZE, REG_ADDR_SIZE);
            int rightReg = binaryToDecimal(instr + OPCODE_SIZE + REG_ADDR_SIZE, REG_ADDR_SIZE);

            int leftVal = binaryToDecimal(regFile[leftReg], WORD_SIZE);
            int rightVal = binaryToDecimal(regFile[rightReg], WORD_SIZE);

            printf("found branch instr\n");
            if (leftVal != rightVal) {
                printf("branch taken\n");
                addr = signedBinaryToDecimal(instr + OPCODE_SIZE + REG_ADDR_SIZE + REG_ADDR_SIZE, IMM_SIZE) + 1;
                int currPC = binaryToDecimal(PC, PC_SIZE) + addr;
                strcpy(PC, freeHandle = decimalToBinary(currPC, PC_SIZE)); //move to next instruction
                free(freeHandle);
            } else {
                jumpOrBra = false; //branch was not taken
            }
        }else if(strncmp(BEQ, instr, OPCODE_SIZE) == 0) {
            jumpOrBra = true;
            int leftReg = binaryToDecimal(instr + OPCODE_SIZE, REG_ADDR_SIZE);
            int rightReg = binaryToDecimal(instr + OPCODE_SIZE + REG_ADDR_SIZE, REG_ADDR_SIZE);

            int leftVal = binaryToDecimal(regFile[leftReg], WORD_SIZE);
            int rightVal = binaryToDecimal(regFile[rightReg], WORD_SIZE);

            printf("found branch instr\n");
            if (leftVal == rightVal) {
                printf("branch taken\n");
                addr = signedBinaryToDecimal(instr + OPCODE_SIZE + REG_ADDR_SIZE + REG_ADDR_SIZE, IMM_SIZE) + 1;
                int currPC = binaryToDecimal(PC, PC_SIZE) + addr;
                strcpy(PC, freeHandle = decimalToBinary(currPC, PC_SIZE)); //move to next instruction
                free(freeHandle);
            } else {
                jumpOrBra = false; //branch was not taken
            }
        }else if(strncmp(MOV, instr, OPCODE_SIZE) == 0) {

            strncpy(rd, instr + OPCODE_SIZE, REG_ADDR_SIZE);
            strncpy(rs, instr + OPCODE_SIZE + REG_ADDR_SIZE, REG_ADDR_SIZE);

            result = ALU(ADDI_OP, regFile[binaryToDecimal(rs, REG_ADDR_SIZE)], "0000000000000000" , WORD_SIZE, 1);
            strcpy(regFile[binaryToDecimal(rd, REG_ADDR_SIZE)], result);
        }else if(strncmp(LEA, instr, OPCODE_SIZE) == 0) {

            strncpy(rd, instr + OPCODE_SIZE, REG_ADDR_SIZE);
            strncpy(rs, instr + OPCODE_SIZE + REG_ADDR_SIZE, IMM_SIZE);
            result = ALU(ADDI_OP, rs, "0000000000000000" , IMM_SIZE, 1);
            int result16bit=binaryToDecimal(result,16);
            result=decimalToBinary(result16bit,WORD_SIZE);

            strcpy(regFile[binaryToDecimal(rd, REG_ADDR_SIZE)], result);
        }

        if(strcmp(instr, "00000000000000000000000000000000") != 0)
            printExecutionData(i);

        if(jumpOrBra == false) {
            strcpy(PC, freeHandle = ALU(ADD_OP, PC, "1", PC_SIZE, 0)); //move to next instruction
            free(freeHandle);
        }else i += addr - 1;

        if(result != NULL)
            free(result);
    }
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
    }else if(strncmp(BNE, instrFromMem, OPCODE_SIZE) == 0) {
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

        sprintf(instrBuilder, "%s $%d, $%d, %s", "BNE", binaryToDecimal(left, REG_ADDR_SIZE), binaryToDecimal(right, REG_ADDR_SIZE), label);
    }else if(strncmp(BEQ, instrFromMem, OPCODE_SIZE) == 0) {
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

        sprintf(instrBuilder, "%s $%d, $%d, %s", "BEQ", binaryToDecimal(left, REG_ADDR_SIZE), binaryToDecimal(right, REG_ADDR_SIZE), label);
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
