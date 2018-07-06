#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

typedef struct listaArquivos {
    char *nomeArquivo;
    struct listaArquivos *proximoArquivo;
} listaArquivos;
typedef struct listaArquivos* Arquivo;

typedef unsigned long int Codigo;

typedef struct registro {
    char * arquivo;
    ssize_t linha;
    struct registro * proximoRegistro;
} registro;
typedef registro * Registro;

typedef struct funcionario {
    Codigo codigo;
    double rendimentos;
    int numRegistros;
    Registro pilhaRegistros;
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
        printf("Codigo: %zu, Registros: %d, Rendimentos: %.2f\n", arvore->funcionario->codigo, arvore->funcionario->numRegistros, arvore->funcionario->rendimentos);
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

Registro ultimoRegistro(Registro reg) {
    if (reg) {
        while (reg->proximoRegistro) {
            reg = reg->proximoRegistro;
        }
    }
    return reg;
}

No carregarRegistros(Arquivo listaArquivos, size_t limite) {

    FILE *arquivo;
    No arvore = NULL;
    char *linha = NULL, *token = NULL, *separador = "\t";
    int numArquivo = 0;

    while (listaArquivos) {

        numArquivo += 1;

        char *nomeArquivo = listaArquivos->nomeArquivo;

        ssize_t caracteres = 0;
        size_t len = 0, numLinha = 0;

        arquivo = fopen(nomeArquivo, "r");
        if (arquivo) {
            printf("\nProcessando o arquivo %s...", nomeArquivo);
        } else {
            printf("\nO arquivo %s nao pode ser aberto!\n\n", nomeArquivo);
            return NULL;
        }

        while (((caracteres = getline(&linha, &len, arquivo)) != -1) && (numLinha < limite)) {

            numLinha += 1;

            //Remove o cabecalho

            if (numLinha > 1) {

                if ((numLinha % 500) == 0) {
                    printf("%zu...", numLinha);
                }

                token = strtok(linha, separador);

                Codigo codigo;
                double rendimentos = 0.0;
                int num_campo = 0;

                while (token != NULL) {

                    num_campo += 1;

                    //printf(" %s\n", token);
                    switch (num_campo) {
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

                    token = strtok(NULL, separador);
                }

                //printf("Buscanco funcionario...\n");

                Registro reg = (Registro) malloc(sizeof (registro));

                if (reg) {
                    reg->arquivo = nomeArquivo;
                    reg->linha = numLinha;
                    reg->proximoRegistro = NULL;
                }

                No no_funcionario = buscarNo(codigo, arvore);

                if (no_funcionario == NULL) {
                    no_funcionario = (No) malloc(sizeof (no));
                    no_funcionario->funcionario = (Funcionario) malloc(sizeof (funcionario));
                    no_funcionario->funcionario->codigo = codigo;
                    no_funcionario->funcionario->rendimentos = rendimentos;
                    no_funcionario->funcionario->numRegistros = 1;
                    no_funcionario->funcionario->pilhaRegistros = reg;
                    no_funcionario->direito = NULL;
                    no_funcionario->esquerdo = NULL;
                    inserirNo(no_funcionario, &arvore);
                    //inserirNoPorRendimento(no_funcionario, &arvore);
                } else {
                    no_funcionario->funcionario->rendimentos += rendimentos;
                    no_funcionario->funcionario->numRegistros += 1;
                    ultimoRegistro(no_funcionario->funcionario->pilhaRegistros)->proximoRegistro = reg;
                }

                reg = NULL;
                no_funcionario = NULL;

            }

        }

        printf("[%zu]!", numLinha);

        if (arquivo) {
            fclose(arquivo);
            arquivo = NULL;
        }
        arquivo = NULL;
        if (linha) {
            free(linha);
            linha = NULL;
        }

        listaArquivos = listaArquivos->proximoArquivo;
    }

    return arvore;
}

Arquivo ultimoArquivo(Arquivo lista) {
    if (lista) {
        while (lista->proximoArquivo) {
            lista = lista->proximoArquivo;
        }
    }
    return lista;
}

int main(int argc, char** argv) {

    Arquivo listaArquivos = NULL;
    size_t limite = 10;

    if (argc != 2) {
        printf("\nInforme o arquivo contendo o caminho para os arquivos que serao analisados!\n");
        exit(EXIT_FAILURE);
    }

    FILE *arquivo = fopen(argv[1], "r");

    if (arquivo == NULL) {
        printf("\nErro ao abrir arquivo \'%s\'!\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    char *linha = NULL;
    size_t len = 0;
    ssize_t caracteres = 0;

    while (caracteres = getline(&linha, &len, arquivo) != -1) {

        int ultimoCaractere = strlen(linha) - 1;

        if (linha[ultimoCaractere] == '\n') {
            linha[ultimoCaractere] = '\0';
        }

        Arquivo arq = (Arquivo) malloc(sizeof (listaArquivos));
        char *str = (char*) malloc(sizeof (char)*(strlen(linha)));
        strncpy(str, linha, strlen(linha));
        arq->nomeArquivo = str;
        arq->proximoArquivo = NULL;

        Arquivo aux = ultimoArquivo(listaArquivos);

        if (aux) {
            aux->proximoArquivo = arq;
        } else {
            listaArquivos = arq;
        }

    }

    if (arquivo) {
        free(arquivo);
        arquivo = NULL;
    }

    No arvore = carregarRegistros(listaArquivos, limite);

    if (arvore) {

        printf("\n\nFuncionarios:\n\n");
        imprimirArvoreCrescente(arvore);
        No arvoreOrdenadaPorRendimentos = NULL;

        printf("\n\nFuncionarios ordenados:\n\n");

        ordenarPorRendimento(arvore, &arvoreOrdenadaPorRendimentos);
        imprimirArvoreDecrescente(arvoreOrdenadaPorRendimentos);

        printf("\n\nConcluido!\n\n");

    }

    return (EXIT_SUCCESS);
}

