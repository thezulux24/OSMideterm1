#ifndef SJFSCHEDULER_H
#define SJFSCHEDULER_H

#include "../Proceso.h"
#include <vector>

/*
  Clase SJFScheduler
  
  Implementa Shortest Job First usando el tiempo restante como criterio.
  Siempre escoge el proceso que menos tiempo le falta para terminar.
  
  En esta implementacion SJF es no-preemptivo: una vez que escoge un proceso,
  lo deja ejecutar hasta que termine completamente.
  
 */
class SJFScheduler {
private:
    std::vector<Proceso*> procesos;  // Vector ordenado por tiempo restante

public:
    // Inicializa el scheduler vacio
    SJFScheduler();
    
    // Agrega un proceso manteniendo el orden por tiempo restante
    void agregarProceso(Proceso* proceso);
    
    // Obtiene el proceso con menor tiempo restante
    Proceso* obtenerSiguienteProceso();
    
    // Ejecuta el proceso hasta que termine completamente
    void ejecutarProceso(Proceso* proceso, int tiempoActual, int& tiempoEjecutado);
};

#endif