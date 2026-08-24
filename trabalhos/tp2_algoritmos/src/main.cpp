#include "grafo.hpp"

int main() {
    istream& arquivo = cin;  // vai ler dados do stdin

    int numero_linhas, numero_colunas;
    arquivo >> numero_linhas >> numero_colunas;  // lê dimensões da matriz

    vector<vector<int>> matriz(numero_linhas, vector<int>(numero_colunas));

    // preenche a matriz com a quantidade de soldados em cada célula
    for (int i = 0; i < numero_linhas; i++) {
        for (int j = 0; j < numero_colunas; j++) {
            arquivo >> matriz[i][j];
        }
    }

    int coordenada_x, coordenada_y;
    arquivo >> coordenada_x >> coordenada_y;  // lê coordenadas do alvo

    coordenada_x--;  // ajusta para índice baseado em zero
    coordenada_y--;

    // calcula quantidade total de vértices na rede de fluxo
    int total_vertices = numero_linhas * numero_colunas * 2 + 2;

    int fonte = total_vertices - 2;      // índice do vértice fonte
    int sumidouro = total_vertices - 1;  // índice do vértice sumidouro

    rede_fluxo rede(total_vertices);  // instancia rede de fluxo com n vértices

    // constroi o grafo de acordo com a matriz e a célula alvo
    grafo_estrutura(
        numero_linhas,
        numero_colunas,
        matriz,
        coordenada_x,
        coordenada_y,
        rede,
        fonte,
        sumidouro
    );

    // calcula o fluxo máximo da fonte até o sumidouro
    int resultado = rede.ford_fulkerson(fonte, sumidouro);

    cout << resultado;  // imprime o resultado
    return 0;
}
