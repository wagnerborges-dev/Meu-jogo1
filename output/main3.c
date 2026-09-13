#include "raylib.h"
#include <stdlib.h>
#include <time.h>

#define LARGURA_JANELA 800
#define ALTURA_JANELA  600
#define RAIO_JOGADOR   20.0f
#define TOTAL_ITENS    10

typedef enum {
    ITEM_ARMA,
    ITEM_POCAO,
    ITEM_ESCUDO // [EXERCÍCIO 1] Novo tipo de item adicionado ao enum
} TipoItem;

typedef struct {
    float dano;
    int   alcance;
} DadosArma;

typedef struct {
    int cura;
} DadosPocao;

// [EXERCÍCIO 1] Struct contendo os dados do escudo
typedef struct {
    int absorcao;
} DadosEscudo;

/* union: em cada Item só um destes dois campos é válido por vez,
 * e isso é decidido pelo campo "tipo" (enum) da struct Item */
typedef union {
    DadosArma   arma;
    DadosPocao  pocao;
    DadosEscudo escudo; // [EXERCÍCIO 1] Novo campo adicionado à union
} DadosItem;

typedef struct {
    Vector2   pos;
    float     raio;
    TipoItem  tipo;
    DadosItem dados;
    bool      coletado;
} Item;

typedef struct {
    Vector2 pos;
    float   raio;
    int     vida;
    float   dano;
    int     armadura; // [EXERCÍCIO 1] Nova propriedade de armadura adicionada ao jogador
} Jogador;

/* cria o vetor dinâmico de itens sorteando tipo e preenchendo a
 * union de acordo com o tipo sorteado */
Item *criarItens(int quantidade) {
    Item *itens = (Item *)malloc(quantidade * sizeof(Item));
    if (itens == NULL) return NULL;

    for (int i = 0; i < quantidade; i++) {
        Item *it = (itens + i);
        it->pos      = (Vector2){ GetRandomValue(30, LARGURA_JANELA - 30),
                                   GetRandomValue(30, ALTURA_JANELA - 30) };
        it->raio     = 12.0f;
        it->coletado = false;
        it->tipo     = (TipoItem)GetRandomValue(ITEM_ARMA, ITEM_ESCUDO); // [EXERCÍCIO 1] Sorteia até ITEM_ESCUDO

        if (it->tipo == ITEM_ARMA) {
            it->dados.arma.dano    = (float)GetRandomValue(2, 8);
            it->dados.arma.alcance = GetRandomValue(1, 3);
        } else if (it->tipo == ITEM_POCAO) {
            // [EXERCÍCIO 2] Poção envenenada com probabilidade de 30% (0, 1 ou 2 em 10)
            if (GetRandomValue(0, 9) < 3) {
                it->dados.pocao.cura = -GetRandomValue(10, 20); // Cura negativa (dano/veneno)
            } else {
                it->dados.pocao.cura = GetRandomValue(10, 30);
            }
        } else if (it->tipo == ITEM_ESCUDO) {
            // [EXERCÍCIO 1] Preenche os dados do escudo
            it->dados.escudo.absorcao = GetRandomValue(5, 15);
        }
    }
    return itens;
}

/* recebe PONTEIROS para o jogador e para o item: aplica o efeito do
 * item lendo o campo correto da union de acordo com o enum "tipo" */
void aplicarItem(Jogador *j, Item *item) {
    switch (item->tipo) {
        case ITEM_ARMA:
            j->dano += item->dados.arma.dano;
            break;
        case ITEM_POCAO:
            j->vida += item->dados.pocao.cura;
            // [EXERCÍCIO 2] Impede que a vida fique negativa se a poção for envenenada
            if (j->vida < 0) {
                j->vida = 0;
            }
            break;
        case ITEM_ESCUDO:
            // [EXERCÍCIO 1] Aplica o aumento de armadura ao jogador
            j->armadura += item->dados.escudo.absorcao;
            break;
    }
    item->coletado = true;
}

bool colidiu(Vector2 a, float raioA, Vector2 b, float raioB) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    float distancia2 = dx * dx + dy * dy;
    float somaRaios2 = (raioA + raioB) * (raioA + raioB);
    return distancia2 <= somaRaios2;
}

void desenharItem(Item *item) {
    if (item->coletado) return;
    Color cor = (item->tipo == ITEM_ARMA) ? RED : GREEN;

    // [EXERCÍCIO 2] Se for poção envenenada (cura < 0), desenha com a cor roxa (PURPLE)
    if (item->tipo == ITEM_POCAO && item->dados.pocao.cura < 0) {
        cor = PURPLE;
    }
    // [EXERCÍCIO 1] Se for escudo, desenha com a cor azul (BLUE)
    else if (item->tipo == ITEM_ESCUDO) {
        cor = BLUE;
    }

    DrawCircleV(item->pos, item->raio, cor);
}

int main(void) {
    srand((unsigned int)time(NULL));

    InitWindow(LARGURA_JANELA, ALTURA_JANELA, "Atividade 3 - Union + Enum + Struct");
    SetTargetFPS(60);

    Jogador jogador = { { LARGURA_JANELA / 2.0f, ALTURA_JANELA / 2.0f }, RAIO_JOGADOR, 100, 5.0f, 0 }; // [EXERCÍCIO 1] Inicializado com 0 de armadura
    Item *itens = criarItens(TOTAL_ITENS); // vetor dinâmico de struct com union

    while (!WindowShouldClose()) {

        float vel = 250.0f * GetFrameTime();
        if (IsKeyDown(KEY_RIGHT)) jogador.pos.x += vel;
        if (IsKeyDown(KEY_LEFT))  jogador.pos.x -= vel;
        if (IsKeyDown(KEY_UP))    jogador.pos.y -= vel;
        if (IsKeyDown(KEY_DOWN))  jogador.pos.y += vel;

        for (int i = 0; i < TOTAL_ITENS; i++) {
            Item *it = (itens + i);
            if (!it->coletado && colidiu(jogador.pos, jogador.raio, it->pos, it->raio)) {
                aplicarItem(&jogador, it); // &jogador: ponteiro para struct
            }
        }

        BeginDrawing();
            ClearBackground(RAYWHITE);

            for (int i = 0; i < TOTAL_ITENS; i++) {
                desenharItem(itens + i);
            }

            DrawCircleV(jogador.pos, jogador.raio, DARKBLUE);

            // [EXERCÍCIO 1] Exibe o atributo Armadura junto com Vida e Dano
            DrawText(TextFormat("Vida: %d   Dano: %.1f   Armadura: %d", jogador.vida, jogador.dano, jogador.armadura), 10, 10, 22, DARKGRAY);
            // [EXERCÍCIO 1 e 2] Legenda atualizada
            DrawText("Vermelho = Arma | Verde = Pocao | Roxa = Pocao Envenenada | Azul = Escudo", 10, 34, 18, GRAY);
            DrawText("Setas movem o jogador | ESC sai", 10, ALTURA_JANELA - 25, 16, GRAY);

        EndDrawing();
    }

    free(itens); // libera o vetor dinâmico

    CloseWindow();
    return 0;
}
