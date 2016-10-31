//
// Created by dearm on 10/31/2016.
//

#ifndef DESIGN_ISA_UTILITY_H
#define DESIGN_ISA_UTILITY_H

//conversion functions
char *decimalToBinary(int toConvert, int numOfBits);
char *decimalToComplementBinary(int toConvert, int numOfBits);
int binaryToDecimal(char *binary, int size);
int signedBinaryToDecimal(char *binary, int size);

void printExecutionData(int instrNum);
void mallocErrorCheck(char *ptr);
char *buildInstrForRTypePrint(char *instr, char *instrName);
char *buildInstrForITypePrint(char *instr, char *instrName);
void preprocessFile(FILE *fp);

#endif //DESIGN_ISA_UTILITY_H
