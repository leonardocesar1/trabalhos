#include <stdio.h>
#include <stdlib.h>

#define MAX_A 50
#define MAX_R 2500

int ordena(const void *x, const void *y);
void leitura(int A[], int *elementos, int R[][2], int *pares);
int busca(int R[][2], int pares, int x, int y);

int reflexiva(int A[], int elementos, int R[][2], int pares);
void imprimereflexiva(int A[], int elementos, int R[][2], int pares);
int irreflexiva(int A[], int elementos, int R[][2], int pares);
void imprimeirreflexiva(int A[], int elementos, int R[][2], int pares);
int simetrica(int R[][2], int pares);
void imprimesimetrica(int R[][2], int pares);
int antisimetrica(int R[][2], int pares);
void imprimeantisimetrica(int R[][2], int pares);
int transitiva(int R[][2], int pares);
void imprimetransitiva(int R[][2], int pares);

void imprimefechoreflexivo(int A[], int elementos, int R[][2], int pares);
void imprimefechosimetrico(int R[][2], int pares);
void imprimefechotransitivo(int R[][2], int pares);

void magia(int A[], int elementos, int R[][2], int pares);

int main() {
    int A[MAX_A];
    int R[MAX_R][2];
    int elementos, pares;

    leitura(A, &elementos, R, &pares);

    magia(A, elementos, R, pares);

    return 0;
}

int ordena(const void *x, const void *y) {
    int *xaux = (int *)x;
    int *yaux = (int *)y;

    if(xaux[0] != yaux[0]) {
        return xaux[0] - yaux[0];
    } else {
        return xaux[1] - yaux[1];
    }
}

void leitura(int A[], int *elementos, int R[][2], int *pares) {
    scanf("%d", elementos);

    for(int i = 0; i < *elementos; i++) {
        scanf("%d", &A[i]);
    }

    for(*pares = 0; scanf("%d %d", &R[*pares][0], &R[*pares][1]) != EOF; (*pares)++) {}

    qsort(R, *pares, sizeof(int)*2, ordena);

    printf("Propriedades\n");
}

int busca(int R[][2], int pares, int x, int y) {
    for(int i = 0; i < pares; i++) {
        if(R[i][0] == x && R[i][1] == y) {
            return 1;
        }
    }
    return 0;
}

int reflexiva(int A[], int elementos, int R[][2], int pares) {
    for(int i = 0; i < elementos; i++) {
        if(!busca(R, pares, A[i], A[i])) {
            return 0;
        }
    }
    return 1;
}

void imprimereflexiva(int A[], int elementos, int R[][2], int pares) {
    printf("1. Reflexiva: %s\n", reflexiva(A, elementos, R, pares) ? "V" : "F");
    if(!reflexiva(A, elementos, R, pares)) {
        int aux = 1;
        for(int i = 0; i < elementos; i++) {
            if(!busca(R, pares, A[i], A[i])) {
                if(!aux) {
                    printf(", ");
                }
                aux = 0;
                printf("(%d, %d)", A[i], A[i]);
            }
        }
        printf("\n");
    }
}

int irreflexiva(int A[], int elementos, int R[][2], int pares) {
    for(int i = 0; i < elementos; i++) {
        if(busca(R, pares, A[i], A[i])) {
            return 0;
        }
    }
    return 1;
}

void imprimeirreflexiva(int A[], int elementos, int R[][2], int pares) {
    printf("2. Irreflexiva: %s\n", irreflexiva(A, elementos, R, pares) ? "V" : "F");
    if(!irreflexiva(A, elementos, R, pares)) {
        int aux = 1;
        for(int i = 0; i < elementos; i++) {
            if(busca(R, pares, A[i], A[i])) {
                if(!aux) {
                    printf(", ");
                }
                aux = 0;
                printf("(%d, %d)", A[i], A[i]);
            }
        }
        printf("\n");
    }
}

int simetrica(int R[][2], int pares) {
    for(int i = 0; i < pares; i++) {
        if(!busca(R, pares, R[i][1], R[i][0])) {
            return 0;
        }
    }
    return 1;
}

void imprimesimetrica(int R[][2], int pares) {
    printf("3. Simetrica: %s\n", simetrica(R, pares) ? "V" : "F");
    if(!simetrica(R, pares)) {
        int Raux[MAX_R][2];
        int paresaux = 0;

        for(int i = 0; i < pares; i++) {
            if(!busca(R, pares, R[i][1], R[i][0])) {
                Raux[paresaux][0] = R[i][1];
                Raux[paresaux][1] = R[i][0];
                paresaux++;
            }
        }

        qsort(Raux, paresaux, sizeof(int) * 2, ordena);

        int aux = 1;
        for(int i = 0; i < paresaux; i++) {
            if(!aux) {
                printf(", ");
            }
            aux = 0;
            printf("(%d, %d)", Raux[i][0], Raux[i][1]);
        }
        printf("\n");
    }
}

int antisimetrica(int R[][2], int pares) {
    for(int i = 0; i < pares; i++) {
        if(busca(R, pares, R[i][1], R[i][0]) && R[i][0] != R[i][1]) {
            return 0;
        }
    }
    return 1;
}

void imprimeantisimetrica(int R[][2], int pares) {
    printf("4. Anti-simetrica: %s\n", antisimetrica(R, pares) ? "V" : "F");
    if(!antisimetrica(R, pares)) {
        int aux = 1;
        for(int i = 0; i < pares; i++) {
            if(busca(R, pares, R[i][1], R[i][0]) && R[i][0] != R[i][1] && R[i][0] < R[i][1]) {
                if(!aux) {
                    printf(", ");
                }
                aux = 0;
                printf("((%d, %d), (%d, %d))", R[i][0], R[i][1], R[i][1], R[i][0]);
            }
        }
        printf("\n");
    }
}

