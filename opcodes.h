//
// Created by dearm on 9/17/2016.
//

#ifndef DESIGN_ISA_OPCODES_H
#define DESIGN_ISA_OPCODES_H

//I Types
const char *LW = "100011";
const char *SW = "101011";
const char *LD = "100010";
const char *ST = "100100";
const char *ADDI = "100101";

const char *BLT = "100110";
const char *BNE = "100001";
const char *BEQ = "100111";

//R Types and functions
const char *ADD = "000001";
const char *SUB = "000010";
const char *MUL = "000011";
const char *DIV = "000100";
const char *MOD = "000101";

//J Types
const char *J = "010000";

const char *MOV= "000111";
const char *LEA= "000110";

#endif //DESIGN_ISA_OPCODES_H
