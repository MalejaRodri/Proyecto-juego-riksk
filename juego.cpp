#include "juego.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>

using namespace std;

// Funciones auxiliares del archivo (no pertenecen a la clase)

// Muestra un mensaje y lee una linea completa del teclado
static string leerLinea(const string &mensaje)
{
    cout << mensaje;

    string linea;
    getline(cin, linea);

    return linea;
}

// Indica si ya no hay mas entrada disponible, para poder salir de los ciclos
static bool entradaTerminada()
{
    return !cin.good();
}

// Quita los espacios sobrantes al inicio y al final de un texto
static string limpiar(const string &texto)
{
    unsigned int inicio = 0;
    unsigned int fin = texto.size();

    while (inicio < fin && (texto[inicio] == ' ' || texto[inicio] == '\t' ||
                            texto[inicio] == '\r' || texto[inicio] == '\n'))
    {
        inicio++;
    }

    while (fin > inicio && (texto[fin - 1] == ' ' || texto[fin - 1] == '\t' ||
                            texto[fin - 1] == '\r' || texto[fin - 1] == '\n'))
    {
        fin--;
    }

    return texto.substr(inicio, fin - inicio);
}

// Convierte un texto a entero; retorna false si el texto no es un numero
static bool aEntero(const string &texto, int &resultado)
{
    string valor = limpiar(texto);

    if (valor.empty())
    {
        return false;
    }

    for (unsigned int i = 0; i < valor.size(); i++)
    {
        if (valor[i] < '0' || valor[i] > '9')
        {
            return false;
        }
    }

    stringstream ss(valor);
    ss >> resultado;

    return true;
}

// Pregunta algo que se responde con s o n, y repite hasta recibir una de las dos
static bool preguntarSiNo(const string &mensaje)
{
    while (true)
    {
        if (entradaTerminada())
        {
            return false;
        }

        string respuesta = limpiar(leerLinea(mensaje));

        if (entradaTerminada())
        {
            return false;
        }

        if (respuesta == "s" || respuesta == "S" ||
            respuesta == "si" || respuesta == "SI" || respuesta == "Si")
        {
            return true;
        }

        if (respuesta == "n" || respuesta == "N" ||
            respuesta == "no" || respuesta == "NO" || respuesta == "No")
        {
            return false;
        }

        cout << "Respuesta no valida. Escriba 's' o 'n'." << endl;
    }
}

// Constructor

// Arranca la partida vacia y arma el tablero fijo de Risk
Juego::Juego()
{
    turnoActual = 0;
    inicializado = false;
    terminado = false;
    unidadesObtenidas = false;
    ataqueRealizado = false;

    cargarTerritorios();
    cargarVecinos();

    // Semilla para que los dados no den siempre lo mismo
    srand(time(NULL));
}

// Busquedas

// Recorre el vector de jugadores buscando un nombre
int Juego::buscarJugador(const string &nombre) const
{
    for (unsigned int i = 0; i < jugadores.size(); i++)
    {
        if (jugadores[i].getNombre() == nombre)
        {
            return i;
        }
    }

    return -1;
}

// Recorre el vector de territorios buscando un codigo como 1.1 o 5.12
int Juego::buscarTerritorio(const string &codigo) const
{
    for (unsigned int i = 0; i < territorios.size(); i++)
    {
        if (territorios[i].getCodigo() == codigo)
        {
            return i;
        }
    }

    return -1;
}

// Busca al jugador dueño de un color
int Juego::buscarJugadorPorColor(const string &color) const
{
    for (unsigned int i = 0; i < jugadores.size(); i++)
    {
        if (jugadores[i].getColor() == color)
        {
            return i;
        }
    }

    return -1;
}

// La usa la interfaz para validar el territorio del comando costo_conquista
bool Juego::codigoTerritorioValido(const string &codigo) const
{
    return buscarTerritorio(codigo) != -1;
}

// Reglas del juego

// Cuenta cuantos territorios del tablero tienen ese color
int Juego::contarTerritoriosJugador(const string &color) const
{
    int cantidad = 0;

    for (unsigned int i = 0; i < territorios.size(); i++)
    {
        if (territorios[i].getPropietario() == color)
        {
            cantidad++;
        }
    }

    return cantidad;
}

// Revisa continente por continente: si el jugador tiene todos sus territorios,
// suma la bonificacion correspondiente
int Juego::calcularBonificacionContinentes(const string &color) const
{
    int bonificacion = 0;

    string continentes[6] = {
        "America del Norte",
        "America del Sur",
        "Europa",
        "Africa",
        "Asia",
        "Australia"};

    int bonos[6] = {5, 2, 5, 3, 7, 2};

    for (int i = 0; i < 6; i++)
    {
        bool controlaContinente = true;
        bool tieneTerritorios = false;

        for (unsigned int j = 0; j < territorios.size(); j++)
        {
            if (territorios[j].getContinente() == continentes[i])
            {
                tieneTerritorios = true;

                // Basta un territorio ajeno para no controlar el continente
                if (territorios[j].getPropietario() != color)
                {
                    controlaContinente = false;
                }
            }
        }

        if (tieneTerritorios && controlaContinente)
        {
            bonificacion += bonos[i];
        }
    }

    return bonificacion;
}

// Unidades iniciales segun la cantidad de jugadores; -1 si el numero no sirve
int Juego::unidadesInicialesPorJugador(int cantidadJugadores) const
{
    if (cantidadJugadores == 3)
    {
        return 35;
    }

    if (cantidadJugadores == 4)
    {
        return 30;
    }

    if (cantidadJugadores == 5)
    {
        return 25;
    }

    if (cantidadJugadores == 6)
    {
        return 20;
    }

    return -1;
}

