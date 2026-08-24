#include <iostream>
#include <vector>
#include <queue>
#include <climits>

using namespace std;

// função que faz busca em largura considerando movimentos de rainha
vector<vector<int>> bfs(pair<int, int> inicio, int linhas, int colunas, const vector<string>& tabuleiro) {
    // deslocamentos de linha e coluna para as oito direções
    int movimento_linha[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
    int movimento_coluna[8] = {-1, 0, 1, -1, 1, -1, 0, 1};

    // matriz de distâncias inicializada com valor infinito
    vector<vector<int>> distancia(linhas, vector<int>(colunas, INT_MAX));
    queue<pair<int, int>> fila;

    // ponto de partida recebe distância zero
    distancia[inicio.first][inicio.second] = 0;
    fila.push(inicio);

    // enquanto houver posições para explorar
    while (!fila.empty()) {
        auto atual = fila.front();
        fila.pop();
        int linha = atual.first;
        int coluna = atual.second;
        int custo_atual = distancia[linha][coluna];

        // tenta expandir em cada direção
        for (int dir = 0; dir < 8; dir++) {
            // passo a passo, para simular movimento livre até obstáculo
            for (int passo = 1;; passo++) {
                int nova_linha = linha + movimento_linha[dir] * passo;
                int nova_coluna = coluna + movimento_coluna[dir] * passo;

                // sai do tabuleiro? interrompe essa direção
                if (nova_linha < 0 || nova_linha >= linhas ||
                    nova_coluna < 0 || nova_coluna >= colunas) {
                    break;
                }

                // encontrou casa bloqueada? para nessa direção
                if (tabuleiro[nova_linha][nova_coluna] == '-') {
                    break;
                }

                // se achou caminho mais curto, atualiza e enfileira
                if (distancia[nova_linha][nova_coluna] > custo_atual + 1) {
                    distancia[nova_linha][nova_coluna] = custo_atual + 1;
                    fila.push({nova_linha, nova_coluna});
                }
            }
        }
    }

    return distancia;
}

// solução exata para o problema do caixeiro viajante sobre peões usando programação dinâmica
int tsp(vector<vector<int>>& distancias_entre_pontos, int numero_peoes) {
    int todos_visitados = (1 << numero_peoes) - 1;
    // dp[mascara][ultimo] = custo mínimo para visitar o conjunto mascara terminando em ultimo
    vector<vector<int>> dp(1 << numero_peoes, vector<int>(numero_peoes, INT_MAX));

    // custos iniciais: só a rainha indo para cada peão
    for (int i = 0; i < numero_peoes; i++) {
        dp[1 << i][i] = distancias_entre_pontos[0][i + 1];
    }

    // preenche dp para todas as máscaras de visitados
    for (int mascara = 1; mascara <= todos_visitados; mascara++) {
        for (int ultimo = 0; ultimo < numero_peoes; ultimo++) {
            if (!(mascara & (1 << ultimo))) continue;  // se ultimo não está na máscara, pula

            int custo_atual = dp[mascara][ultimo];
            if (custo_atual == INT_MAX) continue;  // estado inacessível

            // tenta ir para um próximo peão não visitado
            for (int proximo = 0; proximo < numero_peoes; proximo++) {
                if (mascara & (1 << proximo)) continue;

                int nova_mascara = mascara | (1 << proximo);
                int custo_para_proximo =
                    custo_atual + distancias_entre_pontos[ultimo + 1][proximo + 1];

                if (custo_para_proximo < dp[nova_mascara][proximo]) {
                    dp[nova_mascara][proximo] = custo_para_proximo;
                }
            }
        }
    }

    // responde com o menor custo ao visitar todos
    int resposta = INT_MAX;
    for (int i = 0; i < numero_peoes; i++) {
        resposta = min(resposta, dp[todos_visitados][i]);
    }

    return resposta;
}

// heurística: sempre escolhe o próximo mais próximo
int tsp_guloso(vector<vector<int>>& distancias_entre_pontos) {
    int quantidade = static_cast<int>(distancias_entre_pontos.size());
    vector<bool> usado(quantidade, false);
    int atual = 0;
    usado[0] = true;  // começa na rainha
    int total = 0;

    for (int visitados = 1; visitados < quantidade; ++visitados) {
        int melhor = -1;
        int melhor_distancia = INT_MAX;
        // procura o peão mais próximo que ainda não foi visitado
        for (int j = 1; j < quantidade; ++j) {
            if (!usado[j] && distancias_entre_pontos[atual][j] < melhor_distancia) {
                melhor_distancia = distancias_entre_pontos[atual][j];
                melhor = j;
            }
        }
        if (melhor < 0 || melhor_distancia == INT_MAX) return -1; // sem caminho viável
        usado[melhor] = true;
        total += melhor_distancia;
        atual = melhor;
    }
    return total;
}

int main() {
    istream& arquivo = cin;

    int linhas, colunas, numero_peoes;
    arquivo >> linhas >> colunas >> numero_peoes;

    // lê o tabuleiro linha a linha
    vector<string> tabuleiro(linhas);
    for (int i = 0; i < linhas; i++) {
        arquivo >> tabuleiro[i];
    }

    pair<int, int> posicao_rainha;
    vector<pair<int, int>> posicoes_peoes;

    // encontra rainha e peões no tabuleiro
    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            if (tabuleiro[i][j] == 'R') {
                posicao_rainha = {i, j};
            } else if (tabuleiro[i][j] == 'P') {
                posicoes_peoes.push_back({i, j});
            }
        }
    }

    // insere a rainha no início do vetor de posições
    posicoes_peoes.insert(posicoes_peoes.begin(), posicao_rainha);

    int total_vertices = numero_peoes + 1;
    vector<vector<int>> distancias_entre_pontos(total_vertices, vector<int>(total_vertices, INT_MAX));

    // calcula distâncias entre todos os pares usando bfs
    for (int i = 0; i < total_vertices; i++) {
        vector<vector<int>> distancias_greed = bfs(posicoes_peoes[i], linhas, colunas, tabuleiro);
        for (int j = 0; j < total_vertices; j++) {
            distancias_entre_pontos[i][j] = distancias_greed[posicoes_peoes[j].first][posicoes_peoes[j].second];
        }
    }

    // se algum peão não for alcançável, imprime -1
    for (int i = 1; i < total_vertices; i++) {
        if (distancias_entre_pontos[0][i] == INT_MAX) {
            cout << -1 << endl;
            return 0;
        }
    }

    int resultado = tsp(distancias_entre_pontos, numero_peoes);
    // int resultado = tsp_guloso(distancias_entre_pontos);

    cout << resultado;
    return 0;
}
