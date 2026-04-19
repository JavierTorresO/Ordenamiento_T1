#include "utils.h"
#include <cstdlib>
#include <chrono>
#include <vector>

//Función para crear arreglos
void generate_array(int* A, int n) {
    for (int i = 0; i < n; i++) {
        A[i] = rand() % 1000000;
    }
} //arreglo A de tamaño n, con numeros random(entre 0 y 999999)

//Verificador de ordenamiento (de menor a mayor)
bool is_sorted(int* A, int n) {
    for (int i = 1; i < n; i++) {
        if (A[i] < A[i - 1]) return false;
    }
    return true;
}//true si está ordenado, false si está desordenado 

//Medidor de tiempo
double measure_time(void (*sort_func)(int*, int), int* A, int n) {
    auto start = std::chrono::high_resolution_clock::now();

    sort_func(A, n);

    auto end = std::chrono::high_resolution_clock::now();

    return std::chrono::duration<double>(end - start).count();
}//cronometros de inicio y de fin, mide tiempo que demoró "sort_func"(para ordenar el arreglo)

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