// Pasa el turno al siguiente jugador
void Juego::cambiarTurno()
{

    // Avanza al siguiente jugador
    turnoActual++;

    // Si ya paso el ultimo jugador vuelve al primero
    if (turnoActual >= (int)jugadores.size())
    {
        turnoActual = 0;
    }

    // El nuevo jugador todavia no ha obtenido sus unidades
    unidadesObtenidas = false;

    // El nuevo jugador todavia no ha realizado su ataque
    ataqueRealizado = false;
}

// Revisa si algun jugador ya controla todos los territorios
void Juego::verificarGanador()
{

    // Recorre todos los jugadores de la partida
    for (unsigned int i = 0; i < jugadores.size(); i++)
    {

        // Cuenta cuantos territorios tiene el jugador
        int cantidad =
            contarTerritoriosJugador(jugadores[i].getColor());

        // Si tiene los 42 territorios gana la partida
        if (cantidad == 42)
        {

            terminado = true;

            cout << "El jugador "
                 << jugadores[i].getNombre()
                 << " ha ganado la partida." << endl;

            return;
        }
    }
}

// Dado de seis caras
int Juego::lanzarDado() const
{
    return (rand() % 6) + 1;
}

/// Valida que el jugador pueda ejecutar la etapa solicitada de su turno
bool Juego::validarTurnoJugador(const string &nombreJugador, int etapa) const
{
    // Verifica que exista una partida iniciada
    if (!inicializado)
    {
        cout << "(Juego no inicializado) "
             << "Esta partida no ha sido inicializada correctamente." << endl;
        return false;
    }

    // Verifica que la partida no haya terminado
    if (terminado)
    {
        cout << "(Juego terminado) "
             << "Esta partida ya tuvo un ganador." << endl;
        return false;
    }

    // Busca al jugador por su nombre
    int indiceJugador = buscarJugador(nombreJugador);

    // Verifica que el jugador exista en la partida
    if (indiceJugador == -1)
    {
        cout << "(Jugador no valido) El jugador "
             << nombreJugador
             << " no forma parte de esta partida." << endl;
        return false;
    }

    // Verifica que sea el turno del jugador
    if (indiceJugador != turnoActual)
    {
        cout << "(Jugador fuera de turno) "
             << "No es el turno del jugador "
             << nombreJugador << "." << endl;
        return false;
    }

    // Para atacar primero se deben haber obtenido y ubicado las unidades
    if (etapa >= 1 && !unidadesObtenidas)
    {
        cout << "(Jugador no ha ubicado unidades) El jugador "
             << nombreJugador
             << " no ha ejecutado el comando obtener_unidades." << endl;
        return false;
    }

    // Para fortificar primero se debe haber terminado la etapa de ataque
    if (etapa >= 2 && !ataqueRealizado)
    {
        cout << "(Jugador no ha atacado) El jugador "
             << nombreJugador
             << " no ha ejecutado el comando atacar." << endl;
        return false;
    }

    // Si pasa todas las validaciones puede continuar
    return true;
}

// Comando inicializar

