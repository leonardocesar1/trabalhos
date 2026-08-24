#ifndef GRAFO_HPP
#define GRAFO_HPP

#include <iostream>
#include <vector>
#include <climits>
#include <queue>

using namespace std;

struct aresta {
    int destino;         // vértice de destino da aresta
    int indice_reversa;  // índice da aresta reversa no grafo do destino
    int capacidade;      // capacidade residual da aresta
};

class rede_fluxo {
public:
    // inicializa o grafo com total_vertices vértices (lista de adjacência vazia)
    rede_fluxo(int total_vertices);

    // adiciona aresta direcionada com capacidade dada
    void adicionar_aresta(int origem, int destino, int capacidade);

    // executa o algoritmo de ford–fulkerson (busca em largura) e retorna o fluxo máximo
    int ford_fulkerson(int fonte, int sumidouro);

private:
    vector<vector<aresta>> grafo;  // lista de adjacência contendo arestas
};

// monta a estrutura do grafo a partir da matriz de soldados e posição alvo
void grafo_estrutura(
    int numero_linhas,
    int numero_colunas,
    const vector<vector<int>>& matriz,
    int coordenada_x,
    int coordenada_y,
    rede_fluxo& rede,
    int fonte,
    int sumidouro
);

#endif // GRAFO_HPP
