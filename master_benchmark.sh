#!/bin/bash

# Compilar el proyecto primero
make clean && make

# Configuración
SIZES=(1000000 8000000 16777216)
LABELS=("1M_L1_L2" "8M_L3_Stress" "16M_RAM_Heavy")
MODES=(1 4 6)
ALGO_NAMES=("Seq_Mergesort" "Par_Mergesort" "Full_Paralelo")
THREADS=(1 8 8)

REPORT="reporte_final_consolidado.csv"
TMP_PERF="perf_output.tmp"

# Cabecera del CSV
echo "N_Size,Algoritmo,Threads,Tiempo_s,Cache_References,Cache_Misses,IPC" > $REPORT

echo "=========================================================================="
echo " INICIANDO BENCHMARK CONSOLIDADO (Ryzen 5 8600G)"
echo "=========================================================================="
printf "%-15s | %-15s | %-8s | %-12s | %-12s | %-6s\n" "Tamaño" "Algoritmo" "Tiempo" "Ref.Cache" "Miss.Cache" "IPC"
echo "--------------------------------------------------------------------------------"

for i in "${!SIZES[@]}"; do
    N=${SIZES[$i]}
    L=${LABELS[$i]}
    
    for j in "${!MODES[@]}"; do
        MODE=${MODES[$j]}
        ALGO=${ALGO_NAMES[$j]}
        T=${THREADS[$j]}
        
        export OMP_NUM_THREADS=$T
        
        # Ejecutar perf y capturar salida del programa
        PROG_OUT=$(perf stat -x, -e cycles,instructions,cache-references,cache-misses -o $TMP_PERF ./sort $N 4 8192 1 $MODE 2>&1)
        
        TIME=$(echo "$PROG_OUT" | grep -E "secuencial:|8\t" | tail -n 1 | awk '{print $2}')
        CYCLES=$(grep "cycles" $TMP_PERF | cut -d, -f1)
        INST=$(grep "instructions" $TMP_PERF | cut -d, -f1)
        REFS=$(grep "cache-references" $TMP_PERF | cut -d, -f1)
        MISSES=$(grep "cache-misses" $TMP_PERF | cut -d, -f1)
        
        IPC=$(awk -v i=$INST -v c=$CYCLES 'BEGIN {if (c>0) printf "%.2f", i/c; else printf "0.00"}')
        
        echo "$L,$ALGO,$T,$TIME,$REFS,$MISSES,$IPC" >> $REPORT
        printf "%-15s | %-15s | %-8s | %-12s | %-12s | %-6s\n" "$L" "$ALGO" "$TIME" "$REFS" "$MISSES" "$IPC"
    done
    echo "--------------------------------------------------------------------------"
done

rm $TMP_PERF
echo "Benchmark finalizado. Resultados guardados en: $REPORT"
