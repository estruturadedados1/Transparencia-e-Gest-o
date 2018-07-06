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

typedef int (*Comparacao)(No, No);

typedef enum {
    CRESCENTE, DECRESCENTE
} OrdemArvore;

/*
 * Campos do arquivo
 * 
 */

typedef enum {
    ANO,
    MES,
    ID_SERVIDOR_PORTAL,
    CPF,
    NOME,
    REMUNERACAO_BASICA_BRUTA_BRL,
    REMUNERACAO_BASICA_BRUTA_USD,
    ABATE_TETO_BRL,
    ABATE_TETO_USD,
    GRATIFICACAO_NATALINA_BRL,
    GRATIFICACAO_NATALINA_USD,
    ABATE_TETO_DA_GRATIFICACAO_NATALINA_BRL,
    ABATE_TETO_DA_GRATIFICACAO_NATALINA_USD,
    FERIAS_BRL,
    FERIAS_USD,
    OUTRAS_REMUNERACOES_EVENTUAIS_BRL,
    OUTRAS_REMUNERACOES_EVENTUAIS_USD,
    IRRF_BRL,
    IRRF_USD,
    PSS_RPGS_BRL,
    PSS_RPGS_USD,
    PENSAO_MILITAR_BRL,
    PENSAO_MILITAR_USD,
    FUNDO_DE_SAUDE_BRL,
    FUNDO_DE_SAUDE_USD,
    TAXA_DE_OCUPACAO_IMOVEL_FUNCIONAL_BRL,
    TAXA_DE_OCUPACAO_IMOVEL_FUNCIONAL_USD,
    DEMAIS_DEDUCOES_BRL,
    DEMAIS_DEDUCOES_USD,
    REMUNERACAO_APOS_DEDUCOES_OBRIGATORIAS_BRL,
    REMUNERACAO_APOS_DEDUCOES_OBRIGATORIAS_USD,
    VERBAS_INDENIZATORIAS_REGISTRADAS_EM_SISTEMAS_DE_PESSOAL_CIVIL_BRL,
    VERBAS_INDENIZATORIAS_REGISTRADAS_EM_SISTEMAS_DE_PESSOAL_CIVIL_USD,
    VERBAS_INDENIZATORIAS_REGISTRADAS_EM_SISTEMAS_DE_PESSOAL_MILITAR_BRL,
    VERBAS_INDENIZATORIAS_REGISTRADAS_EM_SISTEMAS_DE_PESSOAL_MILITAR_USD,
    VERBAS_INDENIZATORIAS_PROGRAMA_DESLIGAMENTO_VOLUNTARIO_MP_792_2017_BRL,
    VERBAS_INDENIZATORIAS_PROGRAMA_DESLIGAMENTO_VOLUNTARIO_MP_792_2017_USD,
    TOTAL_DE_VERBAS_INDENIZATORIAS_BRL,
    TOTAL_DE_VERBAS_INDENIZATORIAS_USD,
    TOTAL_DE_HONORARIOS_JETONS
}
Campos;

int compararPorCodigo(No n1, No n2) {
    if ((n1 != NULL) && (n2 != NULL)) {
        if ((n1->funcionario != NULL) && (n2->funcionario != NULL)) {
            if (n1->funcionario->codigo > n2->funcionario->codigo) {
                return -1;
            } else if (n1->funcionario->codigo < n2->funcionario->codigo) {
                return 1;
            } else {
                return 0;
            }
        } else {
            printf("\nERRO: Comparacao de nos com funcionarios nulos!\n");
            exit(EXIT_FAILURE);
        }
    } else {
        printf("\nERRO: Comparacao de nos nulos!\n");
        exit(EXIT_FAILURE);
    }
}