// Lee el archivo y valida la informacion antes de iniciar la partida
void Juego::inicializar(const string &nombreArchivo)
{

    // Verifica que no exista una partida iniciada
    if (inicializado)
    {
        cout << "(Juego en curso) "
             << "El juego ya ha sido inicializado." << endl;
        return;
    }

    // Intenta abrir el archivo
    ifstream archivo(nombreArchivo.c_str());

    // Verifica que el archivo exista y se pueda leer
    if (!archivo.is_open())
    {
        cout << "(Archivo erroneo) "
             << nombreArchivo
             << " no se encuentra o no puede leerse." << endl;
        return;
    }

    // Guarda las lineas del archivo que contienen informacion
    vector<string> lineas;
    string linea;

    while (getline(archivo, linea))
    {
        string limpia = limpiar(linea);

        if (!limpia.empty())
        {
            lineas.push_back(limpia);
        }
    }

    archivo.close();

    // Verifica que el archivo no este vacio
    if (lineas.size() == 0)
    {
        cout << "(Archivo vacio) "
             << nombreArchivo
             << " no contiene informacion." << endl;
        return;
    }

    // Lee la cantidad de jugadores de la primera linea
    int cantidadJugadores = 0;

    if (!aEntero(lineas[0], cantidadJugadores))
    {
        cout << "(Archivo sin formato) "
             << nombreArchivo
             << " no contiene informacion en el formato esperado." << endl;
        return;
    }

    // Calcula las unidades iniciales segun la cantidad de jugadores
    int unidadesEsperadas = unidadesInicialesPorJugador(cantidadJugadores);

    // Solo se permiten entre 3 y 6 jugadores
    if (unidadesEsperadas == -1)
    {
        cout << "(Archivo sin formato) "
             << nombreArchivo
             << " no contiene informacion en el formato esperado." << endl;
        return;
    }

    // Debe existir una linea con la cantidad, las lineas de jugadores
    // y las 42 lineas correspondientes a los territorios
    if ((int)lineas.size() != 1 + cantidadJugadores + 42)
    {
        cout << "(Archivo sin formato) "
             << nombreArchivo
             << " no contiene informacion en el formato esperado." << endl;
        return;
    }

    // Guarda temporalmente los jugadores leidos
    vector<Jugador> jugadoresLeidos;

    for (int i = 0; i < cantidadJugadores; i++)
    {

        stringstream ss(lineas[1 + i]);

        string nombre;
        string color;
        string sobrante;

        // Cada jugador debe tener solamente nombre y color
        if (!(ss >> nombre >> color) || (ss >> sobrante))
        {
            cout << "(Archivo sin formato) "
                 << nombreArchivo
                 << " no contiene informacion en el formato esperado." << endl;
            return;
        }

        // El nombre puede tener maximo 8 caracteres
        if (nombre.size() > 8)
        {
            cout << "(Archivo sin formato) "
                 << nombreArchivo
                 << " no contiene informacion en el formato esperado." << endl;
            return;
        }

        // Verifica que no existan nombres ni colores repetidos
        for (unsigned int j = 0; j < jugadoresLeidos.size(); j++)
        {

            if (jugadoresLeidos[j].getNombre() == nombre ||
                jugadoresLeidos[j].getColor() == color)
            {

                cout << "(Archivo sin formato) "
                     << nombreArchivo
                     << " no contiene informacion en el formato esperado." << endl;
                return;
            }
        }

        // Agrega el jugador si toda su informacion es valida
        jugadoresLeidos.push_back(Jugador(nombre, color));
    }

    // Crea una copia del tablero para cargar la informacion
    vector<Territorio> territoriosLeidos = territorios;

    // Deja inicialmente todos los territorios sin dueño y sin unidades
    for (unsigned int i = 0; i < territoriosLeidos.size(); i++)
    {
        territoriosLeidos[i].setPropietario("");
        territoriosLeidos[i].setUnidades(0);
    }

    // Guarda la cantidad de unidades que tiene cada jugador
    vector<int> unidadesPorJugador(jugadoresLeidos.size(), 0);

    // Lee los 42 territorios del archivo
    for (int i = 0; i < 42; i++)
    {

        stringstream ss(lineas[1 + cantidadJugadores + i]);

        string codigo;
        string color;
        string textoUnidades;
        string sobrante;

        // Cada territorio debe tener codigo, color y cantidad de unidades
        if (!(ss >> codigo >> color >> textoUnidades) || (ss >> sobrante))
        {
            cout << "(Archivo sin formato) "
                 << nombreArchivo
                 << " no contiene informacion en el formato esperado." << endl;
            return;
        }

        // Busca el codigo dentro de los territorios del tablero
        int indiceTerritorio = -1;

        for (unsigned int j = 0; j < territoriosLeidos.size(); j++)
        {

            if (territoriosLeidos[j].getCodigo() == codigo)
            {
                indiceTerritorio = j;
            }
        }

        // El codigo debe corresponder a un territorio existente
        if (indiceTerritorio == -1)
        {
            cout << "(Archivo sin formato) "
                 << nombreArchivo
                 << " no contiene informacion en el formato esperado." << endl;
            return;
        }

        // Si ya tiene propietario significa que el territorio esta repetido
        if (territoriosLeidos[indiceTerritorio].getPropietario() != "")
        {
            cout << "(Archivo sin formato) "
                 << nombreArchivo
                 << " no contiene informacion en el formato esperado." << endl;
            return;
        }

        // Busca que el color corresponda a alguno de los jugadores
        int indiceJugador = -1;

        for (unsigned int j = 0; j < jugadoresLeidos.size(); j++)
        {

            if (jugadoresLeidos[j].getColor() == color)
            {
                indiceJugador = j;
            }
        }

        // El color debe pertenecer a un jugador de la partida
        if (indiceJugador == -1)
        {
            cout << "(Archivo sin formato) "
                 << nombreArchivo
                 << " no contiene informacion en el formato esperado." << endl;
            return;
        }

        int unidades = 0;

        // Cada territorio debe tener minimo una unidad
        if (!aEntero(textoUnidades, unidades) || unidades < 1)
        {
            cout << "(Archivo sin formato) "
                 << nombreArchivo
                 << " no contiene informacion en el formato esperado." << endl;
            return;
        }

        // Guarda el propietario y las unidades del territorio
        territoriosLeidos[indiceTerritorio].setPropietario(color);
        territoriosLeidos[indiceTerritorio].setUnidades(unidades);

        // Suma las unidades que tiene el jugador
        unidadesPorJugador[indiceJugador] += unidades;
    }

    // Verifica que cada jugador tenga la cantidad correcta de unidades iniciales
    for (unsigned int i = 0; i < jugadoresLeidos.size(); i++)
    {

        if (unidadesPorJugador[i] != unidadesEsperadas)
        {
            cout << "(Archivo sin formato) "
                 << nombreArchivo
                 << " no contiene informacion en el formato esperado." << endl;
            return;
        }
    }

    // Si todo fue correcto se guarda la informacion en la partida
    jugadores = jugadoresLeidos;
    territorios = territoriosLeidos;

    // El primer jugador empieza el turno
    turnoActual = 0;

    // Actualiza el estado de la partida
    inicializado = true;
    terminado = false;
    unidadesObtenidas = false;
    ataqueRealizado = false;

    // Informa que la inicializacion fue correcta
    cout << "(Comando correcto) "
         << "El juego se ha inicializado correctamente." << endl;
}

// Comando obtener_unidades

// Imprime los territorios que ocupa un color, con sus unidades
void Juego::listarTerritoriosDe(const string &color) const
{
    for (unsigned int i = 0; i < territorios.size(); i++)
    {
        if (territorios[i].getPropietario() == color)
        {
            cout << "  " << territorios[i].getCodigo() << " "
                 << territorios[i].getNombre() << " ("
                 << territorios[i].getUnidades() << " unidades)" << endl;
        }
    }
}

