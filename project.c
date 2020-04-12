#include "spimcore.h"


/* ALU */
/* 10 Points */ //NEEDS WORK
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
    //NOTE: I assumed that the ALUControl characters will come as shown on the table in the PDF... But I am not sure it I need to change the case statements to support a different format (i.e. if the ALUControl character is a hexidecimal value)

    unsigned Z;

    switch(ALUControl) //skeleton, not sure how to handle ALUControl since it is a char
    {
        case 0: //000 = 0 = add
            Z = A + B; //dereferences ALUresult because it is a pointer
        break;
        case 1: //001 = 1 = subtract
            Z = A-B;
        break;
        case 2: //010 = 2 = set-on-less-than......Do we need to cast A and B to integers?
            if(A<B)
                Z = 1;
            else
                Z = 0;
        break;
        case 3: //011 = 3 = set-on-less-than with A and B being unsigned integers
            if(A<B)
                Z = 1;
            else
                Z = 0;
        break;
        case 4: //100 = 4 = A AND B
            Z = A&B; //may need to come back and make sure that this is the correct way to use bitwise AND
        break;
        case 5: //101 = 5 = A OR B
            Z = A|B; //may need to come back and make sure that this is the correct way to use bitwise OR
        break;
        case 6: //110 = 6
            //shift B by 16 bits
        break;
        case 7: //111 = 7
            Z = !A; //may need to come back and make sure that this is the correct way to use bitwise NOT
        break;
    }

    *ALUresult = Z; //assigns the value of Z to the ALUresult
    if(Z == 0) //checks if the result from the ALU operation is a zero and asserts Zero if the condition is met
    {
        *Zero = '1';
    }
    else
    {
        *Zero = '0';
    }

}






/* instruction fetch */
/* 10 Points */ //NEEDS WORK
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
    if(1) //change to a condition that checks if a halt condition has occurred..if a halt has NOT occurred, then update the instruction and return 0....else, return 1
    {
        *instruction = Mem[PC>>2]; //assigns the instruction that is located in memory at the current Program counter to the instruction variable
        return 0;
    }
    else //a halt condition has occurred...return 0.
    {
        return 1;
    }
    //return 1 if there is a halt condition
    //return 0 if there is no halt condition
    //study when a halt condition occurs so we can assign correct return values
        //end of memory = 0xFFF = 65535. PC cannot be greater than 65535
        //Cannot access an address that is NOT word aligned. (address is not a multiple of 4)
}






/* instruction partition */
/* 10 Points */ //DONE
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{

    /*
    1. Partition instruction into several parts (op, r1, r2, r3, funct, offset, jsec).
    2. Read line 41 to 47 of spimcore.c for more information
    */
    /*
    thought process... move the least significant bit of the section of bits that I want to extract to the lowest order position of the 32-bit number,
    then AND that number with a binary number who has the as many 1's as the number of bits in the section that i want to extract.
    That way it only keeps the bits I want to keep, and then makes all the other bits equal to zero
    */

    //op always = first 6 bits
    *op = (instruction >> 26)&((1<<6)-1); //moves the first 6 bits to the lowest order position, then ANDS that value with 0011 1111. I believe that ANDing this with 11 1111 is unnecessary, however I figured I would keep it in just to be safe
    *r1 = (instruction >> 21)&((1<<5)-1); //same process as last step. ANDS with a number who's binary value has as many 1's as the size of the extracted number and 0's for the remaining portion.
    *r2 = (instruction >> 16)&((1<<5)-1);
    *r3 = (instruction >> 11)&((1<<5)-1);
    *funct = instruction&((1<<6)-1); //since funct is already in the lowest order position, no shift is necessary
    *offset = instruction&((1<<16)-1); //since the offset is already in the lowest order position, no shift is necessary
    *jsec = instruction&((1<<26)-1); //since jsec is already in the lowest order position, no shift is required

}












