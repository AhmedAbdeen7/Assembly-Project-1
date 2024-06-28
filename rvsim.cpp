/*
	This is just a skeleton. It DOES NOT implement all the requirements.
	It only recognizes the RV32I "ADD", "SUB" and "ADDI" instructions only.
	It prints "Unkown Instruction" for all other instructions!

	References:
	(1) The risc-v ISA Manual ver. 2.1 @ https://riscv.org/specifications/
	(2) https://github.com/michaeljclark/riscv-meta/blob/master/meta/opcodes
*/

#include <iostream>
#include <fstream>
#include "stdlib.h"
#include <iomanip>

using namespace std;

unsigned int pc;
unsigned int reg[32] = {0};
unsigned char memory[(64 + 64) * 1024];

void emitError(char *s)
{
	cout << s;
	exit(0);
}

void printPrefix(unsigned int instA, unsigned int instW)
{
	cout << "0x" << hex << std::setfill('0') << std::setw(8) << instA << "\t0x" << std::setw(8) << instW;
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
				cout << "\tSUB\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
				reg[rd] = reg[rs1] - reg[rs2];
			}
			else if (funct7 == 0)
			{
				cout << "\tADD\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
				reg[rd] = reg[rs1] + reg[rs2];
			}
			else
			{
				cout << "\tUnkown R Instruction \n";
			}
			break;
		case 1:
			cout << "\tSLL\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
			reg[rd] = reg[rs1] << reg[rs2];
			break;
		case 2:
			cout << "\tSLT\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
			reg[rd] = (reg[rs1] < reg[rs2]) ? 1 : 0;
			break;
		case 3:
			cout << "\tSLTU\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
			reg[rd] = (static_cast<unsigned int>(reg[rs1]) < static_cast<unsigned int>(reg[rs2])) ? 1 : 0;
			break;
		case 4:
			cout << "\tXOR\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
			reg[rd] = reg[rs1] ^ reg[rs2];
			break;
		case 5:
			if (funct7 == 32)
			{
				cout << "\tSRA\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
				reg[rd] = reg[rs1] >> reg[rs2];
			}
			else if (funct7 == 0)
			{
				cout << "\tSRL\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
				reg[rd] = static_cast<unsigned int>(reg[rs1]) >> reg[rs2];
			}
			else
			{
				cout << "\tUnkown R Instruction \n";
			}
			break;
		case 6:
			cout << "\tOR\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
			reg[rd] = reg[rs1] | reg[rs2];
			break;
		case 7:
			cout << "\tAND\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
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
			cout << "\tADDI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
			reg[rd] = reg[rs1] + int(I_imm);
			break;
		case 1:
			cout << "\tSLLI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)shamt << "\n";
			reg[rd] = reg[rs1] << int(I_imm);
			break;
		case 2:
			cout << "\tSLTI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
			reg[rd] = (reg[rs1] < int(I_imm)) ? 1 : 0;
			break;
		case 3:
			cout << "\tSLTIU\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
			reg[rd] = (static_cast<unsigned int>(reg[rs1]) < static_cast<unsigned int>(I_imm)) ? 1 : 0;
			break;
		case 4:
			cout << "\tXORI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
			reg[rd] = reg[rs1] ^ int(I_imm);
			break;
		case 5:
			if (funct7 == 32)
			{
				cout << "\tSRAI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)shamt << "\n";
				reg[rd] = reg[rs1] >> int(I_imm);
			}
			else if (funct7 == 0)
			{
				cout << "\tSRLI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)shamt << "\n";
				reg[rd] = static_cast<unsigned int>(reg[rs1]) >> int(I_imm);
			}
			else
			{
				cout << "\tUnkown I-type ALU Instruction \n";
			}
			break;

		case 6:
			cout << "\tORI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
			reg[rd] = reg[rs1] | int(I_imm);

			break;
		case 7:
			cout << "\tANDI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
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
			cout << "\tLB\tx" << rd << ", " << hex << "0x" << (int)I_imm << "(x" << rs1 << ")" << "\n";
			reg[rd] = memory[rs1 + (int)I_imm];
			break;
		case 1:
			cout << "\tLH\tx" << rd << ", " << hex << "0x" << (int)I_imm << "(x" << rs1 << ")" << "\n";
			reg[rd] = memory[rs1 + (int)I_imm];
			reg[rd] |= (memory[rs1 + (int)I_imm + 1] << 8);
			break;
		case 2:
			cout << "\tLW\tx" << rd << ", " << hex << "0x" << (int)I_imm << "(x" << rs1 << ")" << "\n";
			reg[rd] = memory[rs1 + (int)I_imm];
			reg[rd] |= (memory[rs1 + (int)I_imm + 1] << 8);
			reg[rd] |= (memory[rs1 + (int)I_imm + 2] << 16);
			reg[rd] |= (memory[rs1 + (int)I_imm + 3] << 24);
			break;
		case 4:
			cout << "\tLBU\tx" << rd << ", " << hex << "0x" << (int)I_imm << "(x" << rs1 << ")" << "\n";
			reg[rd] = static_cast<unsigned int>(memory[rs1 + (int)I_imm]);
			reg[rd] &= 0x000000FF;
			break;
		case 5:
			cout << "\tLHU\tx" << rd << ", " << hex << "0x" << (int)I_imm << "(x" << rs1 << ")" << "\n";
			reg[rd] = static_cast<unsigned int>(memory[rs1 + (int)I_imm]) |
					  (static_cast<unsigned int>(memory[rs1 + (int)I_imm + 1]) << 8);
			reg[rd] &= 0x0000FFFF;
			break;
		default:
			cout << "\tUnkown I-type load Instruction \n";
		}
	}
	else
	{
		cout << "\tUnkown Instruction \n";
	}
}

int main(int argc, char *argv[])
{

	unsigned int instWord = 0;
	ifstream inFile;
	ofstream outFile;

	if (argc < 1)
		emitError("use: rvcdiss <machine_code_file_name>\n");

	inFile.open(argv[1], ios::in | ios::binary | ios::ate);

	if (inFile.is_open())
	{
		int fsize = inFile.tellg();

		inFile.seekg(0, inFile.beg);
		if (!inFile.read((char *)memory, fsize))
			emitError("Cannot read from input file\n");

		while (true)
		{
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
	}
	else
		emitError("Cannot access input file\n");
}