// Calcula cuantas unidades le tocan al jugador y le pregunta donde ubicarlas
void Juego::obtenerUnidades(const string &nombreJugador)
{

    // Verifica que la partida, el jugador y el turno sean validos
    if (!validarTurnoJugador(nombreJugador, 0))
    {
        return;
    }

    // Evita que el jugador obtenga unidades dos veces en el mismo turno
    if (unidadesObtenidas)
    {
        cout << "El jugador " << nombreJugador
             << " ya reclamo sus unidades en este turno." << endl;
        return;
    }

    // Obtiene el color del jugador que tiene el turno
    string color = jugadores[turnoActual].getColor();

    // Cuenta cuantos territorios tiene el jugador
    int cantidadTerritorios = contarTerritoriosJugador(color);

    // Calcula las unidades por territorios y por continentes completos
    int porTerritorios = cantidadTerritorios / 3;
    int porContinentes = calcularBonificacionContinentes(color);

    // Suma todas las unidades que puede recibir
    int total = porTerritorios + porContinentes;

    // Muestra como se calcularon las unidades
    cout << "Jugador " << nombreJugador
         << " (color " << color << ")" << endl;

    cout << "Territorios ocupados: "
         << cantidadTerritorios
         << " -> " << porTerritorios
         << " unidades." << endl;

    cout << "Continentes completos -> "
         << porContinentes
         << " unidades." << endl;

    cout << "Total de unidades a ubicar: "
         << total << endl;

    // Si no recibe unidades, se termina esta etapa del turno
    if (total == 0)
    {

        unidadesObtenidas = true;

        cout << "(Comando correcto) "
             << "El jugador " << nombreJugador
             << " ha terminado de reclamar y ubicar sus unidades."
             << endl;

        return;
    }

    // Guarda cuantas unidades faltan por repartir
    int restantes = total;

    // Continua hasta que el jugador ubique todas las unidades
    while (restantes > 0)
    {

        // Si se termina la entrada, se cancela el comando
        if (entradaTerminada())
        {
            cout << endl
                 << "Entrada terminada: el comando se cancela."
                 << endl;
            return;
        }

        cout << endl;
        cout << "Territorios disponibles:" << endl;

        // Muestra solamente los territorios que pertenecen al jugador
        listarTerritoriosDe(color);

        cout << "Unidades por ubicar: "
             << restantes << endl;

        // Pide el territorio donde se quieren poner unidades
        string codigo =
            limpiar(leerLinea("Codigo del territorio: "));

        // Busca el territorio por su codigo
        int indice = buscarTerritorio(codigo);

        // Verifica que el territorio exista
        if (indice == -1)
        {
            cout << "El territorio "
                 << codigo
                 << " no existe." << endl;
            continue;
        }

        // Verifica que el territorio pertenezca al jugador
        if (territorios[indice].getPropietario() != color)
        {
            cout << "El territorio "
                 << territorios[indice].getNombre()
                 << " no pertenece al jugador "
                 << nombreJugador << "." << endl;
            continue;
        }

        int cantidad = 0;

        // Pide cuantas unidades quiere colocar
        if (!aEntero(leerLinea("Cantidad de unidades: "), cantidad))
        {
            cout << "La cantidad debe ser un numero entero positivo."
                 << endl;
            continue;
        }

        // No puede colocar cero unidades ni mas de las disponibles
        if (cantidad < 1 || cantidad > restantes)
        {
            cout << "La cantidad debe estar entre 1 y "
                 << restantes << "." << endl;
            continue;
        }

        // Agrega las unidades al territorio seleccionado
        territorios[indice].agregarUnidades(cantidad);

        // Resta las unidades que ya fueron colocadas
        restantes -= cantidad;

        cout << "Se ubicaron "
             << cantidad
             << " unidades en "
             << territorios[indice].getNombre()
             << "." << endl;
    }

    // Indica que el jugador ya completo esta etapa
    unidadesObtenidas = true;

    // Mensaje final del comando
    cout << "(Comando correcto) "
         << "El jugador " << nombreJugador
         << " ha terminado de reclamar y ubicar sus unidades."
         << endl;
}

// Comando atacar

// Imprime los vecinos enemigos de un territorio, que son los que puede atacar
void Juego::listarObjetivosDesde(int indiceOrigen) const
{
    string color = territorios[indiceOrigen].getPropietario();

    for (unsigned int i = 0; i < territorios.size(); i++)
    {
        if (territorios[i].getPropietario() != color &&
            territorios[indiceOrigen].esVecino(territorios[i].getCodigo()))
        {

            cout << "  " << territorios[i].getCodigo() << " "
                 << territorios[i].getNombre() << " ("
                 << territorios[i].getUnidades() << " unidades, color "
                 << territorios[i].getPropietario() << ")" << endl;
        }
    }
}

