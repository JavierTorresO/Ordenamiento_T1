#!/bin/bash

# Compilar con optimizaciones antes de empezar
make clean && make

# Configuración de tamaños exactos (2^20, 2^22, 2^24, 2^26)
SIZES=(1048576 4194304 16777216 67108864)
LABELS=("2^20" "2^22" "2^24" "2^26")
REPORT="tabla_speedup_eficiencia.csv"
TEMP_OUT="raw_benchmark.tmp"

# Parámetros constantes
K=4
GRANULARITY=8192
REPETITIONS=3

# Inicializar CSV con la nueva columna N_Size
echo "N_Size,Algoritmo,Hilos,Tiempo_s,Speedup,Eficiencia" > "$REPORT"

echo "=========================================================================="
echo " EJECUTANDO ESCALABILIDAD MULTI-TAMAÑO (p = 1, 2, 4, 8)"
echo "=========================================================================="

for i in "${!SIZES[@]}"; do
    N=${SIZES[$i]}
    L=${LABELS[$i]}
    
    echo "Procesando n = $L ($N elementos)..."
    
    # Ejecutamos el modo 0 (Todos los algoritmos)
    # Redirigimos la salida a un archivo temporal para procesarla
    ./sort "$N" "$K" "$GRANULARITY" "$REPETITIONS" 0 > "$TEMP_OUT"
    
    # Función interna para extraer y formatear datos de la tabla de salida
    # $1: Título buscado en la salida, $2: Etiqueta para el CSV
    extract_metrics() {
        TITLE=$1
        ALGO_LABEL=$2
        
        # grep -A 5: Toma el título y las 5 líneas siguientes (Cabecera + 4 filas de datos)
        # tail -n 4: Se queda solo con las filas de datos (p=1, 2, 4, 8)
        # awk: Formatea la salida insertando N_Size y el nombre del algoritmo
        grep -A 5 "$TITLE" "$TEMP_OUT" | tail -n 4 | awk -v n_size="$L" -v algo="$ALGO_LABEL" '
        {
            # $1=p, $2=Tiempo, $3=Speedup, $4=Eficiencia
            print n_size "," algo "," $1 "," $2 "," $3 "," $4
        }' >> "$REPORT"
    }

    # Extraer datos para los algoritmos paralelos clave
    extract_metrics "Mergesort paralelo" "Par_Mergesort"
    extract_metrics "K-way mergesort paralelo" "Par_Kway"
    extract_metrics "Version paralela completa" "Full_Paralelo"

    echo "Finalizado n = $L"
    echo "--------------------------------------------------------------------------"
done

# Limpieza
rm "$TEMP_OUT"

echo "=========================================================================="
echo "BENCHMARK COMPLETADO"
echo "Archivo generado: $REPORT"
echo "=========================================================================="
