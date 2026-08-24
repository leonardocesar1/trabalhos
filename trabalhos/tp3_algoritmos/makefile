# Compilador e flags
CC = g++
CXFLAGS = -Wall -Wextra -Wpedantic -Wconversion -Wformat-security -Werror
CXDEBUG = -g
CXVER = -std=c++17

# Diretórios e arquivos
SRC = src/*.cpp
INCLUDE = include/
EXEC = tp3.out

# Regra padrão
all: compile

# Compilação normal
compile:
	$(CC) $(CXFLAGS) $(CXVER) -I$(INCLUDE) $(SRC) -o $(EXEC)

# Compilação em modo de depuração
debug:
	$(CC) $(CXFLAGS) $(CXVER) $(CXDEBUG) -I$(INCLUDE) $(SRC) -o $(EXEC)

# Limpeza de arquivos compilados
clean:
	rm -f $(EXEC)

.PHONY: all clean debug compile
