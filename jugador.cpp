#include "jugador.h"

// Constructor vacío
Jugador::Jugador() {
    nombre = "";
    color = "";
}

// Constructor con datos
Jugador::Jugador(const string& nombre, const string& color) {
    this->nombre = nombre;
    this->color = color;
}

// Retorna el nombre
string Jugador::getNombre() const {
    return nombre;
}

// Retorna el color
string Jugador::getColor() const {
    return color;
}

// Cambia el nombre
void Jugador::setNombre(const string& nombre) {
    this->nombre = nombre;
}

// Cambia el color
void Jugador::setColor(const string& color) {
    this->color = color;
}