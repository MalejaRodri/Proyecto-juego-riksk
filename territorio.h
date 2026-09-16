#ifndef TERRITORIO_H
#define TERRITORIO_H

#include <string>
#include <vector>

using namespace std;

class Territorio{

private:

    string codigo;
    string nombre;
    string continente;
    string propietario;
    int unidades;

    vector<string> vecinos;

public:

    // Constructores
    Territorio();
    Territorio(const string& codigo,
               const string& nombre,
               const string& continente);

    // Getters
    string getCodigo() const;
    string getNombre() const;
    string getContinente() const;
    string getPropietario() const;
    int getUnidades() const;

    // Setters
    void setPropietario(const string& propietario);
    void setUnidades(int nuevasUnidades);

    // Manejo de unidades
    void agregarUnidades(int cantidad);
    void quitarUnidades(int cantidad);

    // Manejo de vecinos
    void agregarVecino(const string& codigo);
    bool esVecino(const string& codigo) const;
};

#endif