#include "MLFQScheduler.h"
#include "Proceso.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

/*
  Programa principal del simulador MLFQ
  
  Este programa lee un archivo con procesos, configura un scheduler MLFQ
  segun el esquema seleccionado, ejecuta la simulacion y genera un archivo
  con los resultados.
  
  El programa soporta 3 esquemas predefinidos de configuracion de colas
  que representan diferentes estrategias de scheduling.


/*
  Lee el archivo de entrada y crea los procesos
  
  El archivo debe tener el formato:
  # etiqueta;BT;AT;Q;Pr
  A;6;0;3;5
  B;9;0;4;4
  
  Donde cada linea (excepto comentarios con #) representa un proceso
  con su etiqueta, burst time, arrival time, cola inicial y prioridad.
 */
std::vector<Proceso*> leerArchivo(const std::string& rutaArchivo) {
    std::vector<Proceso*> procesos;
    std::ifstream archivo(rutaArchivo);
    
    if (!archivo.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo " << rutaArchivo << std::endl;
        return procesos;
    }
    
    std::string linea;
    std::cout << "Leyendo archivo: " << rutaArchivo << std::endl;
    
    // Leer linea por linea
    while (std::getline(archivo, linea)) {
        // Saltar comentarios y lineas vacias
        if (linea.empty() || linea[0] == '#') {
            continue;
        }
        
        // Parsear la linea separando por punto y coma
        std::istringstream iss(linea);
        std::string etiqueta, bt_str, at_str, q_str, pr_str;
        
        if (std::getline(iss, etiqueta, ';') &&
            std::getline(iss, bt_str, ';') &&
            std::getline(iss, at_str, ';') &&
            std::getline(iss, q_str, ';') &&
            std::getline(iss, pr_str)) {
            
            try {
                // Convertir strings a numeros
                int tiempoRafaga = std::stoi(bt_str);
                int tiempoLlegada = std::stoi(at_str);
                int cola = std::stoi(q_str);
                int prioridad = std::stoi(pr_str);
                
                // Crear el proceso y agregarlo a la lista
                Proceso* proceso = new Proceso(etiqueta, tiempoRafaga, tiempoLlegada, cola, prioridad);
                procesos.push_back(proceso);
                
                std::cout << "Proceso cargado: " << etiqueta 
                          << " (BT=" << tiempoRafaga << ", AT=" << tiempoLlegada 
                          << ", Q=" << cola << ", Pr=" << prioridad << ")" << std::endl;
                          
            } catch (const std::exception& e) {
                std::cerr << "Error al parsear linea: " << linea << std::endl;
            }
        }
    }
    
    archivo.close();
    std::cout << "Total de procesos cargados: " << procesos.size() << std::endl;
    return procesos;
}

/*
  Genera el nombre del archivo de salida
  
  Crea la carpeta 'output' si no existe y genera el nombre del archivo
  de salida basado en el archivo de entrada, agregando '_out' antes
  de la extension.
 */
std::string generarNombreArchivoSalida(const std::string& archivoEntrada) {
    // Crear carpeta output si no existe
    #ifdef _WIN32
        system("mkdir output 2>nul || echo."); 
    #else
        system("mkdir -p output");
    #endif
    
    // Extraer solo el nombre del archivo sin directorio
    std::string nombreArchivo = archivoEntrada;
    size_t pos = nombreArchivo.find_last_of("\\/");
    if (pos != std::string::npos) {
        nombreArchivo = nombreArchivo.substr(pos + 1);
    }
    
    // Cambiar la extension a _out.txt
    std::string nombreSalida = "output/";
    pos = nombreArchivo.find_last_of('.');
    if (pos != std::string::npos) {
        nombreSalida += nombreArchivo.substr(0, pos) + "_out.txt";
    } else {
        nombreSalida += nombreArchivo + "_out.txt";
    }
    
    return nombreSalida;
}

/*
  Define los esquemas de configuracion predefinidos
  
  Cada esquema tiene una configuracion diferente de colas y algoritmos:
  
  Esquema 1: RR(1), RR(3), RR(4), SJF
  - Quantums pequenos en las primeras colas para detectar trabajos interactivos
  - SJF en la ultima cola para trabajos largos
  
  Esquema 2: RR(2), RR(3), RR(4), STCF  
  - Similar al 1 pero con STCF en lugar de SJF
  
  Esquema 3: RR(3), RR(5), RR(6), RR(20)
  - Solo Round Robin con quantums crecientes
 */
