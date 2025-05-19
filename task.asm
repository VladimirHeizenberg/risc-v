li t1, 0x2000
li t3, 9
li t4, 4

li t0, 1
li t5, 0
li t2, 0

li t6, 0
li ra, 6

loop:
    addi t2, t2, 1
    blt t2, t3, loop

loop2:
    addi t5, t5, 1
    sb t0, 0(t1)
    blt t5, t4, loop2


loop3:
    addi t6, t6, 1
    blt t6, ra, loop3

addi t1, t1, 1024
addi t3, t3, 0
addi t4, t4, 0

addi t0, t0, 1
addi t5, t5, -4
addi t2, t2, -9

addi t6, t6, -6
addi ra, ra, 0

loop4:
    addi t2, t2, 1
    blt t2, t3, loop4

loop5:
    addi t5, t5, 1
    sb t0, 0(t1)
    blt t5, t4, loop5


loop6:
    addi t6, t6, 1
    blt t6, ra, loop6

addi t1, t1, 1024
addi t3, t3, 0
addi t4, t4, 0

addi t0, t0, 1
addi t5, t5, -4
addi t2, t2, -9

addi t6, t6, -6
addi ra, ra, 0

loop7:
    addi t2, t2, 1
    blt t2, t3, loop7

loop8:
    addi t5, t5, 1
    sb t0, 0(t1)
    blt t5, t4, loop8


loop9:
    addi t6, t6, 1
    blt t6, ra, loop9

addi t1, t1, 1024
addi t3, t3, 0
addi t4, t4, 0

addi t0, t0, 1
addi t5, t5, -4
addi t2, t2, -9

addi t6, t6, -6
addi ra, ra, 0

loop10:
    addi t2, t2, 1
    blt t2, t3, loop10

loop11:
    addi t5, t5, 1
    sb t0, 0(t1)
    blt t5, t4, loop11


loop12:
    addi t6, t6, 1
    blt t6, ra, loop12
