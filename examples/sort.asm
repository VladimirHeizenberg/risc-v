.section .text
.globl _start

_start:
    la a0, array
    li a1, 80
    li a2, 0

outer_loop:
    li a3, 0
    li a4, 0

inner_loop:
    slli t0, a3, 2
    add t1, a0, t0
    lw t2, 0(t1)
    lw t3, 4(t1)

    blt t2, t3, no_swap

    sw t3, 0(t1)
    sw t2, 4(t1)

    li a4, 1

no_swap:
    addi a3, a3, 1
    blt a3, a1, inner_loop

    beq a4, zero, end

    addi a2, a2, 1
    blt a2, a1, outer_loop

end:
    li a7, 10
    ecall

.section .data
array:
    .word 9, 7, 5, 11, 3, 2, 8, 4, 1, 6, 19, 27, 35, 411, 3, 25, 85, 476, 18, 16, 9,
          7, 5, 11, 3, 2, 8, 4, 1, 6, 19, 27, 35, 411, 3, 25, 85, 476, 18, 16, 9, 7,
          5, 11, 3, 2, 8, 4, 1, 6, 19, 27, 35, 411, 3, 25, 85, 476, 18, 16, 9, 7, 5,
          11, 3, 2, 8, 4, 1, 6, 19, 27, 35, 411, 3, 25, 85, 476, 18, 16