std::vector<EsquemaCola> obtenerEsquema(int numeroEsquema) {
    std::vector<EsquemaCola> esquemas;
    
    switch (numeroEsquema) {
        case 1:
            esquemas.push_back(EsquemaCola(TipoPolitica::ROUND_ROBIN, 1));
            esquemas.push_back(EsquemaCola(TipoPolitica::ROUND_ROBIN, 3));
            esquemas.push_back(EsquemaCola(TipoPolitica::ROUND_ROBIN, 4));
            esquemas.push_back(EsquemaCola(TipoPolitica::SJF));
            std::cout << "Usando Esquema 1: RR(1), RR(3), RR(4), SJF" << std::endl;
            break;
            
        case 2:
            esquemas.push_back(EsquemaCola(TipoPolitica::ROUND_ROBIN, 2));
            esquemas.push_back(EsquemaCola(TipoPolitica::ROUND_ROBIN, 3));
            esquemas.push_back(EsquemaCola(TipoPolitica::ROUND_ROBIN, 4));
            esquemas.push_back(EsquemaCola(TipoPolitica::STCF));
            std::cout << "Usando Esquema 2: RR(2), RR(3), RR(4), STCF" << std::endl;
            break;
            
        case 3:
            esquemas.push_back(EsquemaCola(TipoPolitica::ROUND_ROBIN, 3));
            esquemas.push_back(EsquemaCola(TipoPolitica::ROUND_ROBIN, 5));
            esquemas.push_back(EsquemaCola(TipoPolitica::ROUND_ROBIN, 6));
            esquemas.push_back(EsquemaCola(TipoPolitica::ROUND_ROBIN, 20));
            std::cout << "Usando Esquema 3: RR(3), RR(5), RR(6), RR(20)" << std::endl;
            break;
            
        default:
            std::cerr << "Error: Esquema " << numeroEsquema << " no valido. Use 1, 2 o 3." << std::endl;
            break;
    }
    
    return esquemas;
}

/*
  Funcion principal
  
  Lee los argumentos de linea de comandos, carga los procesos del archivo,
  configura el scheduler segun el esquema seleccionado, ejecuta la simulacion
  y genera el archivo de resultados.
 */
int main(int argc, char* argv[]) {
    std::cout << "=== SIMULADOR MLFQ - SISTEMAS OPERATIVOS ===" << std::endl;
    std::cout << "Universidad Pontificia Javeriana Cali" << std::endl;
    std::cout << "=============================================" << std::endl;
    
    // Verificar argumentos
    if (argc != 3) {
        std::cerr << "Uso: " << argv[0] << " <archivo_entrada> <numero_esquema>" << std::endl;
        std::cerr << "Esquemas disponibles:" << std::endl;
        std::cerr << "  1: RR(1), RR(3), RR(4), SJF" << std::endl;
        std::cerr << "  2: RR(2), RR(3), RR(4), STCF" << std::endl;
        std::cerr << "  3: RR(3), RR(5), RR(6), RR(20)" << std::endl;
        return 1;
    }
    
    std::string archivoEntrada = argv[1];
    int numeroEsquema = std::atoi(argv[2]);
    
    std::cout << "\nParametros de simulacion:" << std::endl;
    std::cout << "Archivo de entrada: " << archivoEntrada << std::endl;
    std::cout << "Numero de esquema: " << numeroEsquema << std::endl;
    
    try {
        // Cargar procesos del archivo
        std::vector<Proceso*> procesos = leerArchivo(archivoEntrada);
        if (procesos.empty()) {
            std::cerr << "Error: No se pudieron cargar procesos del archivo." << std::endl;
            return 1;
        }
        
        // Obtener configuracion del esquema
        std::vector<EsquemaCola> esquemas = obtenerEsquema(numeroEsquema);
        if (esquemas.empty()) {
            std::cerr << "Error: Esquema no valido." << std::endl;
            return 1;
        }
        
        // Crear el scheduler con la configuracion
        MLFQScheduler scheduler(esquemas);
        
        // Agregar todos los procesos al scheduler
        std::cout << "\nAgregando procesos al scheduler..." << std::endl;
        for (Proceso* proceso : procesos) {
            scheduler.agregarProceso(proceso);
        }
        
        // Ejecutar la simulacion
        std::cout << "\n=== INICIANDO SIMULACION ===" << std::endl;
        scheduler.ejecutarSimulacion();
        
        // Mostrar resultados
        scheduler.mostrarResultados();
        
        // Generar archivo de salida
        std::string archivoSalida = generarNombreArchivoSalida(archivoEntrada);
        scheduler.escribirSalida(archivoSalida);
        
        std::cout << "\n=== SIMULACION COMPLETADA EXITOSAMENTE ===" << std::endl;
        std::cout << "Resultados guardados en: " << archivoSalida << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error durante la simulacion: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}