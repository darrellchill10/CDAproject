#include "spimcore.h"





//preforms the ALU operations based on the value of ALUControl. THis is what I showed the professor and she said that it looks good. IDK why thought because ALUControl is a char
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
    //set the test condition for the ALUControl based on the table given for the ALU function in the ProjectGuidelines
    switch (ALUControl) {
        case 0: //ALUControl = 000 (add)
            *ALUresult = A + B;
            break;
        case 1: //ALUControl = 001 (subtract)
            *ALUresult = A - B;
            break;
        case 2: //ALUControl = 010 (set less than signed)
            if((int)A < (int)B)
                *ALUresult = 1;
            else
                *ALUresult = 0;
            break;
        case 3: //ALUControl = 011 (set less than unsigned)
            if(A < B)
                *ALUresult = 1;
            else
                *ALUresult = 0;
            break;
        case 4: //ALUControl = 100 (AND)
            *ALUresult = (A&B);
            break;
        case 5: //ALUControl = 101 (OR)
            *ALUresult = (A|B);
            break;
        case 6: //ALUControl = 110 (Shift left B by 16 bits)
            B = (B<<16);
            break;
        case 7: //ALUControl = 111 (NOT A)
            *ALUresult = (!A);
            break;
    }

    if(*ALUresult == 0 )
        *Zero = 1;
    else
        *Zero = 0;

}



int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{

    // makes sure that it is word aligned
    if(PC % 4 == 0){
        *instruction = Mem[(PC >> 2)]; //go to (PC >> 2) and assign the value to the instruction field
        return 0;
    }
    else
        return 1; //not word aligned, return 0
}



void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{

/*
    thought process...
    1. shift the lowest order bit of the section of bits that we want to the lowest order position of the 32 bit number
    2. AND that number with (1 shifted by the number of bits that we want to extract)-1)
    ....this allows us to keep the bits that we want, and turn the remaining bits in the 32 bit number to 0
*/


    *op = (instruction >> 26)&((1<<6)-1); //moves the first 6 bits to the lowest order position, then ANDS that value with 0011 1111. I believe that ANDing this with 11 1111 is unnecessary, however I figured I would keep it in just to be safe
    *r1 = (instruction >> 21)&((1<<5)-1); //same process as last step. ANDS with a number who's binary value has as many 1's as the size of the extracted number and 0's for the remaining portion.
    *r2 = (instruction >> 16)&((1<<5)-1);
    *r3 = (instruction >> 11)&((1<<5)-1);
    *funct = instruction&((1<<6)-1); //since funct is already in the lowest order position, no shift is necessary
    *offset = instruction&((1<<16)-1); //since the offset is already in the lowest order position, no shift is necessary
    *jsec = instruction&((1<<26)-1); //since jsec is already in the lowest order position, no shift is required

}




//set the control signals based on the information in the Control Signals Recitation power point (recitation 10)
/*1.You can definetly make sure that I assigned the control signals properly because I may have messed up on a few
  2. I still have no idea how to set the control signals for set less than, and set less than unsigned*/
int instruction_decode(unsigned op,struct_controls *controls)
{
    switch(op){

        case 0: //000000 (R-type)
            controls->RegDst = 1;
            controls->RegWrite = 1;
            controls->ALUSrc = 0;
            controls->MemRead = 0;
            controls->MemWrite = 0;
            controls->MemtoReg = 0;
            controls->Branch = 0;
            controls->Jump = 0;
            controls->ALUOp = 7;
            break;

        case 8: //op = 001000 (add imidiate)
            controls->RegDst = 0;
            controls->RegWrite = 1;
            controls->ALUSrc = 1;
            controls->MemRead = 0;
            controls->MemWrite = 0;
            controls->MemtoReg = 0;
            controls->Branch = 0;
            controls->Jump = 0;
            controls->ALUOp = 0;
            break;

        case 15: //op = 001111 (load upper immidiate)
            controls->RegDst = 0;
            controls->RegWrite = 1; //1
            controls->ALUSrc = 1; //1
            controls->MemRead = 0; //0
            controls->MemWrite = 0;
            controls->MemtoReg = 0; //0
            controls->Branch = 0;
            controls->Jump = 0;
            controls->ALUOp = 6;
            break;

        case 35: //op = 100011 (load word)
            controls->RegDst = 0;
            controls->RegWrite = 1;
            controls->ALUSrc = 1;
            controls->MemRead = 1;
            controls->MemWrite = 0;
            controls->MemtoReg = 1;
            controls->Branch = 0;
            controls->Jump = 0;
            controls->ALUOp = 0;
            break;

        case 43: //op = 101011 (store word)
            controls->RegDst = 2;
            controls->RegWrite = 0;
            controls->ALUSrc = 1;
            controls->MemRead = 0;
            controls->MemWrite = 1;
            controls->MemtoReg = 2;
            controls->Branch = 0;
            controls->Jump = 0;
            controls->ALUOp = 0;
            break;

        case 4: //op = 000100 (branch equal)
            controls->RegDst = 2;
            controls->RegWrite = 0;
            controls->ALUSrc = 0;
            controls->MemRead = 0;
            controls->MemWrite = 0;
            controls->MemtoReg = 2;
            controls->Branch = 1;
            controls->Jump = 0;
            controls->ALUOp = 1;
            break;

        case 10: //op = 001010 (set on less than immediate)

            controls->RegDst = 1;
            controls->RegWrite = 1;
            controls->ALUSrc = 1;
            controls->MemRead = 0;
            controls->MemWrite = 0;
            controls->MemtoReg = 0;
            controls->Branch = 0;
            controls->Jump = 0;
            controls->ALUOp = 2;
            break;

        case 11: //op = 001011 (set on less than immediate unsigned)

            controls->RegDst = 1;
            controls->RegWrite = 1;
            controls->ALUSrc = 0;
            controls->MemRead = 0;
            controls->MemWrite = 0;
            controls->MemtoReg = 0;
            controls->Branch = 0;
            controls->Jump = 0;
            controls->ALUOp = 3;
            break;

        //case ADD SET LES THAN UNSIGNED ?????????????????????
        //case ADD SER LESS THAN UN ?????????????????????????//

        case 2: //op = 000010 (jump)
            controls->RegDst = 0;
            controls->RegWrite = 0;
            controls->ALUSrc = 0;
            controls->MemRead = 0;
            controls->MemWrite = 0;
            controls->MemtoReg = 0;
            controls->Branch = 0;
            controls->Jump = 1;
            controls->ALUOp = 0;
            break;


        //  return 1 if halt
        default:
                    return 1;
    }

    return 0;

}