/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op,struct_controls *controls)
{
    /*
    1. Decode the instruction using the opcode (op).
    2. Assign the values of the control signals to the variables in the structure controls
    */

    /*
        NOTES
        opcode 000000 = R-type
        ADD REMAINING OPCODE NUMBERS
    */

    switch(op)
    {
        case 0: //R-type instruction
            controls->RegDst = 1;
            controls->ALUSrc = 0;
            controls->MemtoReg = 0;
            controls->RegWrite = 1;
            controls->MemRead = 0;
            controls->MemWrite = 0;
            controls->Branch = 0;
            controls->ALUOp = 7;
            controls->Jump = 0;
        break;
        /*
        case BRANCH:
            controls->RegDst = 1; //DONE
            controls->RegWrite = 0; //DONE
            controls->ALUScr = 0; //DONE
            controls->Branch = 1; //DONE (Question: does Branch = PCrc
            control->MemRead = 0; //DONE
            controls->MemWrite = 0; //DONE
            controls->MemtoReg = 0; //(double check) ???
            controls->Jump = 0; //DONE
            break;
        case SW:
            controls->RegDst = 1;
            controls->RegWrite = 0;
            controls->ALUScr = 1;
            controls->Branch = 0;
            controls->MemRead = 0;
            controls->MemWrite = 1;
            controls->MemtoReg = 0; //(double check)???
            controls->Jump = 0;
        break;
        case LW:
            controls->RegDst = 0;
            controls->RegWrite = 1;
            controls->ALUScr = 1;
            controls->Branch = 0;
            controls->MemRead = 1;
            controls->MemWrite = 0;
            controls->MemtoReg = 1;
            controls->Jump = 0;
        break;
        case JUMP:
            control->RegDst = 0;
            controls->RegWrite = 0;
            controls->ALUScr = 0;
            controls->Branch = 0;
            controls->MemRead = 0;
            controls->MemWrite = 0;
            controls->MemtoReg = 0;
            controls->Jump = 1;
        break;
        default:
            return 1;
        */
        //ADD REST OF INSTRUCTIONS

    }

}












/* Read Register */
/* 5 Points */ //DONE BUT DOUBLE CHECK
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{
    *data1 = Reg[r1];
    *data2 = Reg[r2];
}













/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset,unsigned *extended_value)
{

}












/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{
    //char operation;

    if(ALUSrc == 1) //I-Type instructions take the sign-extended value
    {
        ALU(data1, extended_value, ALUOp, ALUresult, Zero); //passes the sign extended value instead of data2. No further decoding is needed since the funct code is a Don't Care
    }
    else if(ALUSrc == 7) //R-Type intsructions take read data2, but we have to also pass in the correct ALUop since further decoding of the funct value is needed for R-types. I thought that R-types opcode was 000, bu the FAQ document said that it is 111 or 7)
    {
        switch(funct) //may be missing instructions
        {
            case 32: //100000...... ALU control input = (IDK if this is right 010) (add)
                ALUOp = 0;
            break;
            case 34: //100010...... ALU control input =  (IDK if this is right 110) (subtract)
                //subtract code here
                ALUOp = 1;
            break;
            case 36: //100100...... ALU control input =  (IDK if this is right 000) (AND)
                //AND code here
                ALUOp = 4;
            break;
            case 37: //100101...... ALU control input =  (IDK if this is right 001) (OR)
                //OR code here
                ALUOp = 6;
            break;
            case 42: //101010...... ALU control input =  (IDK if this is right 111) (set on less than)
                //set on less than code here
                ALUOp = 2;
            break;
            default:
                return 1;

        }
        ALU(data1, data2, ALUOp, ALUresult, Zero);
    }
}














/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{

}











/* Write Register */
/* 10 Points */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{

}











/* PC update */
/* 10 Points */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{
    *PC += 4;
    if(Branch == 1 && Zero == 1) //may change to (Branch == 1 && Zero)
    {
        *PC += (extended_value << 2);
    }
    else if(Jump == 1) //if jump is asserted, then jump to the target address (jsec)
    {
        //NEED TO FIGURE OUT HOW TO DO THIS
    }
}

