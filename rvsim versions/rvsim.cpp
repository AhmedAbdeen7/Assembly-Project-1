/*
    This is just a skeleton. It DOES NOT implement all the requirements.
    It only recognizes the RV32I "ADD", "SUB" and "ADDI" instructions only.
    It prints "Unkown Instruction" for all other instructions!

    References:
    (1) The risc-v ISA Manual ver. 2.1 @ https://riscv.org/specifications/
    (2) https://github.com/michaeljclark/riscv-meta/blob/master/meta/opcodes
    (3) https://www.cs.sfu.ca/~ashriram/Courses/CS295/assets/notebooks/RISCV/RISCV_CARD.pdf
*/

#include <iostream>
#include <fstream>
#include "stdlib.h"
#include <iomanip>

using namespace std;

unsigned int pc;
unsigned int reg[32] = {0};
string abiName[32] = {"zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2", "s0", "s1",
                      "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7", "s2", "s3", "s4", "s5",
                      "s6", "s7", "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"};
unsigned char memory[(16 + 64) * 1024];
void instDecExec(unsigned int instWord);

void emitError(char *s)
{
    cout << s;
    exit(0);
}

void printPrefix(unsigned int instA, unsigned int instW)
{
    cout << "0x" << hex << std::setfill('0') << std::setw(8) << instA << "\t0x" << std::setw(8) << instW;
}
bool isCompressed(unsigned int instWord)
{
    unsigned int opcode = instWord & 0x00000003;
    if (opcode == 0x3)
    {
        return false;
    }
    return true;
}
unsigned int deCompress(unsigned int instWord)
{
    // if (!isCompressed(instWord))
    // {
    // 	return;
    // }
    // instWord = 0x00000114;
    int decompressed_instruction = 0;
    unsigned int rd, rs1, rs2, funct4, funct3, offset, op;
    unsigned int CIW_imm, CL_imm, CI_imm;

    op = instWord & 0x00000003;
    funct3 = (instWord >> 13) & 0x00000007;
    funct4 = (instWord >> 12) & 0x0000000F;
    offset = (instWord >> 2) & 0x000007FF;
    if (op == 0x0)
    { // CL-CS-CIW Formats
        switch (funct3)
        {
        case 0:
            // CIW - Format
            rd = (instWord >> 2) & 0x00000007;
            CIW_imm = (instWord >> 5) & 0x000000FF;
            CIW_imm = (((CIW_imm & 0x02) >> 1) | ((CIW_imm & 0x01) << 1) | ((CIW_imm & 0x0C) << 4) | ((CIW_imm & 0x0F0) >> 2)) << 2;
            instWord = 0x00000013; // I-type
            rd = rd << 7;
            instWord |= rd;
            instWord |= 0x00000000;  // ADDI
            instWord |= 0x00010000;  // sp ~x2
            CIW_imm = CIW_imm << 20; // place imm[0] at bit 20
            instWord |= CIW_imm;
            instDecExec(instWord);
            break;
        case 2:
            // CL - Format
            rd = (instWord >> 2) & 0x00000007;
            instWord = 0x00000003; // I-Type LOAD
            rd = rd << 7;
            instWord |= rd;
            // Rest TBC

            break;
        case 6:
            break;

        default:
            break;
        }
    }
    else if (op == 0x2)
    { // CI -Format
        switch (funct3)
        {
        case 0:
            // slli instruction
            rd = (instWord >> 7) & 0x0000001F;
            rs1 = (instWord >> 7) & 0x0000001F;
            CI_imm = (instWord >> 2) & 0x0000001F;
            CI_imm = CI_imm | ((instWord >> 7) & 0x0020);
            if (CI_imm >> 5) // Sign Extension
                CI_imm = CI_imm | 0xFC0;
            else
                CI_imm = CI_imm | 0x0;
            op = 0x13;
            funct3 = 0x0;
            decompressed_instruction = (CI_imm << 20);
            decompressed_instruction = decompressed_instruction | (rs1 << 15);
            decompressed_instruction = decompressed_instruction | (rd << 7);
            decompressed_instruction = decompressed_instruction | op;
            decompressed_instruction = decompressed_instruction | (funct3 >> 12);
            return decompressed_instruction;
            break;

        case 2:
            rd = (instWord >> 7) & 0x0000001F;
            rs1 = 00010;
            CI_imm = (instWord >> 2) & 0x0000001F;
            CI_imm = CI_imm | ((instWord >> 7) & 0x0020);
            if (CI_imm >> 5) // Sign Extension
                CI_imm = CI_imm | 0xFC0;
            else
                CI_imm = CI_imm | 0x0;
            op = 0x3;
            funct3 = 0x2;
            decompressed_instruction = (CI_imm << 20);
            decompressed_instruction = decompressed_instruction | (rs1 << 15);
            decompressed_instruction = decompressed_instruction | (rd << 7);
            decompressed_instruction = decompressed_instruction | op;
            decompressed_instruction = decompressed_instruction | (funct3 >> 12);
            return decompressed_instruction;
            break;
        }
    }
    else if (op == 0x1)
    {
        switch (funct3)
        {
        case 0:
            if (rd == 0)
            { // C.NOP

                decompressed_instruction = 0x00000013;
                return decompressed_instruction;
            }
            else
            { // C.ADDI
                rd = (instWord >> 7) & 0x0000001F;
                rs1 = (instWord >> 7) & 0x0000001F;
                CI_imm = (instWord >> 2) & 0x0000001F;
                CI_imm = CI_imm | ((instWord >> 7) & 0x0020);
                if (CI_imm >> 5) // Sign Extension
                    CI_imm = CI_imm | 0xFC0;
                else
                    CI_imm = CI_imm | 0x0;
                op = 0x13;
                funct3 = 0x2;
                decompressed_instruction = (CI_imm << 20);
                decompressed_instruction = decompressed_instruction | (rs1 << 15);
                decompressed_instruction = decompressed_instruction | (rd << 7);
                decompressed_instruction = decompressed_instruction | op;
                decompressed_instruction = decompressed_instruction | (funct3 >> 12);
                return decompressed_instruction;
            }
            break;

        case 2:
            rd = (instWord >> 7) & 0x0000001F;
            rs1 = 0x0;
            CI_imm = (instWord >> 2) & 0x0000001F;
            CI_imm = CI_imm | ((instWord >> 7) & 0x0020);
            if (CI_imm >> 5) // Sign Extension
                CI_imm = CI_imm | 0xFC0;
            else
                CI_imm = CI_imm | 0x0;
            op = 0x13;
            funct3 = 0x2;
            decompressed_instruction = (CI_imm << 20);
            decompressed_instruction = decompressed_instruction | (rs1 << 15);
            decompressed_instruction = decompressed_instruction | (rd << 7);
            decompressed_instruction = decompressed_instruction | op;
            decompressed_instruction = decompressed_instruction | (funct3 >> 12);
            return decompressed_instruction;
            break;
        case 4:
                }

        return instWord;
    }

    void instDecExec(unsigned int instWord)
    {
        unsigned int rd, rs1, rs2, shamt, funct3, funct7, opcode;
        unsigned int I_imm, S_imm, B_imm, U_imm, J_imm;
        unsigned int address;

        unsigned int instPC = pc - 4;

        opcode = instWord & 0x0000007F;
        rd = (instWord >> 7) & 0x0000001F;
        funct3 = (instWord >> 12) & 0x00000007;
        rs1 = (instWord >> 15) & 0x0000001F;
        rs2 = (instWord >> 20) & 0x0000001F;
        funct7 = (instWord >> 25) & 0x0000007F;
        shamt = (instWord >> 20) & 0x0000001F;
        // — inst[31] — inst[30:25] inst[24:21] inst[20]
        I_imm = ((instWord >> 20) & 0x7FF) | (((instWord >> 31) ? 0xFFFFF800 : 0x0));
        // inst

        printPrefix(instPC, instWord);

        if (opcode == 0x33)
        { // R Instructions
            switch (funct3)
            {
            case 0:
                if (funct7 == 32)
                {
                    cout << "\tSUB\t" << abiName[rd] << ", " << abiName[rs1] << ", " << abiName[rs2] << "\n";
                    reg[rd] = reg[rs1] - reg[rs2];
                }
                else if (funct7 == 0)
                {
                    cout << "\tADD\t" << abiName[rd] << ", " << abiName[rs1] << ", " << abiName[rs2] << "\n";
                    reg[rd] = reg[rs1] + reg[rs2];
                }
                else
                {
                    cout << "\tUnkown R Instruction \n";
                }
                break;
            case 1:
                cout << "\tSLL\t" << abiName[rd] << ", " << abiName[rs1] << ", " << abiName[rs2] << "\n";
                reg[rd] = reg[rs1] << reg[rs2];
                break;
            case 2:
                cout << "\tSLT\t" << abiName[rd] << ", " << abiName[rs1] << ", " << abiName[rs2] << "\n";
                reg[rd] = (int(reg[rs1]) < int(reg[rs2])) ? 1 : 0;
                break;
            case 3:
                cout << "\tSLTU\t" << abiName[rd] << ", " << abiName[rs1] << ", " << abiName[rs2] << "\n";
                reg[rd] = (reg[rs1] < reg[rs2]) ? 1 : 0;
                break;
            case 4:
                cout << "\tXOR\t" << abiName[rd] << ", " << abiName[rs1] << ", " << abiName[rs2] << "\n";
                reg[rd] = reg[rs1] ^ reg[rs2];
                break;
            case 5:
                if (funct7 == 32)
                {
                    cout << "\tSRA\t" << abiName[rd] << ", " << abiName[rs1] << ", " << abiName[rs2] << "\n";
                    reg[rd] = reg[rs1] >> reg[rs2];
                }
                else if (funct7 == 0)
                {
                    cout << "\tSRL\t" << abiName[rd] << ", " << abiName[rs1] << ", " << abiName[rs2] << "\n";
                    reg[rd] = static_cast<unsigned int>(reg[rs1]) >> reg[rs2];
                }
                else
                {
                    cout << "\tUnkown R Instruction \n";
                }
                break;
            case 6:
                cout << "\tOR\t" << abiName[rd] << ", " << abiName[rs1] << ", " << abiName[rs2] << "\n";
                reg[rd] = reg[rs1] | reg[rs2];
                break;
            case 7:
                cout << "\tAND\t" << abiName[rd] << ", " << abiName[rs1] << ", " << abiName[rs2] << "\n";
                reg[rd] = reg[rs1] & reg[rs2];
                break;
            default:
                cout << "\tUnkown R Instruction \n";
            }
        }
        else if (opcode == 0x13)
        { // ALU - I instructions
            switch (funct3)
            {
            case 0:
                cout << "\tADDI\t" << abiName[rd] << ", " << abiName[rs1] << ", " << dec << (int)I_imm << "\n";
                reg[rd] = reg[rs1] + int(I_imm);
                break;
            case 1:
                cout << "\tSLLI\t" << abiName[rd] << ", " << abiName[rs1] << ", " << dec << (int)shamt << "\n";
                reg[rd] = reg[rs1] << int(I_imm);
                break;
            case 2:
                cout << "\tSLTI\t" << abiName[rd] << ", " << abiName[rs1] << ", " << dec << (int)I_imm << "\n";
                if (int(reg[rs1]) < int(I_imm))
                {
                    reg[rd] = 1;
                }
                else
                {
                    reg[rd] = 0;
                }

                // reg[rd] = (reg[rs1] < reg[rs2]) ? 1 : 0;
                break;
            case 3:
                cout << "\tSLTIU\t" << abiName[rd] << ", " << abiName[rs1] << ", " << dec << (int)I_imm << "\n";
                if (static_cast<unsigned int>(reg[rs1]) < static_cast<unsigned int>(I_imm))
                {
                    reg[rd] = 1;
                }
                else
                {
                    reg[rd] = 0;
                }
                // reg[rd] = (static_cast<unsigned int>(reg[rs1]) < static_cast<unsigned int>(I_imm)) ? 1 : 0;
                break;
            case 4:
                cout << "\tXORI\t" << abiName[rd] << ", " << abiName[rs1] << ", " << dec << (int)I_imm << "\n";
                reg[rd] = reg[rs1] ^ int(I_imm);
                break;
            case 5:
                if (funct7 == 32)
                {
                    cout << "\tSRAI\t" << abiName[rd] << ", " << abiName[rs1] << ", " << dec << (int)shamt << "\n";
                    reg[rd] = reg[rs1] >> int(I_imm);
                }
                else if (funct7 == 0)
                {
                    cout << "\tSRLI\t" << abiName[rd] << ", " << abiName[rs1] << ", " << dec << (int)shamt << "\n";
                    reg[rd] = static_cast<unsigned int>(reg[rs1]) >> int(I_imm);
                }
                else
                {
                    cout << "\tUnkown I-type ALU Instruction \n";
                }
                break;

            case 6:
                cout << "\tORI\t" << abiName[rd] << ", " << abiName[rs1] << ", " << dec << (int)I_imm << "\n";
                reg[rd] = reg[rs1] | int(I_imm);

                break;
            case 7:
                cout << "\tANDI\t" << abiName[rd] << ", " << abiName[rs1] << ", " << dec << (int)I_imm << "\n";
                reg[rd] = reg[rs1] & int(I_imm);

                break;
            default:
                cout << "\tUnkown I-type ALU Instruction \n";
            }
        }
        else if (opcode == 0x03)
        {
            // Load - I instructions
            switch (funct3)
            {
            case 0:
                cout << "\tLB\t" << abiName[rd] << ", " << dec << (int)I_imm << "(" << abiName[rs1] << ")" << "\n";
                reg[rd] = memory[rs1 + (int)I_imm];
                break;
            case 1:
                cout << "\tLH\t" << abiName[rd] << ", " << dec << (int)I_imm << "(" << abiName[rs1] << ")" << "\n";
                reg[rd] = memory[rs1 + (int)I_imm];
                reg[rd] |= (memory[rs1 + (int)I_imm + 1] << 8);
                break;
            case 2:
                cout << "\tLW\t" << abiName[rd] << ", " << dec << (int)I_imm << "(" << abiName[rs1] << ")" << "\n";
                reg[rd] = memory[rs1 + (int)I_imm];
                reg[rd] |= (memory[rs1 + (int)I_imm + 1] << 8);
                reg[rd] |= (memory[rs1 + (int)I_imm + 2] << 16);
                reg[rd] |= (memory[rs1 + (int)I_imm + 3] << 24);
                break;
            case 4:
                cout << "\tLBU\t" << abiName[rd] << ", " << dec << (int)I_imm << "(" << abiName[rs1] << ")" << "\n";
                reg[rd] = static_cast<unsigned int>(memory[rs1 + (int)I_imm]);
                reg[rd] &= 0x000000FF;
                break;
            case 5:
                cout << "\tLHU\t" << abiName[rd] << ", " << dec << (int)I_imm << "(" << abiName[rs1] << ")" << "\n";
                reg[rd] = static_cast<unsigned int>(memory[rs1 + (int)I_imm]) |
                          (static_cast<unsigned int>(memory[rs1 + (int)I_imm + 1]) << 8);
                reg[rd] &= 0x0000FFFF;
                break;
            default:
                cout << "\tUnkown I-type load Instruction \n";
            }
        }
        else if (opcode == 0x23)
        {
            switch (funct3)
            {
            case 0:
                cout << "\tSB\t" << abiName[rs1] << ", " << dec << (int)S_imm << "(" << abiName[rs2] << ")" << "\n";
                memory[rs1] = reg[rs2 + (int)S_imm];

            case 1:
                cout << "\tSh\t" << abiName[rs1] << ", " << dec << (int)S_imm << "(" << abiName[rs2] << ")" << "\n";
                memory[rs1] = reg[rs2 + (int)S_imm];
                memory[rs1] |= (reg[rs2 + (int)S_imm + 1] << 8);

            case 2:
                cout << "\tSW\t" << abiName[rs1] << ", " << dec << (int)S_imm << "(" << abiName[rs2] << ")" << "\n";
                memory[rs1] = reg[rs2 + (int)S_imm];
                memory[rs1] |= (reg[rs2 + (int)S_imm + 1] << 8);
                memory[rs1] |= (reg[rs2 + (int)S_imm + 1] << 16);
                memory[rs1] |= (reg[rs2 + (int)S_imm + 1] << 24);
            }
        }
        else
        {
            cout << "\tUnkown Instruction \n";
        }
    }
    void printRegisterContents()
    {
        for (int i = 0; i < 32; i++)
        {
            cout << "x" << dec << i << "\t" << abiName[i] << "\t0x" << hex << reg[i] << "\n";
        }
    }

    int main(int argc, char *argv[])
    {
        unsigned int instWord = 0;
        ifstream inFile;
        ifstream dataFile;
        ofstream outFile;

        if (argc < 2)
            emitError("use: rvcdiss <machine_code_file_name> <data_section_file_name>\n");

        inFile.open(argv[1], ios::in | ios::binary | ios::ate);
        dataFile.open(argv[2], ios::in | ios::binary | ios::ate);
        if (inFile.is_open())
        {
            int fsize = inFile.tellg();

            inFile.seekg(0, inFile.beg);
            if (!inFile.read((char *)memory, fsize))
                emitError("Cannot read from text file\n");
        }

        if (dataFile.is_open())
        {
            int fsize = dataFile.tellg();

            dataFile.seekg(0, dataFile.beg);
            if (!dataFile.read((char *)(memory + 0x00010000), fsize)) // because data section starts at 0x00010000
                emitError("Cannot read from data file\n");
        }
        else
            emitError("Cannot access data file\n");

        if (inFile.is_open())
        {

            while (true)
            {
                // deCompress(instWord);
                instWord = (unsigned char)memory[pc] |
                           (((unsigned char)memory[pc + 1]) << 8) |
                           (((unsigned char)memory[pc + 2]) << 16) |
                           (((unsigned char)memory[pc + 3]) << 24);
                pc += 4;
                // remove the following line once you have a complete simulator
                if ((pc > 16000) | (instWord == 0))
                    break; // stop when PC reached address 32

                instDecExec(instWord);
            }
            printRegisterContents();
        }
        else
            emitError("Cannot access text file\n");
    }