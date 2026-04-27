#include "utils.h"
#include <cstdlib>
#include <chrono>
#include <vector>
#include <algorithm>
#include <omp.h>

void generate_array(int* A, int n) {
    for (int i = 0; i < n; i++) {
        A[i] = rand() % 1000000;
    }
}

bool is_sorted(const int* A, int n) {
    for (int i = 1; i < n; i++) {
        if (A[i] < A[i - 1]) return false;
    }
    return true;
}

// MERGE OPTIMIZADO: Sin asignaciones internas, usa buffer auxiliar externo
void merge(int* A, int l, int mid, int r, int* aux) {
    int i = l;
    int j = mid + 1;
    int k = l;

    while (i <= mid && j <= r) {
        if (A[i] <= A[j]) {
            aux[k++] = A[i++];
        } else {
            aux[k++] = A[j++];
        }
    }

    while (i <= mid) aux[k++] = A[i++];
    while (j <= r) aux[k++] = A[j++];

    for (int idx = l; idx <= r; idx++) {
        A[idx] = aux[idx];
    }
}

// Implementación de Quicksort Paralelo para contraste (Pauta 3.7.1)
void quicksort_rec(int* A, int l, int r, int granularity) {
    if (l >= r) return;

    int i = l, j = r;
    int pivot = A[l + (r - l) / 2];

    while (i <= j) {
        while (A[i] < pivot) i++;
        while (A[j] > pivot) j--;
        if (i <= j) {
            std::swap(A[i], A[j]);
            i++;
            j--;
        }
    }

    if (r - l > granularity) {
        #pragma omp task shared(A) firstprivate(l, j, granularity)
        quicksort_rec(A, l, j, granularity);
        #pragma omp task shared(A) firstprivate(i, r, granularity)
        quicksort_rec(A, i, r, granularity);
    } else {
        if (l < j) quicksort_rec(A, l, j, granularity);
        if (i < r) quicksort_rec(A, i, r, granularity);
    }
}

void quicksort_parallel(int* A, int n, int granularity) {
    #pragma omp parallel
    {
        #pragma omp single
        quicksort_rec(A, 0, n - 1, granularity);
    }
}
