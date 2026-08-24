#ifndef VOO_HPP
#define VOO_HPP

#include <iostream>

using namespace std;

// estrutura que define as informações de um voo
struct voo {
    string origem;  // cidade de origem
    string destino; // cidade de destino
    double preco;   // preço do voo
    int assentos_disponiveis;  // número de assentos disponíveis
    string data_hora_partida; // data e hora de partida
    string data_hora_chegada; // data e hora de chegada
    int numero_paradas; // número de paradas no voo
    long long tempo_partida;  // tempo da partida
    long long tempo_chegada;  // tempo da chegada
    long long duracao;  // duração do voo
};

// função para comparar dois voos com base em um critério
bool comparar_voos(const voo &a, const voo &b, const string &criterio);

// função para trocar as informações de dois voos
void trocar_voos(voo &a, voo &b);

// função que ordena uma lista de voos com base em um critério
void insertionsort(voo *lista, int n, const string &criterio);

#endif
