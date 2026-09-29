#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#define LARGURA_JANELA 800
#define ALTURA_JANELA  600
#define MAX_ENTIDADES  50
#define RAIO_JOGADOR   15.0f
#define TOTAL_INIMIGOS_INICIAL 5
#define TOTAL_ITENS_INICIAL    6
#define TAM_NOME 16
#define ARQUIVO_PLACAR "placar.txt"
#define ARQUIVO_SAVE   "save.bin"

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
    TipoEntidade  tipo;
    Vector2       pos;
    float         raio;
    int           vida;
    Color         cor;
    ExtraEntidade extra;
} Entidade;

Entidade *vetorEntidades[MAX_ENTIDADES];
int totalEntidades = 0;

char mensagem[64] = "";
float tempoMensagem = 0.0f;

void definirMensagem(const char *texto) {
    snprintf(mensagem, sizeof(mensagem), "%s", texto);
    tempoMensagem = 3.0f;
}

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
            e->extra.valor = 0;
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

bool adicionarEntidade(Entidade *e) {
    if (e == NULL || totalEntidades >= MAX_ENTIDADES) return false;
    vetorEntidades[totalEntidades] = e;
    totalEntidades++;
    return true;
}

void removerEntidade(int indice) {
    if (indice < 0 || indice >= totalEntidades) return;

    free(vetorEntidades[indice]);
    vetorEntidades[indice] = vetorEntidades[totalEntidades - 1];
    totalEntidades--;
}

void liberarTodasEntidades(void) {
    for (int i = 0; i < totalEntidades; i++) {
        free(vetorEntidades[i]);
        vetorEntidades[i] = NULL;
    }
    totalEntidades = 0;
}

Vector2 posicaoAleatoria(void) {
    return (Vector2){ (float)GetRandomValue(30, LARGURA_JANELA - 30),
                       (float)GetRandomValue(30, ALTURA_JANELA - 30) };
}

void ordenarMaisProximoParaSegundaPosicao(Entidade *jogador) {
    if (totalEntidades < 3) return;

    int indiceMaisProximo = 1;
    float menorDistancia = 0.0f;
    bool primeiro = true;

    for (int i = 1; i < totalEntidades; i++) {
        Entidade *e = vetorEntidades[i];
        float dx = e->pos.x - jogador->pos.x;
        float dy = e->pos.y - jogador->pos.y;
        float distancia = sqrtf(dx * dx + dy * dy);

        if (primeiro || distancia < menorDistancia) {
            menorDistancia = distancia;
            indiceMaisProximo = i;
            primeiro = false;
        }
    }

    if (indiceMaisProximo != 1) {
        Entidade *tmp = vetorEntidades[1];
        vetorEntidades[1] = vetorEntidades[indiceMaisProximo];
        vetorEntidades[indiceMaisProximo] = tmp;
    }
}

void desenharEntidade(Entidade *e) {
    DrawCircleV(e->pos, e->raio, e->cor);
    DrawCircleLines((int)e->pos.x, (int)e->pos.y, e->raio, BLACK);
}

void salvarPlacarTexto(const char *nome, int pontuacao) {
    FILE *arquivo = fopen(ARQUIVO_PLACAR, "a");
    if (arquivo == NULL) {
        definirMensagem("Erro ao abrir placar.txt");
        return;
    }

    fprintf(arquivo, "%s %d\n", nome, pontuacao);
    fclose(arquivo);
    definirMensagem("Pontuacao salva em placar.txt");
}

int lerMelhorPontuacao(char *nomeMelhor) {
    nomeMelhor[0] = '\0';

    FILE *arquivo = fopen(ARQUIVO_PLACAR, "r");
    if (arquivo == NULL) return 0;

    char nomeLido[TAM_NOME];
    int melhor = 0, valor = 0;

    while (fscanf(arquivo, "%15s %d", nomeLido, &valor) == 2) {
        if (valor > melhor) {
            melhor = valor;
            strcpy(nomeMelhor, nomeLido);
        }
    }

    fclose(arquivo);
    return melhor;
}

