#include "mergesort.h"
#include <vector>
#include "utils/utils.h"

void mergesort_rec(int* A, int l, int r, int* aux) {
    if (l >= r) return;

    int mid = l + (r - l) / 2;
    mergesort_rec(A, l, mid, aux);
    mergesort_rec(A, mid + 1, r, aux);
    merge(A, l, mid, r, aux);
}

void mergesort(int* A, int n) {
    std::vector<int> aux(n);
    mergesort_rec(A, 0, n - 1, aux.data());
}
