#include "MLFQScheduler.h"
#include "schedulers/RoundRobinScheduler.h"
#include "schedulers/SJFScheduler.h" 
#include "schedulers/STCFScheduler.h"
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <climits>

/*
  Constructor del scheduler MLFQ
  
  Recibe la configuracion de esquemas que define como se comporta cada cola.
  Inicializa el vector de colas vacias segun la cantidad de esquemas.
  El tiempo global empieza en 0.
 */
MLFQScheduler::MLFQScheduler(const std::vector<EsquemaCola>& esq) 
    : esquemas(esq), tiempoGlobal(0) {
    // Crear tantas colas como esquemas se definieron
    colas.resize(esquemas.size());
}

/*
  Destructor
  
  Libera la memoria de todos los procesos creados durante la simulacion.
  Esto incluye procesos finalizados, procesos que quedaron en colas,
  y procesos que aun estaban en la cola de llegadas.
 */
MLFQScheduler::~MLFQScheduler() {
    // Liberar procesos finalizados
    for (auto proceso : procesosFinalizados) {
        delete proceso;
    }
    
    // Liberar procesos que quedaron en las colas
    for (auto& cola : colas) {
        while (!cola.empty()) {
            delete cola.front();
            cola.pop();
        }
    }
    
    // Liberar procesos que no llegaron a ejecutar
    for (auto proceso : colaLlegadas) {
        delete proceso;
    }
}

/*
 * Agrega un proceso al scheduler
  
  El proceso se agrega a la cola de llegadas ordenada por tiempo de llegada.
  No se pone directamente en las colas de scheduling porque puede que
  aun no haya llegado al sistema.


 */
void MLFQScheduler::agregarProceso(Proceso* proceso) {
    colaLlegadas.push_back(proceso);
    
    // Mantener la cola de llegadas ordenada por tiempo de llegada
    std::sort(colaLlegadas.begin(), colaLlegadas.end(), 
              [](Proceso* a, Proceso* b) { 
                  return a->getTiempoLlegada() < b->getTiempoLlegada(); 
              });
}

/*
  Mueve procesos que ya llegaron a sus colas de scheduling
  
  Revisa la cola de llegadas y mueve a las colas de scheduling todos
  los procesos cuyo tiempo de llegada ya paso. Los procesos se ponen
  en la cola especificada en su campo de cola original.
 */

void MLFQScheduler::moverProcesosLlegados() {
    while (!colaLlegadas.empty() && colaLlegadas[0]->getTiempoLlegada() <= tiempoGlobal) {
        Proceso* proceso = colaLlegadas[0];
        colaLlegadas.erase(colaLlegadas.begin());
        
        // Poner el proceso en su cola inicial, convirtiendo de 1-indexed a 0-indexed
        int nivelCola = proceso->getColaOriginal() - 1;
        proceso->setCola(nivelCola);
        colas[nivelCola].push(proceso);
    }
}

/*
  Planifica el siguiente proceso a ejecutar
  
  Implementa la politica MLFQ: busca procesos en orden de prioridad
  (cola 0 primero, luego cola 1, etc.) y dentro de cada cola aplica
  el algoritmo correspondiente.
  
  Para Round Robin simplemente saca el primero de la cola.
  Para SJF y STCF busca el proceso con menor tiempo restante.
 */
std::pair<int, Proceso*> MLFQScheduler::planificar() {
    // Buscar en orden de prioridad (cola 0 tiene mayor prioridad)
    for (int i = 0; i < colas.size(); i++) {
        if (!colas[i].empty()) {
            TipoPolitica politica = esquemas[i].politica;
            
            if (politica == TipoPolitica::ROUND_ROBIN) {
                // Round Robin: simplemente tomar el primero
                Proceso* proceso = colas[i].front();
                colas[i].pop();
                return std::make_pair(i, proceso);
                
            } else {
                // SJF o STCF: buscar el de menor tiempo restante
                std::vector<Proceso*> temp;
                Proceso* mejor = nullptr;
                int menorTiempo = INT_MAX;
                
                // Sacar todos los procesos de la cola para buscar el mejor
                while (!colas[i].empty()) {
                    Proceso* p = colas[i].front();
                    colas[i].pop();
                    
                    // Verificar si este proceso es mejor que el actual mejor
                    if (p->getTiempoRestante() < menorTiempo || 
                        (p->getTiempoRestante() == menorTiempo && 
                         (mejor == nullptr || p->getTiempoLlegada() < mejor->getTiempoLlegada()))) {
                        
                        // Si habia un mejor anterior, guardarlo en temp
                        if (mejor) temp.push_back(mejor);
                        mejor = p;
                        menorTiempo = p->getTiempoRestante();
                    } else {
                        // Este proceso no es el mejor, guardarlo en temp
                        temp.push_back(p);
                    }
                }
                
                // Devolver todos los procesos que no fueron seleccionados
                for (Proceso* p : temp) colas[i].push(p);
                
                return std::make_pair(i, mejor);
            }
        }
    }
    
    // No hay procesos listos
    return std::make_pair(-1, nullptr);
}

