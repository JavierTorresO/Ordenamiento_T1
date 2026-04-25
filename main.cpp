#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <vector>

#include <omp.h>

#include "algoritmos/kway.h"
#include "algoritmos/mergesort.h"
#include "paralelismo/merge_paralelo.h"
#include "paralelismo/mergesort_paralelo.h"
#include "utils/utils.h"

struct Metric {
    int threads;
    double time;
    double speedup;
    double efficiency;
    bool ok;
};

template <typename Prepare, typename Run, typename Check>
std::vector<Metric> benchmark_threads(
    const std::vector<int>& base_data,
    const std::vector<int>& thread_values,
    int repetitions,
    Prepare prepare_input,
    Run run_algorithm,
    Check check_output
) {
    std::vector<Metric> results;
    results.reserve(thread_values.size());

    double baseline = 0.0;

    for (int threads : thread_values) {
        omp_set_num_threads(threads);
        double total_elapsed = 0.0;
        bool ok = true;

        for (int rep = 0; rep < repetitions; ++rep) {
            std::vector<int> input = prepare_input(base_data);

            const auto start = std::chrono::high_resolution_clock::now();
            run_algorithm(input.data(), static_cast<int>(input.size()));
            const auto end = std::chrono::high_resolution_clock::now();

            total_elapsed += std::chrono::duration<double>(end - start).count();
            ok = ok && check_output(input);
        }

        const double elapsed = total_elapsed / repetitions;

        if (threads == 1) {
            baseline = elapsed;
        }

        results.push_back({
            threads,
            elapsed,
            baseline > 0.0 ? baseline / elapsed : 0.0,
            (baseline > 0.0 && threads > 0) ? (baseline / elapsed) / threads : 0.0,
            ok
        });
    }

    return results;
}

void print_metrics(const char* title, const std::vector<Metric>& metrics) {
    std::printf("%s\n", title);
    std::printf("p\tTiempo(s)\tSpeedup\t\tEficiencia\tEstado\n");

    for (const Metric& metric : metrics) {
        std::printf(
            "%d\t%.6f\t%.4f\t\t%.4f\t\t%s\n",
            metric.threads,
            metric.time,
            metric.speedup,
            metric.efficiency,
            metric.ok ? "OK" : "ERROR"
        );
    }

    std::printf("\n");
}

int main(int argc, char** argv) {
    const int n = argc > 1 ? std::atoi(argv[1]) : 200000;
    const int k = argc > 2 ? std::atoi(argv[2]) : 4;
    const int granularity = argc > 3 ? std::atoi(argv[3]) : 4096;
    const int repetitions = argc > 4 ? std::atoi(argv[4]) : 3;

    omp_set_dynamic(0);
    std::srand(42);

    std::vector<int> base_data(n);
    generate_array(base_data.data(), n);

    std::printf("n = %d, k = %d, granularidad = %d, repeticiones = %d\n\n", n, k, granularity, repetitions);

    std::vector<int> serial_merge = base_data;
    const double serial_mergesort_time = measure_time(mergesort, serial_merge.data(), n);
    std::printf(
        "Mergesort secuencial: %.6f s (%s)\n",
        serial_mergesort_time,
        is_sorted(serial_merge.data(), n) ? "OK" : "ERROR"
    );

    std::vector<int> serial_kway = base_data;
    const auto serial_kway_start = std::chrono::high_resolution_clock::now();
    kway_mergesort(serial_kway.data(), n, k);
    const auto serial_kway_end = std::chrono::high_resolution_clock::now();
    const double serial_kway_time = std::chrono::duration<double>(serial_kway_end - serial_kway_start).count();
    std::printf(
        "K-way secuencial: %.6f s (%s)\n\n",
        serial_kway_time,
        is_sorted(serial_kway.data(), n) ? "OK" : "ERROR"
    );

    const std::vector<int> thread_values = {1, 2, 4, 8};

    auto sorted_check = [](const std::vector<int>& data) {
        return is_sorted(data.data(), static_cast<int>(data.size()));
    };

    const auto merge_prepare = [n](const std::vector<int>& data) {
        std::vector<int> merged_input = data;
        const int mid = n / 2;
        std::sort(merged_input.begin(), merged_input.begin() + mid);
        std::sort(merged_input.begin() + mid, merged_input.end());
        return merged_input;
    };

    const auto merge_results = benchmark_threads(
        base_data,
        thread_values,
        repetitions,
        merge_prepare,
        [n, granularity](int* data, int /*size*/) {
            const int mid = (n / 2) - 1;
            parallel_merge(data, 0, mid, n - 1, granularity);
        },
        sorted_check
    );

    const auto mergesort_results = benchmark_threads(
        base_data,
        thread_values,
        repetitions,
        [](const std::vector<int>& data) { return data; },
        [granularity](int* data, int size) {
            mergesort_parallel(data, size, granularity);
        },
        sorted_check
    );

    const auto kway_results = benchmark_threads(
        base_data,
        thread_values,
        repetitions,
        [](const std::vector<int>& data) { return data; },
        [k, granularity](int* data, int size) {
            kway_mergesort_parallel(data, size, k, granularity);
        },
        sorted_check
    );

    const auto full_results = benchmark_threads(
        base_data,
        thread_values,
        repetitions,
        [](const std::vector<int>& data) { return data; },
        [k, granularity](int* data, int size) {
            kway_mergesort_parallel_full(data, size, k, granularity);
        },
        sorted_check
    );

    print_metrics("Merge paralelo por ranks", merge_results);
    print_metrics("Mergesort paralelo", mergesort_results);
    print_metrics("K-way mergesort paralelo", kway_results);
    print_metrics("Version paralela completa", full_results);

    return 0;
}