// Permite que un jugador ataque territorios vecinos de otros jugadores
void Juego::atacar(const string &nombreJugador)
{

    // Verifica que sea el turno correcto y que ya haya obtenido unidades
    if (!validarTurnoJugador(nombreJugador, 1))
    {
        return;
    }

    // Guarda el color del jugador que esta atacando
    string color = jugadores[turnoActual].getColor();

    // Controla si el jugador quiere seguir atacando
    bool continuar = true;

    while (continuar && !terminado)
    {

        // Si se termina la entrada se cancela el comando
        if (entradaTerminada())
        {
            cout << endl
                 << "Entrada terminada: el comando se cancela."
                 << endl;
            return;
        }

        cout << endl;
        cout << "Territorios desde los que puede atacar "
             << "(requieren 2 o mas unidades):" << endl;

        bool hayOrigen = false;

        // Muestra los territorios propios que tienen suficientes unidades
        for (unsigned int i = 0; i < territorios.size(); i++)
        {

            if (territorios[i].getPropietario() == color &&
                territorios[i].getUnidades() >= 2)
            {

                cout << "  "
                     << territorios[i].getCodigo() << " "
                     << territorios[i].getNombre() << " ("
                     << territorios[i].getUnidades()
                     << " unidades)" << endl;

                hayOrigen = true;
            }
        }

        // Si no tiene territorios con suficientes unidades no puede atacar
        if (!hayOrigen)
        {
            cout << "El jugador no tiene territorios con unidades "
                 << "suficientes para atacar." << endl;
            break;
        }

        // Pide el territorio desde el que va a atacar
        string codigoOrigen =
            limpiar(leerLinea("Territorio desde el que ataca: "));

        int origen = buscarTerritorio(codigoOrigen);

        // Verifica que el territorio exista
        if (origen == -1)
        {
            cout << "El territorio "
                 << codigoOrigen
                 << " no existe." << endl;
            continue;
        }

        // Verifica que el territorio pertenezca al jugador
        if (territorios[origen].getPropietario() != color)
        {
            cout << "El territorio "
                 << territorios[origen].getNombre()
                 << " no pertenece al jugador "
                 << nombreJugador << "." << endl;
            continue;
        }

        // Necesita minimo 2 unidades para poder atacar
        if (territorios[origen].getUnidades() < 2)
        {
            cout << "El territorio "
                 << territorios[origen].getNombre()
                 << " necesita al menos 2 unidades para atacar."
                 << endl;
            continue;
        }

        // Muestra los territorios vecinos enemigos
        cout << "Territorios que puede atacar:" << endl;
        listarObjetivosDesde(origen);

        // Pide el territorio que quiere atacar
        string codigoDestino =
            limpiar(leerLinea("Territorio que quiere atacar: "));

        int destino = buscarTerritorio(codigoDestino);

        // Verifica que exista
        if (destino == -1)
        {
            cout << "El territorio "
                 << codigoDestino
                 << " no existe." << endl;
            continue;
        }

        // Verifica que ambos territorios sean vecinos
        if (!territorios[origen].esVecino(
                territorios[destino].getCodigo()))
        {

            cout << "El territorio "
                 << territorios[destino].getNombre()
                 << " no es vecino de "
                 << territorios[origen].getNombre()
                 << "." << endl;

            continue;
        }

        // No se puede atacar un territorio propio
        if (territorios[destino].getPropietario() == color)
        {
            cout << "El territorio "
                 << territorios[destino].getNombre()
                 << " ya pertenece al jugador "
                 << nombreJugador << "." << endl;
            continue;
        }

        // El atacante lanza 3 dados y el defensor 2
        int rojos[3];
        int blancos[2];

        for (int i = 0; i < 3; i++)
        {
            rojos[i] = lanzarDado();
        }

        for (int i = 0; i < 2; i++)
        {
            blancos[i] = lanzarDado();
        }

        // Ordena los dados del atacante de mayor a menor
        for (int i = 0; i < 3; i++)
        {

            for (int j = i + 1; j < 3; j++)
            {

                if (rojos[j] > rojos[i])
                {
                    int temporal = rojos[i];
                    rojos[i] = rojos[j];
                    rojos[j] = temporal;
                }
            }
        }

        // Ordena los dados del defensor de mayor a menor
        if (blancos[1] > blancos[0])
        {
            int temporal = blancos[0];
            blancos[0] = blancos[1];
            blancos[1] = temporal;
        }

        // Muestra los dados obtenidos
        cout << "Dados del atacante: "
             << rojos[0] << " "
             << rojos[1] << " "
             << rojos[2]
             << " (usa "
             << rojos[0] << " y "
             << rojos[1] << ")" << endl;

        cout << "Dados del defensor: "
             << blancos[0] << " "
             << blancos[1] << endl;

        int perdidasAtacante = 0;
        int perdidasDefensor = 0;

        // Compara los dos dados mas altos
        for (int i = 0; i < 2; i++)
        {

            // El atacante siempre debe conservar una unidad
            if (territorios[origen].getUnidades() - perdidasAtacante <= 1)
            {
                break;
            }

            // Si el defensor ya no tiene unidades termina la comparacion
            if (territorios[destino].getUnidades() - perdidasDefensor <= 0)
            {
                break;
            }

            // Si el dado atacante es mayor pierde el defensor
            if (rojos[i] > blancos[i])
            {
                perdidasDefensor++;
            }

            // Si empatan o gana el defensor pierde el atacante
            else
            {
                perdidasAtacante++;
            }
        }

        // Aplica las perdidas
        territorios[origen].quitarUnidades(perdidasAtacante);
        territorios[destino].quitarUnidades(perdidasDefensor);

        cout << "El atacante pierde "
             << perdidasAtacante
             << " unidades y el defensor pierde "
             << perdidasDefensor
             << " unidades." << endl;

        cout << territorios[origen].getNombre()
             << ": "
             << territorios[origen].getUnidades()
             << " unidades." << endl;

        cout << territorios[destino].getNombre()
             << ": "
             << territorios[destino].getUnidades()
             << " unidades." << endl;

        // Si el defensor se queda sin unidades se conquista
        if (territorios[destino].getUnidades() == 0)
        {

            cout << "El territorio "
                 << territorios[destino].getNombre()
                 << " ha sido conquistado." << endl;

            // Calcula cuantas unidades se pueden mover
            int maximo =
                territorios[origen].getUnidades() - 1;

            int aMover = 0;

            // Pregunta cuantas unidades quiere trasladar
            while (true)
            {

                if (entradaTerminada())
                {
                    aMover = 1;
                    break;
                }

                stringstream mensaje;

                mensaje << "Unidades que traslada al nuevo territorio (1 a "
                        << maximo << "): ";

                if (aEntero(leerLinea(mensaje.str()), aMover) &&
                    aMover >= 1 &&
                    aMover <= maximo)
                {
                    break;
                }

                cout << "La cantidad debe estar entre 1 y "
                     << maximo << "." << endl;
            }

            // Quita las unidades del territorio original
            territorios[origen].quitarUnidades(aMover);

            // Cambia el dueño del territorio conquistado
            territorios[destino].setPropietario(color);

            // Coloca las unidades trasladadas
            territorios[destino].setUnidades(aMover);

            // Revisa si el jugador gano toda la partida
            verificarGanador();

            if (terminado)
            {
                break;
            }
        }

        // Pregunta si quiere atacar otra vez
        continuar =
            preguntarSiNo("Desea realizar otro ataque? (s/n): ");
    }

    // Indica que ya termino su etapa de ataque
    ataqueRealizado = true;

    cout << "(Comando correcto) "
         << "El jugador "
         << nombreJugador
         << " ha terminado de atacar."
         << endl;
}

