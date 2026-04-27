#include "mergesort_paralelo.h"
#include <algorithm>
#include <vector>
#include <omp.h>
#include "paralelismo/merge_paralelo.h"
#include "utils/utils.h"

struct Block {
    int l, r;
};

// RECURSIÓN PARALELA BASE
void mergesort_parallel_rec(int* A, int l, int r, int* aux, int granularity) {
    if (l >= r) return;

    if (r - l + 1 <= granularity) {
        // Fallback secuencial optimizado
        for (int i = l; i <= r; i++) {
            int key = A[i];
            int j = i - 1;
            while (j >= l && A[j] > key) {
                A[j + 1] = A[j];
                j--;
            }
            A[j + 1] = key;
        }
        return;
    }

    int mid = l + (r - l) / 2;

    #pragma omp task shared(A, aux) firstprivate(l, mid, granularity)
    mergesort_parallel_rec(A, l, mid, aux, granularity);
    
    #pragma omp task shared(A, aux) firstprivate(mid, r, granularity)
    mergesort_parallel_rec(A, mid + 1, r, aux, granularity);

    #pragma omp taskwait
    merge(A, l, mid, r, aux);
}

// K-WAY PARALELO RECURSIVO
void kway_parallel_rec(int* A, int l, int r, int k, int* aux, int granularity, bool full) {
    if (l >= r) return;

    int n = r - l + 1;
    if (n <= granularity) {
        // Usar la versión secuencial ya optimizada
        std::vector<int> local_aux(n);
        // Aquí simplificamos, pero lo ideal es pasar el aux global
        // Para no romper la lógica de bloques, llamamos a mergesort secuencial
        std::sort(A + l, A + r + 1);
        return;
    }

    int parts = std::max(2, k);
    int base = n / parts;
    int remainder = n % parts;
    std::vector<Block> blocks(parts);
    int current = l;

    for (int i = 0; i < parts; i++) {
        int size = base + (i < remainder ? 1 : 0);
        blocks[i] = {current, current + size - 1};
        current += size;
    }

    #pragma omp taskgroup
    for (int i = 0; i < parts; i++) {
        if (blocks[i].l <= blocks[i].r) {
            #pragma omp task shared(A, aux) firstprivate(i, blocks, k, granularity, full)
            kway_parallel_rec(A, blocks[i].l, blocks[i].r, k, aux, granularity, full);
        }
    }

    // MEZCLA POR RONDAS (Torneo)
    std::vector<Block> current_blocks;
    for (const auto& b : blocks) if (b.l <= b.r) current_blocks.push_back(b);

    while (current_blocks.size() > 1) {
        std::vector<Block> next_blocks;
        #pragma omp taskgroup
        for (size_t i = 0; i + 1 < current_blocks.size(); i += 2) {
            int b_l = current_blocks[i].l;
            int b_m = current_blocks[i].r;
            int b_r = current_blocks[i+1].r;
            
            #pragma omp task shared(A, aux) firstprivate(b_l, b_m, b_r, granularity, full)
            {
                if (full) parallel_merge_optimized(A, b_l, b_m, b_r, aux, granularity);
                else merge(A, b_l, b_m, b_r, aux);
            }
            next_blocks.push_back({b_l, b_r});
        }
        if (current_blocks.size() % 2 == 1) next_blocks.push_back(current_blocks.back());
        current_blocks = next_blocks;
    }
}

void mergesort_parallel(int* A, int n, int granularity) {
    std::vector<int> aux(n);
    #pragma omp parallel
    {
        #pragma omp single
        mergesort_parallel_rec(A, 0, n - 1, aux.data(), granularity);
    }
}

void kway_mergesort_parallel(int* A, int n, int k, int granularity) {
    std::vector<int> aux(n);
    #pragma omp parallel
    {
        #pragma omp single
        kway_parallel_rec(A, 0, n - 1, k, aux.data(), granularity, false);
    }
}

void kway_mergesort_parallel_full(int* A, int n, int k, int granularity) {
    std::vector<int> aux(n);
    #pragma omp parallel
    {
        #pragma omp single
        kway_parallel_rec(A, 0, n - 1, k, aux.data(), granularity, true);
    }
}
