#include "utils.h"
#include <cstdlib>
#include <chrono>

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