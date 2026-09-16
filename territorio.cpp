#include "territorio.h"

using namespace std;

// territorio sin datos, se usa cuando el vector necesita crear elementos vacios
Territorio::Territorio(){
    codigo = "";
    nombre = "";
    continente = "";
    propietario = "";
    unidades = 0;
}


// Territorio del tablero; al crearlo todavia no lo ocupa nadie
Territorio::Territorio(const string& codigo, const string& nombre, const string& continente){
    this->codigo = codigo;
    this->nombre = nombre;
    this->continente = continente;

    propietario = "";
    unidades = 0;
}


string Territorio::getCodigo() const{
    return codigo;
}


string Territorio::getNombre() const{
    return nombre;
}


string Territorio::getContinente() const{
    return continente;
}


// Color del jugador que lo controla; vacio si no tiene dueño
string Territorio::getPropietario() const{
    return propietario;
}


int Territorio::getUnidades() const{
    return unidades;
}


void Territorio::setPropietario(const string& propietario){
    this->propietario = propietario;
}


// Cambia la cantidad de unidades del territorio
void Territorio::setUnidades(int nuevasUnidades){

    // Solo permite valores mayores o iguales a cero
    if (nuevasUnidades >= 0){
        unidades = nuevasUnidades;
    }
}


void Territorio::agregarUnidades(int cantidad){

    // Solo agrega unidades si la cantidad es positiva
    if (cantidad > 0){
        unidades += cantidad;
    }
}


void Territorio::quitarUnidades(int cantidad){

    // Solo quita unidades si la cantidad es valida
    // y no supera las unidades que tiene el territorio
    if (cantidad > 0 && cantidad <= unidades){
        unidades -= cantidad;
    }
}

// Agrega un territorio vecino evitando repetirlo
void Territorio::agregarVecino(const string& codigo){

    // Evita agregar el mismo territorio como su propio vecino
    if (codigo == this->codigo){
        return;
    }

    // Revisa si el vecino ya existe
    for (unsigned int i = 0; i < vecinos.size(); i++){

        if (vecinos[i] == codigo){
            return;
        }
    }

    // Si no estaba guardado, lo agrega
    vecinos.push_back(codigo);
}

// Revisa si un territorio esta conectado con otro
bool Territorio::esVecino(const string& codigo) const{

    // Busca el codigo dentro de la lista de vecinos
    for (unsigned int i = 0; i < vecinos.size(); i++){

        if (vecinos[i] == codigo){
            return true;
        }
    }

    // Si no lo encuentra, no son vecinos
    return false;
}