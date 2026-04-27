#include "merge_paralelo.h"
#include <algorithm>
#include <utility>
#include <vector>
#include <omp.h>

namespace {

// Merge secuencial que usa buffer auxiliar
void sequential_merge_with_aux(const int* A, int l1, int r1, int l2, int r2, int* out, int out_l) {
    int i = l1, j = l2, k = out_l;
    while (i <= r1 && j <= r2) {
        out[k++] = (A[i] <= A[j]) ? A[i++] : A[j++];
    }
    while (i <= r1) out[k++] = A[i++];
    while (j <= r2) out[k++] = A[j++];
}

std::pair<int, int> merge_path_partition(const int* A, int l1, int n1, int l2, int n2, int diagonal) {
    int low = std::max(0, diagonal - n2);
    int high = std::min(diagonal, n1);

    while (low <= high) {
        int i = low + (high - low) / 2;
        int j = diagonal - i;

        bool left_ok = (i == 0) || (j == n2) || (A[l1 + i - 1] <= A[l2 + j]);
        bool right_ok = (j == 0) || (i == n1) || (A[l2 + j - 1] < A[l1 + i]);

        if (left_ok && right_ok) return {i, j};
        if (!left_ok) high = i - 1;
        else low = i + 1;
    }
    return {low, diagonal - low};
}

void parallel_merge_internal(int* A, int l, int mid, int r, int* aux, int granularity) {
    int n1 = mid - l + 1;
    int n2 = r - mid;
    int total = n1 + n2;

    if (total <= granularity) {
        sequential_merge_with_aux(A, l, mid, mid + 1, r, aux, l);
    } else {
        int team_size = omp_get_num_threads();
        int partitions = std::max(2, team_size * 2);

        #pragma omp taskgroup
        for (int p = 0; p < partitions; p++) {
            int diag_start = (p * total) / partitions;
            int diag_end = ((p + 1) * total) / partitions;

            #pragma omp task firstprivate(p, diag_start, diag_end) shared(A, aux)
            {
                auto cut_start = merge_path_partition(A, l, n1, mid + 1, n2, diag_start);
                auto cut_end = merge_path_partition(A, l, n1, mid + 1, n2, diag_end);

                sequential_merge_with_aux(A, 
                    l + cut_start.first, l + cut_end.first - 1,
                    mid + 1 + cut_start.second, mid + 1 + cut_end.second - 1,
                    aux, l + diag_start);
            }
        }
    }

    // Copiar de vuelta (paralelizado si es grande)
    if (total > granularity) {
        #pragma omp taskloop shared(A, aux) firstprivate(l, total)
        for (int i = 0; i < total; i++) A[l + i] = aux[l + i];
    } else {
        for (int i = 0; i < total; i++) A[l + i] = aux[l + i];
    }
}

} // namespace

void parallel_merge(int* A, int l, int mid, int r, int granularity) {
    int n = r - l + 1;
    std::vector<int> aux(n); 
    // Nota: Para máxima eficiencia, este aux debería venir desde arriba, 
    // pero mantenemos la firma para compatibilidad.
    parallel_merge_internal(A, l, mid, r, aux.data() - l, granularity);
}

// Versión que acepta el aux desde fuera para evitar mallocs recursivos
void parallel_merge_optimized(int* A, int l, int mid, int r, int* aux, int granularity) {
    parallel_merge_internal(A, l, mid, r, aux, granularity);
}
