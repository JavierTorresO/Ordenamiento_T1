#!/bin/bash
# N sugerido para esta prueba: 2^24 (16.7M)
N=16777216
REPORT="tabla_speedup_eficiencia.csv"

make clean && make

echo "Algoritmo,Hilos,Tiempo_s,Speedup,Eficiencia" > $REPORT

echo "Ejecutando escalabilidad de hilos para N=2^24..."
# Ejecutamos el modo 0 (todo) que ya tiene la lógica de 1, 2, 4, 8 hilos
./sort $N 4 8192 3 0 > temp_output.txt

# Extraer los datos de las tablas que imprime el programa
grep -A 5 "Mergesort paralelo" temp_output.txt | tail -n 4 | awk -v algo="Par_Mergesort" '{print algo","$1","$2","$3","$4}' >> $REPORT
grep -A 5 "Version paralela completa" temp_output.txt | tail -n 4 | awk -v algo="Full_Paralelo" '{print algo","$1","$2","$3","$4}' >> $REPORT

rm temp_output.txt
echo "Tabla generada en $REPORT"
