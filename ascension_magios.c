#include <time.h>
#include "ascension_magios.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "utiles.h"
#include <locale.h>

/*
    -> Falta pre y pos
    -> ERROR ALGOTRON: timeout despues o durante (no se sabe) de realizar jugada.
*/

const int CANTIDAD_INICIAL_ANTORCHAS = 5;
const int CANTIDAD_INICIAL_HECHIZOS_REVELADORES = 5;
const int CANTIDAD_INICIAL_VIDAS = 5;
const int DISTANCIA_MANHATTAN_REVELAR = 3;
const int NIVEL_INICIAL = 0;
const int NUMERO_NIVEL_PASAJE = 1;
const int MAX_PIEDRAS_CASTIGO = 10;
const int MAX_TOTEMS = 5;
const char MOVIMIENTO_ARRIBA = 'W';
const char MOVIMIENTO_ABAJO = 'S';
const char MOVIMIENTO_DERECHA = 'D';
const char MOVIMIENTO_IZQUIERDA = 'A';
const char SIMBOLO_HECHIZO_REVELADOR = 'H';
const char SIMBOLO_ANTORCHA = 'L';
const char SIMBOLO_HOMERO = 'H';
const char SIMBOLO_PERGAMINO = 'P';
const char SIMBOLO_PIEDRA_CASTIGO = 'R';
const char SIMBOLO_RUNA = 'U';
const char SIMBOLO_ALTAR = 'A';
const char SIMBOLO_CAMINO = 'C';
const char SIMBOLO_PARED = 'X';
const char SIMBOLO_CATAPULTA = 'F';
const char SIMBOLO_TOTEM = 'T';
const int INICIALIZACION_NO_BASURA = -1;
const int ESTADO_JUEGO_GANADO_ = 1;
const int ESTADO_JUEGO_PERDIDO_ = -1;
const int ESTADO_JUEGO_EN_CURSO_ = 0;

void generar_posiciones_aleatorias(int *fila_aleatoria_generada, int *columna_aleatoria_generada)
{
    *fila_aleatoria_generada = rand() % MAX_FILAS;
    *columna_aleatoria_generada = rand() % MAX_COLUMNAS;
}
bool son_posiciones_iguales(int fila_aleatoria_generada, int columna_aleatoria_generada, int fila_comparacion, int columna_comparacion)
{
    bool son_iguales = false;
    if ((fila_aleatoria_generada == fila_comparacion) && (columna_aleatoria_generada == columna_comparacion))
    {
        son_iguales = true;
    }
    return son_iguales;
}

bool piso_es_runa(coordenada_t camino[MAX_CAMINO], int posicion_fila_homero, int posicion_col_homero, int tope_camino)
{
    bool es_runa = false;
    if (tope_camino <= 0)
    {
        return false;
    }
    if ((son_posiciones_iguales(posicion_fila_homero, posicion_col_homero, camino[0].fil, camino[0].col)))
    {
        es_runa = true;
    }
    return es_runa;
}
bool es_igual_posicion_altar(coordenada_t camino[MAX_CAMINO], int tope_camino, int fila_aleatoria_generada, int columna_aleatoria_generada)
{
    bool es_misma_posicion_altar = false;
    if ((son_posiciones_iguales(fila_aleatoria_generada, columna_aleatoria_generada, camino[tope_camino - 1].fil, camino[tope_camino - 1].col)))
    {
        es_misma_posicion_altar = true;
    }
    return es_misma_posicion_altar;
}
bool es_pared(coordenada_t paredes[MAX_PAREDES], int tope_paredes, int fila_mov, int col_mov)
{
    bool posicion_es_pared = false;
    int i = 0;
    while (!posicion_es_pared && (i < tope_paredes))
    {
        if ((son_posiciones_iguales(fila_mov, col_mov, paredes[i].fil, paredes[i].col)))
        {
            posicion_es_pared = true;
        }
        i++;
    }
    return posicion_es_pared;
}
bool es_igual_a_posicion_inicial_homero(coordenada_t camino[MAX_CAMINO], int posicion_fil, int posicion_col)
{
    bool posicion_igual_homero = false;
    if (son_posiciones_iguales(posicion_fil, posicion_col, camino[0].fil, camino[0].col))
    {
        posicion_igual_homero = true;
    }
    return posicion_igual_homero;
}
bool es_posicion_homero(int posicion_fila, int posicion_col, int pos_fila_homero, int pos_col_homero)
{
    bool posicion_igual_homero = false;
    if (son_posiciones_iguales(posicion_fila, posicion_col, pos_fila_homero, pos_col_homero))
    {
        posicion_igual_homero = true;
    }
    return posicion_igual_homero;
}
bool es_camino(coordenada_t camino[MAX_CAMINO], int tope_camino, int fil_mov, int col_mov)
{
    bool posicion_es_camino = false;
    int i = 0;
    while (!posicion_es_camino && (i < tope_camino))
    {
        if (son_posiciones_iguales(fil_mov, col_mov, camino[i].fil, camino[i].col))
        {
            posicion_es_camino = true;
        }
        i++;
    }
    return posicion_es_camino;
}
bool es_igual_posicion_totem(nivel_t nivel_juego, int fila_aleatoria_generada, int columna_aleatoria_generada)
{
    bool es_misma_posicion_totem = false;
    for (int i = 0; i < nivel_juego.tope_herramientas; i++)
    {
        if (son_posiciones_iguales(fila_aleatoria_generada, columna_aleatoria_generada, nivel_juego.herramientas[i].posicion.fil, nivel_juego.herramientas[i].posicion.col))
        {
            es_misma_posicion_totem = true;
        }
    }
    return es_misma_posicion_totem;
}

bool es_correcta_inicializacion_pergamino(nivel_t nivel_juego, int fila_aleatoria_generada, int columna_aleatoria_generada)
{
    bool posicion_valida = true;
    if (es_pared(nivel_juego.paredes, nivel_juego.tope_paredes, fila_aleatoria_generada, columna_aleatoria_generada))
    {
        posicion_valida = false;
    }
    else if (es_igual_a_posicion_inicial_homero(nivel_juego.camino, fila_aleatoria_generada, columna_aleatoria_generada))
    {
        posicion_valida = false;
    }
    else if (!(es_camino(nivel_juego.camino, nivel_juego.tope_camino, fila_aleatoria_generada, columna_aleatoria_generada)))
    {
        posicion_valida = false;
    }
    else if (es_igual_posicion_altar(nivel_juego.camino, nivel_juego.tope_camino, fila_aleatoria_generada, columna_aleatoria_generada))
    {
        posicion_valida = false;
    }
    else if (piso_es_runa(nivel_juego.camino, fila_aleatoria_generada, columna_aleatoria_generada, nivel_juego.tope_camino))
    {
        posicion_valida = false;
    }
    return posicion_valida;
}
void generar_posicion_valida_pergamino(nivel_t nivel_juego, int *fila_aleatoria_generada, int *columna_aleatoria_generada)
{
    while (!(es_correcta_inicializacion_pergamino(nivel_juego, *fila_aleatoria_generada, *columna_aleatoria_generada)))
    {
        generar_posiciones_aleatorias(fila_aleatoria_generada, columna_aleatoria_generada);
    }
}
void asignar_posicion_objeto(coordenada_t *objeto, int fila_aleatoria_generada, int columna_aleatoria_generada)
{
    objeto->fil = fila_aleatoria_generada;
    objeto->col = columna_aleatoria_generada;
}
void inicializar_posicion_pergamino(nivel_t nivel_juego, coordenada_t *pergamino)
{
    int fila_aleatoria_generada_pergamino = INICIALIZACION_NO_BASURA;
    int columna_aleatoria_generada_pergamino = INICIALIZACION_NO_BASURA;
    generar_posiciones_aleatorias(&fila_aleatoria_generada_pergamino, &columna_aleatoria_generada_pergamino);
    generar_posicion_valida_pergamino(nivel_juego, &fila_aleatoria_generada_pergamino, &columna_aleatoria_generada_pergamino);
    asignar_posicion_objeto(pergamino, fila_aleatoria_generada_pergamino, columna_aleatoria_generada_pergamino);
}
bool validez_posicion_igual_pergamino(nivel_t nivel_juego, int fila_aleatoria_generada, int columna_aleatoria_generada)
{
    bool es_misma_posicion = false;
    if (son_posiciones_iguales(fila_aleatoria_generada, columna_aleatoria_generada, nivel_juego.pergamino.fil, nivel_juego.pergamino.col))
    {
        es_misma_posicion = true;
    }
    return es_misma_posicion;
}
bool validez_superposicion_obstaculo(nivel_t nivel_juego, int fila_aleatoria_generada, int columna_aleatoria_generada)
{
    bool superposicion = false;
    for (int i = 0; i < nivel_juego.tope_obstaculos; i++)
    {
        if (son_posiciones_iguales(fila_aleatoria_generada, columna_aleatoria_generada, nivel_juego.obstaculos[i].posicion.fil, nivel_juego.obstaculos[i].posicion.col))
        {
            superposicion = true;
        }
    }
    return superposicion;
}
bool posicion_obstaculo_superposicion_en_mapa(nivel_t nivel_juego, int fila_aleatoria_generada, int columna_aleatoria_generada)
{
    bool posicion_valida = true;
    if (es_pared(nivel_juego.paredes, nivel_juego.tope_paredes, fila_aleatoria_generada, columna_aleatoria_generada))
    {
        posicion_valida = false;
    }
    else if (es_igual_a_posicion_inicial_homero(nivel_juego.camino, fila_aleatoria_generada, columna_aleatoria_generada))
    {
        posicion_valida = false;
    }
    else if (validez_posicion_igual_pergamino(nivel_juego, fila_aleatoria_generada, columna_aleatoria_generada))
    {
        posicion_valida = false;
    }
    else if (validez_superposicion_obstaculo(nivel_juego, fila_aleatoria_generada, columna_aleatoria_generada))
    {
        posicion_valida = false;
    }
    else if (es_igual_posicion_totem(nivel_juego, fila_aleatoria_generada, columna_aleatoria_generada))
    {
        posicion_valida = false;
    }
    else if (es_igual_posicion_altar(nivel_juego.camino, nivel_juego.tope_camino, fila_aleatoria_generada, columna_aleatoria_generada))
    {
        posicion_valida = false;
    }
    else if (piso_es_runa(nivel_juego.camino, fila_aleatoria_generada, columna_aleatoria_generada, nivel_juego.tope_camino))
    {
        posicion_valida = false;
    }

    return posicion_valida;
}

void validez_posicion_obstaculo(nivel_t nivel_juego, int *fila_aleatoria_generada, int *columna_aleatoria_generada)
{
    while (!(posicion_obstaculo_superposicion_en_mapa(nivel_juego, *fila_aleatoria_generada, *columna_aleatoria_generada)))
    {
        generar_posiciones_aleatorias(fila_aleatoria_generada, columna_aleatoria_generada);
    }
}

bool es_posicion_valida_bola_catapulta(nivel_t nivel_juego, int fila_generada_aleatoria_catapulta, int columna_generada_aleatoria_catapulta)
{
    bool posicion_valida = true;
    if (es_pared(nivel_juego.paredes, nivel_juego.tope_paredes, fila_generada_aleatoria_catapulta, columna_generada_aleatoria_catapulta))
    {
        posicion_valida = false;
    }
    else if (es_camino(nivel_juego.camino, nivel_juego.tope_camino, fila_generada_aleatoria_catapulta, columna_generada_aleatoria_catapulta))
    {
        posicion_valida = false;
    }
    else if (validez_posicion_igual_pergamino(nivel_juego, fila_generada_aleatoria_catapulta, columna_generada_aleatoria_catapulta))
    {
        posicion_valida = false;
    }
    else if (es_igual_posicion_totem(nivel_juego, fila_generada_aleatoria_catapulta, columna_generada_aleatoria_catapulta))
    {
        posicion_valida = false;
    }
    else if (validez_superposicion_obstaculo(nivel_juego, fila_generada_aleatoria_catapulta, columna_generada_aleatoria_catapulta))
    {
        posicion_valida = false;
    }

    return posicion_valida;
}
void inicializar_catapulta(nivel_t *nivel_juego)
{
    int fila_generada_aleatoria_catapulta = INICIALIZACION_NO_BASURA;
    int columna_generada_aleatoria_catapulta = INICIALIZACION_NO_BASURA;
    int tope = nivel_juego->tope_obstaculos;

    generar_posiciones_aleatorias(&fila_generada_aleatoria_catapulta, &columna_generada_aleatoria_catapulta);
    while (!(es_posicion_valida_bola_catapulta(*nivel_juego, fila_generada_aleatoria_catapulta, columna_generada_aleatoria_catapulta)))
    {
        generar_posiciones_aleatorias(&fila_generada_aleatoria_catapulta, &columna_generada_aleatoria_catapulta);
    }

    nivel_juego->obstaculos[tope].tipo = SIMBOLO_CATAPULTA;
    nivel_juego->tope_obstaculos++;
    asignar_posicion_objeto(&nivel_juego->obstaculos[tope].posicion, fila_generada_aleatoria_catapulta, columna_generada_aleatoria_catapulta);
}
void inicializar_posiciones_obstaculos(nivel_t *nivel_juego)
{
    nivel_juego->tope_obstaculos = 0;
    int columna_generada_aleatoria_obs = INICIALIZACION_NO_BASURA;
    int fila_generada_aleatoria_obs = INICIALIZACION_NO_BASURA;

    for (int i = 0; i < MAX_PIEDRAS_CASTIGO; i++)
    {
        generar_posiciones_aleatorias(&fila_generada_aleatoria_obs, &columna_generada_aleatoria_obs);
        validez_posicion_obstaculo(*nivel_juego, &fila_generada_aleatoria_obs, &columna_generada_aleatoria_obs);
        nivel_juego->obstaculos[i].tipo = SIMBOLO_PIEDRA_CASTIGO;
        asignar_posicion_objeto(&nivel_juego->obstaculos[i].posicion, fila_generada_aleatoria_obs, columna_generada_aleatoria_obs);
        nivel_juego->tope_obstaculos++;
    }
    inicializar_catapulta(nivel_juego);
}
bool superpone_totem_otras_inicializaciones_y_es_camino(nivel_t nivel_juego, int fila_generada_aleatoria_totem, int columna_generada_aleatoria_totem)
{
    bool posicion_valida = true;
    if (es_pared(nivel_juego.paredes, nivel_juego.tope_paredes, fila_generada_aleatoria_totem, columna_generada_aleatoria_totem))
    {
        posicion_valida = false;
    }
    else if (es_igual_a_posicion_inicial_homero(nivel_juego.camino, fila_generada_aleatoria_totem, columna_generada_aleatoria_totem))
    {
        posicion_valida = false;
    }
    else if (validez_posicion_igual_pergamino(nivel_juego, fila_generada_aleatoria_totem, columna_generada_aleatoria_totem))
    {
        posicion_valida = false;
    }
    else if (es_igual_posicion_totem(nivel_juego, fila_generada_aleatoria_totem, columna_generada_aleatoria_totem))
    {
        posicion_valida = false;
    }
    return posicion_valida;
}

void validez_posicion_totem(nivel_t nivel_juego, int *fila_generada_aleatoria_totem, int *columna_generada_aleatoria_totem)
{
    while (!(superpone_totem_otras_inicializaciones_y_es_camino(nivel_juego, *fila_generada_aleatoria_totem, *columna_generada_aleatoria_totem)))
    {
        generar_posiciones_aleatorias(fila_generada_aleatoria_totem, columna_generada_aleatoria_totem);
    }
}

void inicializar_totems(nivel_t *nivel_juego)
{
    int fila_generada_aleatoria_totem = INICIALIZACION_NO_BASURA;
    int columna_generada_aleatoria_totem = INICIALIZACION_NO_BASURA;
    for (int i = 0; i < MAX_TOTEMS; i++)
    {
        generar_posiciones_aleatorias(&fila_generada_aleatoria_totem, &columna_generada_aleatoria_totem);
        validez_posicion_totem(*nivel_juego, &fila_generada_aleatoria_totem, &columna_generada_aleatoria_totem);
        nivel_juego->herramientas[i].tipo = SIMBOLO_TOTEM;
        asignar_posicion_objeto(&nivel_juego->herramientas[i].posicion, fila_generada_aleatoria_totem, columna_generada_aleatoria_totem);
        nivel_juego->tope_herramientas++;
    }
}
void inicializar_herramientas(personaje_t *homero, nivel_t *nivel_juego)
{
    homero->antorchas = CANTIDAD_INICIAL_ANTORCHAS;
    homero->hechizos_reveladores = CANTIDAD_INICIAL_HECHIZOS_REVELADORES;
    homero->antorcha_encendida = false;
    homero->recolecto_pergamino = false;
    inicializar_totems(nivel_juego);
}

void inicializar_posicion_inicial_homero(personaje_t *homero, coordenada_t camino[MAX_CAMINO])
{

    homero->posicion.fil = camino[0].fil;
    homero->posicion.col = camino[0].col;
}

void inicializar_mapas(juego_t *juego)
{
    juego->tope_niveles = 0;
    for (int i = 0; i < MAX_NIVELES; i++)
    {
        obtener_mapa(juego->niveles[i].paredes, &juego->niveles[i].tope_paredes, juego->niveles[i].camino, &juego->niveles[i].tope_camino, i + NUMERO_NIVEL_PASAJE);
        juego->niveles[i].tope_herramientas = 0;
        juego->niveles[i].tope_obstaculos = 0;
        juego->tope_niveles++;
    }
}

/****************************************************************/
void inicializar_juego(juego_t *juego)
{
    juego->camino_visible = true;
    juego->nivel_actual = NIVEL_INICIAL + 1;
    inicializar_mapas(juego);
    inicializar_posicion_inicial_homero(&juego->homero, juego->niveles[0].camino);
    juego->homero.vidas_restantes = CANTIDAD_INICIAL_VIDAS;
    for (int nivel = 0; nivel < MAX_NIVELES; nivel++)
    {
        inicializar_posicion_pergamino(juego->niveles[nivel], &juego->niveles[nivel].pergamino);
        inicializar_herramientas(&juego->homero, &juego->niveles[nivel]);
        inicializar_posiciones_obstaculos(&juego->niveles[nivel]);
    }
    juego->tope_niveles = MAX_NIVELES;
};
/****************************************************************/
void reflejar_movimiento_realizado(int *fila_posicion_homero, int *col_posicion_homero, char movimiento)
{
    if (movimiento == MOVIMIENTO_ARRIBA)
    {
        *fila_posicion_homero -= 1;
    }
    else if (movimiento == MOVIMIENTO_IZQUIERDA)
    {
        *col_posicion_homero -= 1;
    }
    else if (movimiento == MOVIMIENTO_ABAJO)
    {
        *fila_posicion_homero += 1;
    }
    else if (movimiento == MOVIMIENTO_DERECHA)
    {
        *col_posicion_homero += 1;
    }
}
bool es_correcto_mover_homero(int fila_posicion_homero, int col_posicion_homero)
{
    bool movimiento_dentro_limites = false;

    if ((fila_posicion_homero >= 0) && (col_posicion_homero >= 0) && (fila_posicion_homero < MAX_FILAS) && (col_posicion_homero < MAX_COLUMNAS))
    {
        movimiento_dentro_limites = true;
    }

    return movimiento_dentro_limites;
}
bool homero_piso_obstaculo(int pos_fila_homero, int pos_col_homero, objeto_t obstaculos[MAX_ELEMENTOS], int tope_obstaculos)
{
    bool homero_piso_obstaculo = false;
    int i = 0;
    while (!homero_piso_obstaculo && (i < tope_obstaculos))
    {
        if (son_posiciones_iguales(pos_fila_homero, pos_col_homero, obstaculos[i].posicion.fil, obstaculos[i].posicion.col))
        {
            homero_piso_obstaculo = true;
        }
        i++;
    }
    return homero_piso_obstaculo;
}
void eliminar_posicion_obstaculo(objeto_t obstaculos[MAX_ELEMENTOS], int *tope_obstaculos, int posicion_fila_homero, int posicion_col_homero)
{
    int indice_obstaculo_a_eliminar = INICIALIZACION_NO_BASURA;
    bool homero_piso_obstaculo = false;
    int i = 0;
    while (i < *tope_obstaculos && !homero_piso_obstaculo)
    {
        if (son_posiciones_iguales(posicion_fila_homero, posicion_col_homero, obstaculos[i].posicion.fil, obstaculos[i].posicion.col))
        {
            homero_piso_obstaculo = true;
            indice_obstaculo_a_eliminar = i;
        }
        i++;
    }
    if (indice_obstaculo_a_eliminar != INICIALIZACION_NO_BASURA)
    {
        for (int f = indice_obstaculo_a_eliminar; f < *tope_obstaculos - 1; f++)
        {
            obstaculos[f] = obstaculos[f + 1];
        }

        (*tope_obstaculos)--;
    }
}

bool homero_piso_pergamino(coordenada_t pergamino, int posicion_fila_homero, int posicion_col_homero)
{
    bool homero_piso_pergamino = false;
    if ((pergamino.fil == posicion_fila_homero) && (pergamino.col == posicion_col_homero))
    {
        homero_piso_pergamino = true;
    }
    return homero_piso_pergamino;
}
void eliminar_posicion_pergamino(coordenada_t *pergamino)
{
    pergamino->fil = INICIALIZACION_NO_BASURA;
    pergamino->col = INICIALIZACION_NO_BASURA;
}
void inicializar_matriz_juego(char matriz_mapa[MAX_FILAS][MAX_COLUMNAS])
{
    for (int fila = 0; fila < MAX_FILAS; fila++)
    {
        for (int col = 0; col < MAX_COLUMNAS; col++)
        {
            matriz_mapa[fila][col] = ' ';
        }
    }
}

bool homero_piso_altar(coordenada_t camino[MAX_CAMINO], int tope_camino, int posicion_fila_homero, int posicion_col_homero)
{
    bool homero_piso_altar = false;
    if (tope_camino <= 0)
    {
        return false;
    }
    if ((son_posiciones_iguales(posicion_fila_homero, posicion_col_homero, camino[tope_camino - 1].fil, camino[tope_camino - 1].col)))
    {
        homero_piso_altar = true;
    }

    return homero_piso_altar;
}
bool validar_entrega_pergamino_al_altar(bool *recolecto_pergamino)
{
    bool pergamino_entregado = false;
    if (*recolecto_pergamino)
    {

        pergamino_entregado = true;
    }

    return pergamino_entregado;
}

void cambiar_posicion_pergamino_a_altar(nivel_t *nivel_juego, bool pergamino_entregado)
{
    if (pergamino_entregado)
    {
        nivel_juego->pergamino.fil = nivel_juego->camino[nivel_juego->tope_camino - 1].fil;
        nivel_juego->pergamino.col = nivel_juego->camino[nivel_juego->tope_camino - 1].col;
    }
}

bool homero_piso_totem(objeto_t herramientas[MAX_ELEMENTOS], int posicion_fila_homero, int posicion_col_homero)
{
    bool homero_piso_totem = false;
    int i = 0;
    while (i < MAX_ELEMENTOS && !homero_piso_totem)
    {
        if ((herramientas[i].tipo == SIMBOLO_TOTEM) && (herramientas[i].posicion.fil == posicion_fila_homero) && (herramientas[i].posicion.col == posicion_col_homero))
        {
            homero_piso_totem = true;
        }
        i++;
    }
    return homero_piso_totem;
}
void eliminar_posicion_totem(objeto_t *herramientas, int *tope_herramientas, int posicion_fila_homero, int posicion_col_homero)
{
    int indice_obstaculo_a_eliminar = INICIALIZACION_NO_BASURA;
    bool homero_piso_totem = false;
    int i = 0;
    while (i < *tope_herramientas && !homero_piso_totem)
    {
        if ((herramientas[i].tipo == SIMBOLO_TOTEM) && (son_posiciones_iguales(posicion_fila_homero, posicion_col_homero, herramientas[i].posicion.fil, herramientas[i].posicion.col)))
        {
            homero_piso_totem = true;
            indice_obstaculo_a_eliminar = i;
        }
        i++;
    }
    if (indice_obstaculo_a_eliminar != INICIALIZACION_NO_BASURA)
    {
        for (int f = indice_obstaculo_a_eliminar; f < *tope_herramientas - 1; f++)
        {
            herramientas[f] = herramientas[f + 1];
        }
        if (*tope_herramientas > 0)
        {
            (*tope_herramientas)--;
            printf("totem eliminado, quedan %d totems\n", (*tope_herramientas));
        }
    }
}
void sumar_vida_extra_homero(int *vidas_restantes)
{
    (*vidas_restantes)++;
}
bool bola_de_fuego_posicion_valida(nivel_t *nivel_juego, int fila_afectada_catapulta, int columna_afectada_catapulta, int posicion_fila_homero, int posicion_col_homero)
{
    bool caida_bola_fuego_valida = true;
    if (es_pared(nivel_juego->paredes, nivel_juego->tope_paredes, fila_afectada_catapulta, columna_afectada_catapulta))
    {
        caida_bola_fuego_valida = false;
    }
    else if (es_posicion_homero(fila_afectada_catapulta, columna_afectada_catapulta, posicion_fila_homero, posicion_col_homero))
    {
        caida_bola_fuego_valida = false;
    }
    else if (es_igual_posicion_altar(nivel_juego->camino, nivel_juego->tope_camino, fila_afectada_catapulta, columna_afectada_catapulta))
    {
        caida_bola_fuego_valida = false;
    }
    else if (piso_es_runa(nivel_juego->camino, fila_afectada_catapulta, columna_afectada_catapulta, nivel_juego->tope_camino))
    {
        caida_bola_fuego_valida = false;
    }

    return caida_bola_fuego_valida;
}
void posicion_afectada_random_catapulta(nivel_t *nivel_juego, int *fil_afectada_catapulta, int *col_afectada_catapulta, int posicion_fila_homero, int posicion_col_homero)
{
    do
    {
        generar_posiciones_aleatorias(fil_afectada_catapulta, col_afectada_catapulta);

    } while (!(bola_de_fuego_posicion_valida(nivel_juego, *fil_afectada_catapulta, *col_afectada_catapulta, posicion_fila_homero, posicion_col_homero)));
}

bool eliminar_posicion_camino_catapulta(nivel_t *nivel_juego, int fil_afectada_catapulta, int col_afectada_catapulta)
{
    int indice_posicion_a_eliminar = INICIALIZACION_NO_BASURA;
    bool indice_camino_a_eliminar_encontrado = false;
    int i = 0;
    bool camino_modificado = false;

    while ((i < nivel_juego->tope_camino) && !indice_camino_a_eliminar_encontrado)
    {
        if (son_posiciones_iguales(fil_afectada_catapulta, col_afectada_catapulta, nivel_juego->camino[i].fil, nivel_juego->camino[i].col))
        {
            indice_posicion_a_eliminar = i;
            indice_camino_a_eliminar_encontrado = true;
        }
        i++;
    }
    if (indice_posicion_a_eliminar != INICIALIZACION_NO_BASURA)
    {
        for (int f = indice_posicion_a_eliminar; f < nivel_juego->tope_camino - 1; f++)
        {
            nivel_juego->camino[f] = nivel_juego->camino[f + 1];
        }
        nivel_juego->tope_camino--;
        camino_modificado = true;
    }

    return camino_modificado;
}

void activar_catapulta(nivel_t *nivel_juego, int posicion_fila_homero, int posicion_col_homero)
{
    int fil_afectada_catapulta = INICIALIZACION_NO_BASURA;
    int col_afectada_catapulta = INICIALIZACION_NO_BASURA;
    bool camino_modificado = false;

    posicion_afectada_random_catapulta(nivel_juego, &fil_afectada_catapulta, &col_afectada_catapulta, posicion_fila_homero, posicion_col_homero);
    camino_modificado = eliminar_posicion_camino_catapulta(nivel_juego, fil_afectada_catapulta, col_afectada_catapulta);

    if (camino_modificado)
    {
        printf("SE ACTIVÓ LA CATAPULTA!! El camino se ha modificado!!\n");
    }
    else
    {
        printf("SE ACTIVÓ LA CATAPULTA!! Cayo en vacio y no paso nada.\n");
    }
}

