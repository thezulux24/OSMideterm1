#ifndef MLFQSCHEDULER_H
#define MLFQSCHEDULER_H

#include "Proceso.h"
#include <vector>
#include <queue>
#include <string>
#include <fstream>
#include <utility>

/*
  Enumeracion para los tipos de algoritmos de scheduling
  que pueden usarse en cada cola del MLFQ
 */
enum class TipoPolitica {
    ROUND_ROBIN,    // Round Robin con quantum
    SJF,           // Shortest Job First
    STCF           // Shortest Time-to-Completion First
};

/*
  Estructura que define como se comporta cada cola
  Cada cola tiene un algoritmo y opcionalmente un quantum
 */
struct EsquemaCola {
    TipoPolitica politica;  // Que algoritmo usa esta cola
    int quantum;           // Quantum para RR (se ignora en SJF/STCF)
    
    EsquemaCola(TipoPolitica pol, int q = -1) : politica(pol), quantum(q) {}
};

/*
  Clase MLFQScheduler
  
  Este es el scheduler principal que implementa Multilevel Feedback Queue.
  Maneja multiples colas con diferentes prioridades y algoritmos.
  
  Los procesos empiezan en la cola de mayor prioridad (indice 0) y pueden
  ser degradados a colas de menor prioridad si no terminan en su quantum.
  
  El scheduler siempre ejecuta procesos de la cola de mayor prioridad que
  tenga procesos disponibles.
 */
class MLFQScheduler {
private:
    std::vector<EsquemaCola> esquemas;              // Configuracion de cada cola
    std::vector<std::queue<Proceso*>> colas;       // Las colas de procesos
    std::vector<Proceso*> colaLlegadas;            // Procesos que aun no llegan
    std::vector<Proceso*> procesosFinalizados;     // Procesos terminados
    int tiempoGlobal;                              // Tiempo actual de simulacion
    
    // Mueve procesos que ya llegaron a sus colas correspondientes
    void moverProcesosLlegados();
    
    // Selecciona el siguiente proceso a ejecutar siguiendo las prioridades MLFQ
    std::pair<int, Proceso*> planificar();
    
    // Ejecuta un proceso usando el scheduler apropiado para su cola
    void ejecutarConScheduler(Proceso* proceso, int indiceCola);
    
    // Verifica si quedan procesos en alguna cola
    bool hayProcesosPendientes() const;
    
    // Calcula los promedios de las metricas
    void calcularPromedios(double& promWT, double& promCT, double& promRT, double& promTAT);

public:
    // Crea el scheduler con la configuracion de esquemas especificada
    MLFQScheduler(const std::vector<EsquemaCola>& esq);
    
    // Destructor que libera la memoria de todos los procesos
    ~MLFQScheduler();
    
    // Agrega un proceso al scheduler (lo pone en cola de llegadas)
    void agregarProceso(Proceso* proceso);
    
    // Ejecuta toda la simulacion hasta que terminen todos los procesos
    void ejecutarSimulacion();
    
    // Escribe los resultados en un archivo
    void escribirSalida(const std::string& rutaArchivo);
    
    // Muestra los resultados en pantalla
    void mostrarResultados();
    
    // Getters para acceso de solo lectura
    int getTiempoGlobal() const { return tiempoGlobal; }
    const std::vector<Proceso*>& getProcesosFinalizados() const { return procesosFinalizados; }
};

#endif