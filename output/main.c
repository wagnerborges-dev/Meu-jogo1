#include "raylib.h"
#include <stdlib.h>
#include <time.h>

#define LARGURA_JANELA 800
#define ALTURA_JANELA  600
#define TAM_CELULA     40   // tamanho de cada célula da grade (matriz)

typedef struct {
    Vector2 pos;
    Vector2 vel;
    float   raio;
    Color   cor;
} Bola;


int **criarMatriz(int linhas, int colunas) {
    // aloca o vetor de ponteiros (um ponteiro por linha)
    int **matriz = (int **)malloc(linhas * sizeof(int *));
    if (matriz == NULL) return NULL;

    for (int i = 0; i < linhas; i++) {
        // aloca cada linha como um vetor de inteiros
        matriz[i] = (int *)malloc(colunas * sizeof(int));
        for (int j = 0; j < colunas; j++) {
            // Inicializa com 0 (Exercício 2)
            matriz[i][j] = 0;
        }
    }
    return matriz;
}

/* libera a memória da matriz: primeiro cada linha, depois o vetor de linhas */
void liberarMatriz(int **matriz, int linhas) {
    for (int i = 0; i < linhas; i++) {
        free(matriz[i]);   // libera cada linha
    }
    free(matriz);           // libera o vetor de ponteiros
}

/* desenha a matriz na tela, célula por célula */
void desenharMatriz(int **matriz, int linhas, int colunas) {
    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            // Células visitadas (1) ficam mais claras; não visitadas (0) ficam escuras
            Color cor = (matriz[i][j] == 1) ? LIGHTGRAY 
                                            : (Color){15, 30, 55, 255};
            DrawRectangle(j * TAM_CELULA, i * TAM_CELULA,
                          TAM_CELULA - 2, TAM_CELULA - 2, cor);
        }
    }
}

/* ---------------------------------------------------------------
 * cria o vetor dinâmico inicial de bolas
 * --------------------------------------------------------------- */
Bola *criarBolas(int quantidade) {
    if (quantidade <= 0) return NULL;

    Bola *bolas = (Bola *)malloc(quantidade * sizeof(Bola));
    if (bolas == NULL) return NULL;

    for (int i = 0; i < quantidade; i++) {
        Bola *b = (bolas + i);
        b->pos = (Vector2){ (float)GetRandomValue(50, LARGURA_JANELA - 50),
                            (float)GetRandomValue(50, ALTURA_JANELA - 50) };
        b->vel = (Vector2){ (float)GetRandomValue(-4, 4),
                            (float)GetRandomValue(-4, 4) };
        if (b->vel.x == 0 && b->vel.y == 0) b->vel.x = 2; // Garante movimento
        b->raio = (float)GetRandomValue(10, 25);
        b->cor  = (Color){ GetRandomValue(100, 255), GetRandomValue(100, 255),
                           GetRandomValue(100, 255), 255 };
    }
    return bolas;
}

/* atualiza a posição de UMA bola e trata rebate nas bordas */
void atualizarBola(Bola *b) {
    b->pos.x += b->vel.x;
    b->pos.y += b->vel.y;

    // rebate nas bordas
    if (b->pos.x - b->raio < 0 || b->pos.x + b->raio > LARGURA_JANELA)
        b->vel.x *= -1;
    if (b->pos.y - b->raio < 0 || b->pos.y + b->raio > ALTURA_JANELA)
        b->vel.y *= -1;
}

/* EXERCÍCIO 2: Atualiza a matriz de acordo com a posição de cada bola */
void atualizarMapaDeCalor(int **matriz, int linhas, int colunas, Bola *bolas, int quantidade) {
    for (int i = 0; i < quantidade; i++) {
        Bola *b = (bolas + i);
        
        // Converte a posição em pixels para índices da matriz
        int col = (int)(b->pos.x / TAM_CELULA);
        int lin = (int)(b->pos.y / TAM_CELULA);

        // Garante que o acesso está estritamente dentro dos limites da memória
        if (lin >= 0 && lin < linhas && col >= 0 && col < colunas) {
            matriz[lin][col] = 1; // Marca a célula como visitada
        }
    }
}

