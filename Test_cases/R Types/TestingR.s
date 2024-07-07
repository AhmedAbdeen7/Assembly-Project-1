addi x1, x0, 10       # x1 = 10
addi x2, x0, 20       # x2 = 20
addi x3, x0, -15      # x3 = -15
addi x4, x0, 0        # x4 = 0
addi x5, x0, 2047     # x5 = 2047 (max positive 12-bit immediate value)
addi x6, x0, -2048    # x6 = -2048 (min negative 12-bit immediate value)
addi x7, x0, 1        # x7 = 1


add x8, x1, x2        # x8 = 10 + 20 = 30
add x9, x1, x3        # x9 = 10 - 15 = -5
add x10, x3, x3       # x10 = -15 + (-15) = -30


sub x11, x2, x1       # x11 = 20 - 10 = 10
sub x12, x1, x3       # x12 = 10 - (-15) = 25
sub x13, x3, x1       # x13 = -15 - 10 = -25


xor x14, x1, x2       # x14 = 10 ^ 20
xor x15, x3, x3       # x15 = -15 ^ -15 = 0
xor x16, x5, x6       # x16 = 2047 ^ -2048


or x17, x1, x4        # x17 = 10 | 0 = 10
or x18, x3, x4        # x18 = -15 | 0 = -15
or x19, x5, x6        # x19 = 2047 | -2048


and x20, x1, x2       # x20 = 10 & 20
and x21, x3, x4       # x21 = -15 & 0 = 0
and x22, x5, x6       # x22 = 2047 & -2048

sll x23, x1, x7       # x23 = 10 << 1 = 20
sll x24, x2, x7       # x24 = 20 << 1 = 40
sll x25, x5, x7       # x25 = 2047 << 1


srl x26, x1, x7       # x26 = 10 >> 1 = 5
srl x27, x2, x7       # x27 = 20 >> 1 = 10
srl x28, x5, x7       # x28 = 2047 >> 1


sra x29, x1, x7       # x29 = 10 >> 1 = 5
sra x30, x3, x7       # x30 = -15 >> 1 (arithmetic shift)

slt x31, x1, x2        # x8 = (10 < 20) = 1
