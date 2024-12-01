#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TAMANHO_HASH 2000
#define TAMANHO_PALAVRA 50

typedef struct Produto {
    int id;
    char nome[TAMANHO_PALAVRA];
    char categoria[TAMANHO_PALAVRA];
    float valor;
} Produto;

typedef struct {
    Produto* tabela[TAMANHO_HASH];
} TabelaHash;


int funcao_hash(int id) {
    return id % TAMANHO_HASH;
}

// função "inserir" trata as colisões na tabela hash
void inserir(TabelaHash *tabela, Produto* produto) {
    int indice = funcao_hash(produto->id);
    while (tabela->tabela[indice] != NULL) {
        if (indice == TAMANHO_HASH) {
            printf("Erro! Limite TAMANHO_HASH atingido...\n");
        }
        indice++;
    }
    tabela->tabela[indice] = produto;
}

Produto* criar(int id, char nome[], char categoria[], float valor) {
    Produto* novo_produto = (Produto*) malloc(sizeof(Produto));
    if (novo_produto != NULL) {
        novo_produto->id = id;
        strcpy(novo_produto->nome, nome);
        strcpy(novo_produto->categoria, categoria);
        novo_produto->valor = valor;
    }
    return novo_produto;

}

Produto* buscar(TabelaHash *tabela, int id_busca) {
    for (int i = 0; i < TAMANHO_HASH; i++) {
        if (tabela->tabela[i] != NULL && tabela->tabela[i]->id == id_busca) {
            return tabela->tabela[i];
        }
    }
    return NULL;
}

float conversaoMoeda(char MOEDA[], float valor_produto) {
    // considerando que os valores são e serão inseridos em reais
    // suportando inicialmente 5 moedas: real (BRL), dólar americano (USD),
    // yuan chinês (CNY), euro (EUR) e peso argentino (ARG)
    int total_moedas = 5;
    
    char moedas_suportadas[][TAMANHO_PALAVRA] = {"BRL", "USD", "CNY", "EUR", "ARG"};
    
    // cotação, todos em função do real 
    float cotacao_moedas[] = {1, 0.17, 1.21, 0.16, 169.11};
    
    for (int i = 0; i < total_moedas; i++) {
        if (strcmp(MOEDA, moedas_suportadas[i]) == 0) {
            return valor_produto * cotacao_moedas[i];
        }
    }
    
    return -1;
}


void exibirInfoProduto(Produto* produto_atual, char MOEDA[]) {
    if (produto_atual != NULL) {
            printf("ID = %d\n", produto_atual->id);
            printf("Nome = %s\n", produto_atual->nome);
            printf("Categoria = %s\n", produto_atual->categoria);
            printf("Valor = %s %.2f\n", MOEDA, conversaoMoeda(MOEDA, produto_atual->valor)); // obs: casas decimais
            printf("\n");
        }
}

void exibir(TabelaHash *tabela, char MOEDA[]) {
    for (int i = 0; i < TAMANHO_HASH; i++) {
        Produto* produto_atual = tabela->tabela[i];
        exibirInfoProduto(produto_atual, MOEDA);
    }
}

void remover(TabelaHash *tabela, int id_busca) {
    for (int i = 0; i < TAMANHO_HASH; i++) {
        if (tabela->tabela[i] != NULL && tabela->tabela[i]->id == id_busca) {
            tabela->tabela[i] = NULL;
            break;
        }
    }
}

// comparacao relevancia
void trocar(Produto* p, Produto* q) {
    Produto* temp = p;
    p = q;
    q = temp;
}

int comparacaoValor(const void *a, const void *b) {
    Produto* p_A = *(Produto **)a;
    Produto* p_B = *(Produto **)b;

    return (p_A->valor - p_B->valor);
}

// a forma das funcoes de ordenacao abaixo (ordenacaoQsort e ordenacaoSelectionSort)
// são idênticas, pois é necessário clonar a tabela hash numa array para ordenar, 
// e depois voltar o resultado ordenado à tabela hash.

// qsort: função nativa do C que implementa o algoritmo de "quick sort"
// https://www.tutorialspoint.com/c_standard_library/c_function_qsort.htm
void ordenacaoQsort(TabelaHash *tabela) {
    // copiando para uma array
    Produto** produtos_validos = malloc(TAMANHO_HASH * sizeof(Produto)); // produtos != NULL
    int count = 0;
    for (int i = 0; i < TAMANHO_HASH; i++) {
        if (tabela->tabela[i] != NULL) {
            produtos_validos[count] = tabela->tabela[i];
            count++;
        }
    }
    
    // ordenação propriamente dita
    qsort(produtos_validos, count, sizeof(Produto*), comparacaoValor);
    
    for (int i = 0; i < TAMANHO_HASH; i++) {
        tabela->tabela[i] = NULL;
    }
    
    for (int i = 0; i < count; i++) {
        tabela->tabela[i] = produtos_validos[i];
    }
    
    free(produtos_validos);
}

// selection sort: implementação do algoritmo de "selection sort", feita na mão
void ordenacaoSelectionSort(TabelaHash* tabela) {
    Produto** produtos_validos = malloc(TAMANHO_HASH * sizeof(Produto)); // produtos != NULL
    int count = 0;
    for (int i = 0; i < TAMANHO_HASH; i++) {
        if (tabela->tabela[i] != NULL) {
            produtos_validos[count] = tabela->tabela[i];
            count++;
        }
    }
    int i,j,k;
    Produto *temp;
    
    // ordenação propriamente dita
    for (i = 0; i < count - 1; i++) {
        k = i;
        for (j = i + 1; j < count; j++) {
            if (produtos_validos[j]->valor < produtos_validos[k]->valor) {
                k = j;
            }
        }
        if (k != i) {
            temp = produtos_validos[i];
            produtos_validos[i] = produtos_validos[k];
            produtos_validos[k] = temp;
        }
    }
    
    for (int i = 0; i < TAMANHO_HASH; i++) {
        tabela->tabela[i] = NULL;
    }
    
    
    for (int i = 0; i < count; i++) {
        tabela->tabela[i] = produtos_validos[i];
    }
    
    free(produtos_validos);
    
}

