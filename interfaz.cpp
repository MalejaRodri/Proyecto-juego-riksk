#include "interfaz.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <string>

using namespace std;


// verifica que el nombre sea una palabra de maximo 8 caracteres sin espacios
bool nombreJugadorValido(const string& nombre){

    if (nombre.empty() || nombre.size() > 8){
        return false;
    }

    for (unsigned int i = 0; i < nombre.size(); i++){

        if (nombre[i] == ' ' || nombre[i] == '\t'){
            return false;
        }
    }

    return true;
}


// imprime la forma correcta de llamar a un comando
void mostrarUso(const string& comando){

    if (comando == "inicializar"){
        cout << "Uso: inicializar <archivo>" << endl;
    }

    else if (comando == "obtener_unidades"){
        cout << "Uso: obtener_unidades <nombre_jugador>" << endl;
    }

    else if (comando == "atacar"){
        cout << "Uso: atacar <nombre_jugador>" << endl;
    }

    else if (comando == "fortificar"){
        cout << "Uso: fortificar <nombre_jugador>" << endl;
    }

    else if (comando == "estado_juego"){
        cout << "Uso: estado_juego" << endl;
    }

    else if (comando == "guardar"){
        cout << "Uso: guardar <nombre_archivo>" << endl;
    }

    else if (comando == "guardar_comprimido"){
        cout << "Uso: guardar_comprimido <nombre_archivo>" << endl;
    }

    else if (comando == "costo_conquista"){
        cout << "Uso: costo_conquista <nombre_jugador> <territorio>" << endl;
    }

    else if (comando == "conquista_mas_barata"){
        cout << "Uso: conquista_mas_barata <nombre_jugador>" << endl;
    }

    else if (comando == "ayuda"){
        cout << "Uso: ayuda" << endl;
        cout << "Uso: ayuda <comando>" << endl;
    }

    else if (comando == "salir"){
        cout << "Uso: salir" << endl;
    }
}


// muestra el error cuando la cantidad de parametros no es correcta
void errorParametros(const string& comando){

    cout << "Error: cantidad incorrecta de parametros." << endl;

    mostrarUso(comando);
}


// muestra el error cuando el nombre del jugador no tiene un formato valido
void errorNombreJugador(const string& comando){

    cout << "Error: el nombre del jugador debe tener maximo 8 caracteres "
         << "y no contener espacios." << endl;

    mostrarUso(comando);
}


// muestra todos los comandos disponibles
void mostrarAyuda(){

    cout << "            COMANDOS DISPONIBLES             " << endl;

    cout << "- inicializar <archivo>" << endl;
    cout << "- obtener_unidades <nombre_jugador>" << endl;
    cout << "- atacar <nombre_jugador>" << endl;
    cout << "- fortificar <nombre_jugador>" << endl;
    cout << "- estado_juego" << endl;

    cout << "- guardar <nombre_archivo>" << endl;
    cout << "- guardar_comprimido <nombre_archivo>" << endl;

    cout << "- costo_conquista <nombre_jugador> <territorio>" << endl;
    cout << "- conquista_mas_barata <nombre_jugador>" << endl;

    cout << "- ayuda" << endl;
    cout << "- ayuda <comando>" << endl;

    cout << "- salir" << endl;
}


// muestra el uso y la descripcion de un comando especifico
void mostrarAyudaComando(const string& comando){

    if (comando == "inicializar"){

        mostrarUso(comando);

        cout << "Descripcion: inicializa el juego usando la informacion "
             << "guardada en un archivo." << endl;
    }

    else if (comando == "obtener_unidades"){

        mostrarUso(comando);

        cout << "Descripcion: permite al jugador obtener y distribuir nuevas "
             << "unidades en sus territorios." << endl;
    }

    else if (comando == "atacar"){

        mostrarUso(comando);

        cout << "Descripcion: permite realizar la fase de ataque del jugador "
             << "indicado." << endl;
    }

    else if (comando == "fortificar"){

        mostrarUso(comando);

        cout << "Descripcion: permite mover unidades entre territorios propios "
             << "para fortalecerlos." << endl;
    }

    else if (comando == "estado_juego"){

        mostrarUso(comando);

        cout << "Descripcion: muestra el estado actual del juego y la "
             << "informacion de los jugadores." << endl;
    }

    else if (comando == "guardar"){

        mostrarUso(comando);

        cout << "Descripcion: guarda el estado actual del juego en un archivo "
             << "de texto." << endl;
    }

    else if (comando == "guardar_comprimido"){

        mostrarUso(comando);

        cout << "Descripcion: guarda el estado actual del juego en un archivo "
             << "binario comprimido." << endl;
    }

    else if (comando == "costo_conquista"){

        mostrarUso(comando);

        cout << "Descripcion: calcula el costo y la ruta de conquista hacia un "
             << "territorio." << endl;
    }

    else if (comando == "conquista_mas_barata"){

        mostrarUso(comando);

        cout << "Descripcion: determina cual territorio puede conquistar el "
             << "jugador con el menor costo." << endl;
    }

    else if (comando == "ayuda"){

        mostrarUso(comando);

        cout << "Descripcion: muestra los comandos disponibles o informacion "
             << "sobre un comando especifico." << endl;
    }

    else if (comando == "salir"){

        mostrarUso(comando);

        cout << "Descripcion: termina la ejecucion del programa." << endl;
    }

    else{
        cout << "Error: comando no reconocido." << endl;
    }
}


