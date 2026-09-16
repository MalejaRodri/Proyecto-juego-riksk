# Proyecto RISK

## Descripción

Este proyecto consiste en el desarrollo de una versión en consola del juego de estrategia **RISK**, implementada en **C++20**.

El programa permite representar jugadores, territorios, unidades y relaciones de vecindad entre territorios, además de controlar las diferentes etapas de un turno de juego.

Actualmente, el proyecto contiene el desarrollo correspondiente a la **Entrega 0 y la Entrega 1**, incluyendo las correcciones realizadas a partir de la retroalimentación recibida.



## Tecnologías utilizadas

- C++20
- GNU g++
- Visual Studio Code
- Git / GitHub

---

## Estructura del proyecto

El proyecto está organizado principalmente en los siguientes archivos:

### `main.cpp`

Es el punto de entrada del programa.

Se encarga de:

- Crear la instancia principal de `Juego`.
- Mostrar el indicador `$` para recibir comandos.
- Leer las instrucciones ingresadas por el usuario.
- Enviar los comandos a la interfaz para su procesamiento.
- Mantener la ejecución hasta utilizar el comando `salir`.

### `interfaz.h` / `interfaz.cpp`

Se encargan de la interacción entre el usuario y el juego.

Sus principales responsabilidades son:

- Interpretar los comandos ingresados.
- Separar el comando de sus parámetros.
- Validar la cantidad de parámetros.
- Mostrar mensajes de ayuda y errores de uso.
- Invocar las operaciones correspondientes de `Juego`.

### `jugador.h` / `jugador.cpp`

Implementan el TAD `Jugador`.

Cada jugador contiene principalmente:

- Nombre.
- Color.

El color permite relacionar al jugador con los territorios que controla.

### `territorio.h` / `territorio.cpp`

Implementan el TAD `Territorio`.

Cada territorio contiene:

- Código.
- Nombre.
- Continente.
- Propietario.
- Número de unidades.
- Lista de territorios vecinos.

Las relaciones de vecindad permiten determinar qué movimientos de ataque y fortificación son válidos.

### `juego.h` / `juego.cpp`

Implementan el TAD `Juego` y contienen la lógica principal de la partida.

`Juego` administra:

- Jugadores.
- Los 42 territorios.
- Turno actual.
- Estado de inicialización.
- Estado de finalización.
- Obtención de unidades.
- Ataques.
- Fortificaciones.
- Verificación del ganador.

---

## Estructuras de datos utilizadas

Para esta primera etapa del proyecto se utilizan principalmente **estructuras lineales mediante `vector`**.

### Jugadores

```cpp
vector<Jugador> jugadores;
```

Almacena los jugadores participantes en la partida.

### Territorios

```cpp
vector<Territorio> territorios;
```

Almacena los 42 territorios del tablero.

### Vecinos

Cada territorio mantiene una estructura con los códigos de sus territorios vecinos:

```cpp
vector<string> vecinos;
```

Esta estructura permite validar las relaciones necesarias para realizar ataques y fortificaciones.

---

## Funcionalidades implementadas

### Inicializar partida

```text
inicializar <archivo>
```

Carga una partida desde un archivo de texto.

Durante la inicialización se valida la información de jugadores y territorios antes de establecer el estado inicial del juego.

---

### Obtener unidades

```text
obtener_unidades <nombre_jugador>
```

Permite al jugador correspondiente al turno obtener y distribuir sus unidades.

La cantidad se calcula teniendo en cuenta los territorios controlados y las bonificaciones correspondientes por continentes.

Al finalizar correctamente esta etapa se habilita la fase de ataque.

---

### Atacar

```text
atacar <nombre_jugador>
```

Permite realizar ataques desde un territorio propio hacia un territorio enemigo vecino.

Antes del combate se valida:

- Que la partida esté activa.
- Que el jugador exista.
- Que corresponda a su turno.
- Que haya completado la obtención de unidades.
- Que el territorio atacante exista y sea de su propiedad.
- Que tenga suficientes unidades para atacar.
- Que el territorio objetivo exista.
- Que el objetivo pertenezca a otro jugador.
- Que ambos territorios sean vecinos.

El combate se realiza mediante lanzamientos de dados. Se comparan los mejores resultados del atacante y del defensor, teniendo en cuenta que los empates favorecen al defensor.

Si un territorio enemigo pierde todas sus unidades, es conquistado y cambia de propietario. Después se realiza el traslado correspondiente de unidades y se verifica si existe un ganador.

---

### Fortificar

```text
fortificar <nombre_jugador>
```

Corresponde a la etapa final del turno.

Permite trasladar unidades entre dos territorios propios y vecinos.

Se valida que:

- Ambos territorios existan.
- Ambos pertenezcan al jugador.
- Sean vecinos.
- La cantidad de unidades trasladadas sea válida.
- El territorio de origen conserve al menos una unidad.

Al finalizar la fortificación se cambia el turno al siguiente jugador.

---

### Estado del juego

```text
estado_juego
```

Muestra el estado actual de la partida sin modificarlo.

Presenta información como:

- Número de jugadores.
- Nombre y color de los jugadores.
- Cantidad de territorios controlados.
- Jugador que tiene el turno actual.
- Territorios.
- Propietario de cada territorio.
- Número de unidades de cada territorio.

---

### Ayuda

```text
ayuda
```

Muestra los comandos disponibles.

También puede consultarse la forma de uso de un comando específico:

```text
ayuda <comando>
```

---

### Salir

```text
salir
```

Finaliza la ejecución del programa.

---

## Flujo general de un turno

Una vez inicializada la partida, cada turno sigue el siguiente orden:

```text
Obtener unidades
       ↓
     Atacar
       ↓
   Fortificar
       ↓
 Cambiar turno
```

El programa utiliza variables de control para evitar que un jugador ejecute las etapas en un orden incorrecto.

---

## Sistema de combate

Durante un ataque se generan resultados de dados para el atacante y el defensor.

Las comparaciones se realizan utilizando los resultados de mayor valor.

Reglas principales:

- Si el dado del atacante es mayor, el defensor pierde una unidad.
- Si el dado del defensor es mayor, el atacante pierde una unidad.
- Si existe empate, el atacante pierde una unidad.

Por lo tanto, **los empates favorecen al defensor**.

---

## Condición de victoria

Después de una conquista se verifica el número de territorios controlados por el jugador.

La partida termina cuando un único jugador controla los **42 territorios** del tablero.

---

## Compilación

El proyecto utiliza **C++20**.

Para compilar con GNU g++:

```bash
g++ -std=c++20 main.cpp interfaz.cpp juego.cpp jugador.cpp territorio.cpp -o risk.exe
```

En PowerShell puede ejecutarse con:

```powershell
.\risk.exe
```

---

## Estado actual del proyecto

### Entrega 0

Se desarrolló la estructura inicial del proyecto y el diseño base de los TAD utilizados para representar los elementos principales del juego.

### Entrega 1

Se implementó el primer componente funcional del proyecto utilizando estructuras lineales.

Actualmente se encuentran implementadas las funcionalidades principales de:

- Inicialización de partidas.
- Manejo de jugadores.
- Manejo de los 42 territorios.
- Relaciones de vecindad.
- Obtención y distribución de unidades.
- Ataques y sistema de dados.
- Conquista de territorios.
- Fortificación.
- Cambio de turnos.
- Consulta del estado del juego.
- Verificación de la condición de victoria.
- Validación de comandos y parámetros.

También se incorporaron las correcciones correspondientes a la retroalimentación de la Entrega 0.

---

## Próximas etapas

El proyecto continuará incorporando las funcionalidades correspondientes a los siguientes componentes de RISK, incluyendo estructuras de datos adicionales requeridas durante las próximas entregas.
