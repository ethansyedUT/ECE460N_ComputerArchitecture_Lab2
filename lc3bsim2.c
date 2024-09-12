/*
    Remove all unnecessary lines (including this one)
    in this comment.
    REFER TO THE SUBMISSION INSTRUCTION FOR DETAILS

    Name 1: Full name of the first partner
    Name 2: Full name of the second partner
    UTEID 1: UT EID of the first partner
    UTEID 2: UT EID of the second partner
*/

/***************************************************************/
/*                                                             */
/*   LC-3b Instruction Level Simulator                         */
/*                                                             */
/*   EE 460N                                                   */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************/
/*                                                             */
/* Files: isaprogram   LC-3b machine language program file     */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void process_instruction();

/***************************************************************/
/* A couple of useful definitions.                             */
/***************************************************************/
#define FALSE 0
#define TRUE 1

/***************************************************************/
/* Use this to avoid overflowing 16 bits on the bus.           */
/***************************************************************/
#define Low16bits(x) ((x)&0xFFFF)

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
   MEMORY[A][1] stores the most significant byte of word at word address A
*/

#define WORDS_IN_MEM 0x08000
int MEMORY[WORDS_IN_MEM][2];

/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
#define LC_3b_REGS 8

int RUN_BIT; /* run bit */

typedef struct System_Latches_Struct
{

    int PC,               /* program counter */
        N,                /* n condition bit */
        Z,                /* z condition bit */
        P;                /* p condition bit */
    int REGS[LC_3b_REGS]; /* register file. */
} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int INSTRUCTION_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands                    */
/*                                                             */
/***************************************************************/
void help()
{
    printf("----------------LC-3b ISIM Help-----------------------\n");
    printf("go               -  run program to completion         \n");
    printf("run n            -  execute program for n instructions\n");
    printf("mdump low high   -  dump memory from low to high      \n");
    printf("rdump            -  dump the register & bus values    \n");
    printf("?                -  display this help menu            \n");
    printf("quit             -  exit the program                  \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle()
{

    process_instruction();
    CURRENT_LATCHES = NEXT_LATCHES;
    INSTRUCTION_COUNT++;
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles                 */
/*                                                             */
/***************************************************************/
void run(int num_cycles)
{
    int i;

    if (RUN_BIT == FALSE)
    {
        printf("Can't simulate, Simulator is halted\n\n");
        return;
    }

    printf("Simulating for %d cycles...\n\n", num_cycles);
    for (i = 0; i < num_cycles; i++)
    {
        if (CURRENT_LATCHES.PC == 0x0000)
        {
            RUN_BIT = FALSE;
            printf("Simulator halted\n\n");
            break;
        }
        cycle();
    }
}

/***************************************************************/
/*                                                             */
/* Procedure : go                                              */
/*                                                             */
/* Purpose   : Simulate the LC-3b until HALTed                 */
/*                                                             */
/***************************************************************/
void go()
{
    if (RUN_BIT == FALSE)
    {
        printf("Can't simulate, Simulator is halted\n\n");
        return;
    }

    printf("Simulating...\n\n");
    while (CURRENT_LATCHES.PC != 0x0000)
        cycle();
    RUN_BIT = FALSE;
    printf("Simulator halted\n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : mdump                                           */
/*                                                             */
/* Purpose   : Dump a word-aligned region of memory to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void mdump(FILE *dumpsim_file, int start, int stop)
{
    int address; /* this is a byte address */

    printf("\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
    printf("-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
        printf("  0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    printf("\n");

    /* dump the memory contents into the dumpsim file */
    fprintf(dumpsim_file, "\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
    fprintf(dumpsim_file, "-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
        fprintf(dumpsim_file, " 0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    fprintf(dumpsim_file, "\n");
    fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : rdump                                           */
/*                                                             */
/* Purpose   : Dump current register and bus values to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void rdump(FILE *dumpsim_file)
{
    int k;

    printf("\nCurrent register/bus values :\n");
    printf("-------------------------------------\n");
    printf("Instruction Count : %d\n", INSTRUCTION_COUNT);
    printf("PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
    printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    printf("Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
        printf("%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
    printf("\n");

    /* dump the state information into the dumpsim file */
    fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
    fprintf(dumpsim_file, "-------------------------------------\n");
    fprintf(dumpsim_file, "Instruction Count : %d\n", INSTRUCTION_COUNT);
    fprintf(dumpsim_file, "PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
    fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    fprintf(dumpsim_file, "Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
        fprintf(dumpsim_file, "%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
    fprintf(dumpsim_file, "\n");
    fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : get_command                                     */
/*                                                             */
/* Purpose   : Read a command from standard input.             */
/*                                                             */
/***************************************************************/
void get_command(FILE *dumpsim_file)
{
    char buffer[20];
    int start, stop, cycles;

    printf("LC-3b-SIM> ");

    scanf("%s", buffer);
    printf("\n");

    switch (buffer[0])
    {
    case 'G':
    case 'g':
        go();
        break;

    case 'M':
    case 'm':
        scanf("%i %i", &start, &stop);
        mdump(dumpsim_file, start, stop);
        break;

    case '?':
        help();
        break;
    case 'Q':
    case 'q':
        printf("Bye.\n");
        exit(0);

    case 'R':
    case 'r':
        if (buffer[1] == 'd' || buffer[1] == 'D')
            rdump(dumpsim_file);
        else
        {
            scanf("%d", &cycles);
            run(cycles);
        }
        break;

    default:
        printf("Invalid Command\n");
        break;
    }
}

/***************************************************************/
/*                                                             */
/* Procedure : init_memory                                     */
/*                                                             */
/* Purpose   : Zero out the memory array                       */
/*                                                             */
/***************************************************************/
void init_memory()
{
    int i;

    for (i = 0; i < WORDS_IN_MEM; i++)
    {
        MEMORY[i][0] = 0;
        MEMORY[i][1] = 0;
    }
}

/**************************************************************/
/*                                                            */
/* Procedure : load_program                                   */
/*                                                            */
/* Purpose   : Load program and service routines into mem.    */
/*                                                            */
/**************************************************************/
void load_program(char *program_filename)
{
    FILE *prog;
    int ii, word, program_base;

    /* Open program file. */
    prog = fopen(program_filename, "r");
    if (prog == NULL)
    {
        printf("Error: Can't open program file %s\n", program_filename);
        exit(-1);
    }

    /* Read in the program. */
    if (fscanf(prog, "%x\n", &word) != EOF)
        program_base = word >> 1;
    else
    {
        printf("Error: Program file is empty\n");
        exit(-1);
    }

    ii = 0;
    while (fscanf(prog, "%x\n", &word) != EOF)
    {
        /* Make sure it fits. */
        if (program_base + ii >= WORDS_IN_MEM)
        {
            printf("Error: Program file %s is too long to fit in memory. %x\n",
                   program_filename, ii);
            exit(-1);
        }

        /* Write the word to memory array. */
        MEMORY[program_base + ii][0] = word & 0x00FF;
        MEMORY[program_base + ii][1] = (word >> 8) & 0x00FF;
        ii++;
    }

    if (CURRENT_LATCHES.PC == 0)
        CURRENT_LATCHES.PC = (program_base << 1);

    printf("Read %d words from program into memory.\n\n", ii);
}

/************************************************************/
/*                                                          */
/* Procedure : initialize                                   */
/*                                                          */
/* Purpose   : Load machine language program                */
/*             and set up initial state of the machine.     */
/*                                                          */
/************************************************************/
void initialize(char *program_filename, int num_prog_files)
{
    int i;

    init_memory();
    for (i = 0; i < num_prog_files; i++)
    {
        load_program(program_filename);
        while (*program_filename++ != '\0')
            ;
    }
    CURRENT_LATCHES.Z = 1;
    NEXT_LATCHES = CURRENT_LATCHES;

    RUN_BIT = TRUE;
}

/***************************************************************/
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/***************************************************************/
int main(int argc, char *argv[])
{
    FILE *dumpsim_file;

    /* Error Checking */
    if (argc < 2)
    {
        printf("Error: usage: %s <program_file_1> <program_file_2> ...\n",
               argv[0]);
        exit(1);
    }

    printf("LC-3b Simulator\n\n");

    initialize(argv[1], argc - 1);

    if ((dumpsim_file = fopen("dumpsim", "w")) == NULL)
    {
        printf("Error: Can't open dumpsim file\n");
        exit(-1);
    }

    while (1)
        get_command(dumpsim_file);
}

/***************************************************************/
/* Do not modify the above code.
   You are allowed to use the following global variables in your
   code. These are defined above.

   MEMORY

   CURRENT_LATCHES
   NEXT_LATCHES

   You may define your own local/global variables and functions.
   You may use the functions to get at the control bits defined
   above.
   Begin your code here 	  			       */

/***************************************************************/

#define opCode(x) ((x & 0xF0) >> 4)
#define destReg(MSB) ((MSB & 0x0E) >> 1) // given MSB, returns destination reg
#define src1BaseReg(MSB, LSB) (((MSB & 0x1) << 2) + ((LSB & 0xC0) >> 6))
#define low8Bits(x) ((x)&0xFF)
#define high8Bits(x) (((x)&0xFF00) >> 8)

void clearNZP();
void setNZP(int value);
int SEXT(int value, int bits);
void printBinary(int value);

enum opcodes
{
    add = 0x01,     // 1
        and = 0x05, // 5
    br = 0x00,      // 0
    brn = 0x00,     // 0
    brz = 0x00,     // 0
    brp = 0x00,     // 0
    brzp = 0x00,    // 0
    brnz = 0x00,    // 0
    brnp = 0x00,    // 0
    brnzp = 0x00,   // 0
    halt = 0x0F,    // 15
    jmp = 0x0C,     // 12
    jsr = 0x04,     // 4
    jsrr = 0x04,    // 4
    ldb = 0x02,     // 2
    ldw = 0x06,     // 6
    lea = 0x0E,     // 14
    nop = 0x00,     // 0
    not = 0x09,     // 9
    ret = 0x0C,     // 12
    lshf = 0x0D,    // 13
    rshfl = 0x0D,   // 13
    rshfa = 0x0D,   // 13
    rti = 0x08,     // 8
    stb = 0x03,     // 3
    stw = 0x07,     // 7
    trap = 0x0F,    // 15
    xor = 0x09      // 9
};
// watch out for xor and brnzp

void process_instruction()
{
    /*  function: process_instruction
     *
     *    Process one instruction at a time
     *       -Fetch one instruction
     *       -Decode
     *       -Execute
     *       -Update NEXT_LATCHES
     */
    int insLSB = Low16bits(MEMORY[CURRENT_LATCHES.PC >> 1][0]);
    int insMSB = Low16bits(MEMORY[CURRENT_LATCHES.PC >> 1][1]);
    int opcode = opCode(insMSB);

    int ins = insMSB << 8;
    ins += insLSB;

    // printf("ins: %x\n", ins);
    // printf("opcode: %x\n", opcode);
    // printf("lsb: %x\n", insLSB);

    // printf("\nThis is what the pc is at %d\n", CURRENT_LATCHES.PC);

    // printf("\nThis is opcode: %d\n", opcode);
    // printf("This is opcode hex: %X\n", opcode);

    NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;

    if (opcode == add) // reads from reg file and sets NZP bits
    {
        int dr = destReg(insMSB);
        int src1 = src1BaseReg(insMSB, insLSB);

        int result;

        // add w/ 2 reg
        if (((insLSB & 0x20) >> 5) == 0)
        {
            int src2 = insLSB & 0x7; // last 3 bits
            // reading vals of regs
            result = CURRENT_LATCHES.REGS[src1] + CURRENT_LATCHES.REGS[src2];
        }
        else
        {
            // add w/ 1 reg 1 immediate 5 bit
            int immed5 = insLSB & 0x1F;
            immed5 = SEXT(immed5, 5);
            result = CURRENT_LATCHES.REGS[src1] + immed5;
        }
        NEXT_LATCHES.REGS[dr] = Low16bits(result);
        setNZP(result);
    }
    else if (opcode == and) // reads from regfile and sets NZP bits
    {
        int dr = destReg(insMSB);
        int src1 = src1BaseReg(insMSB, insLSB);
        int result;

        // and
        if (((insLSB & 0x20) >> 5) == 0)
        {
            int src2 = insLSB & 0x7;
            result = CURRENT_LATCHES.REGS[src1] & CURRENT_LATCHES.REGS[src2];
        }
        else
        {
            int immed5 = insLSB & 0x1F;
            immed5 = SEXT(immed5, 5);
            result = CURRENT_LATCHES.REGS[src1] & immed5;
        }
        NEXT_LATCHES.REGS[dr] = Low16bits(result);
        setNZP(result);
    }
    else if (opcode == ldb)
    {
        int dr = destReg(insMSB);
        int baseReg = src1BaseReg(insMSB, insLSB);
        int offset6 = insLSB & 0x3F;
        offset6 = SEXT(offset6, 6);

        int addr = (CURRENT_LATCHES.REGS[baseReg] + offset6) / 2;

        setNZP(SEXT(MEMORY[addr][0], 8));
        NEXT_LATCHES.REGS[dr] = SEXT(MEMORY[addr][0], 8);
    }
    else if (opcode == ldw)
    {
        int dr = destReg(insMSB);
        int baseReg = src1BaseReg(insMSB, insLSB);
        int offset6 = insLSB & 0x3F;
        offset6 = (SEXT(offset6, 6) << 1);

        int addr = (CURRENT_LATCHES.REGS[baseReg] + offset6) / 2;

        int memVal = (low8Bits(MEMORY[addr][1]) << 8) + low8Bits(MEMORY[addr][0]);

        NEXT_LATCHES.REGS[dr] = Low16bits(memVal);
        setNZP(memVal);
    }
    else if (opcode == lea)
    {
        int DR = destReg(insMSB);
        int PCoffset9 = ((low8Bits(insMSB) & 0x1) << 8) + low8Bits(insLSB);
        PCoffset9 = (SEXT(PCoffset9, 9) << 1);
        // printf("THIS IS OFFSET %x\n", PCoffset9);

        NEXT_LATCHES.REGS[DR] = Low16bits(NEXT_LATCHES.PC + PCoffset9);
        // should I use current latches or next latches
        // I suspect NEXT latches
    }
    else if (opcode == stb)
    {
        int SR = destReg(insMSB);
        int BR = src1BaseReg(insMSB, insLSB);
        int boffset6 = insLSB & 0x3F;
        boffset6 = SEXT(boffset6, 6);

        int addr = ((CURRENT_LATCHES.REGS[BR] + boffset6)) / 2;
        printf("This addr to store %x\n", addr);
        MEMORY[addr][0] = low8Bits(CURRENT_LATCHES.REGS[SR]);
        MEMORY[addr][1] = 0;
        // what should I do with the 8 high bits ???
    }
    else if (opcode == stw)
    {
        int SR = destReg(insMSB);
        int BR = src1BaseReg(insMSB, insLSB);
        int boffset6 = insLSB & 0x3F;
        boffset6 = (SEXT(boffset6, 6) << 1);

        int addr = (CURRENT_LATCHES.REGS[BR] + boffset6) / 2;

        MEMORY[addr][0] = low8Bits(CURRENT_LATCHES.REGS[SR]);
        MEMORY[addr][1] = high8Bits(CURRENT_LATCHES.REGS[SR]);
    }
    else if (opcode == jsr || opcode == jsrr)
    {
        int BR = src1BaseReg(insMSB, insLSB);

        NEXT_LATCHES.REGS[7] = NEXT_LATCHES.PC;
        if (((insMSB & (1 << 3)) >> 3) == 1)
        {
            int PCoffset11 = ((insMSB & 0x7) << 8) + insLSB;
            PCoffset11 = (SEXT(PCoffset11, 11) << 1);
            NEXT_LATCHES.PC = NEXT_LATCHES.PC + PCoffset11;
        }
        else
        {
            NEXT_LATCHES.PC = CURRENT_LATCHES.REGS[BR];
        }
    }
    else if (opcode == not || opcode == xor)
    {
        int DR = destReg(insMSB);
        int SR = src1BaseReg(insMSB, insLSB);
        int val;

        if ((((insLSB & (1 << 5)) >> 5) == 0))
        {
            int src2 = insLSB & 0x7;
            val = CURRENT_LATCHES.REGS[SR] ^ CURRENT_LATCHES.REGS[src2];
        }
        else
        {
            int imm5 = insLSB & 0x1F;
            imm5 = SEXT(imm5, 5);
            printBinary(imm5);
            val = CURRENT_LATCHES.REGS[SR] ^ imm5;
        }
        NEXT_LATCHES.REGS[DR] = Low16bits(val);
        setNZP(val);
    }
    else if (opcode == jmp || opcode == ret)
    {
        int BR = src1BaseReg(insMSB, insLSB);
        NEXT_LATCHES.PC = CURRENT_LATCHES.REGS[BR];
    }
    else if (opcode == lshf || opcode == rshfl || opcode == rshfa)
    {
        int DR = destReg(insMSB);
        int SR = src1BaseReg(insMSB, insLSB);

        int val;
        int shiftType = (insLSB & 0x30) >> 4;
        // do I have to worry abt neg nums
        if (shiftType == 0)
        {
            // lshf
            val = Low16bits(CURRENT_LATCHES.REGS[SR] << (insLSB & 0xF));
            NEXT_LATCHES.REGS[DR] = val;
        }
        else if (shiftType == 1)
        {
            val = Low16bits(CURRENT_LATCHES.REGS[SR] >> (insLSB & 0xF));
            NEXT_LATCHES.REGS[DR] = val;
        }
        else
        {
            int rshfAmm = insLSB & 0xF;
            int retainer = ((CURRENT_LATCHES.REGS[SR] & (1 << 15)) >> 15);

            val = Low16bits(CURRENT_LATCHES.REGS[SR] >> (insLSB & 0xF));

            int mask = ~(0 << rshfAmm);
            // check might throw errors
            mask = mask << (15 - rshfAmm);

            if (retainer == 1)
            {
                // fill leading bits w/ 1
                val = val | mask;
            }
            else
            {
                // fill w/ 0
                mask = ~mask;
                val = mask & val;
            }
            NEXT_LATCHES.REGS[DR] = Low16bits(val);
        }
        setNZP(val);
    }
    else if (opcode == nop || opcode == br)
    {
        int PCoffset9 = ((low8Bits(insMSB) & 0x1) << 8) + low8Bits(insLSB);
        PCoffset9 = (SEXT(PCoffset9, 9) << 1);

        //  br, brn, brz, brp, brzp, brnz, brnp, brnzp
        if (((((insMSB & 0x8) >> 3) == 1) && CURRENT_LATCHES.N) == 1)
        {
            NEXT_LATCHES.PC = Low16bits(NEXT_LATCHES.PC + PCoffset9);
        }
        else if (((((insMSB & 0x4) >> 2) == 1) && CURRENT_LATCHES.Z) == 1)
        {
            NEXT_LATCHES.PC = Low16bits(NEXT_LATCHES.PC + PCoffset9);
        }
        else if (((((insMSB & 0x2) >> 1) == 1) && CURRENT_LATCHES.P) == 1)
        {
            NEXT_LATCHES.PC = Low16bits(NEXT_LATCHES.PC + PCoffset9);
        }
        // printf("CL PC: %x\n", CURRENT_LATCHES.PC);
        // printf("N NL PC: %x\n", NEXT_LATCHES.PC);
        // printf("offset: %x\n", PCoffset9);
    }
    else if (opcode == halt || opcode == trap)
    {
        NEXT_LATCHES.REGS[7] = NEXT_LATCHES.PC;
        if ((insLSB & 0xFF) == 0x25)
        {
            NEXT_LATCHES.PC = 0;
        }
        else
        {
            int addr = insLSB;
            addr = low8Bits(addr);
            addr = ((Low16bits(addr)) << 1) / 2;
            NEXT_LATCHES.PC = ((MEMORY[addr][1]) << 8) + MEMORY[addr][0];
        }
    }
}

void clearNZP()
{
    NEXT_LATCHES.N = 0;
    NEXT_LATCHES.Z = 0;
    NEXT_LATCHES.P = 0;
}

void setNZP(int value)
{
    NEXT_LATCHES.N = 0;
    NEXT_LATCHES.Z = 0;
    NEXT_LATCHES.P = 0;

    int sign = ((Low16bits(value) & (1 << 15)) >> 15);
    if ((value & 0xFFFF) == 0)
    {
        NEXT_LATCHES.Z = 1;
    }
    else if (sign == 1)
    {
        NEXT_LATCHES.N = 1;
    }
    else if (sign == 0)
    {
        NEXT_LATCHES.P = 1;
    }
}

// sign extends value with bits number of bits to 16 bits
int SEXT(int value, int bits)
{
    int store = value;
    int sign = ((store & (1 << (bits - 1))) >> (bits - 1));

    int mask = ~(0 << (15 - bits));
    // check might throw errors
    mask = mask << bits;

    if (sign == 1)
    {
        // neg
        store = mask | store;
    }
    else
    {
        // pos
        mask = ~mask;
        store = mask & store;
    }
    store = Low16bits(store);
    return store;
}

// debugging methods
void printBinary(int n)
{
    for (int i = 31; i >= 0; i--)
    {
        printf("%d", (n >> i) & 1);
    }
    printf("\n");
}