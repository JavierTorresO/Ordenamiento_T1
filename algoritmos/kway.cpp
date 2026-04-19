#include "kway.h"
#include <vector>
#include "utils/utils.h"

// función recursiva k-way
void kway_rec(int* A, int l, int r, int k) {
    if (l >= r) return;

    int n = r - l + 1;

    // tamaño base de cada bloque, repartimiento equitativo
    int block_size = n / k;
    int remainder = n % k; //los primeros "remainder" bloques bloques reciben uno mas para evitar desbalance

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
        if (starts[i] <= ends[i]) { //aqui controlamos bloques vacios
            kway_rec(A, starts[i], ends[i], k);
        }
    }

    // merge en cadena
    for (int i = 1; i < k; i++) {
        if (starts[i] <= ends[i]) {
            merge(A, l, ends[i-1], ends[i]);
        }
    }
}


// interfaz principal
void kway_mergesort(int* A, int n, int k) {
    kway_rec(A, 0, n - 1, k);
}

//divide en k partes, profundidad: logₖ(n) más chica pero el Merge es más caro (entre k mayor->profundidad menor->costo merge mayor)

/*
kway_mergesort(A, l, r):
1. dividir en k partes
2. llamar recursivamente a cada parte
3. hacer merge de las k parte 
    Funcionará como un merge en cadena: 
    (A1 + A2) → T1
    (T1 + A3) → T2
    (T2 + A4) → ...
*/