// Comando fortificar

// Permite mover unidades entre dos territorios propios que sean vecinos
void Juego::fortificar(const string &nombreJugador)
{

    // Verifica que sea el turno y que ya haya realizado el ataque
    if (!validarTurnoJugador(nombreJugador, 2))
    {
        return;
    }

    // Guarda el color del jugador
    string color = jugadores[turnoActual].getColor();

    // Pregunta si desea fortificar
    bool quiereFortificar =
        preguntarSiNo("Desea fortificar algun territorio? (s/n): ");

    // Si no quiere fortificar termina su turno
    if (!quiereFortificar)
    {

        cout << "(Comando correcto) "
             << "El jugador " << nombreJugador
             << " ha terminado su etapa de fortificacion." << endl;

        cambiarTurno();
        return;
    }

    // Muestra los territorios que tiene el jugador
    cout << endl;
    cout << "Territorios del jugador:" << endl;
    listarTerritoriosDe(color);

    // Pide el territorio desde el que quiere mover unidades
    string codigoOrigen =
        limpiar(leerLinea("Territorio desde el que desea mover unidades: "));

    int origen = buscarTerritorio(codigoOrigen);

    // Verifica que exista
    if (origen == -1)
    {
        cout << "El territorio "
             << codigoOrigen
             << " no existe." << endl;
        return;
    }

    // Verifica que pertenezca al jugador
    if (territorios[origen].getPropietario() != color)
    {
        cout << "El territorio "
             << territorios[origen].getNombre()
             << " no pertenece al jugador "
             << nombreJugador << "." << endl;
        return;
    }

    // Debe tener minimo 2 unidades para poder mover alguna
    if (territorios[origen].getUnidades() < 2)
    {
        cout << "El territorio "
             << territorios[origen].getNombre()
             << " no tiene suficientes unidades para fortificar."
             << endl;
        return;
    }

    // Pide el territorio al que quiere mover las unidades
    string codigoDestino =
        limpiar(leerLinea("Territorio al que desea mover unidades: "));

    int destino = buscarTerritorio(codigoDestino);

    // Verifica que exista
    if (destino == -1)
    {
        cout << "El territorio "
             << codigoDestino
             << " no existe." << endl;
        return;
    }

    // El territorio destino tambien debe pertenecer al jugador
    if (territorios[destino].getPropietario() != color)
    {
        cout << "El territorio "
             << territorios[destino].getNombre()
             << " no pertenece al jugador "
             << nombreJugador << "." << endl;
        return;
    }

    // Los dos territorios deben ser vecinos
    if (!territorios[origen].esVecino(
            territorios[destino].getCodigo()))
    {

        cout << "Los territorios "
             << territorios[origen].getNombre()
             << " y "
             << territorios[destino].getNombre()
             << " no son vecinos." << endl;

        return;
    }

    // Siempre debe quedar por lo menos una unidad en el territorio origen
    int maximo =
        territorios[origen].getUnidades() - 1;

    int cantidad = 0;

    // Pide cuantas unidades desea mover
    if (!aEntero(
            leerLinea("Cantidad de unidades que desea mover: "),
            cantidad))
    {

        cout << "La cantidad debe ser un numero entero positivo."
             << endl;
        return;
    }

    // Verifica que la cantidad sea valida
    if (cantidad < 1 || cantidad > maximo)
    {

        cout << "La cantidad debe estar entre 1 y "
             << maximo << "." << endl;

        return;
    }

    // Quita las unidades del territorio de origen
    territorios[origen].quitarUnidades(cantidad);

    // Agrega las unidades al territorio destino
    territorios[destino].agregarUnidades(cantidad);

    cout << "Se movieron "
         << cantidad
         << " unidades de "
         << territorios[origen].getNombre()
         << " a "
         << territorios[destino].getNombre()
         << "." << endl;

    // Indica que la fortificacion termino correctamente
    cout << "(Comando correcto) "
         << "El jugador "
         << nombreJugador
         << " ha terminado su etapa de fortificacion."
         << endl;

    // Pasa al siguiente jugador
    cambiarTurno();
}
// Comando estado_juego

