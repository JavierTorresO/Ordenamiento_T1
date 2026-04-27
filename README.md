# Tarea 1: Algoritmos de Ordenamiento Secuenciales y Paralelos

Este proyecto implementa y compara diversos algoritmos de ordenamiento (Mergesort, K-way Mergesort y Quicksort) utilizando OpenMP para la paralelización en arquitecturas multinúcleo.

## Estructura del Proyecto

- `algoritmos/`: Versiones secuenciales de Mergesort y K-way.
- `paralelismo/`: Implementaciones paralelas (Merge Path, Task-based Mergesort).
- `utils/`: Utilidades para generación de datos, validación y Quicksort.
- `scripts/`: Scripts de Bash para la recolección automatizada de datos.

## Requisitos

- Compilador `g++` con soporte para C++11 y OpenMP.
- Herramienta `make`.
- `perf` (Linux) para el análisis de hardware e IPC.

## Compilación

Para compilar todas las versiones y el validador:

```bash
make
```

Esto generará el ejecutable `sort` y el ejecutable `validator`.

## Ejecución Manual

```bash
./sort [n] [k] [granularity] [repetitions] [mode]
```

### Parámetros:
- `n`: Tamaño del arreglo (ej. 67108864 para 2^26).
- `k`: Cantidad de particiones para K-way (ej. 4).
- `granularity`: Umbral de corte para tareas paralelas (recomendado: 4096).
- `repetitions`: Número de veces que se ejecuta cada prueba para promediar.
- `mode`: Selecciona el algoritmo a probar:
  - `0`: Ejecutar batería de pruebas completa (Todos).
  - `1`: Mergesort Secuencial.
  - `2`: K-way Mergesort Secuencial.
  - `4`: Mergesort Paralelo (Merge Secuencial).
  - `6`: Versión Paralela Completa (K-way + Merge Path).
  - `7`: Quicksort Paralelo.

## Reproducción de Experimentos

Para reproducir los resultados presentados en el informe y generar los archivos CSV de métricas (`estudio_k.csv` y `reporte_final_v2.csv`):

```bash
chmod +x run_all_benchmark.sh
./run_all_benchmark.sh
```

*Nota: Este script requiere permisos de root o configuración de `perf_event_paranoid` para capturar métricas de CPU.*

## Validación

Para verificar la corrección de todas las implementaciones frente a `std::sort`:

```bash
make validate
```
