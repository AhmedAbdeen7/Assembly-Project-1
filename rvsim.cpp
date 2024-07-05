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
unsigned int reg[32];
string abiName[32] = {"zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2", "s0", "s1",
					  "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7", "s2", "s3", "s4", "s5",
					  "s6", "s7", "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"};
unsigned char memory[(64 + 64) * 1024];
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
	instWord = 0x00000114;
	unsigned int rd_dash, rs1_dash, rs1, rs2, funct4, funct3, offset, op;
	unsigned int CIW_imm, CL_imm, CS_imm_func, CJ_offset, CSS_imm, S_imm_func, shift_imm, S_imm, CB_imm;

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
			rd_dash = (instWord >> 2) & 0x00000007;
			CIW_imm = (instWord >> 5) & 0x000000FF;
			CIW_imm = (((CIW_imm & 0x02) >> 1) | ((CIW_imm & 0x01) << 1) | ((CIW_imm & 0x0C0) >> 4) | ((CIW_imm & 0x03c) << 2));
			// Print
			cout << "\tC.ADDI4SPN\t" << abiName[rd_dash + 8] << ", " << dec << (int)(CIW_imm) << "\n";
			// WILL CHANGE WITH CONTROL SIGNAL
			CIW_imm = CIW_imm << 2;
			instWord = 0x00000013; // I-type
			rd_dash = rd_dash << 7;
			instWord |= (rd_dash + 8);
			instWord |= 0x00000000;	 // ADDI
			instWord |= 0x00010000;	 // sp ~x2
			CIW_imm = CIW_imm << 20; // place imm[0] at bit 20
			instWord |= CIW_imm;
			// cout << "\tC.ADDI4SPN\t" << abiName[rd_dash + 8] << ", " << dec << (int)(CIW_imm >> 2) << "\n";
			instDecExec(instWord);
			break;
		case 2:
			// CL - Format
			rd_dash = (instWord >> 2) & 0x00000007;
			rs1_dash = (instWord >> 7) & 0x00000007;

			CL_imm = ((instWord & 0x060) >> 5) | ((instWord >> 8) & 0x1C);
			CL_imm = (((CL_imm & 0x01) << 4) | ((CL_imm & 0x02) >> 1) | (CL_imm & 0x1C) >> 1);
			cout << "\tC.LW\t" << abiName[rd_dash + 8] << ", " << dec << (int)(CL_imm) << "(" << abiName[rs1_dash + 8] << ")" << "\n";
			instWord = 0x00000003; // I-Type LOAD
			rd_dash |= 0b01000;
			rd_dash <<= 7;
			instWord |= rd_dash;
			instWord |= 0x00002000; // LW
			rs1_dash |= 0b01000;
			rs1_dash <<= 15;
			instWord |= rs1_dash;
			CL_imm = CL_imm << 22;
			instWord |= CL_imm;
			instDecExec(instWord);
			break;
		case 6:
			op = 0b0100011;
			funct3 = 0x2;
			rs1 = (instWord >> 7) & 0x7;
			rs2 = (instWord >> 2) & 0x7;
			S_imm = ((instWord >> 5) & 0x3) | (((instWord >> 10) & 0x7) << 2);
			instWord = op | (((S_imm * 4) & 0x1F) << 7) | (funct3 << 12) | (rs1 << 15) | (rs2 << 20) | ((S_imm >> 5) << 25) | (S_imm >> 4 ? 0xFE000000 : 0x00000000);
			return instWord;
			break;

		default:
			break;
		}
	}
	else if (op == 0x1)
	{
		switch (funct3)
		{
			CS_imm_func = ((instWord >> 5) & 0x3) | (((instWord >> 10) & 0x7) << 2);

		case 1:
			// C.JAL
			CJ_offset = (instWord >> 2) & 0x00000FFF; // 11 bits
			CJ_offset = (((CJ_offset & 0x001) << 4) | ((CJ_offset & 0x00E) >> 1) |
						 ((CJ_offset & 0x010) << 2) | ((CJ_offset & 0x040) << 3) |
						 ((CJ_offset & 0x200) >> 6) | (CJ_offset & 0x5A0));
			cout << "\tC.J\t" << dec << (int)(CJ_offset) << "\n";
			instWord = 0x000000EF; // J-type
			instWord |= ((CJ_offset & 0x3FF) << 21) | ((CJ_offset & 0x0400) << 10);
			if ((CJ_offset & 0x0400) != 0) // sign-extend
			{
				instWord |= 0x801FF000;
			}
			instDecExec(instWord);
			break;
		case 0x4:

			if (CS_imm_func == 0xF) // C.AND
			{
				rd_dash = (instWord >> 7) & 0x7;
				rs1_dash = rd_dash;
				op = 0b0110011;
				funct3 = 0x7;
				rs2 = (instWord >> 2) & 0x7;
				instWord = op | (rd_dash << 7) | (funct3 << 12) | (rs1_dash << 15) | (rs2 << 20) | (0x00000000);
				return instWord;
			}

			else if (CS_imm_func == 0xE) // C.OR
			{
				rd_dash = (instWord >> 7) & 0x7;
				rs1 = rd_dash;
				op = 0b0110011;
				funct3 = 0x6;
				rs2 = (instWord >> 2) & 0x7;
				instWord = op | (rd_dash << 7) | (funct3 << 12) | (rs1 << 15) | (rs2 << 20) | (0x00000000);

				return instWord;
			}
			else if (CS_imm_func == 0xD) // C.XOR
			{
				rd_dash = (instWord >> 7) & 0x7;
				rs1 = rd_dash;
				op = 0b0110011;
				funct3 = 0x4;
				rs2 = (instWord >> 2) & 0x7;
				instWord = op | (rd_dash << 7) | (funct3 << 12) | (rs1 << 15) | (rs2 << 20) | (0x00000000);

				return instWord;
			}
			else if (CS_imm_func == 0xC) // C.SUB
			{
				rd_dash = (instWord >> 7) & 0x7;
				rs1 = rd_dash;
				op = 0b0110011;
				funct3 = 0x0;
				rs2 = (instWord >> 2) & 0x7;
				instWord = op | (rd_dash << 7) | (funct3 << 12) | (rs1 << 15) | (rs2 << 20) | (0x40000000);

				return instWord;
			}
			else if (((instWord >> 10) & 0x3) == 0) // C.SRLI
			{
				rs1 = (instWord >> 7) & 0x7; // Check later for choice of registers
				rd_dash = rs1;
				op = 0b0010011;
				funct3 = 0x5;
				shift_imm = ((instWord >> 2) & 0x1F) | (((instWord >> 10) & 0x1) << 5);
				instWord = op | (rd_dash << 7) | (funct3 << 12) | (rs1 << 15) | (shift_imm << 20);

				return instWord;
			}
			else if (((instWord >> 10) & 0x3) == 0b01) // C.SRAI
			{
				rs1 = (instWord >> 7) & 0x7; // Check later for choice of registers
				rd_dash = rs1;
				op = 0b0010011;
				funct3 = 0x5;
				shift_imm = ((instWord >> 2) & 0x1F) | (((instWord >> 10) & 0x1) << 5);
				instWord = op | (rd_dash << 7) | (funct3 << 12) | (rs1 << 15) | (shift_imm << 20) | 0x20000000;

				return instWord;
			}
			else if (((instWord >> 10) & 0x3) == 10) // C.ANDI
			{
				rs1 = (instWord >> 7) & 0x7; // Check later for choice of registers
				rd_dash = rs1;
				op = 0b0010011;
				funct3 = 0x7;
				shift_imm = ((instWord >> 2) & 0x1F) | (((instWord >> 10) & 0x1) << 5);
				instWord = op | (rd_dash << 7) | (funct3 << 12) | (rs1 << 15) | (shift_imm << 20) | ((shift_imm >> 5) ? 0xFE000000 : 0x0);

				return instWord;
			}
			break;
		case 0x6: // C.BEQZ
			op = 0b1100011;
			funct3 = 0x0;
			rs1 = (instWord >> 7) & 0x7;
			rs2 = rs1;
			CB_imm = (((instWord >> 3) & 0x3) << 1) | (((instWord >> 10) & 0x3) << 3) | (((instWord >> 2) & 0x1) << 5) | (((instWord >> 5) & 0x3) << 6) | (((instWord >> 12) & 0x1) << 8);
			return instWord;
			break;
		case 0x7: // C.BNEZ
			op = 0b1100011;
			funct3 = 0x1;
			rs1 = (instWord >> 7) & 0x7;
			rs2 = rs1;
			CB_imm = (((instWord >> 3) & 0x3) << 1) | (((instWord >> 10) & 0x3) << 3) | (((instWord >> 2) & 0x1) << 5) | (((instWord >> 5) & 0x3) << 6) | (((instWord >> 12) & 0x1) << 8);
			return instWord;
			break;
		case 5:
			// CJ - Format C.J instruction
			CJ_offset = (instWord >> 2) & 0x00000FFF; // 11 bits
			CJ_offset = (((CJ_offset & 0x001) << 4) | ((CJ_offset & 0x00E) >> 1) |
						 ((CJ_offset & 0x010) << 2) | ((CJ_offset & 0x040) << 3) |
						 ((CJ_offset & 0x200) >> 6) | (CJ_offset & 0x5A0));

			cout << "\tC.J\t" << dec << (int)(CJ_offset) << "\n";

			instWord = 0x0000006F; // J-type
			instWord |= ((CJ_offset & 0x3FF) << 21) | ((CJ_offset & 0x0400) << 10);
			if ((CJ_offset & 0x0400) != 0) // sign-extend
			{
				instWord |= 0x801FF000;
			}
			instDecExec(instWord);
			break;
			// case 2:
			// 	// CL - Format
			// 	rd_dash = (instWord >> 2) & 0x00000007;
			// 	instWord = 0x00000003; // I-Type LOAD
			// 	rd_dash = rd_dash << 7;
			// 	instWord |= rd_dash;
			// 	// Rest TBC

			// 	break;

		default:
			break;
		}
	}
	else if (op == 0x2)
	{
		switch (funct3)
		{
		case 6:
			rs2 = (instWord >> 2) & 0x1F;
			CSS_imm = (instWord >> 7) & 0x3F;
			CSS_imm = ((CSS_imm & 0x3) << 4) | ((CSS_imm & 0x3C) >> 4);
			cout << "\tC.SWSP\t" << abiName[rs2] << ", " << dec << (int)(CSS_imm) << "\n";
			instWord = 0x00000023;
			rs2 <<= 20;
			instWord |= rs2;
			instWord |= ((CSS_imm & 0x7) << 9) | ((CSS_imm & 0x1FA) << 22);
			instWord |= 0x00012000;
			instDecExec(instWord);
			break;
		default:
			break;
		}

		return instWord;
	}
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
	S_imm = ((instWord >> 7) & 0x1F) |	 // imm[4:0]
			((instWord >> 20) & 0xFE0) | // imm[11:5]
			((instWord & 0x80000000) ? 0xFFFFF800 : 0);
	U_imm = ((instWord >> 12) & 0xFFFFF);
	B_imm = (((instWord >> 8) & 0xF) << 1) | (((instWord >> 25) & 0x3F) << 5) | (((instWord >> 7) & 0x1) << 11) | (((instWord >> 31) & 0x1) << 12);
	if (B_imm >> 12)
		B_imm |= 0xFFFFF000;
	J_imm = (((instWord >> 31) & 0x1) << 20) |
			(((instWord >> 12) & 0xFF) << 12) |
			(((instWord >> 20) & 0x1) << 11) |
			(((instWord >> 21) & 0x3FF) << 1) |
			(((instWord >> 31) & 0x1) ? 0xFFF00000 : 0);

	// if (J_imm >> 20)
	// 	J_imm |= 0xFFF00000;
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
			reg[rd] = memory[reg[rs1] + (int)I_imm];
			if (reg[rd] & 0x80)
			{
				reg[rd] |= 0xFFFFFF00; // If the MSB is set, extend the sign bit
			}
			break;
		case 1:
			cout << "\tLH\t" << abiName[rd] << ", " << dec << (int)I_imm << "(" << abiName[rs1] << ")" << "\n";
			reg[rd] = memory[reg[rs1] + (int)I_imm];
			reg[rd] |= (memory[reg[rs1] + (int)I_imm + 1] << 8);
			if (reg[rd] & 0x8000)
			{
				reg[rd] |= 0xFFFF0000; // If the MSB is set, extend the sign bit
			}
			break;
		case 2:
			cout << "\tLW\t" << abiName[rd] << ", " << dec << (int)I_imm << "(" << abiName[rs1] << ")" << "\n";
			reg[rd] = memory[reg[rs1] + (int)I_imm];
			reg[rd] |= (memory[reg[rs1] + (int)I_imm + 1] << 8);
			reg[rd] |= (memory[reg[rs1] + (int)I_imm + 2] << 16);
			reg[rd] |= (memory[reg[rs1] + (int)I_imm + 3] << 24);
			break;
		case 4:
			cout << "\tLBU\t" << abiName[rd] << ", " << dec << (int)I_imm << "(" << abiName[rs1] << ")" << "\n";
			reg[rd] = static_cast<unsigned int>(memory[reg[rs1] + (int)I_imm]);
			reg[rd] &= 0x000000FF;
			break;
		case 5:
			cout << "\tLHU\t" << abiName[rd] << ", " << dec << (int)I_imm << "(" << abiName[rs1] << ")" << "\n";
			reg[rd] = static_cast<unsigned int>(memory[reg[rs1] + (int)I_imm]) |
					  (static_cast<unsigned int>(memory[reg[rs1] + (int)I_imm + 1]) << 8);
			reg[rd] &= 0x0000FFFF;
			break;
		default:
			cout << "\tUnkown I-type load Instruction \n";
		}
	}
	else if (opcode == 0x67)
	{ // JALR (I - Instruction)
		cout << "\tJALR\t" << abiName[rd] << ", " << abiName[rs1] << ", " << dec << (int)I_imm << "\n";
		reg[rd] = instPC + 4;
		pc = reg[rs1] + (int)I_imm;
	}
	else if (opcode == 0x73)

	{ // ecall (I -Instruction)
		cout << "\tECALL\n";
		if (reg[17] == 1)
		{
			cout << dec << (int)reg[10] << "\n";
		}
		else if (reg[17] == 4)
		{
			int i = 0;

			while (memory[reg[10] + i] != 0)
			{
				cout << (char)(memory[reg[10] + i]);
				i++;
			}
			cout << endl;
		}
		else if (reg[17] == 10)
		{
			exit(0);
		}
	}
	else if (opcode == 0x23)
	{ // S-type Instructions
		switch (funct3)
		{
		case 0:
			cout << "\tSB\t" << abiName[rs2] << ", " << dec << (int)S_imm << "(" << abiName[rs1] << ")" << "\n";
			memory[reg[rs1] + (int)S_imm] = reg[rs2] & 0xFF;
			break;

		case 1:
			cout << "\tSH\t" << abiName[rs2] << ", " << dec << (int)S_imm << "(" << abiName[rs1] << ")" << "\n";
			memory[reg[rs1] + (int)S_imm] = reg[rs2] & 0xFF;
			memory[reg[rs1] + (int)S_imm + 1] = (reg[rs2] >> 8) & 0xFF;
			break;

		case 2:
			cout << "\tSW\t" << abiName[rs2] << ", " << dec << (int)S_imm << "(" << abiName[rs1] << ")" << "\n";
			memory[reg[rs1] + (int)S_imm] = reg[rs2] & 0xFF;
			memory[reg[rs1] + (int)S_imm + 1] = (reg[rs2] >> 8) & 0xFF;
			memory[reg[rs1] + (int)S_imm + 2] = (reg[rs2] >> 16) & 0xFF;
			memory[reg[rs1] + (int)S_imm + 3] = (reg[rs2] >> 24) & 0xFF;
			break;

		default:
			cout << "\tUnkown I-type load Instruction \n";
		}
	}
	else if (opcode == 0x37)
	{ // U-type Instruction (Load Upper Immediate)
		cout << "\tLUI\t" << abiName[rd] << ", " << dec << (int)U_imm << "\n";
		reg[rd] = U_imm << 12;
	}
	else if (opcode == 0x17)
	{ // U-type Instruction (Add Upper Immediate to PC)
		cout << "\tAUIPC\t" << abiName[rd] << ", " << dec << (int)U_imm << "\n";
		reg[rd] = pc + (U_imm << 12);
	}
	else if (opcode == 0x73)
	{ // ebreak (I -type)
		cout << "\tEBREAK\t" << endl;
	}
	else if (opcode == 0x63)
	{
		switch (funct3)
		{
		case 0:
			cout << "\tBEQ\t" << abiName[rs1] << ", " << abiName[rs2] << ", " << B_imm << "\n";
			if (reg[rs1] == reg[rs2])
				pc = instPC + B_imm;
			break;
		case 1:
			cout << "\tBNE\t" << abiName[rs1] << ", " << abiName[rs2] << ", " << B_imm << "\n";
			if (reg[rs1] != reg[rs2])
				pc = instPC + B_imm;
			break;
		case 4:
			cout << "\tBLT\t" << abiName[rs1] << ", " << abiName[rs2] << ", " << B_imm << "\n";
			if (reg[rs1] < reg[rs2])
				pc = instPC + B_imm;
			break;
		case 5:
			cout << "\tBGE\t" << abiName[rs1] << ", " << abiName[rs2] << ", " << B_imm << "\n";
			if (reg[rs1] >= reg[rs2])
				pc = instPC + B_imm;
			break;
		case 6:
			cout << "\tBLTU\t" << abiName[rs1] << ", " << abiName[rs2] << ", " << B_imm << "\n";
			if (static_cast<unsigned>(reg[rs1]) < static_cast<unsigned>(reg[rs2]))
				pc = instPC + B_imm;
			break;
		case 7:
			cout << "\tBGEU\t" << abiName[rs1] << ", " << abiName[rs2] << ", " << B_imm << "\n";
			if (static_cast<unsigned>(reg[rs1]) >= static_cast<unsigned>(reg[rs2]))
				pc = instPC + B_imm;
			break;
		default:
			break;
		}
	}
	else if (opcode == 0x6F)
	{
		cout << "\tJAL\t" << abiName[rd] << ", " << dec << (int)J_imm << "\n";
		reg[rd] = instPC + 4;
		pc = instPC + J_imm;
	}
	else
	{
		cout << "\tUnkown Instruction \n";
	}
	reg[0] = 0;
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
			if ((instWord == 0))
				break; // stop when PC reached address 32

			instDecExec(instWord);
		}
		printRegisterContents();
	}
	else
		emitError("Cannot access text file\n");

	cout << "\n\n\n\n\n"
		 << memory[reg[6]] << "\n\n\n\nHHHH\n";
}
