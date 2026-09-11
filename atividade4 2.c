#include "raylib.h"
#include <stdlib.h>
#include <math.h>

#define LARGURA 800
#define ALTURA 600
#define TOTAL_INIMIGOS 8
#define DANO_TIRO 20
#define VIDA_INICIAL 60
#define VIDA_MAXIMA 60
#define CURA_QUANTIDADE 15

typedef enum {
    INIMIGO_VIVO,
    INIMIGO_MORTO
} EstadoInimigo;

typedef struct {
    Vector2 pos;
    float raio;
    int vida;
    EstadoInimigo estado;
} Inimigo;

void inicializarInimigos(Inimigo *vetor, int n) {
    for (int i = 0; i < n; i++) {
        Inimigo *ini = (vetor + i);
        ini->pos.x = (float)GetRandomValue(50, LARGURA - 50);
        ini->pos.y = (float)GetRandomValue(50, ALTURA - 50);
        ini->raio = 18.0f;
        ini->vida = VIDA_INICIAL;
        ini->estado = INIMIGO_VIVO;
    }
}

void atingirInimigo(Inimigo *inimigo, int dano) {
    if (inimigo == NULL || inimigo->estado == INIMIGO_MORTO) return;

    inimigo->vida -= dano;
    if (inimigo->vida <= 0) {
        inimigo->vida = 0;
        inimigo->estado = INIMIGO_MORTO;
    }
}

Inimigo *encontrarInimigoMaisProximo(Inimigo *vetor, int n, Vector2 posJogador) {
    Inimigo *maisProximo = NULL;
    float menorDistancia = 0.0f;

    for (int i = 0; i < n; i++) {
        Inimigo *ini = (vetor + i);
        if (ini->estado == INIMIGO_MORTO) continue;

        float dx = ini->pos.x - posJogador.x;
        float dy = ini->pos.y - posJogador.y;
        float distancia = sqrtf(dx * dx + dy * dy);

        if (maisProximo == NULL || distancia < menorDistancia) {
            maisProximo = ini;
            menorDistancia = distancia;
        }
    }
    return maisProximo;
}

void curarTodos(Inimigo *vetor, int n, int cura) {
    for (int i = 0; i < n; i++) {
        Inimigo *ini = (vetor + i);
        if (ini->estado == INIMIGO_MORTO) continue;

        ini->vida += cura;
        if (ini->vida > VIDA_MAXIMA) ini->vida = VIDA_MAXIMA;
    }
}

Inimigo *encontrarInimigoMaisFraco(Inimigo *vetor, int n) {
    Inimigo *maisFraco = NULL;

    for (int i = 0; i < n; i++) {
        Inimigo *ini = (vetor + i);
        if (ini->estado == INIMIGO_MORTO) continue;

        if (maisFraco == NULL || ini->vida < maisFraco->vida) {
            maisFraco = ini;
        }
    }
    return maisFraco;
}

int main(void) {
    InitWindow(LARGURA, ALTURA, "Atividade 4 - Vetor de Struct");
    SetTargetFPS(60);

    Inimigo *inimigos = (Inimigo *)malloc(TOTAL_INIMIGOS * sizeof(Inimigo));
    inicializarInimigos(inimigos, TOTAL_INIMIGOS);

    Vector2 jogador = { LARGURA / 2.0f, ALTURA / 2.0f };
    float velocidade = 200.0f;
    int usarMaisFraco = 0;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        if (IsKeyDown(KEY_RIGHT)) jogador.x += velocidade * dt;
        if (IsKeyDown(KEY_LEFT)) jogador.x -= velocidade * dt;
        if (IsKeyDown(KEY_UP)) jogador.y -= velocidade * dt;
        if (IsKeyDown(KEY_DOWN)) jogador.y += velocidade * dt;

        if (IsKeyPressed(KEY_TAB)) usarMaisFraco = !usarMaisFraco;

        if (IsKeyPressed(KEY_SPACE)) {
            Inimigo *alvo = usarMaisFraco
                ? encontrarInimigoMaisFraco(inimigos, TOTAL_INIMIGOS)
                : encontrarInimigoMaisProximo(inimigos, TOTAL_INIMIGOS, jogador);
            atingirInimigo(alvo, DANO_TIRO);
        }

        if (IsKeyPressed(KEY_C)) {
            curarTodos(inimigos, TOTAL_INIMIGOS, CURA_QUANTIDADE);
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawCircleV(jogador, 20.0f, BLUE);

        for (int i = 0; i < TOTAL_INIMIGOS; i++) {
            Inimigo *ini = (inimigos + i);
            if (ini->estado == INIMIGO_MORTO) continue;

            DrawCircleV(ini->pos, ini->raio, MAROON);
            DrawText(TextFormat("%d", ini->vida), (int)ini->pos.x - 10, (int)ini->pos.y - 30, 16, BLACK);
        }

        DrawText("SETAS: mover | ESPACO: atirar | C: curar todos | TAB: alternar alvo", 10, 10, 18, DARKGRAY);
        DrawText(usarMaisFraco ? "Alvo: mais fraco" : "Alvo: mais proximo", 10, 35, 18, DARKGRAY);

        EndDrawing();
    }

    free(inimigos);
    CloseWindow();
    return 0;
}
