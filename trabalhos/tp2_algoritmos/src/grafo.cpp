#include "grafo.hpp"

// construtor: cria um grafo com total_vertices vértices (lista de adjacência vazia)
rede_fluxo::rede_fluxo(int total_vertices)
    : grafo(total_vertices)
{}

// adiciona aresta com capacidade e cria também a aresta reversa com capacidade zero
void rede_fluxo::adicionar_aresta(int origem, int destino, int capacidade) {
    aresta a1 = { destino, (int)grafo[destino].size(), capacidade };
    aresta a2 = { origem,   (int)grafo[origem].size(),   0         };

    grafo[origem].push_back(a1);   // aresta direta
    grafo[destino].push_back(a2);  // aresta reversa (inicialmente sem capacidade)
}

// implementa ford–fulkerson usando busca em largura (algoritmo de edmonds–karp)
int rede_fluxo::ford_fulkerson(int fonte, int sumidouro) {
    int fluxo_total = 0;
    auto n = grafo.size();

    while (true) {
        // parent guarda par (vértice pai, índice da aresta) para reconstruir caminho
        vector<pair<int,int>> parent(n, make_pair(-1, -1));

        // fluxo guarda a capacidade mínima até cada vértice a partir da fonte
        vector<int> fluxo(n, 0);

        queue<int> q;  // fila para bfs
        fluxo[fonte] = INT_MAX;  // fluxo inicial é infinito (INT_MAX)
        q.push(fonte);

        // faz busca em largura até encontrar o sumidouro ou esgotar caminhos
        while (!q.empty() && parent[sumidouro].first == -1) {
            int u = q.front();
            q.pop();

            for (int i = 0; i < (int)grafo[u].size(); i++) {
                aresta &a = grafo[u][i];
                int v = a.destino;

                // só expande se ainda não visitou e se há capacidade positiva
                if (parent[v].first == -1 && a.capacidade > 0 && v != fonte) {
                    parent[v] = make_pair(u, i);
                    fluxo[v] = min(fluxo[u], a.capacidade);
                    q.push(v);
                }
            }
        }

        // se não encontrou caminho até o sumidouro, encerra o loop
        if (parent[sumidouro].first == -1) {
            break;
        }

        int f = fluxo[sumidouro];  // fluxo possível nesse caminho
        fluxo_total += f;          // acumula ao fluxo total

        int v = sumidouro;
        // ajusta capacidades nas arestas do caminho encontrado
        while (v != fonte) {
            int u = parent[v].first;
            int idx = parent[v].second;

            aresta &a = grafo[u][idx];
            a.capacidade -= f;  // diminui capacidade na aresta direta

            // acha a aresta reversa e aumenta sua capacidade
            grafo[v][a.indice_reversa].capacidade += f;

            v = u;
        }
    }

    return fluxo_total;
}

// monta o grafo para representar cada célula da matriz como dois nós (entrada/saída)
void grafo_estrutura(
    int numero_linhas,
    int numero_colunas,
    const vector<vector<int>>& matriz,
    int coordenada_x,
    int coordenada_y,
    rede_fluxo& rede,
    int fonte,
    int sumidouro
) {
    // calcula índice do nó de entrada de uma célula
    auto no_entrada = [numero_colunas](int linha, int coluna) {
        return (linha * numero_colunas + coluna) * 2;
    };

    // calcula índice do nó de saída de uma célula
    auto no_saida = [numero_colunas](int linha, int coluna) {
        return (linha * numero_colunas + coluna) * 2 + 1;
    };

    // vetores para iterar sobre as quatro direções possíveis
    int delta_linha[4] = {-1, 1, 0, 0};
    int delta_coluna[4] = {0, 0, -1, 1};

    for (int i = 0; i < numero_linhas; i++) {
        for (int j = 0; j < numero_colunas; j++) {
            int soldados = matriz[i][j];

            if (soldados == 0) {
                continue;  // célula sem soldados (vazia)
            }

            int entrada = no_entrada(i, j);
            int saida = no_saida(i, j);

            // conecta nó de entrada ao nó de saída com capacidade = número de soldados
            rede.adicionar_aresta(entrada, saida, soldados);

            // se célula fica na borda, conecta a fonte ao seu nó de entrada
            if (
                i == 0 ||
                i == numero_linhas - 1 ||
                j == 0 ||
                j == numero_colunas - 1
            ) {
                rede.adicionar_aresta(fonte, entrada, INT_MAX);
            }

            // se essa célula for a coordenada alvo, conecta saída ao sumidouro
            if (i == coordenada_x && j == coordenada_y) {
                rede.adicionar_aresta(saida, sumidouro, INT_MAX);
            }

            // para cada direção, conecta a saída dessa célula à entrada da célula vizinha
            for (int d = 0; d < 4; d++) {
                int ni = i + delta_linha[d];
                int nj = j + delta_coluna[d];

                // verifica se vizinho está dentro dos limites
                if (ni < 0 || ni >= numero_linhas || nj < 0 || nj >= numero_colunas) {
                    continue;
                }

                if (matriz[ni][nj] == 0) {
                    continue;  // pula célula vazia
                }

                int entrada_vizinha = no_entrada(ni, nj);

                // aresta com capacidade infinita para permitir movimento de fluxo
                rede.adicionar_aresta(saida, entrada_vizinha, INT_MAX);
            }
        }
    }
}