int transitiva(int R[][2], int pares) {
    for(int i = 0; i < pares; i++) {
        for(int j = 0; j < pares; j++) {
            if(R[i][1] == R[j][0]) {
                if(!busca(R, pares, R[i][0], R[j][1])) {
                    return 0;
                }
            }
        }
    }
    return 1;
}

void imprimetransitiva(int R[][2], int pares) {
    printf("6. Transitiva: %s\n", transitiva(R, pares) ? "V" : "F");
    if(!transitiva(R, pares)) {
        int Raux[MAX_R][2];
        int paresaux = 0;

        for(int i = 0; i < pares; i++) {
            for(int j = 0; j < pares; j++) {
                if(R[i][1] == R[j][0]) {
                    if(!busca(R, pares, R[i][0], R[j][1]) && !busca(Raux, paresaux, R[i][0], R[j][1])) {
                        Raux[paresaux][0] = R[i][0];
                        Raux[paresaux][1] = R[j][1];
                        paresaux++;
                    }
                }
            }
        }

        qsort(Raux, paresaux, sizeof(int)*2, ordena);

        int aux = 1;
        for(int i = 0; i < paresaux; i++) {
            if(!aux) {
                printf(", ");
            }
            aux = 0;
            printf("(%d, %d)", Raux[i][0], Raux[i][1]);
        }
        printf("\n");
    }
}

void imprimefechoreflexivo(int A[], int elementos, int R[][2], int pares) {
    if(reflexiva(A, elementos, R, pares)) {
        printf("Fecho reflexivo da relacao: R\n");
    } else {
        printf("Fecho reflexivo da relacao:\n");
        int Raux[MAX_R][2];

        for(int i = 0; i < pares; i++) {
            Raux[i][0] = R[i][0];
            Raux[i][1] = R[i][1];
        }

        for(int i = 0; i < elementos; i++) {
            if(!busca(R, pares, A[i], A[i])) {
                Raux[pares][0] = A[i];
                Raux[pares][1] = A[i];
                pares++;
            }
        }

        qsort(Raux, pares, sizeof(int)*2, ordena);

        int aux = 1;
        for(int i = 0; i < pares; i++) {
            if(!aux) {
                printf(", ");
            }
            aux = 0;
            printf("(%d, %d)", Raux[i][0], Raux[i][1]);
        }
        printf("\n");
    }
}

void imprimefechosimetrico(int R[][2], int pares) {
    if(simetrica(R, pares)) {
        printf("Fecho simetrico da relacao: R\n");
    } else {
        printf("Fecho simetrico da relacao:\n");
        int Raux[MAX_R][2];

        for(int i = 0; i < pares; i++) {
            Raux[i][0] = R[i][0];
            Raux[i][1] = R[i][1];
        }

        for(int i = 0; i < pares; i++) {
            if(!busca(R, pares, R[i][1], R[i][0])) {
                Raux[pares][0] = R[i][1];
                Raux[pares][1] = R[i][0];
                pares++;
            }
        }

        qsort(Raux, pares, sizeof(int)*2, ordena);

        int aux = 1;
        for(int i = 0; i < pares; i++) {
            if(!aux) {
                printf(", ");
            }
            aux = 0;
            printf("(%d, %d)", Raux[i][0], Raux[i][1]);
        }
        printf("\n");
    }
}

void imprimefechotransitivo(int R[][2], int pares) {
    if(transitiva(R, pares)) {
        printf("Fecho transitivo da relacao: R\n");
    } else {
        printf("Fecho transitivo da relacao:\n");
        int Raux[MAX_R][2];

        for(int i = 0; i < pares; i++) {
            Raux[i][0] = R[i][0];
            Raux[i][1] = R[i][1];
        }

        for(int i = 0; i < pares; i++) {
            for(int j = 0; j < pares; j++) {
                if(Raux[i][1] == Raux[j][0]) {
                    if(!busca(Raux, pares, Raux[i][0], Raux[j][1])) {
                        Raux[pares][0] = Raux[i][0];
                        Raux[pares][1] = Raux[j][1];
                        pares++;
                    }
                }
            }
        }

        qsort(Raux, pares, sizeof(int)*2, ordena);

        int aux = 1;
        for(int i = 0; i < pares; i++) {
            if(!aux) {
                printf(", ");
            }
            aux = 0;
            printf("(%d, %d)", Raux[i][0], Raux[i][1]);
        }
        printf("\n");
    }
}

void magia(int A[], int elementos, int R[][2], int pares) {
    imprimereflexiva(A, elementos, R, pares);

    imprimeirreflexiva(A, elementos, R, pares);

    imprimesimetrica(R, pares);

    imprimeantisimetrica(R, pares);

    printf("5. Assimetrica: %s\n", irreflexiva(A, elementos, R, pares) && antisimetrica(R, pares) ? "V" : "F");

    imprimetransitiva(R, pares);

    printf("\nRelacao de equivalencia: %s\n", (reflexiva(A, elementos, R, pares) && simetrica(R, pares) && transitiva(R, pares)) ? "V" : "F");
    printf("Relacao de ordem parcial: %s\n\n", (reflexiva(A, elementos, R, pares) && antisimetrica(R, pares) && transitiva(R, pares)) ? "V" : "F");

    imprimefechoreflexivo(A, elementos, R, pares);

    imprimefechosimetrico(R, pares);

    imprimefechotransitivo(R, pares);

    printf("\n");
}
