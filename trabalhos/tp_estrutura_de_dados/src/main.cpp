#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "avl.hpp"
#include "predicado.hpp"
#include "voo.hpp"

using namespace std;

int main(int argc, char **argv) {
    string nome_arquivo = argv[1];
    ifstream entrada(nome_arquivo.c_str()); // abre o arquivo de entrada

    if (!entrada.is_open()) { // verifica se o arquivo foi aberto com sucesso
        cerr << "Erro ao abrir o arquivo!" << nome_arquivo << endl;
        return 1;
    }

    no<string> *indice_org = 0; // índice para origem
    no<string> *indice_dst = 0; // índice para destino
    no<double> *indice_prc = 0; // índice para preço
    no<int> *indice_sea = 0; // índice para assentos disponíveis
    no<int> *indice_sto = 0; // índice para número de paradas
    no<long long> *indice_dep = 0; // índice para tempo de partida
    no<long long> *indice_arr = 0; // índice para tempo de chegada
    no<long long> *indice_dur = 0; // índice para duração

    int numero_voos;
    entrada >> numero_voos; // lê o número total de voos
    voo *lista_voos = new voo[numero_voos]; // aloca um vetor para os voos

    for (int i = 0; i < numero_voos; i++) {
        // lê os dados de cada voo
        entrada >> lista_voos[i].origem >> lista_voos[i].destino >> lista_voos[i].preco >> lista_voos[i].assentos_disponiveis 
                >> lista_voos[i].data_hora_partida >> lista_voos[i].data_hora_chegada >> lista_voos[i].numero_paradas;

        lista_voos[i].tempo_partida = decompor_data_hora(lista_voos[i].data_hora_partida); // decompõe data/hora de partida
        lista_voos[i].tempo_chegada = decompor_data_hora(lista_voos[i].data_hora_chegada); // decompõe data/hora de chegada
        lista_voos[i].duracao = lista_voos[i].tempo_chegada - lista_voos[i].tempo_partida; // calcula a duração do voo

        // insere o índice de cada atributo na respectiva árvore avl
        indice_org = inserir(indice_org, lista_voos[i].origem, i);
        indice_dst = inserir(indice_dst, lista_voos[i].destino, i);
        indice_prc = inserir(indice_prc, lista_voos[i].preco, i);
        indice_sea = inserir(indice_sea, lista_voos[i].assentos_disponiveis, i);
        indice_sto = inserir(indice_sto, lista_voos[i].numero_paradas, i);
        indice_dep = inserir(indice_dep, lista_voos[i].tempo_partida, i);
        indice_arr = inserir(indice_arr, lista_voos[i].tempo_chegada, i);
        indice_dur = inserir(indice_dur, lista_voos[i].duracao, i);
    }

    int numero_consultas;
    entrada >> numero_consultas; // lê o número de consultas
    string auxiliar;
    getline(entrada, auxiliar); // consome a linha residual

    for (int i = 0; i < numero_consultas; i++) {
        string linha_consulta;
        getline(entrada, linha_consulta); // lê a linha da consulta

        if (linha_consulta == "") { // ignora linhas vazias
            i--;
            continue;
        }

        cout << linha_consulta << endl; // imprime a consulta recebida
        istringstream iss(linha_consulta); // cria stream para decompor a linha
        int max_resultados;
        string criterio_ordem;
        iss >> max_resultados >> criterio_ordem; // lê o número máximo de resultados e o critério de ordenação
        string expressao_filtro;
        getline(iss, expressao_filtro); // lê a expressão de filtro
        expressao_filtro = retirar_espacos(expressao_filtro); // remove espaços desnecessários
        predicado *lista_predicados = 0;
        int numero_predicados = 0;
        decompor_predicados(expressao_filtro, &lista_predicados, numero_predicados); // decompõe os predicados da consulta
        int *conjunto_atual = new int[numero_voos]; // cria o conjunto inicial com todos os voos
        int tamanho_atual = numero_voos;

        for (int j = 0; j < numero_voos; j++) conjunto_atual[j] = j; // preenche o conjunto com índices de todos os voos

        for (int p = 0; p < numero_predicados; p++) {
            int *lista_parcial = 0;
            int tamanho_parcial = 0;
            int capacidade_parcial = 10;
            lista_parcial = new int[capacidade_parcial]; // vetor para armazenar resultados parciais do predicado

            if (lista_predicados[p].atributo == "org") { // se o atributo é origem
                no<string> *no = buscar(indice_org, lista_predicados[p].valor);

                if (no) {
                    for (int k = 0; k < no->numero_indices; k++) {
                        if (tamanho_parcial >= capacidade_parcial) { // realoca se necessário
                            int novo_capacidade = capacidade_parcial * 2;
                            int *novo_arr = new int[novo_capacidade];

                            for (int l = 0; l < tamanho_parcial; l++) novo_arr[l] = lista_parcial[l];

                            delete[] lista_parcial;
                            lista_parcial = novo_arr;
                            capacidade_parcial = novo_capacidade;
                        }

                        lista_parcial[tamanho_parcial++] = no->lista_indices[k];
                    }
                }
            }
            else if (lista_predicados[p].atributo == "dst") { // se o atributo é destino
                no<string> *no = buscar(indice_dst, lista_predicados[p].valor);

                if (no) {
                    for (int k = 0; k < no->numero_indices; k++) {
                        if (tamanho_parcial >= capacidade_parcial) { // realoca se necessário
                            int novo_capacidade = capacidade_parcial * 2;
                            int *novo_arr = new int[novo_capacidade];

                            for (int l = 0; l < tamanho_parcial; l++) novo_arr[l] = lista_parcial[l];

                            delete[] lista_parcial;
                            lista_parcial = novo_arr;
                            capacidade_parcial = novo_capacidade;
                        }

                        lista_parcial[tamanho_parcial++] = no->lista_indices[k];
                    }
                }
            }
            else if (lista_predicados[p].atributo == "prc") { // se o atributo é preço
                double valor = atof(lista_predicados[p].valor.c_str());

                if (lista_predicados[p].operador == "==") {
                    no<double> *no = buscar(indice_prc, valor);

                    if (no) {
                        for (int k = 0; k < no->numero_indices; k++) {
                            if (tamanho_parcial >= capacidade_parcial) { // realoca se necessário
                                int novo_capacidade = capacidade_parcial * 2;
                                int *novo_arr = new int[novo_capacidade];

                                for (int l = 0; l < tamanho_parcial; l++) novo_arr[l] = lista_parcial[l];

                                delete[] lista_parcial;
                                lista_parcial = novo_arr;
                                capacidade_parcial = novo_capacidade;
                            }

                            lista_parcial[tamanho_parcial++] = no->lista_indices[k];
                        }
                    }
                }
                else if (lista_predicados[p].operador == "<=") {
                    buscar_intervalo(indice_prc, valor, true, &lista_parcial, tamanho_parcial, capacidade_parcial);
                }
                else if (lista_predicados[p].operador == ">=") {
                    buscar_intervalo(indice_prc, valor, false, &lista_parcial, tamanho_parcial, capacidade_parcial);
                }
            }
            else if (lista_predicados[p].atributo == "sea") { // se o atributo é assentos disponíveis
                int valor = stoi(lista_predicados[p].valor);

                if (lista_predicados[p].operador == "==") {
                    no<int> *no = buscar(indice_sea, valor);
                    if (no) {
                        for (int k = 0; k < no->numero_indices; k++) {
                            if (tamanho_parcial >= capacidade_parcial) { // realoca se necessário
                                int novo_capacidade = capacidade_parcial * 2;
                                int *novo_arr = new int[novo_capacidade];

                                for (int l = 0; l < tamanho_parcial; l++) novo_arr[l] = lista_parcial[l];

                                delete[] lista_parcial;
                                lista_parcial = novo_arr;
                                capacidade_parcial = novo_capacidade;
                            }

                            lista_parcial[tamanho_parcial++] = no->lista_indices[k];
                        }
                    }
                }
                else if (lista_predicados[p].operador == "<=") {
                    buscar_intervalo(indice_sea, valor, true, &lista_parcial, tamanho_parcial, capacidade_parcial);
                }
                else if (lista_predicados[p].operador == ">=") {
                    buscar_intervalo(indice_sea, valor, false, &lista_parcial, tamanho_parcial, capacidade_parcial);
                }
            }
            else if (lista_predicados[p].atributo == "sto") { // se o atributo é número de paradas
                int valor = stoi(lista_predicados[p].valor);

                if (lista_predicados[p].operador == "==") {
                    no<int> *no = buscar(indice_sto, valor);

                    if (no) {
                        for (int k = 0; k < no->numero_indices; k++) {
                            if (tamanho_parcial >= capacidade_parcial) { // realoca se necessário
                                int novo_capacidade = capacidade_parcial * 2;
                                int *novo_arr = new int[novo_capacidade];

                                for (int l = 0; l < tamanho_parcial; l++) novo_arr[l] = lista_parcial[l];

                                delete[] lista_parcial;
                                lista_parcial = novo_arr;
                                capacidade_parcial = novo_capacidade;
                            }

                            lista_parcial[tamanho_parcial++] = no->lista_indices[k];
                        }
                    }
                }
                else if (lista_predicados[p].operador == "<=") {
                    buscar_intervalo(indice_sto, valor, true, &lista_parcial, tamanho_parcial, capacidade_parcial);
                }
                else if (lista_predicados[p].operador == ">=") {
                    buscar_intervalo(indice_sto, valor, false, &lista_parcial, tamanho_parcial, capacidade_parcial);
                }
            }
            else if (lista_predicados[p].atributo == "dep") { // se o atributo é tempo de partida
                long long valor = atoll(lista_predicados[p].valor.c_str());

                if (lista_predicados[p].operador == "==") {
                    no<long long> *no = buscar(indice_dep, valor);

                    if (no) {
                        for (int k = 0; k < no->numero_indices; k++) {
                            if (tamanho_parcial >= capacidade_parcial) { // realoca se necessário
                                int novo_capacidade = capacidade_parcial * 2;
                                int *novo_arr = new int[novo_capacidade];

                                for (int l = 0; l < tamanho_parcial; l++) novo_arr[l] = lista_parcial[l];

                                delete[] lista_parcial;
                                lista_parcial = novo_arr;
                                capacidade_parcial = novo_capacidade;
                            }

                            lista_parcial[tamanho_parcial++] = no->lista_indices[k];
                        }
                    }
                }
                else if (lista_predicados[p].operador == "<=") {
                    buscar_intervalo(indice_dep, valor, true, &lista_parcial, tamanho_parcial, capacidade_parcial);
                }
                else if (lista_predicados[p].operador == ">=") {
                    buscar_intervalo(indice_dep, valor, false, &lista_parcial, tamanho_parcial, capacidade_parcial);
                }
            }
            else if (lista_predicados[p].atributo == "arr") { // se o atributo é tempo de chegada
                long long valor = atoll(lista_predicados[p].valor.c_str());

                if (lista_predicados[p].operador == "==") {
                    no<long long> *no = buscar(indice_arr, valor);

                    if (no) {
                        for (int k = 0; k < no->numero_indices; k++) {
                            if (tamanho_parcial >= capacidade_parcial) { // realoca se necessário
                                int novo_capacidade = capacidade_parcial * 2;
                                int *novo_arr = new int[novo_capacidade];

                                for (int l = 0; l < tamanho_parcial; l++) novo_arr[l] = lista_parcial[l];

                                delete[] lista_parcial;
                                lista_parcial = novo_arr;
                                capacidade_parcial = novo_capacidade;
                            }

                            lista_parcial[tamanho_parcial++] = no->lista_indices[k];
                        }
                    }
                }
                else if (lista_predicados[p].operador == "<=") {
                    buscar_intervalo(indice_arr, valor, true, &lista_parcial, tamanho_parcial, capacidade_parcial);
                }
                else if (lista_predicados[p].operador == ">=") {
                    buscar_intervalo(indice_arr, valor, false, &lista_parcial, tamanho_parcial, capacidade_parcial);
                }
            }
            else if (lista_predicados[p].atributo == "dur") { // se o atributo é duração
                long long valor = atoll(lista_predicados[p].valor.c_str());

                if (lista_predicados[p].operador == "==") {
                    no<long long> *no = buscar(indice_dur, valor);
                    if (no) {
                        for (int k = 0; k < no->numero_indices; k++) {
                            if (tamanho_parcial >= capacidade_parcial) { // realoca se necessário
                                int novo_capacidade = capacidade_parcial * 2;
                                int *novo_arr = new int[novo_capacidade];

                                for (int l = 0; l < tamanho_parcial; l++) novo_arr[l] = lista_parcial[l];

                                delete[] lista_parcial;
                                lista_parcial = novo_arr;
                                capacidade_parcial = novo_capacidade;
                            }

                            lista_parcial[tamanho_parcial++] = no->lista_indices[k];
                        }
                    }
                }
                else if (lista_predicados[p].operador == "<=") {
                    buscar_intervalo(indice_dur, valor, true, &lista_parcial, tamanho_parcial, capacidade_parcial);
                }
                else if (lista_predicados[p].operador == ">=") {
                    buscar_intervalo(indice_dur, valor, false, &lista_parcial, tamanho_parcial, capacidade_parcial);
                }
            }

            int *intersecao = new int[tamanho_atual < tamanho_parcial ? tamanho_atual : tamanho_parcial];
            int novo_tamanho = 0;

            for (int i = 0; i < tamanho_atual; i++) {
                for (int j = 0; j < tamanho_parcial; j++) {
                    if (conjunto_atual[i] == lista_parcial[j]) { // realiza a intersecção dos índices
                        intersecao[novo_tamanho++] = conjunto_atual[i];
                        break;
                    }
                }
            }

            delete[] conjunto_atual; // libera memória do conjunto anterior
            delete[] lista_parcial; // libera memória da lista parcial
            conjunto_atual = intersecao; // atualiza o conjunto com a intersecção
            tamanho_atual = novo_tamanho; // atualiza o tamanho do conjunto
        }

        delete[] lista_predicados; // libera os predicados
        voo *lista_resultados = new voo[tamanho_atual]; // aloca vetor para os voos resultantes

        for (int ires = 0; ires < tamanho_atual; ires++) {
            lista_resultados[ires] = lista_voos[conjunto_atual[ires]]; // preenche os resultados com os voos filtrados
        }

        insertionsort(lista_resultados, tamanho_atual, criterio_ordem); // ordena os resultados conforme o critério
        
        for (int ires = 0; ires < tamanho_atual && ires < max_resultados; ires++) {
            // imprime os resultados conforme o limite máximo
            cout << lista_resultados[ires].origem << " "
                 << lista_resultados[ires].destino << " "
                 << lista_resultados[ires].preco << " "
                 << lista_resultados[ires].assentos_disponiveis << " "
                 << lista_resultados[ires].data_hora_partida << " "
                 << lista_resultados[ires].data_hora_chegada << " "
                 << lista_resultados[ires].numero_paradas << endl;
        }

        delete[] lista_resultados; // libera os resultados
        delete[] conjunto_atual; // libera o conjunto de índices
    }

    delete[] lista_voos; // libera a lista de voos

    // libera as árvores avl utilizadas como índices
    liberar_arvore(indice_org);
    liberar_arvore(indice_dst);
    liberar_arvore(indice_prc);
    liberar_arvore(indice_sea);
    liberar_arvore(indice_sto);
    liberar_arvore(indice_dep);
    liberar_arvore(indice_arr);
    liberar_arvore(indice_dur);

    return 0; // encerra o programa com sucesso
}
