  .data

  ##### R1 START MODIFIQUE AQUI START #####
  #
  # Este espaço é para você definir as suas constantes e vetores auxiliares.
  #

  vetor1: .word 2, 3, 4

  vetor2: .word 1, 5, 4, 6, 8

  ##### R1 END MODIFIQUE AQUI END #####

  .text
    add s0, zero, zero            # s0 armazenará o número de testes que passaram
    la a0, vetor1
    addi a1, zero, 3
    jal e_crescente
    addi t0, zero, 1
    bne a0, t0, teste_2
    addi s0,s0,1

  teste_2:
    la a0, vetor2
    addi a1, zero, 5
    jal e_crescente
    addi t0, zero, 0
    bne a0, t0, FIM
    addi s0,s0,1
    j FIM

  ##### R2 START MODIFIQUE AQUI START #####

  e_crescente:
    addi t0, zero, 0              # i = 0
    addi t5, a1, -1               # t5 = n - 1
    blt t5, zero, L2              # se n < 1, é crescente
  L1:
    beq t0, t5, L2                # se i == n - 1, é crescente
    slli t1, t0, 2                # t1 = i * 4
    add t1, a0, t1                # t1 = &vetor[i]
    lw t2, 0(t1)                  # t2 = vetor[i]
    lw t3, 4(t1)                  # t3 = vetor[i + 1]
    bge t2, t3, L3                # se vetor[i] >= vetor[i + 1], não é crescente
    addi t0, t0, 1                # i++
    j L1
  L2:
    li a0, 1                      # retorna 1
    jalr zero, 0(ra)
  L3:
    li a0, 0                      # retorna 0
    jalr zero, 0(ra)

  ##### R2 END MODIFIQUE AQUI END #####

  FIM:
