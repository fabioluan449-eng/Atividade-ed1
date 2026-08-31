
#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>

#define LARGURA_JANELA 800
#define ALTURA_JANELA  600
#define TAM_CELULA     40

typedef struct {
    Vector2 pos;
    Vector2 vel;
    float   raio;
    Color   cor;
} Bola;

int **criarMatriz(int linhas, int colunas) {
    int **matriz = (int **)malloc(linhas * sizeof(int *));
    for (int i = 0; i < linhas; i++) {
        matriz[i] = (int *)malloc(colunas * sizeof(int));
        for (int j = 0; j < colunas; j++) {
            matriz[i][j] = 0;
        }
    }
    return matriz;
}

void liberarMatriz(int **matriz, int linhas) {
    for (int i = 0; i < linhas; i++) {
        free(matriz[i]);
    }
    free(matriz);
}

Bola *criarBolas(int quantidade) {
    Bola *bolas = (Bola *)malloc(quantidade * sizeof(Bola));
    for (int i = 0; i < quantidade; i++) {
        Bola *b = (bolas + i);
        b->pos = (Vector2){
            (float)GetRandomValue(50, LARGURA_JANELA - 50),
            (float)GetRandomValue(50, ALTURA_JANELA - 50)
        };
        b->vel = (Vector2){
            (float)GetRandomValue(-4, 4),
            (float)GetRandomValue(-4, 4)
        };
        if (b->vel.x == 0) b->vel.x = 2;
        if (b->vel.y == 0) b->vel.y = 2;
        b->raio = 12.0f;
        b->cor  = (Color){
            (unsigned char)GetRandomValue(50, 255),
            (unsigned char)GetRandomValue(50, 255),
            (unsigned char)GetRandomValue(50, 255),
            255
        };
    }
    return bolas;
}

void atualizarBola(Bola *b) {
    b->pos.x += b->vel.x;
    b->pos.y += b->vel.y;

    if (b->pos.x - b->raio < 0 || b->pos.x + b->raio > LARGURA_JANELA)
        b->vel.x *= -1;
    if (b->pos.y - b->raio < 0 || b->pos.y + b->raio > ALTURA_JANELA)
        b->vel.y *= -1;
}

Bola *adicionarBola(Bola *bolas, int *quantidadeBolas) {
    int nova_quantidade = *quantidadeBolas + 1;
    Bola *novo = (Bola *)realloc(bolas, nova_quantidade * sizeof(Bola));
    if (novo == NULL) {

        return bolas;
    }
    bolas = novo;

    Bola *b = (bolas + (*quantidadeBolas));
    b->pos = (Vector2){
        (float)GetRandomValue(50, LARGURA_JANELA - 50),
        (float)GetRandomValue(50, ALTURA_JANELA - 50)
    };
    b->vel = (Vector2){
        (float)GetRandomValue(-4, 4),
        (float)GetRandomValue(-4, 4)
    };
    if (b->vel.x == 0) b->vel.x = 2;
    if (b->vel.y == 0) b->vel.y = 2;
    b->raio = 12.0f;
    b->cor  = (Color){
        (unsigned char)GetRandomValue(50, 255),
        (unsigned char)GetRandomValue(50, 255),
        (unsigned char)GetRandomValue(50, 255),
        255
    };

    *quantidadeBolas = nova_quantidade;
    return bolas;
}

Bola *removerBola(Bola *bolas, int *quantidadeBolas) {
    if (*quantidadeBolas <= 0) return bolas;

    int nova_quantidade = *quantidadeBolas - 1;

    if (nova_quantidade == 0) {
        free(bolas);
        *quantidadeBolas = 0;
        return NULL;
    }

    Bola *novo = (Bola *)realloc(bolas, nova_quantidade * sizeof(Bola));
    if (novo == NULL) {
        return bolas;
    }

    *quantidadeBolas = nova_quantidade;
    return novo;
}

void marcarCelulaVisitada(int **matriz, int linhas, int colunas,
                           Bola *b, int *celulasVisitadas) {
    int coluna = (int)(b->pos.x / TAM_CELULA);
    int linha  = (int)(b->pos.y / TAM_CELULA);

    if (linha >= 0 && linha < linhas && coluna >= 0 && coluna < colunas) {
        if (matriz[linha][coluna] == 0) {
            matriz[linha][coluna] = 1;
            (*celulasVisitadas)++;
        }
    }
}

void desenharMatriz(int **matriz, int linhas, int colunas) {
    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            Color cor = (matriz[i][j] == 1)
                ? (Color){255, 230, 150, 255}
                : (Color){40, 40, 40, 255};

            DrawRectangle(j * TAM_CELULA, i * TAM_CELULA,
                           TAM_CELULA, TAM_CELULA, cor);
            DrawRectangleLines(j * TAM_CELULA, i * TAM_CELULA,
                                TAM_CELULA, TAM_CELULA, (Color){20, 20, 20, 255});
        }
    }
}

void desenharBolas(Bola *bolas, int quantidadeBolas) {
    for (int i = 0; i < quantidadeBolas; i++) {
        Bola *b = (bolas + i);
        DrawCircleV(b->pos, b->raio, b->cor);
    }
}

int main(void) {
    InitWindow(LARGURA_JANELA, ALTURA_JANELA,
               "Atividade 1 - Alocacao Dinamica de Memoria");
    SetTargetFPS(60);

    int linhas   = ALTURA_JANELA / TAM_CELULA;
    int colunas  = LARGURA_JANELA / TAM_CELULA;
    int **grade  = criarMatriz(linhas, colunas);

    int quantidadeBolas = 12;
    Bola *bolas = criarBolas(quantidadeBolas);

    int celulasVisitadas = 0;

    while (!WindowShouldClose()) {

        if (IsKeyPressed(KEY_SPACE)) {
            bolas = adicionarBola(bolas, &quantidadeBolas);
        }
        if (IsKeyPressed(KEY_BACKSPACE)) {
            bolas = removerBola(bolas, &quantidadeBolas);
        }

        for (int i = 0; i < quantidadeBolas; i++) {
            Bola *b = (bolas + i);
            atualizarBola(b);
            marcarCelulaVisitada(grade, linhas, colunas, b, &celulasVisitadas);
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        desenharMatriz(grade, linhas, colunas);
        desenharBolas(bolas, quantidadeBolas);

        DrawText(TextFormat("Bolas: %d", quantidadeBolas), 10, 10, 20, BLACK);
        DrawText(TextFormat("Celulas visitadas: %d / %d",
                             celulasVisitadas, linhas * colunas),
                  10, 35, 20, BLACK);
        DrawText("ESPACO: adicionar bola | BACKSPACE: remover bola",
                  10, ALTURA_JANELA - 30, 18, DARKGRAY);

        EndDrawing();
    }

    if (bolas != NULL) free(bolas);
    liberarMatriz(grade, linhas);

    CloseWindow();
    return 0;
}
