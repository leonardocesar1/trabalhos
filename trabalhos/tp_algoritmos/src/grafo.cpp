#include "grafo.hpp"
#include <queue>

using namespace std;

// definições das variáveis globais
int numero_abrigos;
vector<int> raios;
vector<int> coordenadas_x;
vector<int> coordenadas_y;
vector<vector<int>> lista_adjacencia;

// constrói o grafo conectando abrigos que se tocam
void construir_grafo() {
    lista_adjacencia.assign(numero_abrigos, vector<int>());

    for (int i = 0; i < numero_abrigos; ++i) {
        for (int j = i + 1; j < numero_abrigos; ++j) {
            int dx = coordenadas_x[i] - coordenadas_x[j];
            int dy = coordenadas_y[i] - coordenadas_y[j];
            int soma_raios = raios[i] + raios[j];

            // verifica se os círculos tocam (distância <= soma dos raios)
            if (dx * dx + dy * dy <= soma_raios * soma_raios) {
                lista_adjacencia[i].push_back(j);
                lista_adjacencia[j].push_back(i);
            }
        }
    }
}

// retorna o abrigo que contém o ponto dado (ou -1 se nenhum)
int encontrar_abrigo(int x_ponto, int y_ponto) {
    for (int i = 0; i < numero_abrigos; ++i) {
        int dx = coordenadas_x[i] - x_ponto;
        int dy = coordenadas_y[i] - y_ponto;

        // teste de inclusão: ponto dentro do círculo de raio raios[i]
        if (dx * dx + dy * dy <= raios[i] * raios[i]) return i;
    }

    return -1;
}

// calcula distância mínima entre dois abrigos com BFS
int calcular_distancia_bfs(int origem, int destino) {
    // inicia vetor de distâncias com -1 (não visitado)
    vector<int> distancia(numero_abrigos, -1);
    queue<int> fila;
    distancia[origem] = 0;
    fila.push(origem);

    while (!fila.empty()) {
        int atual = fila.front();
        fila.pop(); // remove o elemento que já foi processado

        if (atual == destino) break;

        for (int vizinho : lista_adjacencia[atual]) {
            if (distancia[vizinho] == -1) {
                distancia[vizinho] = distancia[atual] + 1;
                fila.push(vizinho);
            }
        }
    }

    return distancia[destino];
}

// retorna abrigo mais distante a partir de 'inicio'
resultado_busca buscar_abrigo_mais_distante(int inicio) {
    vector<int> distancia(numero_abrigos, -1);
    queue<int> fila;
    distancia[inicio] = 0;
    fila.push(inicio);
    int mais_distante = inicio;

    while (!fila.empty()) {
        int atual = fila.front();
        fila.pop(); // pop() para avançar na BFS

        for (int vizinho : lista_adjacencia[atual]) {
            if (distancia[vizinho] == -1) {
                distancia[vizinho] = distancia[atual] + 1;
                fila.push(vizinho);

                // atualiza se este nó estiver mais distante
                if (distancia[vizinho] > distancia[mais_distante])
                    mais_distante = vizinho;
            }
        }
    }

    return {mais_distante, distancia};
}

// calcula o maior diâmetro entre os componentes do grafo
int calcular_diametro_do_grafo() {
    vector<bool> visitado(numero_abrigos, false);
    int diametro = 0;
    queue<int> fila;

    for (int i = 0; i < numero_abrigos; ++i) {
        if (visitado[i]) continue;
        vector<int> componente;
        fila.push(i);
        visitado[i] = true;

        // encontra todos os nós do componente conectado
        while (!fila.empty()) {
            int atual = fila.front(); fila.pop();
            componente.push_back(atual);

            for (int vizinho : lista_adjacencia[atual]) {
                if (!visitado[vizinho]) {
                    visitado[vizinho] = true;
                    fila.push(vizinho);
                }
            }
        }

        // usa duas BFS para achar diâmetro: de um nó qualquer ao mais distante, e deste ao mais distante
        resultado_busca r1 = buscar_abrigo_mais_distante(componente[0]);
        resultado_busca r2 = buscar_abrigo_mais_distante(r1.indice);

        for (int no : componente) {
            diametro = max(diametro, r2.distancias[no]);
        }
    }

    return diametro;
}

// pontos de articulação
vector<bool> ponto_articulacao;
vector<int> descoberta;
vector<int> menor_descoberta;
vector<int> pai;
int contador_tempo;

// DFS para encontrar pontos de articulação
void dfs_articulacao(int u) {
    // marca tempo de descoberta e inicializa menor_descoberta
    descoberta[u] = menor_descoberta[u] = ++contador_tempo;
    int filhos = 0;

    for (int v : lista_adjacencia[u]) {
        if (descoberta[v] == -1) {
            ++filhos;
            pai[v] = u;
            dfs_articulacao(v);
            // atualiza valor de menor_descoberta com base nos filhos
            menor_descoberta[u] = min(menor_descoberta[u], menor_descoberta[v]);

            if ((pai[u] == -1 && filhos > 1) ||
                (pai[u] != -1 && menor_descoberta[v] >= descoberta[u])) {
                ponto_articulacao[u] = true;
            }
        } else if (v != pai[u]) {
            // atualiza menor_descoberta por meio de back-edge
            menor_descoberta[u] = min(menor_descoberta[u], descoberta[v]);
        }
    }
}
