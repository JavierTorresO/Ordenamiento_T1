#include "kway_paralelo.h"
#include <vector>
#include "utils/utils.h"
#include <omp.h>

// Umbral para decidir cuándo dejar de crear tareas paralelas
const int THRESHOLD = 10000;

void kway_rec_paralelo(int* A, int l, int r, int k) {
    if (l >= r) return;

    int n = r - l + 1;

    // tamaño base de cada bloque, repartimiento equitativo
    int block_size = n / k;
    int remainder = n % k;

    std::vector<int> starts(k);
    std::vector<int> ends(k);

    int current = l;

    // dividir en k subarreglos
    for (int i = 0; i < k; i++) {
        int size = block_size + (i < remainder ? 1 : 0);

        starts[i] = current;
        ends[i] = current + size - 1;

        current += size;
    }

    // ordenar cada subarreglo recursivamente
    for (int i = 0; i < k; i++) {
        if (starts[i] <= ends[i]) {
            int s = starts[i];
            int e = ends[i];
            if (n > THRESHOLD) {
                #pragma omp task shared(A) firstprivate(s, e, k)
                kway_rec_paralelo(A, s, e, k);
            } else {
                kway_rec_paralelo(A, s, e, k);
            }
        }
    }

    // Esperar a que todas las tareas de este nivel terminen antes de mezclar
    #pragma omp taskwait

    // merge en cadena
    for (int i = 1; i < k; i++) {
        if (starts[i] <= ends[i]) {
            merge(A, l, ends[i-1], ends[i]);
        }
    }
}

// interfaz principal paralela
void kway_mergesort_paralelo(int* A, int n, int k) {
    #pragma omp parallel
    {
        #pragma omp single
        {
            kway_rec_paralelo(A, 0, n - 1, k);
        }
    }
}