int buscaPorCategoria(TabelaHash *tabela, char MOEDA[], char categoria[]) {
    int resultados = 0;
    for (int i = 0; i < TAMANHO_HASH; i++) {
        Produto* produto_atual = tabela->tabela[i];
        if (produto_atual != NULL) {
            if (strcmp(produto_atual->categoria, categoria) == 0) {
                exibirInfoProduto(produto_atual, MOEDA);
                resultados++;
            }
        }
    }
    
    return resultados;
}

void formatarString(char string[]) {
    for (int i = 0; i < strlen(string); i++) {
        if (string[i] == '\n') {
            string[i] = '\0';
            break;
        }
    }
}


int main() {
    TabelaHash tabela = {0};
    char MOEDA[TAMANHO_PALAVRA] = "BRL";
    
    // testando se moeda é válida
    if (conversaoMoeda(MOEDA, 1) == -1) {
        printf("Erro! O programa não é compatível com a moeda %s\n", MOEDA);
        return 1;
    }
    
    // inserindo produtos para testar
    inserir(&tabela, criar(101, "Computador Dell", "Tecnologia", 2500));
    inserir(&tabela, criar(102, "Celular Motorola G42", "Telefonia", 600));
    inserir(&tabela, criar(103, "Console Playstation 5", "Tecnologia", 3000));
    inserir(&tabela, criar(104, "Camisa da Insider", "Roupas", 80));
    inserir(&tabela, criar(105, "Óculos RayBan", "Acessórios", 400));
    inserir(&tabela, criar(106, "iPhone 15", "Telefonia", 4000));
    inserir(&tabela, criar(107, "Chaveiro", "Acessórios", 40));
    inserir(&tabela, criar(108, "Uniforme do Vila Nova 2025", "Roupas", 250));
    inserir(&tabela, criar(109, "Macbook 2024", "Tecnologia", 9000));
    inserir(&tabela, criar(110, "Boné", "Acessórios", 100));
    
    
    // velho e clássico menu interativo
    char nome[TAMANHO_PALAVRA], categoria[TAMANHO_PALAVRA];
    int id;
    float valor;
    
    int resposta = -1;
    while (resposta != 0) {
        ordenacaoQsort(&tabela);
        
        printf("\nMenu\n");
        printf("(1) - Inserir novo produto\n");
        printf("(2) - Remover produto\n");
        printf("(3) - Buscar por produto\n");
        printf("(4) - Buscar produtos por categoria\n");
        printf("(5) - Exibir todos os produtos\n");
        printf("(6) - Trocar moeda (atual: %s)\n", MOEDA);
        printf("(0) - Sair do programa\n");
        
        scanf("%d", &resposta);
        switch(resposta) {
            case 1:
                getchar();
                printf("Insira o ID do produto: ");
                scanf("%d", &id);
                getchar();
                printf("Insira o nome do produto: ");
                fgets(nome, TAMANHO_PALAVRA, stdin);
                printf("Insira a categoria do produto: ");
                scanf("%s", categoria);
                printf("Insira o valor do produto (em reais): ");
                scanf("%f", &valor);
                
                // garantir que ID não existe
                if (buscar(&tabela, id) != NULL) {
                    printf("O ID %d já foi cadastrado!\n", id);
                }
                else {
                    formatarString(nome);
                    formatarString(categoria);
                    inserir(&tabela, criar(id, nome, categoria, valor));
                }
                break;
            case 2:
                printf("Insira o ID do produto: ");
                scanf("%d", &id);
                if (buscar(&tabela, id) != NULL) {
                    remover(&tabela, id);
                }
                else {
                    printf("ID %d não existe!\n", id);
                }
                break;
            case 3:
                printf("Insira o ID do produto: ");
                scanf("%d", &id);
                if (buscar(&tabela, id) != NULL) {
                    printf("\n");
                    exibirInfoProduto(buscar(&tabela, id), MOEDA);
                }
                else {
                    printf("ID %d não existe!\n", id);
                }
                break;
            case 4:
                printf("Exemplos de categorias: Tecnologia, Roupas, Acessórios\n");
                printf("Insira a categoria desejada: ");
                scanf("%s", categoria);
                formatarString(categoria);
                printf("Resultados da sua busca por %s\n\n", categoria);
                int qtd_resultados = buscaPorCategoria(&tabela, MOEDA, categoria);
                if (qtd_resultados == 0) {
                    printf("Nenhum resultado encontrado\n");
                }
                break;
                
            
            case 5:
                exibir(&tabela, MOEDA);
                break;
            
            case 6:
                printf("Moedas suportadas: Real (BRL), Dólar (USD), Yuan (CNY), Euro (EUR), Peso Argentino (ARG)\n");
                printf("Moeda atual = %s\n", MOEDA);
                printf("Insira a moeda desejada: ");
                getchar();
                scanf("%s", MOEDA);
                if (conversaoMoeda(MOEDA, 1) == -1) {
                    printf("Erro! O programa não é compatível com a moeda %s\n", MOEDA);
                    return 1;
                }
                else {
                    printf("Moeda atualizada para %s\n", MOEDA);
                }
                break;
 
        }
    }
    

    
}
