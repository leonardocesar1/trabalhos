# alvo principal que gera os diretórios e o executável
all: bin obj bin/tp3.out

# alvo para executar o programa com arquivo de entrada
run: bin/tp3.out
	./bin/tp3.out $(filter %.txt, $(MAKECMDGOALS))

# cria o executável a partir dos objetos gerados
bin/tp3.out: obj/main.o obj/avl.o obj/predicado.o obj/voo.o
	g++ -Wall -g -std=c++11 obj/main.o obj/avl.o obj/predicado.o obj/voo.o -o bin/tp3.out

# compila o arquivo principal
obj/main.o: src/main.cpp include/avl.hpp include/predicado.hpp include/voo.hpp
	g++ -Wall -g -std=c++11 -Iinclude -c src/main.cpp -o obj/main.o

# compila a implementação da árvore avl
obj/avl.o: src/avl.cpp include/avl.hpp
	g++ -Wall -g -std=c++11 -Iinclude -c src/avl.cpp -o obj/avl.o

# compila os predicados
obj/predicado.o: src/predicado.cpp include/predicado.hpp
	g++ -Wall -g -std=c++11 -Iinclude -c src/predicado.cpp -o obj/predicado.o

# compila a implementação dos voos
obj/voo.o: src/voo.cpp include/voo.hpp
	g++ -Wall -g -std=c++11 -Iinclude -c src/voo.cpp -o obj/voo.o

# cria os diretórios de saída, se não existirem
bin obj:
	mkdir -p bin obj

# limpa os arquivos gerados
clean:
	rm -rf bin/* obj/*
