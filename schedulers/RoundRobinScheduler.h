#ifndef ROUNDROBINSCHEDULER_H
#define ROUNDROBINSCHEDULER_H

#include "../Proceso.h"
#include <queue>

/*
  Clase RoundRobinScheduler
  
  Implementa Round Robin con quantum fijo. Los procesos se atienden en orden
  FIFO, cada uno ejecuta maximo por su quantum, y si no termina vuelve al final.
  
  En esta implementacion, el scheduler solo maneja los procesos de una cola especifica
  del MLFQ. El MLFQScheduler principal se encarga de mover procesos entre colas.
 */
class RoundRobinScheduler {
private:
    int quantum;                    // Tiempo maximo que puede ejecutar cada proceso
    std::queue<Proceso*> cola;      // Cola FIFO de procesos listos

public:
    // Crea el scheduler con el quantum especificado
    RoundRobinScheduler(int q);
    
    // Agrega un proceso al final de la cola
    void agregarProceso(Proceso* proceso);
    
    // Saca el primer proceso de la cola para ejecutarlo
    Proceso* obtenerSiguienteProceso();
    
    // Ejecuta el proceso por su quantum o hasta que termine
    void ejecutarProceso(Proceso* proceso, int tiempoActual, int& tiempoEjecutado);
};

#endif