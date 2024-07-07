
<h2>Spring 2024 Project Report<h/>

Dr. Mohamed Shalan

Logic Circuits Simulator
  
Islam M.Abdeen 900225835

aly elaswad 900225517

Ahmed M.Abdeen 900225815 

CSCE 2301





Objective: Create an ISS for RISC-V RV32IC Base Integer Instruction Set with support for instruction compression.



How to Build the simulator
(1) Choose a suitable programming language (We chose c++)
(2) Allocate Memory to simulate the memory in the assembler (For example: an array)
(3) Create an array for the 32 registers and their abiNames 
(4) Read the machine code instruction words and classify them accroding to their format based on their opcode
(5) To distinguish between the instructions within each format, use funct3, funct7 in addition to the opcode
(6) When the instruction is detected, it is executed and printed on the terminal 
(7) For the compressed instructions, they are detected based on the first two bits of the opcode. When it is not 11 then the instruction is compressed
(8) When a compressed instruction is detected, there is a function that prints it and decompresses it and turns it to a normal 32 bit instruction to be executed by the simulator
(9) Use a pc (program counter) which is an integer variable that is incremented by 4 after each instruction word (or by 2 if the instruction is compressed). It has the address of the next instruction
(10) Use another counter for the instruction (Instpc) to point to the present instruction
(11) Print the values of the registers at the end of the program 
(12) Create two parameters for the int main(): one is the input file and the other is the data file (for data variables, strings, etc)
(13) Handle potential errors, such as not being able to access the input or the data file and reading an unknown instruction that is not supported RV32IC
  







<h>Steps on how to run the Simulator<h/>

1. Open terminal at the folder that contains the rvsim.cpp file
2. run: g++ rvsim.cpp -o rvsim.exe
3. run: /rvsim.exe YourTestCase.bin (optional: datafile.bin)
<h4>Steps on getting a correct .bin file<h4/>

1. Open [RARS]([url](https://github.com/TheThirdOne/rars))
2. Write any code you want to test
3. Assemble on RARS
4. Press this icon to Dump machine code or data in an avaialable format : <img width="42" alt="Screenshot 2024-06-29 at 1 15 15 AM" src="https://github.com/alyelaswad/Project1DigitalDesign/assets/124714695/a46ffc95-fa11-492c-b0a7-52da394af334"><br/>
5. 5.Choose Binary
6. Put the .bin file in the same folder as the rvsim.cpp file
7. (OPTIONAL) put a data file.bin in the same folder as the rvsim.cpp file

Simulator Design
Key Components: -

-Memory: It is an array of size (64 + 64)*1024 to simulate the memory used in the assembler

-Registers: It is an array of type unsigned int and of size 32 (The total number of registers) and there is another array to store the abiNames of those registers

-Decoder/Executor: The function "instDecExec" takes the instruction word in machine code. Then, disassembles it and prints on the terminal

-Decompressor: The function "deCompress" takes a 16 bit instruction. Then, it prints the compressed instruction and decompresses it into a 32 bit instrution to be passed in the instDecExec that would execute it.

-Read stream: The intmain() function takes up to two arguments: the input file and data file. It then reads the contents of those files and stores them in the memory using their addresses as the index in the array

Challenges and Limitations:-
Challenges: -
- One of the challenges was testing the load and store instructions. The store could be tested through loading, where we can store a value in the memory and then load it into a new register and then print the value. However, this would assume that the load instruction was executed properly, which was a plausible assumption. However, to make sure of the contents in the registers, we created a function to print the contents in the registers after execution.

- Another challenge was the process of merging the work of the group members where each group member had a number of instructions to work on, and some of the instructions of one group member depended on some instructions of another group member. Thus, the merging process took more time because proper testing might require more than one member to finish their work at the same time.

- Limitations: -
- The simulator does not support pseudo instructions. The simulator represents them in true instructions instead.
- The simulator does not print lables. It prints the addresses of the lables
  

