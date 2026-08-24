#include <iostream>      // para entrada padrão
#include "grafo.hpp"     // declarações de funções e variáveis do grafo

using namespace std;

int main() {
    // leitura dos pontos de Ana e Bernardo
    int posicao_ax, posicao_ay, posicao_bx, posicao_by;
    cin >> posicao_ax >> posicao_ay >> posicao_bx >> posicao_by;

    // leitura do número de abrigos e seus dados
    cin >> numero_abrigos;

    // função resize ajusta o tamanho do vetor
    raios.resize(numero_abrigos);
    coordenadas_x.resize(numero_abrigos);
    coordenadas_y.resize(numero_abrigos);

    for (int i = 0; i < numero_abrigos; ++i) {
        cin >> raios[i] >> coordenadas_x[i] >> coordenadas_y[i];  // raio e coordenadas
    }

    // construção do grafo de abrigos
    construir_grafo();

    // Parte 1: distância mínima em número de saltos de abrigo
    int abrigo_ana = encontrar_abrigo(posicao_ax, posicao_ay);
    int abrigo_bernardo = encontrar_abrigo(posicao_bx, posicao_by);
    int distancia_minima = -1;

    if (abrigo_ana != -1 && abrigo_bernardo != -1) {
        distancia_minima = calcular_distancia_bfs(abrigo_ana, abrigo_bernardo);
    }

    cout << "Parte 1: " << distancia_minima << endl;

    // Parte 2: diâmetro do grafo (máxima distância entre quaisquer abrigos)
    int diametro = calcular_diametro_do_grafo();
    cout << "Parte 2: " << diametro << endl;

    // Parte 3: pontos de articulação (abrigos críticos cuja remoção desconecta o grafo)
    // função assign substitui todo o conteúdo do vetor por tantas cópias do que foi passado depois
    ponto_articulacao.assign(numero_abrigos, false);
    descoberta.assign(numero_abrigos, -1);
    menor_descoberta.assign(numero_abrigos, -1);
    pai.assign(numero_abrigos, -1);
    contador_tempo = 0;

    for (int i = 0; i < numero_abrigos; ++i) {
        if (descoberta[i] == -1) dfs_articulacao(i);
    }

    vector<int> abrigos_criticos;

    for (int i = 0; i < numero_abrigos; ++i) {
        if (ponto_articulacao[i]) abrigos_criticos.push_back(i + 1);
    }

    cout << "Parte 3: " << abrigos_criticos.size();

    for (int index : abrigos_criticos) cout << " " << index;

    cout << endl;

    return 0;
}
