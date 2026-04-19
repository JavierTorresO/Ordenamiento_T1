#ifndef UTILS_H
#define UTILS_H

// Generador de arreglos
void generate_array(int* A, int n); 

// Verificador si está ordenado o no
bool is_sorted(int* A, int n);

// Medidor de tiempo
double measure_time(void (*sort_func)(int*, int), int* A, int n);

// Merge
void merge(int* A, int l, int mid, int r);

#endif