// Muestra la informacion actual de la partida
void Juego::estadoJuego() const
{

    // Verifica que exista una partida inicializada
    if (!inicializado)
    {
        cout << "(Juego no inicializado) "
             << "Esta partida no ha sido inicializada correctamente."
             << endl;
        return;
    }

    // Verifica si la partida ya termino
    if (terminado)
    {
        cout << "(Juego terminado) "
             << "Esta partida ya tuvo un ganador."
             << endl;
        return;
    }

    // Muestra la cantidad de jugadores
    cout << "Numero de jugadores: "
         << jugadores.size()
         << endl;

    cout << endl;
    cout << "Jugadores:" << endl;

    // Muestra el nombre, color y territorios de cada jugador
    for (unsigned int i = 0; i < jugadores.size(); i++)
    {

        cout << "  "
             << jugadores[i].getNombre()
             << " - Color: "
             << jugadores[i].getColor()
             << " - Territorios: "
             << contarTerritoriosJugador(
                    jugadores[i].getColor())
             << endl;
    }

    cout << endl;

    // Muestra quien tiene actualmente el turno
    cout << "Turno actual: "
         << jugadores[turnoActual].getNombre()
         << endl;

    cout << endl;
    cout << "Territorios:" << endl;

    // Recorre y muestra todos los territorios del tablero
    for (unsigned int i = 0; i < territorios.size(); i++)
    {

        cout << "  "
             << territorios[i].getCodigo()
             << " "
             << territorios[i].getNombre()
             << " - Color: "
             << territorios[i].getPropietario()
             << " - Unidades: "
             << territorios[i].getUnidades()
             << endl;
    }
}

// Construccion del tablero

// Crea los 42 territorios con su codigo, nombre y continente (tabla 1)
void Juego::cargarTerritorios()
{
    territorios.clear();

    // America del Norte
    territorios.push_back(Territorio("1.1", "Alaska", "America del Norte"));
    territorios.push_back(Territorio("1.2", "Alberta", "America del Norte"));
    territorios.push_back(Territorio("1.3", "America Central", "America del Norte"));
    territorios.push_back(Territorio("1.4", "Estados Unidos Orientales", "America del Norte"));
    territorios.push_back(Territorio("1.5", "Groenlandia", "America del Norte"));
    territorios.push_back(Territorio("1.6", "Territorio Noroccidental", "America del Norte"));
    territorios.push_back(Territorio("1.7", "Ontario", "America del Norte"));
    territorios.push_back(Territorio("1.8", "Quebec", "America del Norte"));
    territorios.push_back(Territorio("1.9", "Estados Unidos Occidentales", "America del Norte"));

    // America del Sur
    territorios.push_back(Territorio("2.1", "Argentina", "America del Sur"));
    territorios.push_back(Territorio("2.2", "Brasil", "America del Sur"));
    territorios.push_back(Territorio("2.3", "Peru", "America del Sur"));
    territorios.push_back(Territorio("2.4", "Venezuela", "America del Sur"));

    // Europa
    territorios.push_back(Territorio("3.1", "Gran Bretana", "Europa"));
    territorios.push_back(Territorio("3.2", "Islandia", "Europa"));
    territorios.push_back(Territorio("3.3", "Europa del Norte", "Europa"));
    territorios.push_back(Territorio("3.4", "Escandinavia", "Europa"));
    territorios.push_back(Territorio("3.5", "Europa del Sur", "Europa"));
    territorios.push_back(Territorio("3.6", "Ucrania", "Europa"));
    territorios.push_back(Territorio("3.7", "Europa Occidental", "Europa"));

    // Africa
    territorios.push_back(Territorio("4.1", "Congo", "Africa"));
    territorios.push_back(Territorio("4.2", "Africa Oriental", "Africa"));
    territorios.push_back(Territorio("4.3", "Egipto", "Africa"));
    territorios.push_back(Territorio("4.4", "Madagascar", "Africa"));
    territorios.push_back(Territorio("4.5", "Africa del Norte", "Africa"));
    territorios.push_back(Territorio("4.6", "Africa del Sur", "Africa"));

    // Asia
    territorios.push_back(Territorio("5.1", "Afghanistan", "Asia"));
    territorios.push_back(Territorio("5.2", "China", "Asia"));
    territorios.push_back(Territorio("5.3", "India", "Asia"));
    territorios.push_back(Territorio("5.4", "Irkutsk", "Asia"));
    territorios.push_back(Territorio("5.5", "Japon", "Asia"));
    territorios.push_back(Territorio("5.6", "Kamchatka", "Asia"));
    territorios.push_back(Territorio("5.7", "Medio Oriente", "Asia"));
    territorios.push_back(Territorio("5.8", "Mongolia", "Asia"));
    territorios.push_back(Territorio("5.9", "Siam", "Asia"));
    territorios.push_back(Territorio("5.10", "Siberia", "Asia"));
    territorios.push_back(Territorio("5.11", "Ural", "Asia"));
    territorios.push_back(Territorio("5.12", "Yakutsk", "Asia"));

    // Australia
    territorios.push_back(Territorio("6.1", "Australia Oriental", "Australia"));
    territorios.push_back(Territorio("6.2", "Indonesia", "Australia"));
    territorios.push_back(Territorio("6.3", "Nueva Guinea", "Australia"));
    territorios.push_back(Territorio("6.4", "Australia Occidental", "Australia"));
}

// Marca dos territorios como vecinos en los dos sentidos
void Juego::conectar(const string &codigoA, const string &codigoB)
{
    int a = buscarTerritorio(codigoA);
    int b = buscarTerritorio(codigoB);

    if (a == -1 || b == -1)
    {
        return;
    }

    if (!territorios[a].esVecino(codigoB))
    {
        territorios[a].agregarVecino(codigoB);
    }

    if (!territorios[b].esVecino(codigoA))
    {
        territorios[b].agregarVecino(codigoA);
    }
}

