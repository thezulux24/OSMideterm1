#ifndef PROCESO_H
#define PROCESO_H

#include <iostream>
#include <string>

/*
  Clase Proceso
  
  Esta clase representa un proceso individual en el simulador MLFQ.
  Almacena toda la informacion basica del proceso como etiqueta, tiempos
  y posicion en las colas, asi como las metricas calculadas durante la simulacion.
  
  La clase maneja el estado del proceso durante toda su vida util en el sistema,
  desde que llega hasta que termina, calculando automaticamente las metricas
  de rendimiento necesarias para el analisis del scheduler.
 */
class Proceso {
private:
    // Informacion basica del proceso
    std::string etiqueta;           // Nombre del proceso (A, B, C, etc.)
    int tiempoRafaga;              // Tiempo total de CPU que necesita
    int tiempoLlegada;             // Cuando llega al sistema
    int cola;                      // Cola actual (0-indexed internamente)
    int colaOriginal;              // Cola donde empezo (para reportes)
    int prioridad;                 // Valor de prioridad del proceso
    
    // Metricas calculadas al final
    int tiempoEspera;              // Tiempo esperando en colas
    int tiempoFinalizacion;        // Cuando termino de ejecutar
    int tiempoRespuesta;           // Tiempo hasta primera ejecucion
    int tiempoRetorno;             // Tiempo total en el sistema
    
    // Variables de control para la simulacion
    int tiempoRestante;            // Cuanto tiempo de CPU le falta
    int tiempoInicio;              // Cuando ejecuto por primera vez
    bool haIniciado;               // Si ya ha ejecutado alguna vez

public:
    // Constructor que inicializa el proceso con sus datos basicos
    Proceso(std::string etiq, int bt, int at, int q, int pr);
    
    // Getters para acceder a los datos sin modificarlos
    std::string getEtiqueta() const { return etiqueta; }
    int getTiempoRafaga() const { return tiempoRafaga; }
    int getTiempoLlegada() const { return tiempoLlegada; }
    int getCola() const { return cola; }
    int getColaOriginal() const { return colaOriginal; }
    int getPrioridad() const { return prioridad; }
    int getTiempoEspera() const { return tiempoEspera; }
    int getTiempoFinalizacion() const { return tiempoFinalizacion; }
    int getTiempoRespuesta() const { return tiempoRespuesta; }
    int getTiempoRetorno() const { return tiempoRetorno; }
    int getTiempoRestante() const { return tiempoRestante; }
    int getTiempoInicio() const { return tiempoInicio; }
    bool getHaIniciado() const { return haIniciado; }
    
    // Setters para que el scheduler pueda actualizar el estado
    void setCola(int c) { cola = c; }
    void setTiempoEspera(int te) { tiempoEspera = te; }
    void setTiempoFinalizacion(int tf) { tiempoFinalizacion = tf; }
    void setTiempoRespuesta(int tr) { tiempoRespuesta = tr; }
    void setTiempoRetorno(int tt) { tiempoRetorno = tt; }
    void setTiempoRestante(int tr) { tiempoRestante = tr; }
    void setTiempoInicio(int ti) { tiempoInicio = ti; haIniciado = true; }
    
    // Simula la ejecucion del proceso por una unidad de tiempo
    void ejecutar(int tiempoActual);
    
    // Calcula las metricas finales cuando el proceso termina
    void calcularMetricas();
    
    // Verifica si el proceso ya termino de ejecutar
    bool estaCompleto() const { return tiempoRestante <= 0; }
    
    // Muestra la informacion del proceso en pantalla
    void mostrarInfo() const;
    
    // Genera la linea de salida para el archivo de resultados
    std::string toString() const;
};

#endif