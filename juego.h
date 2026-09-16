#ifndef JUEGO_H
#define JUEGO_H

#include <string>
#include <vector>

#include "jugador.h"
#include "territorio.h"

using namespace std;

class Juego{

private:

    // jugadores de la partida
    vector<Jugador> jugadores;

    // territorios del tablero
    vector<Territorio> territorios;

    // indica de quien es el turno
    int turnoActual;

    // indica si la partida ya fue cargada
    bool inicializado;

    // indica si la partida ya termino
    bool terminado;

    // controla las etapas del turno
    bool unidadesObtenidas;
    bool ataqueRealizado;


    // busca un jugador por su nombre
    int buscarJugador(const string& nombre) const;

    // busca un territorio por su codigo
    int buscarTerritorio(const string& codigo) const;

    // busca un jugador usando su color
    int buscarJugadorPorColor(const string& color) const;


    // cuenta cuantos territorios tiene un jugador
    int contarTerritoriosJugador(const string& color) const;

    // calcula la bonificacion por continentes completos
    int calcularBonificacionContinentes(const string& color) const;

    // devuelve la cantidad inicial de unidades segun el numero de jugadores
    int unidadesInicialesPorJugador(int cantidadJugadores) const;


    // cambia el turno al siguiente jugador
    void cambiarTurno();

    // revisa si algun jugador ya gano la partida
    void verificarGanador();

    // genera un numero aleatorio entre 1 y 6
    int lanzarDado() const;


    // valida que un jugador pueda realizar una etapa de su turno
    bool validarTurnoJugador(const string& nombreJugador,
                             int etapa) const;


    // muestra los territorios de un jugador
    void listarTerritoriosDe(const string& color) const;

    // muestra los territorios enemigos que se pueden atacar desde uno dado
    void listarObjetivosDesde(int indiceOrigen) const;


    // crea los 42 territorios del tablero
    void cargarTerritorios();

    // crea las conexiones entre territorios vecinos
    void cargarVecinos();

    // conecta dos territorios en ambos sentidos
    void conectar(const string& codigoA,
                  const string& codigoB);


public:

    // constructor
    Juego();


    // ---------------- COMPONENTE 1 ----------------

    // inicia una partida desde un archivo
    void inicializar(const string& nombreArchivo);

    // obtiene y distribuye las unidades del jugador
    void obtenerUnidades(const string& nombreJugador);

    // realiza la etapa de ataque
    void atacar(const string& nombreJugador);

    // realiza la etapa de fortificacion
    void fortificar(const string& nombreJugador);

    // muestra el estado actual de la partida
    void estadoJuego() const;


    // permite comprobar si un codigo corresponde
    // a uno de los territorios reales del tablero
    bool codigoTerritorioValido(const string& codigo) const;
};

#endif