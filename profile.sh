#!/bin/bash

# Compilar
make clean && make

N=1000000
K=4
GRAN=4096
REPS=1
CSV_FILE="benchmarks.csv"

# Limpiar el archivo CSV anterior y añadir cabecera (opcional, perf añade sus propias columnas)
echo "counter,unit,event,runtime,percentage,label" > $CSV_FILE

run_perf() {
    LABEL=$1
    MODE=$2
    THREADS=$3
    echo "Profiling: $LABEL..."
    export OMP_NUM_THREADS=$THREADS
    
    # -x, : usa coma como separador
    # -o : archivo de salida
    # --append : no sobrescribe, añade al final
    perf stat -x, -o $CSV_FILE --append ./sort $N $K $GRAN $REPS $MODE
}

run_perf "Mergesort_Secuencial" 1 1
run_perf "Mergesort_Paralelo_p8" 4 8
run_perf "Version_Completa_p8" 6 8

echo "----------------------------------------------------------"
echo "Profiling completado. Datos guardados en $CSV_FILE"
echo "----------------------------------------------------------"