// Registra las 83 conexiones del tablero de Risk
void Juego::cargarVecinos()
{
    // America del Norte
    conectar("1.1", "1.6"); // Alaska - Territorio Noroccidental
    conectar("1.1", "1.2"); // Alaska - Alberta
    conectar("1.1", "5.6"); // Alaska - Kamchatka
    conectar("1.2", "1.6"); // Alberta - Territorio Noroccidental
    conectar("1.2", "1.7"); // Alberta - Ontario
    conectar("1.2", "1.9"); // Alberta - Estados Unidos Occidentales
    conectar("1.6", "1.7"); // Territorio Noroccidental - Ontario
    conectar("1.6", "1.5"); // Territorio Noroccidental - Groenlandia
    conectar("1.5", "1.7"); // Groenlandia - Ontario
    conectar("1.5", "1.8"); // Groenlandia - Quebec
    conectar("1.5", "3.2"); // Groenlandia - Islandia
    conectar("1.7", "1.8"); // Ontario - Quebec
    conectar("1.7", "1.4"); // Ontario - Estados Unidos Orientales
    conectar("1.7", "1.9"); // Ontario - Estados Unidos Occidentales
    conectar("1.8", "1.4"); // Quebec - Estados Unidos Orientales
    conectar("1.9", "1.4"); // Estados Unidos Occidentales - Orientales
    conectar("1.9", "1.3"); // Estados Unidos Occidentales - America Central
    conectar("1.4", "1.3"); // Estados Unidos Orientales - America Central
    conectar("1.3", "2.4"); // America Central - Venezuela

    // America del Sur
    conectar("2.4", "2.3"); // Venezuela - Peru
    conectar("2.4", "2.2"); // Venezuela - Brasil
    conectar("2.3", "2.2"); // Peru - Brasil
    conectar("2.3", "2.1"); // Peru - Argentina
    conectar("2.2", "2.1"); // Brasil - Argentina
    conectar("2.2", "4.6"); // Brasil - Africa del Sur

    // Europa
    conectar("3.2", "3.1");  // Islandia - Gran Bretana
    conectar("3.2", "3.4");  // Islandia - Escandinavia
    conectar("3.1", "3.4");  // Gran Bretana - Escandinavia
    conectar("3.1", "3.3");  // Gran Bretana - Europa del Norte
    conectar("3.1", "3.7");  // Gran Bretana - Europa Occidental
    conectar("3.4", "3.3");  // Escandinavia - Europa del Norte
    conectar("3.4", "3.6");  // Escandinavia - Ucrania
    conectar("3.3", "3.6");  // Europa del Norte - Ucrania
    conectar("3.3", "3.5");  // Europa del Norte - Europa del Sur
    conectar("3.3", "3.7");  // Europa del Norte - Europa Occidental
    conectar("3.7", "3.5");  // Europa Occidental - Europa del Sur
    conectar("3.7", "4.5");  // Europa Occidental - Africa del Norte
    conectar("3.5", "3.6");  // Europa del Sur - Ucrania
    conectar("3.5", "5.7");  // Europa del Sur - Medio Oriente
    conectar("3.5", "4.3");  // Europa del Sur - Egipto
    conectar("3.5", "4.5");  // Europa del Sur - Africa del Norte
    conectar("3.6", "5.7");  // Ucrania - Medio Oriente
    conectar("3.6", "5.1");  // Ucrania - Afghanistan
    conectar("3.6", "5.11"); // Ucrania - Ural

    // Africa
    conectar("4.5", "4.3"); // Africa del Norte - Egipto
    conectar("4.5", "4.2"); // Africa del Norte - Africa Oriental
    conectar("4.5", "4.1"); // Africa del Norte - Congo
    conectar("4.3", "4.2"); // Egipto - Africa Oriental
    conectar("4.3", "5.7"); // Egipto - Medio Oriente
    conectar("4.2", "5.7"); // Africa Oriental - Medio Oriente
    conectar("4.2", "4.1"); // Africa Oriental - Congo
    conectar("4.2", "4.6"); // Africa Oriental - Africa del Sur
    conectar("4.2", "4.4"); // Africa Oriental - Madagascar
    conectar("4.1", "4.6"); // Congo - Africa del Sur
    conectar("4.6", "4.4"); // Africa del Sur - Madagascar

    // Asia
    conectar("5.7", "5.1");   // Medio Oriente - Afghanistan
    conectar("5.7", "5.3");   // Medio Oriente - India
    conectar("5.1", "5.11");  // Afghanistan - Ural
    conectar("5.1", "5.2");   // Afghanistan - China
    conectar("5.1", "5.3");   // Afghanistan - India
    conectar("5.3", "5.2");   // India - China
    conectar("5.3", "5.9");   // India - Siam
    conectar("5.9", "5.2");   // Siam - China
    conectar("5.9", "6.2");   // Siam - Indonesia
    conectar("5.11", "5.2");  // Ural - China
    conectar("5.11", "5.10"); // Ural - Siberia
    conectar("5.10", "5.2");  // Siberia - China
    conectar("5.10", "5.8");  // Siberia - Mongolia
    conectar("5.10", "5.4");  // Siberia - Irkutsk
    conectar("5.10", "5.12"); // Siberia - Yakutsk
    conectar("5.2", "5.8");   // China - Mongolia
    conectar("5.4", "5.12");  // Irkutsk - Yakutsk
    conectar("5.4", "5.6");   // Irkutsk - Kamchatka
    conectar("5.4", "5.8");   // Irkutsk - Mongolia
    conectar("5.12", "5.6");  // Yakutsk - Kamchatka
    conectar("5.6", "5.8");   // Kamchatka - Mongolia
    conectar("5.6", "5.5");   // Kamchatka - Japon
    conectar("5.8", "5.5");   // Mongolia - Japon

    // Australia
    conectar("6.2", "6.3"); // Indonesia - Nueva Guinea
    conectar("6.2", "6.4"); // Indonesia - Australia Occidental
    conectar("6.3", "6.4"); // Nueva Guinea - Australia Occidental
    conectar("6.3", "6.1"); // Nueva Guinea - Australia Oriental
    conectar("6.4", "6.1"); // Australia Occidental - Australia Oriental
}