bool salvarJogoBinario(void) {
    FILE *arquivo = fopen(ARQUIVO_SAVE, "wb");
    if (arquivo == NULL) return false;

    fwrite(&totalEntidades, sizeof(int), 1, arquivo);
    for (int i = 0; i < totalEntidades; i++) {
        fwrite(vetorEntidades[i], sizeof(Entidade), 1, arquivo);
    }

    fclose(arquivo);
    return true;
}

bool carregarJogoBinario(void) {
    FILE *arquivo = fopen(ARQUIVO_SAVE, "rb");
    if (arquivo == NULL) return false;

    int totalSalvo = 0;
    if (fread(&totalSalvo, sizeof(int), 1, arquivo) != 1 ||
        totalSalvo < 1 || totalSalvo > MAX_ENTIDADES) {
        fclose(arquivo);
        return false;
    }

    liberarTodasEntidades();
    for (int i = 0; i < totalSalvo; i++) {
        Entidade *e = (Entidade *)malloc(sizeof(Entidade));
        if (e == NULL) break;
        if (fread(e, sizeof(Entidade), 1, arquivo) != 1) {
            free(e);
            break;
        }
        adicionarEntidade(e);
    }

    fclose(arquivo);
    return totalEntidades > 0;
}

void apagarJogoSalvo(void) {
    if (remove(ARQUIVO_SAVE) == 0) {
        definirMensagem("Save apagado com sucesso");
    } else {
        definirMensagem("Nenhum save encontrado");
    }
}

