#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <vector>
#include "utils/utils.h"
#include "algoritmos/mergesort.h"
#include "algoritmos/kway.h"
#include "paralelismo/mergesort_paralelo.h"

void run_test(const char* name, void (*sort_func)(int*, int), int* data, int n) {
    std::vector<int> copy(data, data + n);
    std::vector<int> expected(data, data + n);
    std::sort(expected.begin(), expected.end());

    sort_func(copy.data(), n);

    bool ok = true;
    for(int i = 0; i < n; i++) {
        if(copy[i] != expected[i]) {
            ok = false;
            break;
        }
    }
    printf("[%s] n=%d: %s\n", name, n, ok ? "PASS" : "FAIL");
}

int main() {
    int sizes[] = {0, 1, 10, 1000};
    for (int n : sizes) {
        std::vector<int> data(n);
        for(int i=0; i<n; i++) data[i] = rand() % 1000;

        run_test("Mergesort Sec", mergesort, data.data(), n);
        run_test("K-Way Sec (k=4)", [](int* A, int sz){ kway_mergesort(A, sz, 4); }, data.data(), n);
        run_test("Quicksort Par", [](int* A, int sz){ quicksort_parallel(A, sz); }, data.data(), n);
        run_test("Mergesort Par", [](int* A, int sz){ mergesort_parallel(A, sz); }, data.data(), n);
        run_test("Full Par (k=4)", [](int* A, int sz){ kway_mergesort_parallel_full(A, sz, 4); }, data.data(), n);
    }
    return 0;
}
