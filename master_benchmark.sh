#!/bin/bash

# Compilar con optimizaciones
make clean && make

# Tamaños exactos según PDF: 2^20, 2^22, 2^24, 2^26
SIZES=(1048576 4194304 16777216 67108864)
LABELS=("2^20" "2^22" "2^24" "2^26")
MODES=(1 4 6)
ALGO_NAMES=("Seq_Mergesort" "Par_Mergesort" "Full_Paralelo")
THREADS=(1 8 8)

REPORT="reporte_escala_completa.csv"
TMP_PERF="perf_output.tmp"

echo "N_Size,Algoritmo,Threads,Tiempo_s,Cache_References,Cache_Misses,IPC" > $REPORT

echo "===================================================================================="
echo " INICIANDO ESCALA EXPERIMENTAL COMPLETA (Ryzen 5 8600G)"
echo "===================================================================================="
printf "%-10s | %-15s | %-8s | %-12s | %-12s | %-6s\n" "n" "Algoritmo" "Tiempo" "Ref.Cache" "Miss.Cache" "IPC"
echo "------------------------------------------------------------------------------------"

for i in "${!SIZES[@]}"; do
    N=${SIZES[$i]}
    L=${LABELS[$i]}
    
    for j in "${!MODES[@]}"; do
        MODE=${MODES[$j]}
        ALGO=${ALGO_NAMES[$j]}
        T=${THREADS[$j]}
        
        export OMP_NUM_THREADS=$T
        
        # Ejecutar perf y capturar salida
        # Nota: En n=2^26, perf puede tardar un poco más.
        PROG_OUT=$(LC_NUMERIC=C perf stat -x, -e cycles,instructions,cache-references,cache-misses -o $TMP_PERF ./sort $N 4 8192 1 $MODE 2>&1)
        
        # Extraer Tiempo
        TIME=$(echo "$PROG_OUT" | grep -E "secuencial:|8\t" | tail -n 1 | awk '{print $2}')
        
        # Si no se encuentra el tiempo en la salida (posible error), poner 0
        if [ -z "$TIME" ]; then TIME="0.000000"; fi

        CYCLES=$(grep "cycles" $TMP_PERF | cut -d, -f1)
        INST=$(grep "instructions" $TMP_PERF | cut -d, -f1)
        REFS=$(grep "cache-references" $TMP_PERF | cut -d, -f1)
        MISSES=$(grep "cache-misses" $TMP_PERF | cut -d, -f1)
        
        IPC=$(awk -v i=$INST -v c=$CYCLES 'BEGIN {if (c>0) printf "%.2f", i/c; else printf "0.00"}')
        
        echo "$L,$ALGO,$T,$TIME,$REFS,$MISSES,$IPC" >> $REPORT
        printf "%-10s | %-15s | %-8s | %-12s | %-12s | %-6s\n" "$L" "$ALGO" "$TIME" "$REFS" "$MISSES" "$IPC"
    done
    echo "------------------------------------------------------------------------------------"
done

rm $TMP_PERF
echo "Escala completa finalizada. Datos en: $REPORT"