int compararPorRendimentos(No n1, No n2) {
    if ((n1 != NULL) && (n2 != NULL)) {
        if ((n1->funcionario != NULL) && (n2->funcionario != NULL)) {
            if (n1->funcionario->rendimentos > n2->funcionario->rendimentos) {
                return -1;
            } else if (n1->funcionario->rendimentos < n2->funcionario->rendimentos) {
                return 1;
            } else {
                return 0;
            }
        } else {
            printf("\nERRO: Comparacao de nos com funcionarios nulos!\n");
            exit(EXIT_FAILURE);
        }
    } else {
        printf("\nERRO: Comparacao de nos nulos!\n");
        exit(EXIT_FAILURE);
    }
}

No inserir(No no, No *arvore, Comparacao comparacao) {
    if (*arvore == NULL) {
        *arvore = no;
        return *arvore;
    } else if (comparacao((*arvore), no) < 0) {
        return inserir(no, &((*arvore)->esquerdo), comparacao);
    } else {
        return inserir(no, &((*arvore)->direito), comparacao);
    }
}

void ordenar(No arvore, No *arvoreOrdenada, Comparacao comparacao) {
    if (arvore != NULL) {
        ordenar(arvore->esquerdo, arvoreOrdenada, comparacao);
        ordenar(arvore->direito, arvoreOrdenada, comparacao);
        arvore->direito = NULL;
        arvore->esquerdo = NULL;
        if (*arvoreOrdenada == NULL) {
            *arvoreOrdenada = arvore;
        } else {
            inserir(arvore, arvoreOrdenada, comparacao);
        }
    }
}

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

void imprimir(No arvore, OrdemArvore ordem, int *qtde, FILE *saida) {
    if ((arvore == NULL) || ((qtde != NULL) && (*qtde) >= 0)) {
        return;
    }

    if (saida == NULL) {
        saida = stdout;
    }

    if (ordem == CRESCENTE) {
        imprimir(arvore->esquerdo, ordem, qtde, saida);
        if (arvore->funcionario != NULL) {
            if (qtde) (*qtde) -= 1;
            fprintf(saida, "Codigo: %zu, Registros: %d, Rendimentos: %.2f\n", arvore->funcionario->codigo, arvore->funcionario->numRegistros, arvore->funcionario->rendimentos);
        }
        imprimir(arvore->direito, ordem, qtde, saida);
    } else {
        imprimir(arvore->direito, ordem, qtde, saida);
        if (saida, arvore->funcionario != NULL) {
            if (qtde) (*qtde) -= 1;
            fprintf(saida, "Codigo: %zu, Registros: %d, Rendimentos: %.2f\n", arvore->funcionario->codigo, arvore->funcionario->numRegistros, arvore->funcionario->rendimentos);
        }
        imprimir(arvore->esquerdo, ordem, qtde, saida);
    }

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

                    switch (num_campo) {
                        case ID_SERVIDOR_PORTAL:
                            codigo = atol(token);
                            break;
                        case GRATIFICACAO_NATALINA_BRL:
                        case FERIAS_BRL:
                        case OUTRAS_REMUNERACOES_EVENTUAIS_BRL:
                        case PENSAO_MILITAR_BRL:
                        case VERBAS_INDENIZATORIAS_REGISTRADAS_EM_SISTEMAS_DE_PESSOAL_CIVIL_BRL:
                        case VERBAS_INDENIZATORIAS_REGISTRADAS_EM_SISTEMAS_DE_PESSOAL_MILITAR_BRL:
                            rendimentos += atof(token);
                            break;
                    }

                    token = strtok(NULL, separador);
                    num_campo += 1;

                }

                Registro reg = (Registro) malloc(sizeof (registro));

                if (reg) {
                    reg->arquivo = nomeArquivo;
                    reg->linha = numLinha;
                    reg->proximoRegistro = NULL;
                }

                No no = buscarNo(codigo, arvore);

                if (no == NULL) {
                    no = (No) malloc(sizeof (no));
                    no->funcionario = (Funcionario) malloc(sizeof (funcionario));
                    no->funcionario->codigo = codigo;
                    no->funcionario->rendimentos = rendimentos;
                    no->funcionario->numRegistros = 1;
                    no->funcionario->pilhaRegistros = reg;
                    no->direito = NULL;
                    no->esquerdo = NULL;
                    inserir(no, &arvore, &compararPorCodigo);
                } else {
                    no->funcionario->rendimentos += rendimentos;
                    no->funcionario->numRegistros += 1;
                    ultimoRegistro(no->funcionario->pilhaRegistros)->proximoRegistro = reg;
                }
                reg = NULL;
                no = NULL;

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

