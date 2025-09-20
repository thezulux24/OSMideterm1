#include "Proceso.h"
#include <sstream>

/*
  Constructor del proceso
  
  Inicializa el proceso con la informacion que viene del archivo de entrada.
  El tiempo restante empieza igual al tiempo de rafaga total.
  Las metricas se inicializan en 0 porque se calculan durante la simulacion.
  haIniciado se pone en false porque aun no ha ejecutado.
 */
Proceso::Proceso(std::string etiq, int bt, int at, int q, int pr) 
    : etiqueta(etiq), tiempoRafaga(bt), tiempoLlegada(at), cola(q), colaOriginal(q), prioridad(pr),
      tiempoEspera(0), tiempoFinalizacion(0), tiempoRespuesta(0), tiempoRetorno(0),
      tiempoRestante(bt), tiempoInicio(-1), haIniciado(false) {
}

/*
  Ejecuta el proceso por una unidad de tiempo
  
  Si es la primera vez que ejecuta, guarda el tiempo de inicio.
  Luego reduce el tiempo restante en 1. Esto simula que el proceso
  uso la CPU por una unidad de tiempo.
 */
void Proceso::ejecutar(int tiempoActual) {
    // Si es la primera vez que ejecuta, guardar cuando empezo
    if (!haIniciado) {
        tiempoInicio = tiempoActual;
        haIniciado = true;
    }
    
    // Reducir el tiempo que le falta por ejecutar
    if (tiempoRestante > 0) {
        tiempoRestante--;
    }
}

/*
  Calcula las metricas de rendimiento del proceso
  
  Se llama cuando el proceso ya termino. Usa las formulas estandar:
  - Tiempo de retorno = cuando termino - cuando llego
  - Tiempo de respuesta = cuando empezo a ejecutar - cuando llego
  - Tiempo de espera = tiempo de retorno - tiempo que realmente ejecuto
 */
void Proceso::calcularMetricas() {
    // Cuanto tiempo total estuvo en el sistema
    tiempoRetorno = tiempoFinalizacion - tiempoLlegada;
    
    // Cuanto espero antes de ejecutar por primera vez
    tiempoRespuesta = tiempoInicio - tiempoLlegada;
    
    // Cuanto tiempo paso esperando en colas (no ejecutando)
    tiempoEspera = tiempoRetorno - tiempoRafaga;
}

/*
  Muestra la informacion del proceso en pantalla
  
  Imprime todos los datos importantes del proceso para que el usuario
  pueda ver como quedo despues de la simulacion. Util para debugging.
 */
void Proceso::mostrarInfo() const {
    std::cout << "Proceso " << etiqueta << ":" << std::endl;
    std::cout << "  Tiempo de Rafaga: " << tiempoRafaga << std::endl;
    std::cout << "  Tiempo de Llegada: " << tiempoLlegada << std::endl;
    std::cout << "  Cola Final: " << (cola + 1) << std::endl;
    std::cout << "  Prioridad: " << prioridad << std::endl;
    std::cout << "  Tiempo de Espera: " << tiempoEspera << std::endl;
    std::cout << "  Tiempo de Finalizacion: " << tiempoFinalizacion << std::endl;
    std::cout << "  Tiempo de Respuesta: " << tiempoRespuesta << std::endl;
    std::cout << "  Tiempo de Retorno: " << tiempoRetorno << std::endl;
}

/*
  Genera la linea de salida para el archivo de resultados
  
  Crea el string con el formato exacto que necesita el archivo de salida:
  etiqueta;BT;AT;Q;Pr;WT;CT;RT;TAT
  
  Se suma 1 a la cola porque internamente uso 0-indexed pero en el archivo
  de salida debe ser 1-indexed.
 */
std::string Proceso::toString() const {
    std::ostringstream oss;
    oss << etiqueta << ";"
        << tiempoRafaga << ";"
        << tiempoLlegada << ";"
        << (cola + 1) << ";"  // Convertir a 1-indexed para el archivo
        << prioridad << ";"
        << tiempoEspera << ";"
        << tiempoFinalizacion << ";"
        << tiempoRespuesta << ";"
        << tiempoRetorno;
    return oss.str();
}
