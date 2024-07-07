.text
.globl main
main:
addi s9,zero,-26
slti s1,s9,1
sltiu s2,s9,25
xori s3,s9,0x7ff
ori s4,s9,1
andi s5,s9,0x7FF
la s0, Array
li t0, 5
li s0, 0x1200

li t0, 65
sw t0,0(s0)
li t0, 7
sw t0,2(s0)
li t0, 8
sw t0,4(s0)
li t0, 3
sw t0,6(s0)


lw t5,0(s0)
lw t6,2(s0)
lw s7,4(s0)
lw s6,6(s0)
lw sp,6(s0)
addi s7,zero,24
slli t4,s7,1
srli t2,s7,2
srai t3,s7,2
.data
Array: .space 500
Five: .word 5
firstNumPrompt: .asciz "Enter first number: "
NumSpaces: .asciz "\nThe number of white spaces is: "
