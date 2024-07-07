
  .text
  .globl main
  
   
    main:
    
    

    
    li a0, 5
    li a7, 1
    ecall 
    
    

    
    li t0, 3
    sw t0, 0(t1)
    lw t2, 0(t1)
    li t3, 4
    sb t3, 0(t1)
    lb t4, 0(t1)
    li s1, 9
    sh s1, 0(t1)
    lh s2, 0(t1)
    
    mv a0, t2
    li a7, 1
    ecall
    
        addi s8,zero,1
    addi s9,zero,3
    
    jal ra, label1
 
    
    addi t0, t1, 1
    beq s8,s9,skip
    addi s8,s8,1
 label1:
   jalr zero, ra, 0
   skip:
   lui s0, 10000
   auipc t5, 1000000
       
    li a7, 10
    ecall
    