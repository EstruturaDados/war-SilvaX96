#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*
 * War Estruturado - Missões Estratégicas
 * - Uso de struct, ponteiros, malloc/calloc, free
 * - Modularização: funções para atribuir missão, verificar, exibir, atacar, exibir mapa e liberar memória
 * - Autor: [Seu Nome]
 * - Data: [Data Atual]
 */

// Estrutura que representa um território
typedef struct {
    char nome[30];   // Nome do território
    char cor[10];    // Cor do exército/dono (ex: "vermelho", "azul")
    int tropas;      // Quantidade de tropas presentes
} Territorio;

// Estrutura para representar um jogador
typedef struct {
    char cor[10];    // Cor do jogador
    char *missao;    // Ponteiro para a missão (alocada dinamicamente)
} Jogador;

/* ---------------------------
   Vetor global de missões
   ---------------------------
   Mantemos as descrições globalmente para que atribuirMissao copie
   a descrição para o destino (assinatura pedida) e verificarMissao
   consiga identificar qual regra aplicar comparando a string.
*/
char *missoesDisponiveis[] = {
    "Conquistar ao menos 3 territorios",                      // id 0
    "Eliminar todas as tropas da cor vermelha",               // id 1
    "Controlar um territorio com pelo menos 5 tropas",        // id 2
    "Controlar pelo menos 2 territorios com nome comeca A",   // id 3
    "Ter mais tropas no total que qualquer outro jogador"     // id 4
};
const int TOTAL_MISSOES = sizeof(missoesDisponiveis) / sizeof(missoesDisponiveis[0]);

/* ---------------------------
   Funções exigidas / úteis
   --------------------------- */

// Sorteia uma missão e copia a descrição para 'destino' usando strcpy.
// assinatura exigida: void atribuirMissao(char* destino, char* missoes[], int totalMissoes)
void atribuirMissao(char* destino, char* missoes[], int totalMissoes) {
    int idx = rand() % totalMissoes;
    strcpy(destino, missoes[idx]);
}

// Exibe uma missão (passada por valor - conforme solicitado)
void exibirMissao(const char *missao) {
    printf("Missao: %s\n", missao);
}

// Exibe mapa/territórios
void exibirMapa(Territorio* mapa, int tamanho) {
    printf("\n--- MAPA: %d territórios ---\n", tamanho);
    for (int i = 0; i < tamanho; i++) {
        printf("[%d] %s | Cor: %s | Tropas: %d\n", i, mapa[i].nome, mapa[i].cor, mapa[i].tropas);
    }
    printf("---------------------------\n");
}

// Simula um ataque: atacante e defensor são ponteiros para territórios.
// Usa rand() para rolagem de dados (1..6). Se atacante vence, transfere cor
// e metade (arredondada para baixo) das tropas atacantes para o defensor.
// Se atacante perde, atacante perde 1 tropa (min 0).
void atacar(Territorio* atacante, Territorio* defensor) {
    if (strcmp(atacante->cor, defensor->cor) == 0) {
        printf("Ataque invalido: mesmo dono.\n");
        return;
    }

    int dadoAtacante = (rand() % 6) + 1; // 1-6
    int dadoDefensor = (rand() % 6) + 1; // 1-6

    printf("Rolagem - Atacante: %d | Defensor: %d\n", dadoAtacante, dadoDefensor);

    if (dadoAtacante > dadoDefensor) {
        // atacante vence
        int tropasTransferidas = atacante->tropas / 2; // metade das tropas do atacante
        if (tropasTransferidas <= 0) tropasTransferidas = 1; // garante pelo menos 1
        printf("Atacante vence! Transferindo %d tropas e dominando o territorio '%s'.\n",
               tropasTransferidas, defensor->nome);
        // atualiza defensor: passa a cor e soma tropas transferidas
        strcpy(defensor->cor, atacante->cor);
        defensor->tropas += tropasTransferidas;
        // atacante perde as tropas transferidas (simula perda)
        atacante->tropas -= tropasTransferidas;
        if (atacante->tropas < 0) atacante->tropas = 0;
    } else {
        // defensor vence ou empate - atacante perde 1 tropa
        printf("Defensor resiste! Atacante perde 1 tropa.\n");
        if (atacante->tropas > 0) atacante->tropas -= 1;
    }
}

// Verifica se a missão passada em 'missao' foi cumprida.
// Recebe a string missao, o mapa (vetor de Territorio) e tamanho do mapa.
// Retorna 1 se cumprida, 0 caso contrário.
// Observação: para identificar qual missão aplicar, comparamos a string com o vetor global "missoesDisponiveis".
int verificarMissao(char* missao, Territorio* mapa, int tamanho, Jogador* jogador, Jogador* outros, int numOutros) {
    // Determina qual missão é (procura índice do vetor global)
    int id = -1;
    for (int i = 0; i < TOTAL_MISSOES; i++) {
        if (strcmp(missao, missoesDisponiveis[i]) == 0) {
            id = i;
            break;
        }
    }
    if (id == -1) {
        // missão desconhecida — não cumprida
        return 0;
    }

    // Aplica as regras conforme id
    if (id == 0) {
        // "Conquistar ao menos 3 territorios" -> contar territorios do jogador
        int count = 0;
        for (int i = 0; i < tamanho; i++) {
            if (strcmp(mapa[i].cor, jogador->cor) == 0) count++;
        }
        return (count >= 3) ? 1 : 0;
    } else if (id == 1) {
        // "Eliminar todas as tropas da cor vermelha"
        // Aqui assumimos que a descrição pede a cor "vermelha".
        // Se o jogador não for responsável por "vermelha", a missão ainda pode valer (ex: eliminar vermelho)
        int totalVermelho = 0;
        for (int i = 0; i < tamanho; i++) {
            if (strcmp(mapa[i].cor, "vermelha") == 0) totalVermelho += mapa[i].tropas;
        }
        return (totalVermelho == 0) ? 1 : 0;
    } else if (id == 2) {
        // "Controlar um territorio com pelo menos 5 tropas"
        for (int i = 0; i < tamanho; i++) {
            if (strcmp(mapa[i].cor, jogador->cor) == 0 && mapa[i].tropas >= 5) return 1;
        }
        return 0;
    } else if (id == 3) {
        // "Controlar pelo menos 2 territorios com nome comeca A"
        int count = 0;
        for (int i = 0; i < tamanho; i++) {
            if (strcmp(mapa[i].cor, jogador->cor) == 0) {
                if (mapa[i].nome[0] == 'A' || mapa[i].nome[0] == 'a') count++;
            }
        }
        return (count >= 2) ? 1 : 0;
    } else if (id == 4) {
        // "Ter mais tropas no total que qualquer outro jogador"
        int minhas = 0;
        for (int i = 0; i < tamanho; i++)
            if (strcmp(mapa[i].cor, jogador->cor) == 0) minhas += mapa[i].tropas;
        // compara com cada outro jogador (calculamos suas tropas totais)
        for (int p = 0; p < numOutros; p++) {
            int totalOutro = 0;
            for (int i = 0; i < tamanho; i++)
                if (strcmp(mapa[i].cor, outros[p].cor) == 0) totalOutro += mapa[i].tropas;
            if (!(minhas > totalOutro)) return 0; // se não for estritamente maior que esse outro, falha
        }
        return 1;
    }

    return 0; // padrão
}

// Libera memória alocada dinamicamente: mapa e missões dos jogadores
void liberarMemoria(Territorio* mapa, int tamanho, Jogador* jogadores, int numJogadores) {
    if (mapa) free(mapa);
    // liberar missões
    for (int i = 0; i < numJogadores; i++) {
        if (jogadores[i].missao) {
            free(jogadores[i].missao);
            jogadores[i].missao = NULL;
        }
    }
}

/* ---------------------------
   Função principal: main
   --------------------------- */

