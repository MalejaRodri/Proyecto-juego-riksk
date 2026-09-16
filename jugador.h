#ifndef JUGADOR_H
#define JUGADOR_H

#include <string>

using namespace std;

class Jugador {

private:
    string nombre;
    string color;

public:

    // Constructor vacío
    Jugador();

    // Constructor con nombre y color
    Jugador(const string& nombre, const string& color);

    // Consultar nombre del jugador
    string getNombre() const;

    // Consultar color del jugador
    string getColor() const;

    // Modificar nombre
    void setNombre(const string& nombre);

    // Modificar color
    void setColor(const string& color);
};

#endif