#include "SJFScheduler.h"
#include <algorithm>

/*
  Constructor
  
  Inicializa el vector vacio. Los procesos se van agregando y ordenando
  conforme van llegando a esta cola.
 */
SJFScheduler::SJFScheduler() {
}

/*
  Agrega un proceso manteniendo orden
  
  Cada vez que se agrega un proceso, se reordena todo el vector para que
  el proceso con menor tiempo restante quede primero.
  
  Si dos procesos tienen el mismo tiempo restante, se prioriza el que llego
  primero al sistema (menor tiempo de llegada).
 */
void SJFScheduler::agregarProceso(Proceso* proceso) {
    procesos.push_back(proceso);
    
    // Reordenar por tiempo restante (menor primero)
    std::sort(procesos.begin(), procesos.end(), 
              [](Proceso* a, Proceso* b) {
                  if (a->getTiempoRestante() == b->getTiempoRestante()) {
                      return a->getTiempoLlegada() < b->getTiempoLlegada();
                  }
                  return a->getTiempoRestante() < b->getTiempoRestante();
              });
}

/*
  Obtiene el proceso con menor tiempo restante
  
  Como el vector esta ordenado, el primer proceso es el que tiene
  menor tiempo restante. Se saca del vector porque va a ejecutar
  hasta terminar.
 */
Proceso* SJFScheduler::obtenerSiguienteProceso() {
    if (procesos.empty()) return nullptr;
    
    Proceso* proceso = procesos[0];
    procesos.erase(procesos.begin());
    return proceso;
}

/*
  Ejecuta el proceso hasta completarlo
  
  SJF no-preemptivo significa que una vez que escoge un proceso,
  lo ejecuta hasta que termine. El tiempo ejecutado sera igual
  al tiempo restante que tenia el proceso.
 */
void SJFScheduler::ejecutarProceso(Proceso* proceso, int tiempoActual, int& tiempoEjecutado) {
    // Ejecuta todo el tiempo restante
    tiempoEjecutado = proceso->getTiempoRestante();
    
    // Simular la ejecucion unidad por unidad
    for (int i = 0; i < tiempoEjecutado; i++) {
        proceso->ejecutar(tiempoActual + i);
    }
}