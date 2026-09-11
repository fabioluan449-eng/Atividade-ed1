#include "raylib.h"
#include <stdlib.h>
#include <math.h>

#define LARGURA 800
#define ALTURA 600
#define MAX_ENTIDADES 30
#define RAIO_JOGADOR 20.0f
#define TOTAL_INIMIGOS_INICIAL 6
#define TOTAL_ITENS_INICIAL 5

typedef enum {
    ENTIDADE_JOGADOR,
    ENTIDADE_INIMIGO,
    ENTIDADE_ITEM
} TipoEntidade;

typedef union {
    int dano;
    int valor;
} ExtraEntidade;

typedef struct {
    TipoEntidade tipo;
    Vector2 pos;
    float raio;
    int vida;
    Color cor;
    ExtraEntidade extra;
} Entidade;

Entidade *vetorEntidades[MAX_ENTIDADES];
int totalEntidades = 0;

Entidade *criarEntidade(TipoEntidade tipo, Vector2 pos) {
    Entidade *e = (Entidade *)malloc(sizeof(Entidade));
    if (e == NULL) return NULL;

    e->tipo = tipo;
    e->pos = pos;
    e->raio = (tipo == ENTIDADE_JOGADOR) ? RAIO_JOGADOR
            : (tipo == ENTIDADE_INIMIGO) ? 15.0f : 8.0f;

    switch (tipo) {
        case ENTIDADE_JOGADOR:
            e->vida = 100;
            e->cor = BLUE;
            break;
        case ENTIDADE_INIMIGO:
            e->vida = 40;
            e->cor = MAROON;
            e->extra.dano = GetRandomValue(5, 15);
            break;
        case ENTIDADE_ITEM:
            e->vida = 1;
            e->cor = GOLD;
            e->extra.valor = GetRandomValue(5, 20);
            break;
    }
    return e;
}

int adicionarEntidade(Entidade *e) {
    if (e == NULL || totalEntidades >= MAX_ENTIDADES) return 0;

    vetorEntidades[totalEntidades] = e;
    totalEntidades++;
    return 1;
}

void removerEntidade(int indice) {
    if (indice < 0 || indice >= totalEntidades) return;

    free(vetorEntidades[indice]);
    vetorEntidades[indice] = vetorEntidades[totalEntidades - 1];
    totalEntidades--;
}

void ordenarMaisProximoNaPosicaoUm(Entidade *jogador) {
    if (totalEntidades < 3) return;

    int indiceMaisProximo = 1;
    float menorDistancia = -1.0f;

    for (int i = 1; i < totalEntidades; i++) {
        float dx = vetorEntidades[i]->pos.x - jogador->pos.x;
        float dy = vetorEntidades[i]->pos.y - jogador->pos.y;
        float distancia = sqrtf(dx * dx + dy * dy);

        if (menorDistancia < 0.0f || distancia < menorDistancia) {
            menorDistancia = distancia;
            indiceMaisProximo = i;
        }
    }

    Entidade *tmp = vetorEntidades[1];
    vetorEntidades[1] = vetorEntidades[indiceMaisProximo];
    vetorEntidades[indiceMaisProximo] = tmp;
}

Vector2 posicaoAleatoria(void) {
    Vector2 p;
    p.x = (float)GetRandomValue(20, LARGURA - 20);
    p.y = (float)GetRandomValue(20, ALTURA - 20);
    return p;
}

int main(void) {
    InitWindow(LARGURA, ALTURA, "Atividade 5 - Vetor de Ponteiros para Struct");
    SetTargetFPS(60);

    Entidade *jogador = criarEntidade(ENTIDADE_JOGADOR, (Vector2){ LARGURA / 2.0f, ALTURA / 2.0f });
    adicionarEntidade(jogador);

    for (int i = 0; i < TOTAL_INIMIGOS_INICIAL; i++) {
        adicionarEntidade(criarEntidade(ENTIDADE_INIMIGO, posicaoAleatoria()));
    }
    for (int i = 0; i < TOTAL_ITENS_INICIAL; i++) {
        adicionarEntidade(criarEntidade(ENTIDADE_ITEM, posicaoAleatoria()));
    }

    float velocidade = 200.0f;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        if (IsKeyDown(KEY_RIGHT)) jogador->pos.x += velocidade * dt;
        if (IsKeyDown(KEY_LEFT)) jogador->pos.x -= velocidade * dt;
        if (IsKeyDown(KEY_UP)) jogador->pos.y -= velocidade * dt;
        if (IsKeyDown(KEY_DOWN)) jogador->pos.y += velocidade * dt;

        if (IsKeyPressed(KEY_N) && totalEntidades < MAX_ENTIDADES) {
            Entidade *novoItem = criarEntidade(ENTIDADE_ITEM, posicaoAleatoria());
            adicionarEntidade(novoItem);
        }

        ordenarMaisProximoNaPosicaoUm(jogador);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        for (int i = 0; i < totalEntidades; i++) {
            Entidade *e = vetorEntidades[i];
            DrawCircleV(e->pos, e->raio, e->cor);
        }

        DrawText("SETAS: mover | N: gerar item", 10, 10, 18, DARKGRAY);
        DrawText(TextFormat("Total de entidades: %d", totalEntidades), 10, 35, 18, DARKGRAY);
        if (totalEntidades > 1) {
            DrawText("Entidade mais proxima em vetorEntidades[1]", 10, 60, 18, DARKGRAY);
        }

        EndDrawing();
    }

    for (int i = 0; i < totalEntidades; i++) {
        free(vetorEntidades[i]);
    }

    CloseWindow();
    return 0;
}
