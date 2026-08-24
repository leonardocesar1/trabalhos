# Nome do executável
TARGET = simulador

# Compilador
CC = gcc

# Flags de compilação
CFLAGS = -Wall -Wextra -Wpedantic -std=c11 -O2

# Todos os arquivos .c do diretório
SRCS = $(wildcard *.c)

# Arquivos objeto correspondentes
OBJS = $(SRCS:.c=.o)

# Regra padrão
all: $(TARGET)

# Geração do executável
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

# Compilação dos arquivos .c em .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Remove arquivos gerados
clean:
	rm -f $(OBJS) $(TARGET)

# Recompila tudo
re: clean all

.PHONY: all clean re
