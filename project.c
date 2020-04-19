//Written by Darrell Hill and Daniel Cruz
//CDA3103 - Sarah Angell

#include "spimcore.h"



void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{

    /*
        ALU(…)
            1. Implement the operations on input parameters A and B according to ALUControl.
            2. Output the result (Z) to ALUresult.
            3. Assign Zero to 1 if the result is zero; otherwise, assign 0.
            4. The following table shows the operations of the ALU.
                000 -> Z = A + B
                001 -> Z = A - B
                010 -> if A<B, Z=1; otherwise, Z=0
                011 -> if A<B, Z=1; otherwise, Z=0 (A and B are unsigned integers)
                100 -> Z = A AND B
                101 -> Z = A OR B
                110 -> Shift left B by 16 bits
                111 -> Z = NOT A
    */

    //Set the test condition for the ALUControl based on the table given for the ALU function in the ProjectGuidelines
    switch (ALUControl){

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
            *ALUresult = (A & B);
            break;

        case 5: //ALUControl = 101 (OR)
            *ALUresult = (A | B);
            break;

        case 6: //ALUControl = 110 (Shift left B by 16 bits)
            *ALUresult = (B << 16);
            break;

        case 7: //ALUControl = 111 (NOT A)
            *ALUresult = (!A);
            break;

    }

    if(*ALUresult == 0)
        *Zero = 1;
    else
        *Zero = 0;

}


int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
    /*
        instruction_fetch(…)
            1. Fetch the instruction addressed by PC from Mem and write it to instruction.
            2. Return 1 if a halt condition occurs; otherwise, return 0.
    */

    if(PC % 4 == 0){ //makes sure that PC is word aligned (a multiple of 4)
        *instruction = Mem[(PC >> 2)]; //Go to (PC >> 2) and assign the value to the instruction field
        return 0;
    }
    else
        return 1; //Not word aligned, return 0
}


void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec) //////////////////////I got confused here so I havent touched this at all lmao
{

/*
    instruction_partition(…)
        1. Partition instruction into several parts (op, r1, r2, r3, funct, offset, jsec).
        2. Read line 41 to 47 of spimcore.c for more information.
*/

/*
    =========================================thought process...=============================================
    1. shift the lowest order bit of the section of bits that we want to the lowest order position of the 32 bit number
    2. AND that number with (1 shifted by the number of bits that we want to extract)-1)
    ....this allows us to keep the bits that we want, and turn the remaining bits in the 32 bit number to 0
*/


    *op = (instruction >> 26)&((1<<6)-1); //Moves the first 6 bits to the lowest order position, then ANDS that value with 0011 1111. I believe that ANDing this with 11 1111 is unnecessary, however I figured I would keep it in just to be safe
    *r1 = (instruction >> 21)&((1<<5)-1); //Same process as last step. ANDS with a number who's binary value has as many 1's as the size of the extracted number and 0's for the remaining portion.
    *r2 = (instruction >> 16)&((1<<5)-1);
    *r3 = (instruction >> 11)&((1<<5)-1);
    *funct = instruction&((1<<6)-1); //Since funct is already in the lowest order position, no shift is necessary
    *offset = instruction&((1<<16)-1); //Since the offset is already in the lowest order position, no shift is necessary
    *jsec = instruction&((1<<26)-1); //Since jsec is already in the lowest order position, no shift is required

}




int instruction_decode(unsigned op,struct_controls *controls)
{

    /*
    instruction_decode(…)
        1. Decode the instruction using the opcode (op).
        2. Assign the values of the control signals to the variables in the structure controls
        (See spimcore.h file).
        The meanings of the values of the control signals:
        For MemRead, MemWrite or RegWrite, the value 1 means that enabled, 0 means
        that disabled, 2 means “don’t care”.
        For RegDst, Jump, Branch, MemtoReg or ALUSrc, the value 0 or 1 indicates the
        selected path of the multiplexer; 2 means “don’t care”.
        3. Return 1 if a halt condition occurs; otherwise, return 0.
    */


    /*
        NOTES FROM RECITATION SLIDES
            1. RegDst
                    Asserted when the register destination number for the write register comes from the rd field
                    Deasserted when the register destination number for the write register comes from the rt field
            2. RegWrite
                    Asserted when the register destination on the write register input is written with the value on the write data input
            3. ALUSrc
                    Asserted when the second ALU operand is the sign-extended lower 16 bits of the instruction
                    Deasserted when the second ALU operand come from the second register file output
            4. MemRead
                    Asserted when data memory contents designated by the address input are put on the read data output
            5. MemWrite
                    Asserted when the data memory contents designated by the address input are replaced by the value on the write data input
            6. MemtoReg
                    Asserted when the value fed into the register write data input comes form the data memory
                    Deasserted when the value fed to the register write data input comes from the ALU
            7.  Branch on Equal
                    Asserted means check the zero output of the ALU. If zero is asserted use the branch target address
                    Deasserted means use PC = PC+4
            8. Jump
                Asserted use PC = PC+4[31-280:instruction[25-0]*4
                Deasserted means that PC is either PC+4 or the branch target address
            9. ALUOp
    */

    /*
        NOTES
            1.ALUsrc is a control signal to a multiplexer, which chooses between a sign extended_value,
            or data2 in order to send the outcome to the ALU for operation.
            2. A control signal is a “don’t care” for an instruction, if its value has no effect on the
            correct operation of datapath for that instruction. For instance, the ALUOp signals have no
            impact on the correct operation of the datapath for the jump instruction. Also, since we will not write to a destination register, the value of RegDst would be irrelevant for jump.
    */


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

        case 2: //op = 000010 (jump)
            controls->RegDst = 2;
            controls->RegWrite = 2;
            controls->ALUSrc = 2;
            controls->MemRead = 2;
            controls->MemWrite = 2;
            controls->MemtoReg = 2;
            controls->Branch = 2;
            controls->Jump = 1;
            controls->ALUOp = 2;
            break;

        case 4: //op = 000100 (branch equal)
            controls->RegDst = 2; //was 0
            controls->RegWrite = 0;
            controls->ALUSrc = 0;
            controls->MemRead = 0;
            controls->MemWrite = 0;
            controls->MemtoReg = 2; //was 0
            controls->Branch = 1;
            controls->Jump = 0;
            controls->ALUOp = 1;
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

        case 10: //op = 001010 (set on less than immediate)
            controls->RegDst = 0;
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
            controls->RegDst = 0;
            controls->RegWrite = 1;
            controls->ALUSrc = 1;
            controls->MemRead = 0;
            controls->MemWrite = 0;
            controls->MemtoReg = 0;
            controls->Branch = 0;
            controls->Jump = 0;
            controls->ALUOp = 3;
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
            controls->RegDst = 0; //may change to 2 since it is a DC
            controls->RegWrite = 0;
            controls->ALUSrc = 1;
            controls->MemRead = 0;
            controls->MemWrite = 1;
            controls->MemtoReg = 0; //may change to 2 since it is a DC
            controls->Branch = 0;
            controls->Jump = 0;
            controls->ALUOp = 0;
            break;

        default: // return 1 if halt
            return 1;
    }

    return 0;
}



void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{
    /*
        read_register(…)
            1. Read the registers addressed by r1 and r2 from Reg, and write the read values to
            data1 and data2 respectively
    */

    *data1 = Reg[r1];
    *data2 = Reg[r2];
}



void sign_extend(unsigned offset,unsigned *extended_value)
{
    /*
        sign_extend(…)
            1. Assign the sign-extended value of offset to extended_value.
    */

    /*
        ==================================================thought process...==================================================
        step 1. extract the sign bit (MSB)
        step 2. if it is a 1, then OR that value with a number that has all 0's in the lower order 16 bits, and all 1's in the upper order 16 bits.
        If is is 0, then OR that value with a number that value with a 32 bit representation of 0
        this allows us to keep the original bits, while copying the MSB 16 times in the upper half.
    */
    unsigned MSB = (offset >> 15); //extracts the sign bit and assigns the value to MSB

    if(MSB == 1)
        *extended_value = (offset|4294901760);
    else
        *extended_value = (offset|0);

}



