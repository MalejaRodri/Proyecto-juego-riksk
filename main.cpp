#include <iostream>
#include <string>

#include "interfaz.h"
#include "juego.h"

using namespace std;

// Ciclo principal: muestra el indicador $, lee una linea y la manda a procesar
// Termina cuando el usuario escribe salir o cuando se acaba la entrada
int main(){
    string entrada;
    bool ejecutando = true;

    // Una sola partida para toda la ejecucion, compartida por los comandos
    Juego juego;

    while (ejecutando){
        cout << "$ ";

        // Si la lectura falla (Ctrl+D o fin de archivo) se termina el programa
        if (!getline(cin, entrada)){
            cout << endl;
            break;
        }

        ejecutando = procesarComando(entrada, juego);
    }

    return 0;
}