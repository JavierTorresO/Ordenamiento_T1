# Ordenamiento_T1
Algoritmos secuenciales vs Algoritmos paralelos

## Compilar

En PowerShell:

```powershell
g++ -O3 -fopenmp -I. main.cpp utils/utils.cpp algoritmos/mergesort.cpp algoritmos/kway.cpp paralelismo/merge_paralelo.cpp paralelismo/mergesort_paralelo.cpp -o sort.exe
```

Si tienes `make` disponible:

```bash
make
```

## Ejecutar

```powershell
.\sort.exe [n] [k] [granularity] [repetitions]
```

Ejemplo:

```powershell
.\sort.exe 200000 4 4096 3
```

Parametros:

- `n`: tamano del arreglo.
- `k`: cantidad de particiones para el k-way mergesort.
- `granularity`: umbral para pasar a una version secuencial.
- `repetitions`: cantidad de repeticiones por configuracion de hebras.

El programa reporta:

- tiempos secuenciales;
- tiempo, speedup y eficiencia para `p = 1, 2, 4, 8`;
- resultados para merge paralelo, mergesort paralelo, k-way paralelo y version paralela completa.