// separa lo escrito por el usuario en comando y parametros
// luego valida y llama a la funcion correspondiente
bool procesarComando(const string& entrada, Juego& juego){

    stringstream ss(entrada);

    string comando;
    ss >> comando;

    vector<string> parametros;
    string parametro;

    // guarda todos los parametros escritos despues del comando
    while (ss >> parametro){
        parametros.push_back(parametro);
    }

    // si la linea esta vacia no hace nada
    if (comando.empty()){
        return true;
    }


    // ---------------- SALIR ----------------

    if (comando == "salir"){

        if (parametros.size() != 0){
            errorParametros("salir");
            return true;
        }

        return false;
    }


    // ---------------- INICIALIZAR ----------------

    else if (comando == "inicializar"){

        if (parametros.size() != 1){
            errorParametros("inicializar");
            return true;
        }

        juego.inicializar(parametros[0]);
    }


    // ---------------- OBTENER UNIDADES ----------------

    else if (comando == "obtener_unidades"){

        if (parametros.size() != 1){
            errorParametros("obtener_unidades");
            return true;
        }

        if (!nombreJugadorValido(parametros[0])){
            errorNombreJugador("obtener_unidades");
            return true;
        }

        juego.obtenerUnidades(parametros[0]);
    }


    // ---------------- ATACAR ----------------

    else if (comando == "atacar"){

        if (parametros.size() != 1){
            errorParametros("atacar");
            return true;
        }

        if (!nombreJugadorValido(parametros[0])){
            errorNombreJugador("atacar");
            return true;
        }

        juego.atacar(parametros[0]);
    }


    // ---------------- FORTIFICAR ----------------

    else if (comando == "fortificar"){

        if (parametros.size() != 1){
            errorParametros("fortificar");
            return true;
        }

        if (!nombreJugadorValido(parametros[0])){
            errorNombreJugador("fortificar");
            return true;
        }

        juego.fortificar(parametros[0]);
    }


    // ---------------- ESTADO DEL JUEGO ----------------

    else if (comando == "estado_juego"){

        if (parametros.size() != 0){
            errorParametros("estado_juego");
            return true;
        }

        juego.estadoJuego();
    }


    // ---------------- GUARDAR ----------------
    // componente 2, todavia no se implementa

    else if (comando == "guardar"){

        if (parametros.size() != 1){
            errorParametros("guardar");
            return true;
        }

        cout << "Comando valido. Funcionalidad pendiente (componente 2)."
             << endl;
    }


    // ---------------- GUARDAR COMPRIMIDO ----------------
    // componente 2, todavia no se implementa

    else if (comando == "guardar_comprimido"){

        if (parametros.size() != 1){
            errorParametros("guardar_comprimido");
            return true;
        }

        cout << "Comando valido. Funcionalidad pendiente (componente 2)."
             << endl;
    }


    // ---------------- COSTO CONQUISTA ----------------
    // componente 3, todavia no se implementa

    else if (comando == "costo_conquista"){

        if (parametros.size() != 2){
            errorParametros("costo_conquista");
            return true;
        }

        // valida el nombre del jugador
        if (!nombreJugadorValido(parametros[0])){
            errorNombreJugador("costo_conquista");
            return true;
        }

        // valida el codigo contra los territorios reales del tablero
        if (!juego.codigoTerritorioValido(parametros[1])){

            cout << "Error: el territorio "
                 << parametros[1]
                 << " no corresponde a ningun territorio del tablero."
                 << endl;

            mostrarUso("costo_conquista");

            return true;
        }

        cout << "Comando valido. Funcionalidad pendiente (componente 3)."
             << endl;
    }


    // ---------------- CONQUISTA MAS BARATA ----------------
    // componente 3, todavia no se implementa

    else if (comando == "conquista_mas_barata"){

        if (parametros.size() != 1){
            errorParametros("conquista_mas_barata");
            return true;
        }

        if (!nombreJugadorValido(parametros[0])){
            errorNombreJugador("conquista_mas_barata");
            return true;
        }

        cout << "Comando valido. Funcionalidad pendiente (componente 3)."
             << endl;
    }


    // ---------------- AYUDA ----------------

    else if (comando == "ayuda"){

        // ayuda general
        if (parametros.size() == 0){
            mostrarAyuda();
        }

        // ayuda de un comando especifico
        else if (parametros.size() == 1){
            mostrarAyudaComando(parametros[0]);
        }

        // ayuda no puede recibir mas de un parametro
        else{
            errorParametros("ayuda");
        }
    }


    // ---------------- COMANDO DESCONOCIDO ----------------

    else{

        cout << "Error: comando no reconocido." << endl;

        cout << "Escriba 'ayuda' para ver la lista de comandos disponibles."
             << endl;
    }

    return true;
}