/*
  Ejecuta un proceso usando el scheduler apropiado para su cola
  
  Crea una instancia del scheduler especifico (RR, SJF, o STCF),
  le pasa el proceso, y maneja la ejecucion. Despues del quantum
  o terminacion, decide si degradar el proceso o finalizarlo.
 */
void MLFQScheduler::ejecutarConScheduler(Proceso* proceso, int indiceCola) {
    int tiempoEjecutado = 0;
    TipoPolitica politica = esquemas[indiceCola].politica;
    
    // Si es la primera vez que ejecuta, marcar el tiempo de inicio
    if (!proceso->getHaIniciado()) {
        proceso->setTiempoInicio(tiempoGlobal);
    }
    
    switch (politica) {
        case TipoPolitica::ROUND_ROBIN: {
            // Crear scheduler Round Robin con el quantum de esta cola
            RoundRobinScheduler rrScheduler(esquemas[indiceCola].quantum);
            rrScheduler.agregarProceso(proceso);
            
            Proceso* procesoActual = rrScheduler.obtenerSiguienteProceso();
            if (procesoActual) {
                // Ejecutar el proceso por su quantum
                rrScheduler.ejecutarProceso(procesoActual, tiempoGlobal, tiempoEjecutado);
                
                // Mostrar lo que paso
                std::cout << "Tiempo " << tiempoGlobal << " a " << (tiempoGlobal + tiempoEjecutado) 
                          << ": Proceso " << procesoActual->getEtiqueta() 
                          << " (Cola " << (indiceCola + 1) << ", RR-" << esquemas[indiceCola].quantum << ")" << std::endl;
                
                // Avanzar el tiempo global
                tiempoGlobal += tiempoEjecutado;
                
                if (procesoActual->estaCompleto()) {
                    // El proceso termino
                    procesoActual->setTiempoFinalizacion(tiempoGlobal);
                    procesoActual->calcularMetricas();
                    procesosFinalizados.push_back(procesoActual);
                } else {
                    // El proceso no termino, degradarlo a la siguiente cola
                    int nuevaCola = std::min(indiceCola + 1, (int)colas.size() - 1);
                    procesoActual->setCola(nuevaCola);
                    colas[nuevaCola].push(procesoActual);
                }
            }
            break;
        }
        
        case TipoPolitica::SJF: {
            // Crear scheduler SJF
            SJFScheduler sjfScheduler;
            sjfScheduler.agregarProceso(proceso);
            
            Proceso* procesoActual = sjfScheduler.obtenerSiguienteProceso();
            if (procesoActual) {
                // SJF ejecuta hasta completar
                sjfScheduler.ejecutarProceso(procesoActual, tiempoGlobal, tiempoEjecutado);
                
                std::cout << "Tiempo " << tiempoGlobal << " a " << (tiempoGlobal + tiempoEjecutado) 
                          << ": Proceso " << procesoActual->getEtiqueta() 
                          << " (Cola " << (indiceCola + 1) << ", SJF)" << std::endl;
                
                tiempoGlobal += tiempoEjecutado;
                
                // SJF siempre termina el proceso
                procesoActual->setTiempoFinalizacion(tiempoGlobal);
                procesoActual->calcularMetricas();
                procesosFinalizados.push_back(procesoActual);
            }
            break;
        }
        
        case TipoPolitica::STCF: {
            // Crear scheduler STCF
            STCFScheduler stcfScheduler;
            stcfScheduler.agregarProceso(proceso);
            
            Proceso* procesoActual = stcfScheduler.obtenerSiguienteProceso();
            if (procesoActual) {
                // STCF puede ser interrumpido, calcular cuanto puede ejecutar
                int tiempoMaximo = 1;  // Por defecto ejecuta 1 unidad
                
                // Verificar si hay procesos llegando pronto que podrian interrumpir
                if (!colaLlegadas.empty()) {
                    int proximaLlegada = colaLlegadas[0]->getTiempoLlegada();
                    if (proximaLlegada > tiempoGlobal) {
                        // Puede ejecutar hasta la proxima llegada o hasta terminar
                        tiempoMaximo = std::min(procesoActual->getTiempoRestante(), 
                                              proximaLlegada - tiempoGlobal);
                    }
                } else {
                    // No hay mas llegadas, puede ejecutar hasta terminar
                    tiempoMaximo = procesoActual->getTiempoRestante();
                }
                //ejecucion
                stcfScheduler.ejecutarProceso(procesoActual, tiempoGlobal, tiempoEjecutado, tiempoMaximo);

                
                std::cout << "Tiempo " << tiempoGlobal << " a " << (tiempoGlobal + tiempoEjecutado) 
                          << ": Proceso " << procesoActual->getEtiqueta() 
                          << " (Cola " << (indiceCola + 1) << ", STCF)" << std::endl;
                
                tiempoGlobal += tiempoEjecutado;
                
                if (procesoActual->estaCompleto()) {
                    // El proceso termino
                    procesoActual->setTiempoFinalizacion(tiempoGlobal);
                    procesoActual->calcularMetricas();
                    procesosFinalizados.push_back(procesoActual);
                } else {
                    // El proceso no termino, vuelve a la misma cola 
                    colas[indiceCola].push(procesoActual);
                }
            }
            break;
        }
    }
}

