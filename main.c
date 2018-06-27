#include <stdio.h>
#include <stdlib.h>

typedef unsigned long int Codigo;

typedef struct funcionario {
    Codigo codigo;
    double rendimentos;
} funcionario;
typedef funcionario * Funcionario;

typedef struct no {
    struct no *direito;
    struct no *esquerdo;
    Funcionario funcionario;
} no;
typedef no *No;

No buscarNo(Codigo c, No arvore) {
    if (arvore == NULL) {
        return NULL;
    } else if (arvore->funcionario->codigo > c) {
        return buscarNo(c, arvore->esquerdo);
    } else if (arvore->funcionario->codigo < c) {
        return buscarNo(c, arvore->direito);
    } else if (arvore->funcionario->codigo == c) {
        return arvore;
    }
}

No inserirNo(No no_funcionario, No *arvore) {
    if (*arvore == NULL) {
        *arvore = no_funcionario;
        return *arvore;
    } else if ((*arvore)->funcionario->codigo > no_funcionario->funcionario->codigo) {
        return inserirNo(no_funcionario, &((*arvore)->esquerdo));
    } else if ((*arvore)->funcionario->codigo <= no_funcionario->funcionario->codigo) {
        return inserirNo(no_funcionario, &((*arvore)->direito));
    }
}

void inserirNoPorRendimento(No no_funcionario, No *arvore) {
    if (*arvore == NULL) {
        *arvore = no_funcionario;
    } else if ((*arvore)->funcionario->rendimentos > no_funcionario->funcionario->rendimentos) {
        inserirNoPorRendimento(no_funcionario, &((*arvore)->esquerdo));
    } else if ((*arvore)->funcionario->rendimentos <= no_funcionario->funcionario->rendimentos) {
        inserirNoPorRendimento(no_funcionario, &((*arvore)->direito));
    }
}

void ordenarPorRendimento(No arvore, No *arvoreOrdenada) {
    if (arvore != NULL) {
        ordenarPorRendimento(arvore->esquerdo, arvoreOrdenada);
        ordenarPorRendimento(arvore->direito, arvoreOrdenada);
        arvore->direito = NULL;
        arvore->esquerdo = NULL;
        if (*arvoreOrdenada == NULL) {
            *arvoreOrdenada = arvore;
        } else {
            inserirNoPorRendimento(arvore, arvoreOrdenada);
        }
    }
}

void imprimirArvoreCrescente(No arvore) {
    if (arvore == NULL) {
        return;
    }
    imprimirArvoreCrescente(arvore->esquerdo);
    if (arvore->funcionario != NULL) {
        printf("Codigo: %zu, Rendimentos %.2f\n", arvore->funcionario->codigo, arvore->funcionario->rendimentos);
    }
    imprimirArvoreCrescente(arvore->direito);
}

void imprimirArvoreDecrescente(No arvore) {
    if (arvore == NULL) {
        return;
    }
    imprimirArvoreDecrescente(arvore->direito);
    if (arvore->funcionario != NULL) {
        printf("Codigo: %zu, Rendimentos %.2f\n", arvore->funcionario->codigo, arvore->funcionario->rendimentos);
    }
    imprimirArvoreDecrescente(arvore->esquerdo);
}

int main(int argc, char** argv) {

    FILE *arquivo;
    char *linha = NULL;
    size_t len = 0;
    ssize_t tamanhoDaLinha;
    int limite = 0;
    char *token;
    char *separador = "\t";
    No arvore = NULL;

    arquivo = fopen("/tmp/20180531_Remuneracao.csv", "r");
    if (arquivo == NULL)
        exit(EXIT_FAILURE);

    while (((tamanhoDaLinha = getline(&linha, &len, arquivo)) != -1) && (limite++ < 100000)) {

        //printf("Tamanho da linha %zu :\n", tamanhoDaLinha);
        //printf("%s", linha);

        if ((limite % 500) == 0) {
            printf("%d...", limite);
        }

        token = strtok(linha, separador);

        Codigo codigo;
        double rendimentos = 0.0;

        int num_campo = 1;
        while (token != NULL) {
            //printf(" %s\n", token);
            if (limite > 1) {
                switch (num_campo++) {
                    case 3:
                        //printf("Codigo: %s\n", token);
                        codigo = atol(token);
                        break;
                    case 10:
                    case 14:
                    case 16:
                    case 22:
                    case 32:
                    case 34:
                        //printf("Valor: %.2f\n", atof(token));
                        rendimentos += atof(token);
                        break;
                }
            }
            token = strtok(NULL, separador);
        }

        //printf("Buscanco funcionario...\n");

        No no_funcionario = buscarNo(codigo, arvore);

        if (no_funcionario == NULL) {
            no_funcionario = (No) malloc(sizeof (no));
            no_funcionario->funcionario = (Funcionario) malloc(sizeof (funcionario));
            no_funcionario->funcionario->codigo = codigo;
            no_funcionario->funcionario->rendimentos = rendimentos;
            no_funcionario->direito = NULL;
            no_funcionario->esquerdo = NULL;
            inserirNo(no_funcionario, &arvore);
            //inserirNoPorRendimento(no_funcionario, &arvore);
        } else {
            no_funcionario->funcionario->rendimentos += rendimentos;
        }


    }

    printf("%d...\n", limite);
    
    //imprimirArvoreCrescente(arvore);
    No arvoreOrdenadaPorRendimentos = NULL;
    ordenarPorRendimento(arvore, &arvoreOrdenadaPorRendimentos);
    imprimirArvoreDecrescente(arvoreOrdenadaPorRendimentos);

    fclose(arquivo);
    if (linha)
        free(linha);

    return (EXIT_SUCCESS);
}

