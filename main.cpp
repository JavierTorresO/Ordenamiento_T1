#include <cstdio>
#include <cstdlib>
#include <ctime>

#include "utils/utils.h"
#include "algoritmos/mergesort.h"

int main() {
    srand(42); //se fija semilla(resultado reproductible)

    int n = 20; //elementos del arreglo (ejemplo)
    int* A = new int[n];//se reserva memoria para el arreglo A

    generate_array(A, n);

    printf("Antes (arreglo original):\n");
    for (int i = 0; i < n; i++) {
        printf("%d ", A[i]);
    }
    printf("\n");

    double t = measure_time(mergesort, A, n);

    printf("Despues:\n");
    for (int i = 0; i < n; i++) {
        printf("%d ", A[i]);
    }
    printf("\n");

    if (is_sorted(A, n)) {
        printf("OK: ordenado\n");
    } else {
        printf("ERROR\n");
    }

    printf("Tiempo: %f segundos\n", t);

    delete[] A;//liberar memoria

    return 0;
}