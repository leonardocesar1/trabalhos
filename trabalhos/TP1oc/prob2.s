.data

##### R1 START MODIFIQUE AQUI START #####
#
# Este espaço é para você definir as suas constantes e vetores auxiliares.
#

lista_precos: .word 7, 5, 1     # Vetor com os preços iniciais dos produtos
fator_aumento: .word 3          # Fator de aumentos do preço
taxa_50: .word 2                # Taxa adicional para ajustar o preço

##### R1 END MODIFIQUE AQUI END #####

.text

  add s0, zero, zero            # Inicializa s0 com 0; s0 contará o número de testes que passaram

  li a0, 0                      # Inicializa a0
  la a1, lista_precos           # a1 aponta para o início da lista de preços
  li a2, 0                      # Índice inicial do vetor
  li a3, 3                      # Tamanho do vetor

  jal ra, soma_estoque          # Chama função para somar todos os preços ajustados
  addi t5, zero, 78             # Esperado: (7*3*2) + (5*3*2) + (1*3*2) = 42 + 30 + 6 = 78
  bne a0, t5, teste_2           # Se o resultado for diferente, pula para teste_2
  addi s0,s0,1                  # Incrementa s0 (teste passou)

teste_2:
  li a0, 3                      # Inicializa a0
  lw a1, fator_aumento          # a1 recebe o fator de aumento

  jal ra, ajustar_preco         # Chama a função ajustar_preco (a0 = 3)
  addi t5, zero, 18             # Valor esperado do resultado = 3 * 3 * 2 = 18
  bne a0, t5, teste_3           # Se o resultado for diferente, pula para teste_3
  addi s0,s0,1                  # Incrementa s0 (teste passou)

teste_3:
  li a0, 2                      # a0 recebe 2, elemento i do vetor vetor[i]
  lw a1, taxa_50                # a1 recebe a taxa de aumento

  jal ra, aplicar_taxa          # Chama a função aplicar_taxa (a0 = 2, a3 = 2)
  addi t5, zero, 4              # Esperado: 2 * 2 = 4
  bne a0, t5, FIM               # Se diferente de 4, pula para FIM
  addi s0,s0,1                  # Incrementa s0 (teste passou)
  j FIM

##### R2 START MODIFIQUE AQUI START #####

soma_estoque:
  addi sp, sp, -16              # aloca 16 bytes
  sw ra, 12(sp)                 # salva o endereço de retorno ra
  sw s1,  8(sp)                 # salva o registrador s1
  sw s2,  4(sp)                 # salva o registrador s2
  li s1, 0                      # soma = 0
  mv s2, a1                     # move o ponteiro original para s2
  mv t0, a1                     # move o ponteiro original para t0
L1:
  bge a2, a3, L2                # confere se chegou no final do vetor
  lw a0, 0(t0)                  # carrega o preço
  lw a1, fator_aumento          # carrega o fator
  jal ra, ajustar_preco
  add s1, s1, a0                # soma += preço
  addi t0, t0, 4                # avança o ponteiro
  addi a2, a2, 1                # i++
  j L1
L2:
  mv a0, s1                     # move o resultado para a0
  lw s2,  4(sp)                 # restaura s2
  lw s1,  8(sp)                 # restaura s1
  lw ra, 12(sp)                 # restaura ra
  addi sp, sp, 16               # desaloca 16 bytes
  jalr zero, 0(ra)

ajustar_preco:
  addi sp, sp, -8               # aloca 8 bytes
  sw ra, 4(sp)                  # salva o endereço de retorno ra
  mul a0, a0, a1                # multiplica o preço pelo fator de aumento
  lw a1, taxa_50                # carrega a taxa
  jal ra, aplicar_taxa
  lw ra, 4(sp)                  # restaura ra
  addi sp, sp, 8                # desaloca 8 bytes
  jalr zero, 0(ra)

aplicar_taxa:
  mul a0, a0, a1                # aplica a taxa
  jalr zero, 0(ra)

##### R2 END MODIFIQUE AQUI END #####

FIM:
