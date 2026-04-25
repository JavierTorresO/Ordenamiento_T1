#!/bin/bash

# Compilar
make clean && make

N=1000000
K=4
GRAN=4096
REPS=1

run_perf_csv() {
    LABEL=$1
    MODE=$2
    THREADS=$3
    FILE="$LABEL.csv"
    
    echo "Guardando profiling de $LABEL en $FILE..."
    export OMP_NUM_THREADS=$THREADS
    
    # Cabecera para el CSV
    echo "valor,unidad,evento,tiempo_ms,porcentaje,otros" > "$FILE"
    
    # -x, : separador coma
    # -o : archivo de salida
    perf stat -x, -o "$FILE" --append ./sort $N $K $GRAN $REPS $MODE
}

run_perf_csv "mergesort_secuencial" 1 1
run_perf_csv "kway_secuencial" 2 1
run_perf_csv "mergesort_paralelo" 4 8
run_perf_csv "kway_paralelo_completo" 6 8

echo "----------------------------------------------------------"
echo "Completado. Archivos generados: mergesort_secuencial.csv, kway_secuencial.csv, mergesort_paralelo.csv, kway_paralelo_completo.csv"
echo "----------------------------------------------------------"
