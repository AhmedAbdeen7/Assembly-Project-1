
addi s2,zero,1
addi s3,zero,5

loop:
beq s2,s3,exit_loop
addi s2,s2,1
j loop
exit_loop:

addi t3,zero,-5
addi t2,zero,2

blt t3,t2,label


sub t3,t4,s0

label:

addi s7,zero,-9
addi s8,zero, 4

bge s7,s8,label2


addi t1,zero,4
addi s8,zero,8

label2:

bne t1, s8, skip

add t2,s8,zero

skip:

