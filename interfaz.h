#ifndef INTERFAZ_H
#define INTERFAZ_H

#include <string>
#include "juego.h"

using namespace std;


// valida que el nombre del jugador tenga maximo 8 caracteres y no tenga espacios
bool nombreJugadorValido(const string& nombre);


// muestra la forma correcta de usar un comando
void mostrarUso(const string& comando);


// muestra un error cuando la cantidad de parametros es incorrecta
void errorParametros(const string& comando);


// muestra un error cuando el nombre del jugador no es valido
void errorNombreJugador(const string& comando);


// muestra todos los comandos disponibles
void mostrarAyuda();


// muestra la ayuda de un comando especifico
void mostrarAyudaComando(const string& comando);


// procesa lo que escribe el usuario en consola
// devuelve false cuando el usuario escribe salir
bool procesarComando(const string& entrada, Juego& juego);


#endif