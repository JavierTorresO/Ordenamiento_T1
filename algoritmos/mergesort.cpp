#include "mergesort.h"
#include <vector>

// Función merge auxiliar, combina 2 subarreglos ordenados
void merge(int* A, int l, int mid, int r) {
    int n1 = mid - l + 1; //subarreglo izq
    int n2 = r - mid;  //subarreglo der

    std::vector<int> L(n1), R(n2); //vector aux, para copiar datos
    //copiarlos al arreglo izq (L=left)
    for (int i = 0; i < n1; i++)
        L[i] = A[l + i];
    //copiarlo al arreglo der (D=right)
    for (int j = 0; j < n2; j++)
        R[j] = A[mid + 1 + j];

    int i = 0, j = 0, k = l;
    //merge de ambos en orden
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            A[k++] = L[i++];//toma del lado izq
        } else {
            A[k++] = R[j++];//o der
        }
    }
    //se copia todo lo que pudo llegar a sobrar en izq y der
    while (i < n1) A[k++] = L[i++];
    while (j < n2) A[k++] = R[j++];
}

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