int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{
  /*
    ALU_operations(…)
        1. Apply ALU operations on data1, and data2 or extended_value (determined by ALUSrc).
        2. The operation performed is based on ALUOp and funct.
        3. Apply the function ALU(…).
        4. Output the result to ALUresult.
        5. Return 1 if a halt condition occurs; otherwise, return 0.
  */

  /*
    NOTES
    1. When the ALUOp control signal is 7 or 111, this tells the ALU control that it needs
    to figure out what operation to tell the ALU to do by looking at the funct field (bits[5-0])of the instruction. This is called "multiple levels of decoding".
  */

    if(ALUSrc == 1) //If ALUSrc = 1, then we do not need to decode the funct field
        data2 = extended_value;

    //Got funct values from slide 5 of the MIPS reference power point
    if(ALUOp == 7){ //If ALUOp = 7, then it is an R-type, and we must decode the funct field to determine the signal to be sent as the ALUControl

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

            case 43: //funct = 101011 (set l.t. unsigned)
                ALUOp = 3;
                break;

            case 36: //funct = 100100 (AND)
                ALUOp = 4;
                break;

            case 37: //funct = 100101 (OR)
                ALUOp = 5;
                break;

            case 6: //funct = (shift left) //make sure this is right
                ALUOp = 6;
                break;

            case 39: //funct = 100111 (NOT)
                ALUOp = 7;
                break;

            default: //returns 1 if halt
                return 1;
            }
            ALU(data1,data2,ALUOp,ALUresult,Zero); //calls the ALU function with the correct ALUControl signal that was decoded from the funct code

        }
        else
            ALU(data1,data2,ALUOp,ALUresult,Zero); //takes care of other instruction that set ALUSrc to 0, but are not R-Types (i.e. Branch)

        return 0;
}



int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{

    /*
        rw_memory(…)
            1. Base on the value of MemWrite or MemRead to determine memory write operation or memory read operation.
            2. Read the content of the memory location addressed by ALUresult to memdata.
            3. Write the value of data2 to the memory location addressed by ALUresult.
            4. Return 1 if a halt condition occurs; otherwise, return 0.
    */



    /*
        NOTES: only halt when ALUresult is an address. ALUresult should be an address if MemRead or MemWrite is asserted.
        For instance, on an I-type command, the ALUresult would hold a memory address.
    */

    if(MemRead) {
        if(ALUresult % 4 == 0) //ALUresult is an address, so it checks to make sure it is word aligned (a multiple of 4)
            *memdata = Mem[(ALUresult >> 2)];
        else
            return 1; //halt condition, ALUresult is not word aligned
    }

    if(MemWrite) {
        if(ALUresult % 4 == 0) //ALUresult is an address, so it checks to make sure it is word aligned (a multiple of 4)
            Mem[ALUresult >> 2] = data2;
        else
            return 1; //halt condition, ALUresult is not word aligned
    }

    return 0;

}



void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{

    /*
        write_register(…)
            1. Write the data (ALUresult or memdata) to a register (Reg) addressed by r2 or r3.
    */

    /*
        NOTES
            1. if RegWrite = 1, then we write to a register
            2. if MemtoReg = 1, the value fed to the register write data input comes form memdata
            3. if MemtoReg = 0, the value fed to the register write data input comes from ALUresult
            4. RegDst determines which register to use. r2 or r3
            5. If RegDst = 0, the register destination number for the write register comes from the rt field
            6. If RegDst = 1, the register destination number for the write register comes for the rd field
    */

    if(RegWrite == 1)
    {
        if(MemtoReg == 1)
        {
            if(RegDst == 0)
            {
                Reg[r2] = memdata;
            }
            else
            {
                Reg[r3] = memdata;
            }
        }
        else if(MemtoReg == 0)
        {
            if(RegDst == 0)
            {
                Reg[r2] = ALUresult;
            }
            else
            {
                Reg[r3] = ALUresult;
            }
        }
    }
}



void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{

    /*
        PC_update(…)
            1. Update the program counter (PC).
    */

    /*
        NOTES
            1. When Jump = 1, use PC = PC + 4[31:28]:instruction[25-0]*4
            2. If Branch = 1 and Zero = 1, use PC = PC+4+4*instruction[15-0]
            3. default condition = add 4 to PC counter
    */

    /*
        ==================================================thought process...==================================================
            1. start by incrementing the PC counter by 4. This takes care of the default condition, while shortening the calculates for the address if it encounters a jump or branch
            2. check to see if the branch condition is met (Branch control signal = 1 and Zero control signal = 1). If so then add the extended_value*4 to the program counter
            3. check to see if the jump condition is met (Jump control signal = 1). If so, AND the PC counter with a 32 bit number that has 1's for its 4 highest order bits (4026531840) and concatenate
            that number with the jsec (instruction[15-0]) multiplied by 4
    */


    *PC += 4;

    if(Branch == 1 && Zero == 1)
        *PC += (extended_value << 2);

    if(Jump == 1)
        *PC = (*PC&4026531840) | (jsec<<2);
}
