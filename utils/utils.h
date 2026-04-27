#ifndef UTILS_H
#define UTILS_H

#include <chrono>

// Generador de arreglos
void generate_array(int* A, int n); 

// Verificador si está ordenado o no
bool is_sorted(const int* A, int n);

// Medidor de tiempo (Template para aceptar lambdas con captura)
template <typename Func>
double measure_time(Func sort_func, int* A, int n) {
    auto start = std::chrono::high_resolution_clock::now();
    sort_func(A, n);
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double>(end - start).count();
}

// Merge optimizado con buffer auxiliar
void merge(int* A, int l, int mid, int r, int* aux);

// Quicksort paralelo (requerido por pauta)
void quicksort_parallel(int* A, int n, int granularity = 2048);

#endif
