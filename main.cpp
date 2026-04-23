#include <cstdio>
#include <cstdlib>
#include <ctime>

#include "utils/utils.h"
#include "algoritmos/mergesort.h"
#include "algoritmos/kway.h"
#include "paralelismo/kway_paralelo.h"

int main() {
    srand(42); //se fija semilla(resultado reproductible)

    int n = 100000; // Aumentado para notar paralelismo
    int k = 4; //para el kway

    int* A = new int[n];//se reserva memoria para el arreglo A
    int* B = new int[n];//otro arreglo B
    int* C = new int[n];//para el paralelo

    generate_array(A, n);
    // copiar A en B y C
    for (int i = 0; i < n; i++) {
        B[i] = A[i];
        C[i] = A[i];
    }
    // A, B y C tendrán igual entrada, hace comparación mas justa

    // -----------------------------
    // MERGESORT CLÁSICO
    // -----------------------------
    double t1 = measure_time(mergesort, A, n);

    if (is_sorted(A, n)) {
        printf("Mergesort OK\n");
    } else {
        printf("Mergesort ERROR\n");
    }

    printf("Tiempo mergesort: %f segundos\n\n", t1);

    // -----------------------------
    // K-WAY MERGESORT
    // -----------------------------
    double t2 = measure_time(
        [](int* arr, int size) {
            kway_mergesort(arr, size, 4); // cambiar k aquí
        },
        B,
        n
    );

    if (is_sorted(B, n)) {
        printf("K-way mergesort OK\n");
    } else {
        printf("K-way mergesort ERROR\n");
    }

    printf("Tiempo k-way (k=%d): %f segundos\n\n", k, t2);

    // -----------------------------
    // K-WAY MERGESORT PARALELO
    // -----------------------------
    double t3 = measure_time(
        [](int* arr, int size) {
            kway_mergesort_paralelo(arr, size, 4);
        },
        C,
        n
    );

    if (is_sorted(C, n)) {
        printf("K-way mergesort PARALELO OK\n");
    } else {
        printf("K-way mergesort PARALELO ERROR\n");
    }

    printf("Tiempo k-way PARALELO (k=%d): %f segundos\n", k, t3);

    // -----------------------------
    // LIMPIEZA
    // -----------------------------
    delete[] A;
    delete[] B;
    delete[] C;

    return 0;
}