
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "ascension_magios.h"

const char MOVIMIENTO_ARRIBA_ = 'W';
const char MOVIMIENTO_ABAJO_ = 'S';
const char MOVIMIENTO_DERECHA_ = 'D';
const char MOVIMIENTO_IZQUIERDA_ = 'A';
const char SIMBOLO_HECHIZO_REVELADOR_ = 'H';
const char SIMBOLO_ANTORCHA_ = 'L';
const int ESTADO_JUEGO_GANADO = 1;
const int ESTADO_JUEGO_PERDIDO = -1;
const int ESTADO_JUEGO_EN_CURSO = 0;

void final_juego(int estado_final)
{
    if (estado_final == ESTADO_JUEGO_GANADO)
    {
        printf("PASASTE TODOS LOS NIVELES! FELICIDADES!\n");
    }
    else if (estado_final == ESTADO_JUEGO_PERDIDO)
    {
        printf("...PERDISTE! Te quedaste sin vidas!!\n");
    }
}
bool es_valido_movimiento(char movimiento)
{
    bool movimiento_valido = false;
    if (movimiento == MOVIMIENTO_IZQUIERDA_ || movimiento == MOVIMIENTO_ARRIBA_ || movimiento == MOVIMIENTO_ABAJO_ || movimiento == MOVIMIENTO_DERECHA_ || movimiento == SIMBOLO_HECHIZO_REVELADOR_ || movimiento == SIMBOLO_ANTORCHA_)
    {
        movimiento_valido = true;
    }
    return movimiento_valido;
}
void pedir_movimiento(char *movimiento)
{
    printf("Ingrese su movimiento O herramienta ( -W- -A- -S- -D- -H- -L- ): ");
    scanf(" %c", movimiento);
}
void solicitar_movimiento(char *movimiento)
{
    pedir_movimiento(movimiento);
    while (!(es_valido_movimiento(*movimiento)))
    {
        pedir_movimiento(movimiento);
    };
}
int main()
{
    srand((unsigned)time(NULL));
    juego_t juego;
    inicializar_juego(&juego);
    char movimiento = ' ';
    int estado_juego_actual = ESTADO_JUEGO_EN_CURSO;

    while (estado_juego_actual == ESTADO_JUEGO_EN_CURSO)
    {
        printf("Nivel actual: %d\n", juego.nivel_actual);
        mostrar_juego(juego);
        solicitar_movimiento(&movimiento);
        system("clear");
        realizar_jugada(&juego, movimiento);
        if (estado_nivel(juego.niveles[juego.nivel_actual], juego.homero) == ESTADO_JUEGO_GANADO)
        {
            cambiar_nivel(&juego);
        }

        estado_juego_actual = estado_juego(juego);
    };
    final_juego(estado_juego_actual);
}
