#include "raylib.h"
#include <stdlib.h>
#include <stdbool.h>

#define LARGURA_JANELA 800
#define ALTURA_JANELA  600
#define QTD_MOEDAS     12
#define TEMPO_REAPARECER 3.0f

typedef enum {
    MOEDA_BRONZE,
    MOEDA_PRATA,
    MOEDA_OURO,
    MOEDA_DIAMANTE
} TipoMoeda;

typedef struct {
    Vector2   pos;
    float     raio;
    TipoMoeda tipo;
    int       valor;
    bool      coletada;
    float     tempoColeta;
} Moeda;

Color corDaMoeda(TipoMoeda tipo) {
    switch (tipo) {
        case MOEDA_BRONZE:   return (Color){160, 90, 40, 255};
        case MOEDA_PRATA:    return (Color){190, 190, 190, 255};
        case MOEDA_OURO:     return GOLD;
        case MOEDA_DIAMANTE: return SKYBLUE;
        default:             return WHITE;
    }
}

int valorDaMoeda(TipoMoeda tipo) {
    switch (tipo) {
        case MOEDA_BRONZE:   return 5;
        case MOEDA_PRATA:    return 10;
        case MOEDA_OURO:     return 25;
        case MOEDA_DIAMANTE: return 50;
        default:             return 0;
    }
}

TipoMoeda sortearTipoMoeda(void) {
    int chanceDiamante = GetRandomValue(0, 9);
    if (chanceDiamante == 0) {
        return MOEDA_DIAMANTE;
    }
    return (TipoMoeda)GetRandomValue(MOEDA_BRONZE, MOEDA_OURO);
}

Vector2 sortearPosicao(void) {
    return (Vector2){
        (float)GetRandomValue(30, LARGURA_JANELA - 30),
        (float)GetRandomValue(30, ALTURA_JANELA - 30)
    };
}

Moeda *criarMoedas(int quantidade) {
    Moeda *moedas = (Moeda *)malloc(quantidade * sizeof(Moeda));
    for (int i = 0; i < quantidade; i++) {
        Moeda *m = (moedas + i);
        m->pos         = sortearPosicao();
        m->raio        = 10.0f;
        m->tipo        = sortearTipoMoeda();
        m->valor       = valorDaMoeda(m->tipo);
        m->coletada    = false;
        m->tempoColeta = 0.0f;
    }
    return moedas;
}

bool tentarColetar(Moeda *m, Vector2 posJogador, float raioJogador) {
    if (m->coletada) return false;

    float dx = m->pos.x - posJogador.x;
    float dy = m->pos.y - posJogador.y;
    float distancia = (dx * dx + dy * dy);
    float somaRaios = (m->raio + raioJogador) * (m->raio + raioJogador);

    if (distancia <= somaRaios) {
        m->coletada    = true;
        m->tempoColeta = (float)GetTime();
        return true;
    }
    return false;
}

void atualizarReaparecimento(Moeda *moedas, int quantidade) {
    for (int i = 0; i < quantidade; i++) {
        Moeda *m = (moedas + i);
        if (m->coletada && (GetTime() - m->tempoColeta) >= TEMPO_REAPARECER) {
            m->pos      = sortearPosicao();
            m->tipo     = sortearTipoMoeda();
            m->valor    = valorDaMoeda(m->tipo);
            m->coletada = false;
        }
    }
}

void desenharMoeda(Moeda *m) {
    if (m->coletada) return;
    DrawCircleV(m->pos, m->raio, corDaMoeda(m->tipo));
    DrawCircleLines((int)m->pos.x, (int)m->pos.y, m->raio, BLACK);
}

int main(void) {
    InitWindow(LARGURA_JANELA, ALTURA_JANELA, "Atividade 2");
    SetTargetFPS(60);

    Vector2 posJogador = {LARGURA_JANELA / 2.0f, ALTURA_JANELA / 2.0f};
    float raioJogador = 15.0f;
    float velocidade = 250.0f;
    int pontuacao = 0;

    Moeda *moedas = criarMoedas(QTD_MOEDAS);

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) posJogador.x += velocidade * dt;
        if (IsKeyDown(KEY_LEFT)  || IsKeyDown(KEY_A)) posJogador.x -= velocidade * dt;
        if (IsKeyDown(KEY_DOWN)  || IsKeyDown(KEY_S)) posJogador.y += velocidade * dt;
        if (IsKeyDown(KEY_UP)    || IsKeyDown(KEY_W)) posJogador.y -= velocidade * dt;

        if (posJogador.x < raioJogador) posJogador.x = raioJogador;
        if (posJogador.x > LARGURA_JANELA - raioJogador) posJogador.x = LARGURA_JANELA - raioJogador;
        if (posJogador.y < raioJogador) posJogador.y = raioJogador;
        if (posJogador.y > ALTURA_JANELA - raioJogador) posJogador.y = ALTURA_JANELA - raioJogador;

        for (int i = 0; i < QTD_MOEDAS; i++) {
            Moeda *m = (moedas + i);
            if (tentarColetar(m, posJogador, raioJogador)) {
                pontuacao += m->valor;
            }
        }

        atualizarReaparecimento(moedas, QTD_MOEDAS);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        for (int i = 0; i < QTD_MOEDAS; i++) {
            desenharMoeda(moedas + i);
        }

        DrawCircleV(posJogador, raioJogador, BLUE);

        DrawText(TextFormat("Pontuacao: %d", pontuacao), 10, 10, 24, DARKGRAY);
        DrawText("Setas/WASD para mover", 10, ALTURA_JANELA - 30, 18, GRAY);

        EndDrawing();
    }

    free(moedas);

    CloseWindow();
    return 0;
}
