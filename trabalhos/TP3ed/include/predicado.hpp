#ifndef PREDICADO_HPP
#define PREDICADO_HPP

#include <iostream>

using namespace std;

// estrutura que define um predicado com atributo, operador e valor
struct predicado {
    string atributo; // atributo a ser analisado
    string operador; // operador para comparação
    string valor;    // valor a ser comparado
};

// função que retorna o comprimento de um texto
int comprimento(const string &texto);

// função que retorna uma substring de um texto
string substring(const string &s, int pos, int len);

// função que decompõe uma data e hora no formato de string e retorna como um valor numérico
long long decompor_data_hora(const string &data_hora);

// função que remove espaços de um texto
string retirar_espacos(const string &texto);

// função que cria um predicado a partir de um token
predicado criar_predicado(const string &token);

// função que decompõe uma expressão em uma lista de predicados
void decompor_predicados(const string &expressao, predicado **lista_predicados, int &quantidade_predicados);

#endif