void piedra_castigo_consecuencia(nivel_t *nivel_juego, int posicion_fila_homero, int posicion_col_homero, bool *recolecto_pergamino)
{
    inicializar_posicion_pergamino(*nivel_juego, &nivel_juego->pergamino); // mandar fil  y col de homero
    *recolecto_pergamino = false;
    eliminar_posicion_obstaculo(nivel_juego->obstaculos, &nivel_juego->tope_obstaculos, posicion_fila_homero, posicion_col_homero);
}
void pergamino_consecuencia(coordenada_t *pergamino, bool *recolecto_pergamino)
{
    eliminar_posicion_pergamino(pergamino);
    *recolecto_pergamino = true;
}
void runa_consecuencia(nivel_t *nivel_juego, int nivel_actual, int posicion_fila_homero, int posicion_col_homero)
{
    activar_catapulta(nivel_juego, posicion_fila_homero, posicion_col_homero);
}
void altar_consecuencia(nivel_t *nivel_juego, bool recolecto_pergamino)
{
    bool pergamino_entregado = validar_entrega_pergamino_al_altar(&recolecto_pergamino);
    if (pergamino_entregado)
    {
        printf("Has entregado el PERGAMINO al ALTAR!!\n");
        cambiar_posicion_pergamino_a_altar(nivel_juego, pergamino_entregado);
    }
    else
    {
        printf("No tienes todavpía el PERGAMINO para entregar al ALTAR!!\n");
    }
}
void totem_consecuencia(nivel_t *nivel_juego, int posicion_fila_homero, int posicion_col_homero, int *vidas_restantes)
{
    eliminar_posicion_totem(nivel_juego->herramientas, &nivel_juego->tope_herramientas, posicion_fila_homero, posicion_col_homero);
    sumar_vida_extra_homero(vidas_restantes);
}

void verificar_movimiento_posicion(nivel_t *nivel_juego, personaje_t *homero, int nivel_actual, bool *camino_visible)
{
    int posicion_fila_homero = homero->posicion.fil;
    int posicion_col_homero = homero->posicion.col;

    if (nivel_juego->tope_camino <= 0)
    {
        return;
    }

    if (homero_piso_obstaculo(posicion_fila_homero, posicion_col_homero, nivel_juego->obstaculos, nivel_juego->tope_obstaculos))
    {
        printf("Te encontraste con una PIEDRA DE CASTIGO!! El PERGAMINO se fue una posición aleatoria del mapa!! \n");
        piedra_castigo_consecuencia(nivel_juego, posicion_fila_homero, posicion_col_homero, &homero->recolecto_pergamino);
    }
    else if (homero_piso_pergamino(nivel_juego->pergamino, posicion_fila_homero, posicion_col_homero))
    {
        printf("Has encontrado el PERGAMINO!!\n");
        pergamino_consecuencia(&nivel_juego->pergamino, &homero->recolecto_pergamino);
        *camino_visible = true;
    }
    else if (piso_es_runa(nivel_juego->camino, posicion_fila_homero, posicion_col_homero, nivel_juego->tope_camino))
    {
        runa_consecuencia(nivel_juego, nivel_actual, posicion_fila_homero, posicion_col_homero);
        *camino_visible = true;
    }
    else if (homero_piso_altar(nivel_juego->camino, nivel_juego->tope_camino, posicion_fila_homero, posicion_col_homero))
    {
        altar_consecuencia(nivel_juego, homero->recolecto_pergamino);
    }
    else if (homero_piso_totem(nivel_juego->herramientas, posicion_fila_homero, posicion_col_homero))
    {
        printf("+1!! VIDAS : %d\n", homero->vidas_restantes);
        totem_consecuencia(nivel_juego, posicion_fila_homero, posicion_col_homero, &homero->vidas_restantes);
    }
}
void consecuencia_hechizo_revelador(coordenada_t *camino, int tope_camino, int posicion_homero_col, int posicion_homero_fil, int *hechizos_reveladores)
{
    (*hechizos_reveladores)--;
}
void realizar_hechizo_revelador(nivel_t *nivel_juego, int posicion_homero_fil, int posicion_homero_col, bool *camino_visible)
{
    *camino_visible = true;
}

int calcular_distancia_manhattan(int fila_mapa, int col_mapa, int fil_homero, int col_homero)
{
    int distancia_manhattan = abs(fila_mapa - fil_homero) + abs(col_mapa - col_homero);
    return distancia_manhattan;
}
void cargar_camino_mapa(char matriz_mapa[MAX_FILAS][MAX_COLUMNAS], nivel_t nivel_juego)
{
    for (int i = 0; i < nivel_juego.tope_camino; i++)
    {
        matriz_mapa[nivel_juego.camino[i].fil][nivel_juego.camino[i].col] = SIMBOLO_CAMINO;
    }
}


void realizar_encendido_antorcha(bool *antorcha_encendida)
{
    *antorcha_encendida = true;
}
void eliminar_antorcha_usada(int *antorchas)
{
    (*antorchas)--;
    printf("-1!! ANTORCHAS : %d\n", *antorchas);
}
void eliminar_hechizo_usado(int *hechizos_reveladores)
{
    (*hechizos_reveladores)--;
    printf("-1!! HECHIZOS REVELADORES : %d\n", *hechizos_reveladores);
}
void eliminar_herramienta_usada(int *herramienta, char tipo_herramienta)
{
    (*herramienta)--;
    printf("-1!! %c : %d\n", tipo_herramienta, *herramienta);
}
void realizar_jugada_herramienta(nivel_t *nivel_juego, personaje_t *homero, char movimiento, int nivel_actual, bool *camino_visible)
{
    if (piso_es_runa(nivel_juego->camino, homero->posicion.fil, homero->posicion.col, nivel_juego->tope_camino))
    {
        return;
    }
    if (*camino_visible || homero->antorcha_encendida)
    {
        return;
    }
    if (movimiento == SIMBOLO_HECHIZO_REVELADOR)
    {
        if (homero->hechizos_reveladores > 0)
        {
            realizar_hechizo_revelador(nivel_juego, homero->posicion.fil, homero->posicion.col, camino_visible);
            eliminar_hechizo_usado(&homero->hechizos_reveladores);
        }
    }
    else if (movimiento == SIMBOLO_ANTORCHA)
    {
        if (homero->antorchas > 0)
        {
            realizar_encendido_antorcha(&homero->antorcha_encendida);
            eliminar_antorcha_usada(&homero->antorchas);
        }
    }
}
void consecuencia_movimiento_es_posicion_daño(int *vidas_restantes)
{
    if (*vidas_restantes > 0)
    {
        (*vidas_restantes)--;
    }
    if (*vidas_restantes < 0)
    {
        *vidas_restantes = 0;
    }
}
void asignar_posicion_nueva_homero(personaje_t *homero, int fila_posicion_homero_aux, int col_posicion_homero_aux)
{
    homero->posicion.fil = fila_posicion_homero_aux;
    homero->posicion.col = col_posicion_homero_aux;
}
void consecuencia_movimiento_valido(nivel_t *nivel_juego, personaje_t *homero, int nivel_actual, int fila_posicion_homero_aux, int col_posicion_homero_aux, bool *camino_visible)
{
    if (es_pared(nivel_juego->paredes, nivel_juego->tope_paredes, fila_posicion_homero_aux, col_posicion_homero_aux))
    {
        printf("Te chocaste contra una pared!!!\n");
    }
    else if (es_camino(nivel_juego->camino, nivel_juego->tope_camino, fila_posicion_homero_aux, col_posicion_homero_aux))
    {
        asignar_posicion_nueva_homero(homero, fila_posicion_homero_aux, col_posicion_homero_aux);
        verificar_movimiento_posicion(nivel_juego, homero, nivel_actual, camino_visible);
    }
    else
    {
        asignar_posicion_nueva_homero(homero, fila_posicion_homero_aux, col_posicion_homero_aux);
        consecuencia_movimiento_es_posicion_daño(&(homero->vidas_restantes));
        verificar_movimiento_posicion(nivel_juego, homero, nivel_actual, camino_visible);
        printf("Pisaste un espacio vacio!!! -1 VIDA \n");
        printf("VIDAS : %d\n", homero->vidas_restantes);
    }
}