//this one seemed a little to easy,so you may want to double check.
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{
    /*1. Read the registers addressed by r1 and r2 from Reg,
    and write the read values to data1 and data2 respectively.*/

    *data1 = Reg[r1];
    *data2 = Reg[r2];
}


//change the function
void sign_extend(unsigned offset,unsigned *extended_value)
{
    //finishing now
}




int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{

    if(ALUSrc == 1){//if ALUSrc = 1, then we do not need to decode the funct field
        data2 = extended_value;
    }

    //I think we shoudl try making a new variable instead reasigning a value to ALUOp for each condition and then passing that into the ALU function, but this worked so I didn't change it

    //Got funct values from slide 5 of the MIPS reference power point
    if(ALUOp == 7){//if ALUOp = 7, then it is an R-type, and we must decode the funct field to determine the signal to be sent as the ALUControl
        switch(funct){
            case 32: //funct = 100000 (add)
                ALUOp = 0;
            break;
            case 34: //funct = 100010 (subtract)
                ALUOp = 1;
            break;
            case 42: //funct = 101010 (set l.t.)
                ALUOp = 2;
                break;
            case 43: //funct = 101011 (set less than unsigned)
                ALUOp = 3;
                break;
            case 36: //funct = 100100 (AND)
                ALUOp = 4;
                break;
            case 37: //funct = 100101 (OR)
                ALUOp = 5;
                break;
            /*case 4: i DONT THINK THAT THIS IS RIGHT
                        ALUOp = 6;
                        break;*/
            case 39: //funct = 100111 (NOT)
                ALUOp = 7;
                break;
            default:
                return 1; //returns 1 indicating that a halt has taken place
            }

            ALU(data1,data2, ALUOp,ALUresult,Zero);

        }
        else{ //may need to adjust for branches
            ALU(data1,data2,ALUOp,ALUresult,Zero);
        }


        return 0;
}



int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{
    if(MemRead) {
        if(ALUresult %4 == 0) {
            *memdata = Mem[(ALUresult >> 2)];
        } else {
            return 1; //halt condition, ALUresult is not word aligned
        }
    }

    if(MemWrite) {
        if(ALUresult %4 == 0)
            Mem[ALUresult >> 2] = data2;
        else
            return 1; //halt condition reached, ALUresult is not word aligned
    }

    return 0;

}



//Not sure if this function is right.. you may want to double check my if statement logic
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{
    if(RegWrite == 1)
    {
        if(MemtoReg == 1)
        {
            if(RegDst == 0)
            {
                //use r2
                Reg[r2] = memdata;
            }
            else
            {
                //use r3
                Reg[r3] = memdata;
            }
        }
        else if(MemtoReg == 0)
        {
            if(RegDst == 0)
            {
                //use r2
                Reg[r2] = ALUresult;
            }
            else
            {
                //use r3
                Reg[r3] = ALUresult;
            }
        }
    }


}


void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{


    *PC += 4;

    if(Branch == 1 && Zero == 1)
        *PC += extended_value << 2; //change

    if(Jump == 1)
        *PC = (jsec << 2) | (*PC & 0xf0000000); //change (trying to do this without hexidecimal values


}