/* EXERCÍCIO 2: Conta quantas células foram visitadas */
int contarCelulasVisitadas(int **matriz, int linhas, int colunas) {
    int visitadas = 0;
    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            if (matriz[i][j] == 1) {
                visitadas++;
            }
        }
    }
    return visitadas;
}

int main(void) {
    srand((unsigned int)time(NULL));

    InitWindow(LARGURA_JANELA, ALTURA_JANELA,
               "Ponteiros e Alocacao Dinamica - raylib");
    SetTargetFPS(60);

    int linhas   = ALTURA_JANELA / TAM_CELULA;
    int colunas  = LARGURA_JANELA / TAM_CELULA;
    int **grade  = criarMatriz(linhas, colunas);   // matriz dinâmica

    int quantidadeBolas = 12;
    Bola *bolas = criarBolas(quantidadeBolas);      // vetor dinâmico

    while (!WindowShouldClose()) {

        // ===============================================================
        // EXERCÍCIO 1: Adicionar ou remover bolas em tempo real
        // ===============================================================
        if (IsKeyPressed(KEY_SPACE)) {
            quantidadeBolas++;
            Bola *temp = (Bola *)realloc(bolas, quantidadeBolas * sizeof(Bola));
            if (temp != NULL) {
                bolas = temp;
                // Inicializa a nova bola criada
                Bola *b = (bolas + quantidadeBolas - 1);
                b->pos = (Vector2){ (float)GetRandomValue(50, LARGURA_JANELA - 50),
                                    (float)GetRandomValue(50, ALTURA_JANELA - 50) };
                b->vel = (Vector2){ (float)GetRandomValue(-4, 4),
                                    (float)GetRandomValue(-4, 4) };
                if (b->vel.x == 0 && b->vel.y == 0) b->vel.x = 2;
                b->raio = (float)GetRandomValue(10, 25);
                b->cor  = (Color){ GetRandomValue(100, 255), GetRandomValue(100, 255),
                                   GetRandomValue(100, 255), 255 };
            } else {
                quantidadeBolas--; // Reverte a contagem caso a alocação falhe
            }
        }

        if (IsKeyPressed(KEY_BACKSPACE) && quantidadeBolas > 0) {
            quantidadeBolas--;
            if (quantidadeBolas > 0) {
                Bola *temp = (Bola *)realloc(bolas, quantidadeBolas * sizeof(Bola));
                if (temp != NULL) {
                    bolas = temp;
                }
            } else {
                free(bolas);
                bolas = NULL;
            }
        }

        // ===============================================================
        // Atualizações do jogo
        // ===============================================================
        for (int i = 0; i < quantidadeBolas; i++) {
            atualizarBola(bolas + i);
        }

        // EXERCÍCIO 2: Atualizar mapa de calor na matriz
        atualizarMapaDeCalor(grade, linhas, colunas, bolas, quantidadeBolas);

        // EXERCÍCIO 2: Contar células visitadas
        int totalVisitadas = contarCelulasVisitadas(grade, linhas, colunas);

        // ===============================================================
        // Desenho
        // ===============================================================
        BeginDrawing();
            ClearBackground(RAYWHITE);

            desenharMatriz(grade, linhas, colunas);

            for (int i = 0; i < quantidadeBolas; i++) {
                DrawCircleV(bolas[i].pos, bolas[i].raio, bolas[i].cor);
            }

            // Exibição das informações e contadores
            DrawText(TextFormat("Bolas ativas: %d (ESPACO: +1 | BACKSPACE: -1)", quantidadeBolas),
                     10, 10, 18, RED);
            DrawText(TextFormat("Celulas visitadas: %d / %d", totalVisitadas, linhas * colunas),
                     10, 32, 18, YELLOW);
            DrawText("Pressione ESC para sair", 10, ALTURA_JANELA - 25, 16, WHITE);

        EndDrawing();
    }

    // Libera a memória alocada dinamicamente antes de encerrar
    if (bolas != NULL) {
        free(bolas);
    }
    liberarMatriz(grade, linhas);

    CloseWindow();
    return 0;
}