void realizar_jugada_movimiento(juego_t *juego, char movimiento, int nivel_actual, int fila_posicion_homero_aux, int col_posicion_homero_aux)
{
    bool mover_homero = false;
    reflejar_movimiento_realizado(&fila_posicion_homero_aux, &col_posicion_homero_aux, movimiento);
    juego->homero.antorcha_encendida = false;
    juego->camino_visible = false;
    mover_homero = es_correcto_mover_homero(fila_posicion_homero_aux, col_posicion_homero_aux);
    if (mover_homero)
    {
        consecuencia_movimiento_valido(&juego->niveles[nivel_actual], &juego->homero, nivel_actual, fila_posicion_homero_aux, col_posicion_homero_aux, &juego->camino_visible);
    }
}
bool movimiento_es_herramienta(char movimiento)
{
    bool es_ingreso_herramienta = false;
    if ((movimiento == SIMBOLO_HECHIZO_REVELADOR) || (movimiento == SIMBOLO_ANTORCHA))
    {
        es_ingreso_herramienta = true;
    }
    return es_ingreso_herramienta;
}
void realizar_jugada(juego_t *juego, char movimiento)
{
    bool es_herramienta = false;
    int nivel_actual = juego->nivel_actual - 1;
    es_herramienta = movimiento_es_herramienta(movimiento);
    if (!es_herramienta)
    {
        int fila_posicion_homero_aux = juego->homero.posicion.fil;
        int col_posicion_homero_aux = juego->homero.posicion.col;
        realizar_jugada_movimiento(juego, movimiento, nivel_actual, fila_posicion_homero_aux, col_posicion_homero_aux);
    }
    else
    {
        realizar_jugada_herramienta(&juego->niveles[nivel_actual], &juego->homero, movimiento, nivel_actual, &juego->camino_visible);
    }
}

void revelar_mapa_manhattan(char matriz_mapa[MAX_FILAS][MAX_COLUMNAS], int fila_homero, int col_homero)
{
    int distancia_manhattan = INICIALIZACION_NO_BASURA;
    for (int fila = 0; fila < MAX_FILAS; fila++)
    {
        for (int col = 0; col < MAX_COLUMNAS; col++)
        {
            distancia_manhattan = calcular_distancia_manhattan(fila, col, fila_homero, col_homero);
            if (distancia_manhattan <= DISTANCIA_MANHATTAN_REVELAR)
            {
                printf("%c ", matriz_mapa[fila][col]);
            }
            else
            {
                printf("  ");
            }
        }
        printf("\n");
    }
}
void cargar_paredes_mapa(char matriz_mapa[MAX_FILAS][MAX_COLUMNAS], nivel_t nivel_juego, int posicion_homero_fil, int posicion_homero_col)
{
    for (int i = 0; i < nivel_juego.tope_paredes; i++)
    {
        matriz_mapa[nivel_juego.paredes[i].fil][nivel_juego.paredes[i].col] = SIMBOLO_PARED;
    }
    matriz_mapa[posicion_homero_fil][posicion_homero_col] = SIMBOLO_HOMERO;
}

void cargar_pergamino_mapa(char matriz_mapa[MAX_FILAS][MAX_COLUMNAS], nivel_t nivel_juego, bool homero_recolecto_pergamino)
{
    if (!homero_recolecto_pergamino)
    {
        matriz_mapa[nivel_juego.pergamino.fil][nivel_juego.pergamino.col] = SIMBOLO_PERGAMINO;
    }
}
void cargar_totem_mapa(char matriz_mapa[MAX_FILAS][MAX_COLUMNAS], nivel_t nivel_juego)
{
    if (nivel_juego.tope_herramientas > 0)
    {
        for (int i = 0; i < nivel_juego.tope_herramientas; i++)
        {
            matriz_mapa[nivel_juego.herramientas[i].posicion.fil][nivel_juego.herramientas[i].posicion.col] = SIMBOLO_TOTEM;
        }
    }
}
void cargar_obstaculos_mapa(char matriz_mapa[MAX_FILAS][MAX_COLUMNAS], nivel_t nivel_juego)
{
    for (int i = 0; i < nivel_juego.tope_obstaculos; i++)
    {
        matriz_mapa[nivel_juego.obstaculos[i].posicion.fil][nivel_juego.obstaculos[i].posicion.col] = SIMBOLO_PIEDRA_CASTIGO;
    }
}
void cargar_altar_runa_mapa(char matriz_mapa[MAX_FILAS][MAX_COLUMNAS], nivel_t nivel_juego)
{
    matriz_mapa[nivel_juego.camino[0].fil][nivel_juego.camino[0].col] = SIMBOLO_RUNA;
    matriz_mapa[nivel_juego.camino[nivel_juego.tope_camino - 1].fil][nivel_juego.camino[nivel_juego.tope_camino - 1].col] = SIMBOLO_RUNA;
}

