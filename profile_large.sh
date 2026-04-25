#!/bin/bash
# N=16.777.216 (2^24) - Dominio total de la RAM
N=16777216
CSV="results_large_N24.csv"
echo "valor,unidad,evento,tiempo_ms,porcentaje,label" > $CSV

run_bench() {
    export OMP_NUM_THREADS=$3
    # Aumentamos repeticiones a 2 para estabilidad en tamaños grandes
    perf stat -x, -o $CSV --append ./sort $N 4 8192 2 $2
}

echo "Ejecutando set Grande (N=2^24)..."
run_bench "Seq_Merge" 1 1
run_bench "Par_Merge" 4 8
run_bench "Full_Par" 6 8
echo "Listo. Datos en $CSV"