int main() {
    srand((unsigned int) time(NULL)); // inicializa gerador aleatório

    // Configuração do jogo:
    const int NUM_TERRITORIOS = 6; // exemplo de mapa com 6 territorios
    const int NUM_JOGADORES = 2;   // modo simplificado: 2 jogadores

    // Aloca dinamicamente o mapa (conforme requisito)
    Territorio* mapa = (Territorio*) calloc(NUM_TERRITORIOS, sizeof(Territorio));
    if (!mapa) {
        fprintf(stderr, "Erro ao alocar memoria para o mapa.\n");
        return 1;
    }

    // Preenche o mapa com nomes, cores e tropas iniciais (exemplo)
    // Em um jogo real, poderia receber entrada do usuário; aqui usamos valores iniciais para demo
    strcpy(mapa[0].nome, "Aldea");
    strcpy(mapa[0].cor, "vermelha");
    mapa[0].tropas = 3;

    strcpy(mapa[1].nome, "Brisa");
    strcpy(mapa[1].cor, "azul");
    mapa[1].tropas = 2;

    strcpy(mapa[2].nome, "Areia");
    strcpy(mapa[2].cor, "azul");
    mapa[2].tropas = 4;

    strcpy(mapa[3].nome, "Castelo");
    strcpy(mapa[3].cor, "neutra");
    mapa[3].tropas = 1;

    strcpy(mapa[4].nome, "Aurora");
    strcpy(mapa[4].cor, "neutra");
    mapa[4].tropas = 2;

    strcpy(mapa[5].nome, "Vila");
    strcpy(mapa[5].cor, "vermelha");
    mapa[5].tropas = 3;

    // Cria jogadores e aloca missões dinamicamente
    Jogador jogadores[NUM_JOGADORES];
    // Define cores dos jogadores (poderia ser entrada do usuario)
    strcpy(jogadores[0].cor, "azul");   // jogador 1 usa cor azul
    strcpy(jogadores[1].cor, "verde");  // jogador 2 usa cor verde (não presente inicialmente no mapa)

    // Aloca memória para string de missão para cada jogador e atribui missão
    for (int i = 0; i < NUM_JOGADORES; i++) {
        // aloca buffer (tamanho suficiente para as descrições)
        jogadores[i].missao = (char*) malloc(100 * sizeof(char));
        if (!jogadores[i].missao) {
            fprintf(stderr, "Erro ao alocar memoria para missao do jogador %d\n", i);
            liberarMemoria(mapa, NUM_TERRITORIOS, jogadores, NUM_JOGADORES);
            return 1;
        }
        // atribui missão (usa função com assinatura requerida)
        atribuirMissao(jogadores[i].missao, missoesDisponiveis, TOTAL_MISSOES);
    }

    // Exibe mapa e missões (missão é revelada apenas uma vez no início)
    exibirMapa(mapa, NUM_TERRITORIOS);
    for (int i = 0; i < NUM_JOGADORES; i++) {
        printf("\nJogador %d (cor: %s) - ", i + 1, jogadores[i].cor);
        exibirMissao(jogadores[i].missao);
    }

    // Laço de jogadas (simplificado). A cada turno o jogador ataca escolhendo índices.
    // Ao final de cada turno verificamos se missão foi cumprida.
    int turno = 0;
    const int MAX_TURNOS = 200;
    int vencedor = -1;

    while (turno < MAX_TURNOS && vencedor == -1) {
        int idxJogador = turno % NUM_JOGADORES;
        printf("\n==== TURNO %d - Jogador %d (cor %s) ====\n", turno+1, idxJogador+1, jogadores[idxJogador].cor);
        exibirMapa(mapa, NUM_TERRITORIOS);

        // Simplicidade: pedimos ao jogador escolher território atacante (de sua cor) e defensor (outra cor)
        int idxAtacante, idxDefensor;
        printf("Escolha o indice do territorio atacante (propriedade sua): ");
        if (scanf("%d", &idxAtacante) != 1) {
            // entrada inválida -> limpar buffer e pular
            while (getchar() != '\n');
            printf("Entrada invalida. Pulando turno.\n");
            turno++;
            continue;
        }
        if (idxAtacante < 0 || idxAtacante >= NUM_TERRITORIOS) {
            printf("Indice atacante invalido.\n");
            turno++;
            continue;
        }
        if (strcmp(mapa[idxAtacante].cor, jogadores[idxJogador].cor) != 0) {
            printf("Voce so pode atacar com territorios da sua cor.\n");
            turno++;
            continue;
        }

        printf("Escolha o indice do territorio defensor (inimigo): ");
        if (scanf("%d", &idxDefensor) != 1) {
            while (getchar() != '\n');
            printf("Entrada invalida. Pulando turno.\n");
            turno++;
            continue;
        }
        if (idxDefensor < 0 || idxDefensor >= NUM_TERRITORIOS) {
            printf("Indice defensor invalido.\n");
            turno++;
            continue;
        }
        if (strcmp(mapa[idxDefensor].cor, jogadores[idxJogador].cor) == 0) {
            printf("Voce nao pode atacar um territorio seu.\n");
            turno++;
            continue;
        }

        // Realiza ataque
        atacar(&mapa[idxAtacante], &mapa[idxDefensor]);

        // Verifica missão do jogador atual
        // Prepara array com 'outros' jogadores para missão id 4
        Jogador outros[NUM_JOGADORES - 1];
        int oi = 0;
        for (int j = 0; j < NUM_JOGADORES; j++) {
            if (j == idxJogador) continue;
            outros[oi++] = jogadores[j];
        }

        // chamar verificarMissao (usando a versão que aceita jogadores para comparar tropas)
        int cumpriu = verificarMissao(jogadores[idxJogador].missao, mapa, NUM_TERRITORIOS, &jogadores[idxJogador], outros, NUM_JOGADORES - 1);
        if (cumpriu) {
            printf("\n************ MISSÃO CUMPRIDA! ************\n");
            printf("Jogador %d (cor %s) cumpriu a missão: %s\n", idxJogador+1, jogadores[idxJogador].cor, jogadores[idxJogador].missao);
            vencedor = idxJogador;
            break;
        } else {
            printf("Missao ainda nao cumprida.\n");
        }

        turno++;
    }

    if (vencedor != -1) {
        printf("\n>>> Vencedor: Jogador %d (cor %s)\n", vencedor+1, jogadores[vencedor].cor);
    } else {
        printf("\nFim de jogo sem vencedor por missao apos %d turnos.\n", MAX_TURNOS);
    }

    // Limpa a memória alocada
    liberarMemoria(mapa, NUM_TERRITORIOS, jogadores, NUM_JOGADORES);

    return 0;
}
