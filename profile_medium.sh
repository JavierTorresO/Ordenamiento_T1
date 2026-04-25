#!/bin/bash
# N=8.000.000 (32MB) - Supera la L3 (16MB)
N=8000000
CSV="results_medium_N8M.csv"
echo "valor,unidad,evento,tiempo_ms,porcentaje,label" > $CSV

run_bench() {
    export OMP_NUM_THREADS=$3
    perf stat -x, -o $CSV --append ./sort $N 4 4096 1 $2
}

echo "Ejecutando set Mediano (N=8M)..."
run_bench "Seq_Merge" 1 1
run_bench "Par_Merge" 4 8
run_bench "Full_Par" 6 8
echo "Listo. Datos en $CSV"