/*
  Ejecuta toda la simulacion
  
  Este es el bucle principal que maneja el tiempo global.
  En cada iteracion:
  1. Mueve procesos que ya llegaron a sus colas
  2. Planifica el siguiente proceso a ejecutar
  3. Lo ejecuta usando el scheduler apropiado
  4. Repite hasta que no queden procesos
  
  Si no hay procesos listos pero si hay procesos por llegar,
  avanza el tiempo hasta la proxima llegada.
 */
void MLFQScheduler::ejecutarSimulacion() {
    std::cout << "\nIniciando simulacion MLFQ..." << std::endl;
    
    // Continuar mientras haya procesos por llegar o procesos en colas
    while (!colaLlegadas.empty() || hayProcesosPendientes()) {
        // Mover procesos que ya llegaron
        moverProcesosLlegados();
        
        // Planificar el siguiente proceso
        std::pair<int, Proceso*> resultado = planificar();
        int indiceCola = resultado.first;
        Proceso* proceso = resultado.second;
        
        if (!proceso) {
            // No hay procesos listos, avanzar el tiempo
            if (colaLlegadas.empty()) {
                // No hay mas procesos por llegar, avanzar 1 unidad
                tiempoGlobal++;
            } else {
                // Saltar al tiempo de la proxima llegada
                tiempoGlobal = colaLlegadas[0]->getTiempoLlegada();
            }
            continue;
        }
        
        // Ejecutar el proceso seleccionado
        ejecutarConScheduler(proceso, indiceCola);
    }
    
    std::cout << "Simulacion completada en tiempo: " << tiempoGlobal << std::endl;
}

/*
  Verifica si hay procesos pendientes en alguna cola
  
  Recorre todas las colas de scheduling para ver si alguna tiene procesos.
  Se usa para determinar si la simulacion debe continuar.
 */
bool MLFQScheduler::hayProcesosPendientes() const {
    return std::any_of(colas.begin(), colas.end(), [](const std::queue<Proceso*>& cola) { 
        return !cola.empty(); 
    });
}

/*
  Calcula los promedios de las metricas de rendimiento
  
  Suma todas las metricas de los procesos finalizados y calcula
  los promedios. Si no hay procesos finalizados, todos los promedios son 0.
 */