int main(void) {
    InitWindow(LARGURA_JANELA, ALTURA_JANELA, "Atividade 6 - Arquivos: texto e binario");
    SetTargetFPS(60);

    char nomeJogador[TAM_NOME] = "";
    int tamNome = 0;
    bool digitandoNome = true;

    char nomeMelhor[TAM_NOME];
    int melhorPontuacao = lerMelhorPontuacao(nomeMelhor);

    Entidade *jogador = criarEntidade(ENTIDADE_JOGADOR, (Vector2){LARGURA_JANELA / 2.0f, ALTURA_JANELA / 2.0f});
    adicionarEntidade(jogador);

    for (int i = 0; i < TOTAL_INIMIGOS_INICIAL; i++) {
        adicionarEntidade(criarEntidade(ENTIDADE_INIMIGO, posicaoAleatoria()));
    }
    for (int i = 0; i < TOTAL_ITENS_INICIAL; i++) {
        adicionarEntidade(criarEntidade(ENTIDADE_ITEM, posicaoAleatoria()));
    }

    float velocidade = 250.0f;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        if (tempoMensagem > 0) tempoMensagem -= dt;

        if (digitandoNome) {
            int tecla = GetCharPressed();
            while (tecla > 0) {
                if (tecla > 32 && tecla < 127 && tamNome < TAM_NOME - 1) {
                    nomeJogador[tamNome++] = (char)tecla;
                    nomeJogador[tamNome] = '\0';
                }
                tecla = GetCharPressed();
            }
            if (IsKeyPressed(KEY_BACKSPACE) && tamNome > 0) {
                nomeJogador[--tamNome] = '\0';
            }
            if (IsKeyPressed(KEY_ENTER) && tamNome > 0) {
                digitandoNome = false;
            }

            BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("Digite seu nome e pressione ENTER:", 200, 220, 24, DARKGRAY);
            DrawRectangle(200, 270, 400, 40, LIGHTGRAY);
            DrawText(nomeJogador, 210, 278, 24, BLACK);
            EndDrawing();
            continue;
        }

        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) jogador->pos.x += velocidade * dt;
        if (IsKeyDown(KEY_LEFT)  || IsKeyDown(KEY_A)) jogador->pos.x -= velocidade * dt;
        if (IsKeyDown(KEY_DOWN)  || IsKeyDown(KEY_S)) jogador->pos.y += velocidade * dt;
        if (IsKeyDown(KEY_UP)    || IsKeyDown(KEY_W)) jogador->pos.y -= velocidade * dt;

        if (jogador->pos.x < jogador->raio) jogador->pos.x = jogador->raio;
        if (jogador->pos.x > LARGURA_JANELA - jogador->raio) jogador->pos.x = LARGURA_JANELA - jogador->raio;
        if (jogador->pos.y < jogador->raio) jogador->pos.y = jogador->raio;
        if (jogador->pos.y > ALTURA_JANELA - jogador->raio) jogador->pos.y = ALTURA_JANELA - jogador->raio;

        if (IsKeyPressed(KEY_N) && totalEntidades < MAX_ENTIDADES) {
            Entidade *novoItem = criarEntidade(ENTIDADE_ITEM, posicaoAleatoria());
            adicionarEntidade(novoItem);
        }

        if (IsKeyPressed(KEY_F5)) {
            salvarPlacarTexto(nomeJogador, jogador->extra.valor);
            melhorPontuacao = lerMelhorPontuacao(nomeMelhor);
        }

        if (IsKeyPressed(KEY_F6)) {
            if (salvarJogoBinario()) definirMensagem("Jogo salvo em save.bin");
            else definirMensagem("Erro ao salvar o jogo");
        }

        if (IsKeyPressed(KEY_F9)) {
            if (carregarJogoBinario()) {
                jogador = vetorEntidades[0];
                definirMensagem("Jogo carregado de save.bin");
            } else {
                definirMensagem("Nenhum save valido encontrado");
            }
        }

        if (IsKeyPressed(KEY_DELETE)) {
            apagarJogoSalvo();
        }

        for (int i = totalEntidades - 1; i >= 1; i--) {
            Entidade *e = vetorEntidades[i];
            float dx = e->pos.x - jogador->pos.x;
            float dy = e->pos.y - jogador->pos.y;
            float distancia = (dx * dx + dy * dy);
            float somaRaios = (e->raio + jogador->raio) * (e->raio + jogador->raio);

            if (distancia <= somaRaios) {
                if (e->tipo == ENTIDADE_INIMIGO) {
                    jogador->vida -= e->extra.dano;
                    if (jogador->vida < 0) jogador->vida = 0;
                    removerEntidade(i);
                } else if (e->tipo == ENTIDADE_ITEM) {
                    jogador->extra.valor += e->extra.valor;
                    removerEntidade(i);
                }
            }
        }

        ordenarMaisProximoParaSegundaPosicao(jogador);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        for (int i = 0; i < totalEntidades; i++) {
            desenharEntidade(vetorEntidades[i]);
        }

        if (totalEntidades > 1) {
            DrawCircleLines((int)vetorEntidades[1]->pos.x, (int)vetorEntidades[1]->pos.y,
                             vetorEntidades[1]->raio + 5, LIME);
        }

        DrawText(TextFormat("Jogador: %s", nomeJogador), 10, 10, 20, DARKGRAY);
        DrawText(TextFormat("Vida: %d", jogador->vida), 10, 35, 20, DARKGRAY);
        DrawText(TextFormat("Pontos: %d", jogador->extra.valor), 10, 60, 20, DARKGRAY);
        DrawText(TextFormat("Entidades: %d", totalEntidades), 10, 85, 20, DARKGRAY);

        if (melhorPontuacao > 0) {
            DrawText(TextFormat("Recorde: %s - %d", nomeMelhor, melhorPontuacao), 10, 110, 20, DARKGREEN);
        } else {
            DrawText("Recorde: --", 10, 110, 20, DARKGREEN);
        }

        if (tempoMensagem > 0) {
            DrawText(mensagem, 10, ALTURA_JANELA - 55, 22, MAROON);
        }

        DrawText("SETAS/WASD: mover | N: item | F5: placar | F6: salvar | F9: carregar | DEL: apagar save",
                 10, ALTURA_JANELA - 25, 16, GRAY);

        EndDrawing();
    }

    liberarTodasEntidades();

    CloseWindow();
    return 0;
}
