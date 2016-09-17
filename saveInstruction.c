#include<stdio.h>
#include<stdlib.h>
#include<string.h>

void saveinstruction(char *);
void decodeRegister(char *, char *);

void main()
{
    
    printf("note: \n The instruction that you enter is not considered for now, it can be linked from final function \n    Function works on default instruction that is hardcoded \n     I have printed Instruction register(IR) in each stage.    Final IR will be saved into memory once it is set up");
	char inputIns[256];	//instruction register
						//inputIns= "ADD r1, r2"
	printf("enter instruction \n");
	scanf("%s", &inputIns[0]); //scan input instruction(for now it is dummy)
	saveinstruction(inputIns);
}

void saveinstruction(char *inputIns) //
{
	
	char IR[32];
	
	char temp[]= "LW R, S;";    //this is actual instruction function is working on
	char *opcode;
	char *Rs;
	char *Rt;
opcode = strtok(temp, " ");

Rs = strtok(NULL, ", ");  
Rt = strtok(NULL, ";");
Rt++;

printf("before encoding\n opcode: %s \n", opcode);
printf("Register 1: %s \n", Rs);
printf("Register 2: %s \n", Rt);

if (!(strcmp(opcode, "LW")))  // for opcode bits of Instruction Register
	{

		strcpy(IR, "100011");
	}

	else if (!(strcmp(opcode, "SW")))
	{

		strcpy(IR, "101011");
	}
	else
		strcpy(IR, "000000");

decodeRegister(Rs, IR);
decodeRegister(Rt, IR);


}

void decodeRegister(char *IRregister, char *IR)
{
    if(!(strcmp(IRregister,"R")))                  //R, S, T are geneal purpose registers
    {
        IR= strcat(IR, "00000");
                    printf("IR %s \n", IR);
        
    }
    else if(!(strcmp(IRregister,"S")))                  //R, S, T are geneal purpose registers
    {
        IR= strcat(IR, "00001");
                    printf("IR %s \n", IR);
        
    }
    else
    printf("register name invalid \n");
 
   	
}


//executeInstruction(char *)