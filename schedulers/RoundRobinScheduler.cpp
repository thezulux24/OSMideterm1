#include "RoundRobinScheduler.h"
#include <algorithm>

/*
  Constructor
  
  Guarda el quantum que van a usar todos los procesos en esta cola.
  El quantum define cuanto tiempo maximo puede ejecutar cada proceso.
 */
RoundRobinScheduler::RoundRobinScheduler(int q) : quantum(q) {
}

/*
  Agrega un proceso a la cola
  
  Simplemente lo pone al final de la cola. Los procesos se atienden
  en el orden que llegan (FIFO).
 */
void RoundRobinScheduler::agregarProceso(Proceso* proceso) {
    cola.push(proceso);
}

/*
  Obtiene el siguiente proceso a ejecutar
  
  Saca el proceso que esta al frente de la cola. Si no hay procesos
  retorna nullptr.
 */
Proceso* RoundRobinScheduler::obtenerSiguienteProceso() {
    if (cola.empty()) return nullptr;
    
    Proceso* proceso = cola.front();
    cola.pop();
    return proceso;
}

/*
  Ejecuta el proceso por su quantum
  
  El proceso ejecuta por el quantum completo, PERO si le queda menos tiempo
  del quantum, solo ejecuta el tiempo que le falta.
  
  Ejecuta unidad por unidad para simular el paso del tiempo correctamente.
  Al final reporta cuanto tiempo realmente ejecuto.
 */
void RoundRobinScheduler::ejecutarProceso(Proceso* proceso, int tiempoActual, int& tiempoEjecutado) {
    // Ejecuta por el quantum o por el tiempo restante, el que sea menor
    tiempoEjecutado = std::min(quantum, proceso->getTiempoRestante());
    
    // Simular la ejecucion unidad por unidad
    for (int i = 0; i < tiempoEjecutado; i++) {
        proceso->ejecutar(tiempoActual + i);
    }
}