void cargar_elementos_mapa(char matriz_mapa[MAX_FILAS][MAX_COLUMNAS], nivel_t nivel_juego, int posicion_homero_fil, int posicion_homero_col)
{
    for (int i = 0; i < nivel_juego.tope_camino; i++)
    {
        matriz_mapa[nivel_juego.camino[i].fil][nivel_juego.camino[i].col] = SIMBOLO_CAMINO;
    }
    for (int i = 0; i < nivel_juego.tope_paredes; i++)
    {
        matriz_mapa[nivel_juego.paredes[i].fil][nivel_juego.paredes[i].col] = SIMBOLO_PARED;
    }
    for(int i = 0; i < nivel_juego.tope_obstaculos; i++)
    {
        matriz_mapa[nivel_juego.obstaculos[i].posicion.fil][nivel_juego.obstaculos[i].posicion.col] = nivel_juego.obstaculos[i].tipo;
    }
    for(int i = 0; i < nivel_juego.tope_herramientas; i++)
    {
        matriz_mapa[nivel_juego.herramientas[i].posicion.fil][nivel_juego.herramientas[i].posicion.col] = nivel_juego.herramientas[i].tipo;
    }
    if (nivel_juego.pergamino.fil != INICIALIZACION_NO_BASURA && nivel_juego.pergamino.col != INICIALIZACION_NO_BASURA)
    {
        matriz_mapa[nivel_juego.pergamino.fil][nivel_juego.pergamino.col] = SIMBOLO_PERGAMINO;
    }
    if (nivel_juego.tope_camino > 0)
    {
        matriz_mapa[nivel_juego.camino[nivel_juego.tope_camino - 1].fil][nivel_juego.camino[nivel_juego.tope_camino - 1].col] = SIMBOLO_ALTAR;
        matriz_mapa[nivel_juego.camino[0].fil][nivel_juego.camino[0].col] = SIMBOLO_RUNA;
    }
    matriz_mapa[posicion_homero_fil][posicion_homero_col] = SIMBOLO_HOMERO;
}


void mostrar_juego(juego_t juego)
{
    char matriz_mapa[MAX_FILAS][MAX_COLUMNAS];
    int nivel = juego.nivel_actual - 1;
    inicializar_matriz_juego(matriz_mapa);
    cargar_elementos_mapa(matriz_mapa, juego.niveles[nivel], juego.homero.posicion.fil, juego.homero.posicion.col);

    if (juego.camino_visible)
    {
        for (int fila = 0; fila < MAX_FILAS; fila++)
        {
            for (int col = 0; col < MAX_COLUMNAS; col++)
            {
                printf("%c ", matriz_mapa[fila][col]);
            }
            printf("\n");
        }
    }
    else if(juego.homero.antorcha_encendida)
    {
        revelar_mapa_manhattan(matriz_mapa, juego.homero.posicion.fil, juego.homero.posicion.col);
    }
    else
    {
        for (int fila = 0; fila < MAX_FILAS; fila++)
        {
            for (int col = 0; col < MAX_COLUMNAS; col++)
            {
                if(matriz_mapa[fila][col] == SIMBOLO_CAMINO)
                {
                    printf("  ");
                }
                else
                {
                    printf("%c ", matriz_mapa[fila][col]);
                }
            }
            printf("\n");
        }
    }
}
void cambiar_nivel(juego_t *juego)
{
    int nivel = juego->nivel_actual - 1;
    if (estado_nivel(juego->niveles[nivel], juego->homero) == ESTADO_JUEGO_GANADO_)
    {
        if (nivel < MAX_NIVELES - 1)
        {
            juego->homero.recolecto_pergamino = false;
            juego->homero.antorchas = CANTIDAD_INICIAL_ANTORCHAS;
            juego->camino_visible = true;
            inicializar_posicion_inicial_homero(&juego->homero, juego->niveles[nivel].camino);
            juego->nivel_actual = nivel + 2;
            nivel++;
            printf("¡Has avanzado al siguiente nivel!\n");
        }
        else
        {
            printf("¡Has completado todos los niveles!\n");
        }
    }
}
/*************************************/
int estado_nivel(nivel_t nivel, personaje_t homero)
{
    int resultado_estado_nivel = ESTADO_JUEGO_EN_CURSO_;
    if ((homero.recolecto_pergamino) && (son_posiciones_iguales(homero.posicion.fil, homero.posicion.col, nivel.camino[nivel.tope_camino - 1].fil, nivel.camino[nivel.tope_camino - 1].col)))
    {
        resultado_estado_nivel = ESTADO_JUEGO_GANADO_;
    }

    return resultado_estado_nivel;
}
/*************************************/
int estado_juego(juego_t juego)
{
    int resultado_estado_juego = 0;
    int nivel_actual_juego = juego.nivel_actual - 1;
    if ((nivel_actual_juego == MAX_NIVELES - 1) && (estado_nivel(juego.niveles[nivel_actual_juego], juego.homero) == 1))
    {
        resultado_estado_juego = ESTADO_JUEGO_GANADO_;
    }
    else if (juego.homero.vidas_restantes <= 0)
    {
        resultado_estado_juego = ESTADO_JUEGO_PERDIDO_;
    }
    return resultado_estado_juego;
}
/*************************************/
