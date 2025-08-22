#include "predicado.hpp"

// função que retorna o comprimento de uma string
int comprimento(const string &texto) {
    const char *c = texto.c_str(); // converte a string para um array de caracteres
    int cont = 0;

    // conta o número de caracteres até encontrar o caractere nulo ('\0')
    while(c[cont] != '\0') cont++;

    return cont; // retorna o comprimento da string
}

// função que retorna uma substring de uma string a partir de uma posição e um comprimento
string substring(const string &s, int pos, int len) {
    string resultado = "";
    int tam = comprimento(s); // obtém o comprimento da string
    int fim = pos + len;

    // se o fim for maior que o tamanho da string, ajusta o fim
    if (fim > tam) {
        fim = tam;
    }

    // cria a substring adicionando os caracteres da posição inicial até a posição final
    for (int i = pos; i < fim; i++) {
        resultado = resultado + s[i];
    }

    return resultado; // retorna a substring
}

// função que converte uma string no formato de data e hora para segundos
long long decompor_data_hora(const string &data_hora) {
    int ano = stoi(substring(data_hora, 0, 4)); // obtém o ano
    int mes = stoi(substring(data_hora, 5, 2)); // obtém o mês
    int dia = stoi(substring(data_hora, 8, 2)); // obtém o dia
    int hora = stoi(substring(data_hora, 11, 2)); // obtém a hora
    int minuto = stoi(substring(data_hora, 14, 2)); // obtém os minutos
    int segundo = stoi(substring(data_hora, 17, 2)); // obtém os segundos
    int offset_segundos = 0;

    // verifica se há fuso horário na string e ajusta o offset
    if (comprimento(data_hora) > 19) {
        char sinal_fuso = data_hora[19];
        int fuso_hora = stoi(substring(data_hora, 20, 2));
        int fuso_minuto = stoi(substring(data_hora, 23, 2));
        offset_segundos = fuso_hora * 3600 + fuso_minuto * 60;

        if (sinal_fuso == '+') offset_segundos = -offset_segundos;
    }

    // cálculos para ajustar a data
    int ajuste = (14 - mes) / 12;
    int ano_ajustado = ano + 4800 - ajuste;
    int mes_ajustado = mes + 12 * ajuste - 3;
    int dia_juliano = dia + (153 * mes_ajustado + 2) / 5 + 365 * ano_ajustado + ano_ajustado/4 - ano_ajustado/100 + ano_ajustado/400 - 32045;
    int dias_epoca = dia_juliano - 2440588;
    long long segundos_locais = (long long)dias_epoca * 86400 + hora * 3600 + minuto * 60 + segundo;
    return segundos_locais + offset_segundos; // retorna o tempo em segundos ajustado
}

// função para remover espaços em branco e quebras de linha do início e do fim de uma string
string retirar_espacos(const string &texto) {
    int inicio = 0;

    // pula os espaços iniciais e quebras de linha
    while (inicio < comprimento(texto) && ((texto[inicio] == ' ')  || (texto[inicio] == '\r'))) {
        inicio++;
    }

    if (inicio == comprimento(texto)) return ""; // se não houver texto, retorna uma string vazia

    int fim = comprimento(texto) - 1;

    // pula os espaços finais e quebras de linha
    while (fim >= inicio && ((texto[fim] == ' ')  || (texto[fim] == '\r'))) {
        fim--;
    }

    return substring(texto, inicio, fim - inicio + 1); // retorna a substring sem os espaços
}

// função que cria um predicado a partir de um token de expressão
predicado criar_predicado(const string &token) {
    predicado pred;
    int tamanho = comprimento(token);
    int posicao = -1;
    string op = "";

    // verifica qual operador está presente e encontra sua posição
    for (int i = 0; i < tamanho - 1; i++) {
        if (token[i] == '>' && token[i+1] == '=') {
            posicao = i;
            op = ">=";
            break;
        }
    }

    if (posicao == -1) {
        for (int i = 0; i < tamanho - 1; i++) {
            if (token[i] == '<' && token[i+1] == '=') {
                posicao = i;
                op = "<=";
                break;
            }
        }
    }

    if (posicao == -1) {
        for (int i = 0; i < tamanho - 1; i++) {
            if (token[i] == '=' && token[i+1] == '=') {
                posicao = i;
                op = "==";
                break;
            }
        }
    }

    // caso o operador seja encontrado, separa a string em atributo e valor
    if (posicao != -1) {
        string attr = substring(token, 0, posicao);
        string val = substring(token, posicao + 2, tamanho - (posicao + 2));
        pred.atributo = retirar_espacos(attr);
        pred.operador = op;
        pred.valor = retirar_espacos(val);
    }

    return pred; // retorna o predicado criado
}

// função para decompor uma expressão em vários predicados
void decompor_predicados(const string &expressao, predicado **lista_predicados, int &quantidade_predicados) {
    string resumo = "";
    int tamanho = comprimento(expressao);

    // remove os parênteses da expressão
    for (int i = 0; i < tamanho; i++) {
        if (expressao[i] != '(' && expressao[i] != ')')
            resumo += expressao[i];
    }

    quantidade_predicados = 0;
    bool auxiliar = false;
    tamanho = comprimento(resumo);

    // conta quantos predicados existem na expressão (separados por '&&')
    for (int i = 0; i < tamanho; i++) {
        if (i < tamanho - 1 && resumo[i] == '&' && resumo[i+1] == '&') {
            quantidade_predicados++;
            auxiliar = false;
            i++;
        }
        else {
            auxiliar = true;
        }
    }

    if (auxiliar) quantidade_predicados++;
    *lista_predicados = new predicado[quantidade_predicados]; // aloca memória para os predicados
    int indice = 0;
    string token = "";

    // separa os predicados e os armazena na lista
    for (int i = 0; i < tamanho; i++) {
        if (i < tamanho - 1 && resumo[i] == '&' && resumo[i+1] == '&') {
            token = retirar_espacos(token);
            (*lista_predicados)[indice++] = criar_predicado(token);
            token = "";
            i++;
        } else {
            token += resumo[i];
        }
    }

    // processa o último token se houver
    if (comprimento(token) != 0) {
        token = retirar_espacos(token);
        (*lista_predicados)[indice++] = criar_predicado(token);
    }
}
