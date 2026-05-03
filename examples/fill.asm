.section .text
.globl _start

_start:
    li t0, 0
    li t1, 100
    li t2, 0x2000


loop:
    mul t3, t0, t0
    slli t4, t0, 2
    add t5, t2, t4
    sw t3, 0(t5)

    addi t0, t0, 1
    blt t0, t1, loop

    ebreak
