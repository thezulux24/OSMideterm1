# Simulador MLFQ (Multilevel Feedback Queue)

## Descripción
Simulador de planificación de procesos que implementa el algoritmo MLFQ con diferentes esquemas de colas.

## Estructura del proyecto
```
Scheduler/
├── main.cpp                    # Punto de entrada
├── Proceso.h/cpp              # Clase Proceso
├── MLFQScheduler.h/cpp        # Planificador principal
├── schedulers/                # Algoritmos específicos
│   ├── RoundRobinScheduler.h/cpp
│   ├── SJFScheduler.h/cpp
│   └── STCFScheduler.h/cpp
├── input/                     # Archivos de entrada
└── output/                    # Archivos de salida
```

## Compilación
```bash
g++  -o scheduler main.cpp Proceso.cpp MLFQScheduler.cpp schedulers/*.cpp
```

## Uso
```bash
./scheduler archivo_entrada.txt [esquema]
```

### Esquemas disponibles:
1. RR(1), RR(3), RR(4), SJF
2. RR(2), RR(3), RR(4), STCF  
3. RR(3), RR(5), RR(6), RR(20)

## Formato de entrada
```
# etiqueta;BT;AT;Q;Pr
A;6;0;3;5
B;9;0;4;4
```

## Formato de salida
Los resultados se guardan en `output/archivo_out.txt`# OSMideterm1