/* 
 * Opcoes:
 * Limite               --limite-registros=limite
 * Arquivo de saida     --arquivo-saida={path_do_arquivo}
 * Arquivo de entrada   --lista-arquivos={path_do_arquivo}
 * 
 */

#define LIMITE_REGISTROS "--limite-registros="
#define ARQUIVO_SAIDA  "--arquivo-saida="
#define ARQUIVO_ENTRADA "--lista-de-arquivos="

int main(int argc, char** argv) {

    Arquivo listaArquivos = NULL;
    size_t limite = -1;
    char *strArquivoSaida = NULL;
    char *strArquivoEntrada = NULL;
    FILE *arquivoSaida = NULL;
    FILE *arquivoListaEntrada = NULL;

    for (int i = 1; i < argc; i++) {
        if (strstr(argv[i], LIMITE_REGISTROS)) {
            limite = atoi(&(argv[i][sizeof (LIMITE_REGISTROS) - 1]));
            printf("\nLimite de registros por arquivo: %zu", limite);
        } else if (strstr(argv[i], ARQUIVO_ENTRADA)) {
            strArquivoEntrada = &(argv[i][sizeof (ARQUIVO_ENTRADA) - 1]);
            printf("\nLista de arquivos: %s", strArquivoEntrada);
        } else if (strstr(argv[i], ARQUIVO_SAIDA)) {
            strArquivoSaida = &(argv[i][sizeof (ARQUIVO_SAIDA) - 1]);
            printf("\nArquivo de Saida: %s", strArquivoSaida);
        } else {
            printf("\nArgumento invalido: \'%s\'!\n", argv[i]);
            exit(EXIT_FAILURE);
        }
    }

    if (strArquivoEntrada == NULL) {
        printf("\nInforme o arquivo contendo o caminho para os arquivos que serao analisados!\n");
        exit(EXIT_FAILURE);
    }

    if (strArquivoSaida != NULL) {

        arquivoSaida = fopen(strArquivoSaida, "w");

        if (arquivoSaida == NULL) {
            printf("\nErro ao escrever o arquivo \'%s\'!\n", strArquivoSaida);
            exit(EXIT_FAILURE);
        }

    }
    arquivoListaEntrada = fopen(strArquivoEntrada, "r");

    if (arquivoListaEntrada == NULL) {
        printf("\nErro ao abrir o arquivo \'%s\'!\n", strArquivoEntrada);
        exit(EXIT_FAILURE);
    }

    char *linha = NULL;
    size_t len = 0;
    ssize_t caracteres = 0;

    while (caracteres = getline(&linha, &len, arquivoListaEntrada) != -1) {

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

    if (arquivoListaEntrada) {
        free(arquivoListaEntrada);
        arquivoListaEntrada = NULL;
    }

    No arvore = carregarRegistros(listaArquivos, limite);

    if (arvore) {

        if (!arquivoSaida) {
            printf("\n\nFuncionarios:\n\n");
        }
        imprimir(arvore, CRESCENTE, NULL, arquivoSaida);
        No arvoreOrdenadaPorRendimentos = NULL;

        if (!arquivoSaida) {
            printf("\n\nFuncionarios ordenados:\n\n");
        }

        ordenar(arvore, &arvoreOrdenadaPorRendimentos, &compararPorRendimentos);
        imprimir(arvoreOrdenadaPorRendimentos, DECRESCENTE, NULL, arquivoSaida);

        fflush(arquivoSaida);
        fclose(arquivoSaida);
        arquivoSaida=NULL;
        
        printf("\n\nConcluido!\n\n");

    }

    return (EXIT_SUCCESS);
}