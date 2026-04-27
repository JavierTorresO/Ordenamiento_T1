#!/bin/bash

# Forzar local C para asegurar puntos decimales y consistencia en herramientas
export LC_ALL=C
export LANG=C

make clean && make

SIZES=(1048576 67108864) # 2^20 y 2^26
LABELS=("2^20" "2^26")
KS=(2 4 8 16)
GRAN=4096
REPS=1

REPORT_K="estudio_k.csv"
echo "N,K,Algoritmo,Tiempo_s" > $REPORT_K

echo ">>> Iniciando estudio de K..."
for N in "${SIZES[@]}"; do
    for K in "${KS[@]}"; do
        # K-way Secuencial
        OUT_SEQ=$(./sort $N $K $GRAN $REPS 2)
        T_SEQ=$(echo "$OUT_SEQ" | grep "K-way secuencial" | awk '{print $3}')
        echo "$N,$K,KWay_Seq,$T_SEQ" >> $REPORT_K
        
        # Full Paralelo (Extraemos solo el tiempo de p=8 de la tabla)
        OUT_PAR=$(./sort $N $K $GRAN $REPS 6)
        T_PAR=$(echo "$OUT_PAR" | awk '$1=="8" {print $2}')
        echo "$N,$K,Full_Par_p8,$T_PAR" >> $REPORT_K
    done
done

echo ">>> Generando reporte de escalabilidad con Quicksort..."
REPORT_FINAL="reporte_final_v2.csv"
echo "N,Algoritmo,p,Tiempo,Speedup,IPC,CacheMisses" > $REPORT_FINAL

for i in "${!SIZES[@]}"; do
    N=${SIZES[$i]}
    L=${LABELS[$i]}
    
    # Modos: 7 (Quicksort), 4 (Mergesort Par), 6 (Full Par k=4)
    for MODE in 7 4 6; do
        ALGO="MS_Par"
        [ $MODE -eq 7 ] && ALGO="Quick_Par"
        [ $MODE -eq 6 ] && ALGO="Full_Par"
        
        # Ejecutamos el programa una vez (él ya corre internamente p=1,2,4,8)
        # Capturamos su salida y el perf simultáneamente
        RAW_OUT=$(perf stat -x, -e instructions,cycles,cache-misses ./sort $N 4 $GRAN 1 $MODE 2>&1)
        
        for P in 1 8; do
            # Extraer tiempo de la tabla generada por el programa
            TIME=$(echo "$RAW_OUT" | awk -v p="$P" '$1==p {print $2}')
            
            # Extraer métricas de perf (vienen en líneas separadas por comas)
            INST=$(echo "$RAW_OUT" | grep "instructions" | cut -d, -f1)
            CYC=$(echo "$RAW_OUT" | grep "cycles" | cut -d, -f1)
            MISSES=$(echo "$RAW_OUT" | grep "cache-misses" | cut -d, -f1)
            
            # Calcular IPC con awk (asegurando punto decimal)
            IPC=$(awk -v i="$INST" -v c="$CYC" 'BEGIN {if(c>0) printf "%.2f", i/c; else print "0.00"}')
            
            # En la tabla de p=8 del programa, el speedup está en la columna 3
            SPD=$(echo "$RAW_OUT" | awk -v p="$P" '$1==p {print $3}')
            
            echo "$L,$ALGO,$P,$TIME,$SPD,$IPC,$MISSES" >> $REPORT_FINAL
        done
    done
done

echo "Proceso finalizado. Revisa $REPORT_K y $REPORT_FINAL"
