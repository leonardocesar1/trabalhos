#include "voo.hpp"

// função para comparar dois voos com base no critério especificado (preço, duração, ou número de paradas)
bool comparar_voos(const voo &a, const voo &b, const string &criterio) {
    for (char c : criterio) {
        if (c == 'p') { // se o critério for 'p' (preço)
            if (a.preco != b.preco) return a.preco < b.preco; // compara os preços
        }
        else if (c == 'd') { // se o critério for 'd' (duração)
            if (a.duracao != b.duracao) return a.duracao < b.duracao; // compara as durações
        }
        else if (c == 's') { // se o critério for 's' (número de paradas)
            if (a.numero_paradas != b.numero_paradas) return a.numero_paradas < b.numero_paradas; // compara o número de paradas
        }
    }

    return false; // se os voos são iguais conforme o critério, retorna false
}

// função para trocar os dados de dois voos
void trocar_voos(voo &a, voo &b) {
    voo auxiliar = a;
    a = b;
    b = auxiliar;
}

// função para ordenar uma lista de voos usando o algoritmo de ordenação por inserção
void insertionsort(voo *lista, int n, const string &criterio) {
    for (int i = 1; i < n; i++) {
        voo auxiliar = lista[i]; // salva o voo a ser inserido
        int j = i - 1;

        // move os voos maiores (conforme o critério) para a direita
        while (j >= 0 && comparar_voos(auxiliar, lista[j], criterio)) {
            lista[j+1] = lista[j];
            j--;
        }

        lista[j+1] = auxiliar; // insere o voo na posição correta
    }
}
