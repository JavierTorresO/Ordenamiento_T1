#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <vector>
#include <numeric>

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
    if (!ok && n < 20) {
        printf("  Expected: "); for(int x : expected) printf("%d ", x); printf("\n");
        printf("  Got:      "); for(int x : copy) printf("%d ", x); printf("\n");
    }
}

// Wrapper para algoritmos que necesitan k o parámetros adicionales
void mergesort_par_wrapper(int* A, int n) { mergesort_parallel(A, n); }
void kway_wrapper(int* A, int n) { kway_mergesort(A, n, 4); }
void kway_par_wrapper(int* A, int n) { kway_mergesort_parallel(A, n, 4); }
void kway_comp_wrapper(int* A, int n) { kway_mergesort_parallel_full(A, n, 4); }

int main() {
    struct TestSuite {
        const char* name;
        void (*func)(int*, int);
    } suites[] = {
        {"Mergesort Sec", mergesort},
        {"Mergesort Par", mergesort_par_wrapper},
        {"K-Way Sec (k=4)", kway_wrapper},
        {"K-Way Par (k=4)", kway_par_wrapper},
        {"K-Way Comp (k=4)", kway_comp_wrapper}
    };

    int sizes[] = {0, 1, 2, 10, 15, 100, 1000};
    
    for (auto suite : suites) {
        printf("Testing %s...\n", suite.name);
        for (int n : sizes) {
            // Caso Aleatorio
            int* data = new int[n];
            for(int i=0; i<n; i++) data[i] = rand() % 100;
            run_test("Random", suite.func, data, n);

            // Caso Invertido
            for(int i=0; i<n; i++) data[i] = n - i;
            run_test("Reverse", suite.func, data, n);

            // Caso Iguales
            for(int i=0; i<n; i++) data[i] = 42;
            run_test("Identical", suite.func, data, n);

            delete[] data;
        }
        printf("\n");
    }

    return 0;
}
