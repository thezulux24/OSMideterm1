#include "STCFScheduler.h"
#include <algorithm>

/*
 * Constructor
 * 
 * Inicializa el vector vacio donde se van a mantener los procesos
 * ordenados por tiempo restante.
 */
STCFScheduler::STCFScheduler() {
}

/*
 * Agrega un proceso manteniendo orden
 * 
 * Similar a SJF, pero en STCF el mismo proceso puede volver a ser agregado
 * multiples veces si es interrumpido. Cada vez se reordena segun su
 * tiempo restante actualizado.
 */
void STCFScheduler::agregarProceso(Proceso* proceso) {
    procesos.push_back(proceso);
    
    // Mantener orden por tiempo restante
    std::sort(procesos.begin(), procesos.end(), 
              [](Proceso* a, Proceso* b) {
                  if (a->getTiempoRestante() == b->getTiempoRestante()) {
                      return a->getTiempoLlegada() < b->getTiempoLlegada();
                  }
                  return a->getTiempoRestante() < b->getTiempoRestante();
              });
}


void STCFScheduler::ejecutarProceso(Proceso* proceso, int tiempoActual, int& tiempoEjecutado, int tiempoMaximo) {
    // STCF puede ser interrumpido, ejecutar hasta tiempoMaximo o hasta terminar
    tiempoEjecutado = std::min(tiempoMaximo, proceso->getTiempoRestante());
    
    // Simular la ejecución unidad por unidad
    for (int i = 0; i < tiempoEjecutado; i++) {
        proceso->ejecutar(tiempoActual + i);
    }
}


/*
 * Obtiene el proceso con menor tiempo restante
 * 
 * Retorna el proceso que terminaria mas rapido. A diferencia de SJF,
 * este proceso puede ser devuelto a la cola si es interrumpido.
 */
Proceso* STCFScheduler::obtenerSiguienteProceso() {
    if (procesos.empty()) return nullptr;
    
    Proceso* proceso = procesos[0];
    procesos.erase(procesos.begin());
    return proceso;
}