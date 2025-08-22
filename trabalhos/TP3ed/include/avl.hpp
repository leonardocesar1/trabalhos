#ifndef AVL_HPP
#define AVL_HPP

// estrutura para um nó de uma árvore AVL, que armazena uma chave, índices e ponteiros para os filhos
template <typename t>
struct no {
    t chave; // chave armazenada no nó
    int *lista_indices; // lista de índices associados à chave
    int numero_indices; // número de índices armazenados
    int capacidade_indices; // capacidade máxima da lista de índices
    no<t> *esquerda; // ponteiro para o filho à esquerda
    no<t> *direita; // ponteiro para o filho à direita
    int altura; // altura do nó
};

// função para retornar o valor máximo entre dois números
int max(int a, int b);

// função para calcular a altura de um nó
template <typename t>
int altura(no<t> *no) {
    return no ? no->altura : 0;
}

// função para criar um novo nó com a chave e o índice do voo
template <typename t>
no<t>* novo(t chave, int indice_voo) {
    no<t> *novo = new no<t>;
    novo->chave = chave;
    novo->capacidade_indices = 2; // capacidade inicial para 2 índices
    novo->lista_indices = new int[novo->capacidade_indices];
    novo->lista_indices[0] = indice_voo;
    novo->numero_indices = 1; // começa com um índice
    novo->esquerda = novo->direita = 0; // filhos são nulos inicialmente
    novo->altura = 1; // altura inicial é 1
    return novo;
}

// função para adicionar um índice à lista de índices do nó
template <typename t>
void adicionar_indice(no<t> *no, int indice_voo) {
    if (no->numero_indices >= no->capacidade_indices) {
        int nova_capacidade = no->capacidade_indices * 2;
        int *novo_arr = new int[nova_capacidade];

        // copia os índices antigos para o novo array
        for (int i = 0; i < no->numero_indices; i++) novo_arr[i] = no->lista_indices[i];

        delete[] no->lista_indices;
        no->lista_indices = novo_arr;
        no->capacidade_indices = nova_capacidade;
    }

    no->lista_indices[no->numero_indices++] = indice_voo; // adiciona o novo índice
}

// função para fazer rotação à direita em um nó
template <typename t>
no<t>* rotacao_direita(no<t> *y) {
    no<t> *x = y->esquerda;
    no<t> *T2 = x->direita;
    x->direita = y;
    y->esquerda = T2;
    y->altura = max(altura(y->esquerda), altura(y->direita)) + 1;
    x->altura = max(altura(x->esquerda), altura(x->direita)) + 1;
    return x; // retorna a nova raiz após a rotação
}

// função para fazer rotação à esquerda em um nó
template <typename t>
no<t>* rotacao_esquerda(no<t> *x) {
    no<t> *y = x->direita;
    no<t> *T2 = y->esquerda;
    y->esquerda = x;
    x->direita = T2;
    x->altura = max(altura(x->esquerda), altura(x->direita)) + 1;
    y->altura = max(altura(y->esquerda), altura(y->direita)) + 1;
    return y; // retorna a nova raiz após a rotação
}

// função para calcular o fator de balanceamento de um nó
template <typename t>
int fator(no<t> *no) {
    if (!no) return 0;
    return altura(no->esquerda) - altura(no->direita); // diferença entre as alturas dos filhos
}

// função para inserir um nó com uma chave e índice em uma árvore AVL
template <typename t>
no<t>* inserir(no<t> *no, t chave, int indice_voo) {
    if (!no) return novo(chave, indice_voo); // se o nó é nulo, cria um novo nó

    // insere a chave no nó correto da árvore
    if (chave == no->chave) adicionar_indice(no, indice_voo);
    else if (chave < no->chave) no->esquerda = inserir(no->esquerda, chave, indice_voo);
    else no->direita = inserir(no->direita, chave, indice_voo);
    
    // atualiza a altura do nó
    no->altura = 1 + max(altura(no->esquerda), altura(no->direita));
    
    // verifica o fator de balanceamento e aplica rotações se necessário
    int balanceamento = fator(no);
    
    // caso de desbalanceamento à esquerda
    if (balanceamento > 1 && chave < no->esquerda->chave) return rotacao_direita(no);

    // caso de desbalanceamento à direita
    if (balanceamento < -1 && chave > no->direita->chave) return rotacao_esquerda(no);

    // caso de desbalanceamento à esquerda-direita
    if (balanceamento > 1 && chave > no->esquerda->chave) {
        no->esquerda = rotacao_esquerda(no->esquerda);
        return rotacao_direita(no);
    }

    // caso de desbalanceamento à direita-esquerda
    if (balanceamento < -1 && chave < no->direita->chave) {
        no->direita = rotacao_direita(no->direita);
        return rotacao_esquerda(no);
    }

    return no;
}

// função para buscar um nó com uma chave na árvore
template <typename t>
no<t>* buscar(no<t> *no, t chave) {
    if (!no) return nullptr; // se o nó for nulo, retorna nullptr

    if (chave == no->chave) return no; // se encontrar a chave, retorna o nó
    else if (chave < no->chave) return buscar(no->esquerda, chave); // busca à esquerda
    else return buscar(no->direita, chave); // busca à direita
}

// função para buscar um intervalo de valores na árvore
template <typename t>
void buscar_intervalo(no<t> *no, t valor, bool menor_igual, int **lista, int &total, int &capacidade) {
    if (!no) return;

    buscar_intervalo(no->esquerda, valor, menor_igual, lista, total, capacidade); // busca na subárvore esquerda

    bool adicionar = (menor_igual && no->chave <= valor) || (!menor_igual && no->chave >= valor); // verifica se o nó deve ser adicionado ao intervalo

    if (adicionar) {
        // adiciona os índices do nó à lista
        for (int i = 0; i < no->numero_indices; i++) {
            if (total >= capacidade) { // se a lista estiver cheia, aumenta a capacidade
                int novo_capacidade = capacidade * 2;
                int *novo_arr = new int[novo_capacidade];

                for (int j = 0; j < total; j++) novo_arr[j] = (*lista)[j];

                delete[] *lista;
                *lista = novo_arr;
                capacidade = novo_capacidade;
            }

            (*lista)[total++] = no->lista_indices[i];
        }
    }
    
    buscar_intervalo(no->direita, valor, menor_igual, lista, total, capacidade); // busca na subárvore direita
}

// função para liberar a memória da árvore
template <typename t>
void liberar_arvore(no<t>* raiz) {
    if (!raiz) return;

    liberar_arvore(raiz->esquerda); // libera a subárvore esquerda
    liberar_arvore(raiz->direita); // libera a subárvore direita
    delete[] raiz->lista_indices; // libera a lista de índices
    delete raiz; // libera o nó
}

#endif