void MLFQScheduler::calcularPromedios(double& promWT, double& promCT, double& promRT, double& promTAT) {
    if (procesosFinalizados.empty()) {
        promWT = promCT = promRT = promTAT = 0.0;
        return;
    }
    
    double sumaWT = 0, sumaCT = 0, sumaRT = 0, sumaTAT = 0;
    
    // Sumar todas las metricas
    for (const auto& proceso : procesosFinalizados) {
        sumaWT += proceso->getTiempoEspera();
        sumaCT += proceso->getTiempoFinalizacion();
        sumaRT += proceso->getTiempoRespuesta();
        sumaTAT += proceso->getTiempoRetorno();
    }
    
    // Calcular promedios
    int cantidad = procesosFinalizados.size();
    promWT = sumaWT / cantidad;
    promCT = sumaCT / cantidad;
    promRT = sumaRT / cantidad;
    promTAT = sumaTAT / cantidad;
}

/*
  Escribe los resultados en un archivo

  Genera un archivo con el formato requerido:
  - Header con los nombres de las columnas
  - Una linea por cada proceso con sus metricas
  - Linea final con los promedios
  
  Los procesos se ordenan alfabeticamente por etiqueta en el archivo.
 */
void MLFQScheduler::escribirSalida(const std::string& rutaArchivo) {
    std::ofstream archivo(rutaArchivo);
    
    if (!archivo.is_open()) {
        std::cerr << "Error al abrir el archivo de salida: " << rutaArchivo << std::endl;
        return;
    }
    
    // Escribir header
    archivo << "# etiqueta; BT; AT; Q; Pr; WT; CT; RT; TAT\n";
    
    // Ordenar procesos por etiqueta para el archivo de salida
    std::vector<Proceso*> procesosOrdenados = procesosFinalizados;
    std::sort(procesosOrdenados.begin(), procesosOrdenados.end(), 
              [](Proceso* a, Proceso* b) { 
                  return a->getEtiqueta() < b->getEtiqueta(); 
              });
    
    // Escribir cada proceso
    for (const auto& proceso : procesosOrdenados) {
        archivo << proceso->getEtiqueta() << ";"
                << proceso->getTiempoRafaga() << ";"
                << proceso->getTiempoLlegada() << ";"
                << (proceso->getCola() + 1) << ";"  // Convertir a 1-indexed
                << proceso->getPrioridad() << ";"
                << proceso->getTiempoEspera() << ";"
                << proceso->getTiempoFinalizacion() << ";"
                << proceso->getTiempoRespuesta() << ";"
                << proceso->getTiempoRetorno() << std::endl;
    }
    
    // Calcular y escribir promedios
    double promWT, promCT, promRT, promTAT;
    calcularPromedios(promWT, promCT, promRT, promTAT);
    
    archivo << std::fixed << std::setprecision(1);
    archivo << "WT=" << promWT << ";CT=" << promCT 
            << ";RT=" << promRT << ";TAT=" << promTAT << ";" << std::endl;
    
    archivo.close();
    std::cout << "Resultados escritos en: " << rutaArchivo << std::endl;
}

/*
  Muestra los resultados en pantalla
  
  Imprime un resumen de la simulacion con informacion de cada proceso
  y los promedios calculados. Util para verificar los resultados
  antes de revisar el archivo de salida.
 */
void MLFQScheduler::mostrarResultados() {
    std::cout << "\n=== RESULTADOS DE LA SIMULACION ===" << std::endl;
    std::cout << "Tiempo total de simulacion: " << tiempoGlobal << std::endl;
    std::cout << "Procesos completados: " << procesosFinalizados.size() << std::endl;
    
    // Mostrar informacion de cada proceso
    for (const auto& proceso : procesosFinalizados) {
        proceso->mostrarInfo();
    }
    
    // Calcular y mostrar promedios
    double promWT, promCT, promRT, promTAT;
    calcularPromedios(promWT, promCT, promRT, promTAT);
    
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "\nPromedios:" << std::endl;
    std::cout << "Tiempo de Espera (WT): " << promWT << std::endl;
    std::cout << "Tiempo de Finalizacion (CT): " << promCT << std::endl;
    std::cout << "Tiempo de Respuesta (RT): " << promRT << std::endl;
    std::cout << "Tiempo de Retorno (TAT): " << promTAT << std::endl;
}