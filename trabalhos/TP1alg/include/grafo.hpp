#ifndef GRAFO_HPP
#define GRAFO_HPP

#include <vector>

using namespace std;

// estrutura para armazenar resultados de busca
struct resultado_busca {
    int indice;
    vector<int> distancias;
};

// variáveis globais
extern int numero_abrigos;
extern vector<int> raios;
extern vector<int> coordenadas_x;
extern vector<int> coordenadas_y;
extern vector<vector<int>> lista_adjacencia;

// funções principais do grafo
void construir_grafo();
int encontrar_abrigo(int x_ponto, int y_ponto);
int calcular_distancia_bfs(int origem, int destino);
resultado_busca buscar_abrigo_mais_distante(int inicio);
int calcular_diametro_do_grafo();

// articulação
extern vector<bool> ponto_articulacao;
extern vector<int> descoberta;
extern vector<int> menor_descoberta;
extern vector<int> pai;
extern int contador_tempo;
// extern indica que a definição real está em outro arquivo

void dfs_articulacao(int u);

#endif
