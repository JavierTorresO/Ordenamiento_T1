#include "mergesort.h"
#include <vector>
#include "utils/utils.h"

// FUNCIÓN PRINCIPAL: recursión
void mergesort_rec(int* A, int l, int r) {
    //caso base: 1 solo elemento, ya estaría ordenado
    if (l >= r) return;

    //dividir en 2 mitades
    int mid = (l + r) / 2;

    mergesort_rec(A, l, mid);
    mergesort_rec(A, mid + 1, r);
    //combinar ambas mitades ya ordenadas
    merge(A, l, mid, r);
}

// INTERFAZ para ocupar en main
void mergesort(int* A, int n) {
    mergesort_rec(A, 0, n - 1);
}

//divide en 2, profundidad: log₂(n)
/*
1. divide: A → izquierda + derecha
2. ordena recursivamente cada lado
3. merge: 2 partes ordenadas -> una lista mas grande ordenada
*/