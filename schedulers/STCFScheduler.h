#ifndef STCFSCHEDULER_H
#define STCFSCHEDULER_H

#include "../Proceso.h"
#include <vector>

/*
  Clase STCFScheduler
  
  Implementa Shortest Time-to-Completion First. Similar a SJF pero preemptivo:
  siempre ejecuta el proceso con menor tiempo restante, y puede interrumpir
  un proceso si llega otro con menor tiempo restante.
  
  En el contexto del MLFQ, la preempcion se maneja en el MLFQScheduler principal,
  este scheduler solo se encarga de mantener ordenados los procesos por tiempo restante.
 */
class STCFScheduler {
private:
    std::vector<Proceso*> procesos;  // Vector ordenado por tiempo restante

public:
    // Inicializa el scheduler vacio
    STCFScheduler();
    
    // Agrega un proceso manteniendo orden por tiempo restante
    void agregarProceso(Proceso* proceso);

    void ejecutarProceso(Proceso* proceso, int tiempoActual, int& tiempoEjecutado, int tiempoMaximo);

    
    // Obtiene el proceso con menor tiempo restante
    Proceso* obtenerSiguienteProceso();
};

#endif