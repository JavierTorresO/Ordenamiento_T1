#include "merge_paralelo.h"

#include <algorithm>
#include <utility>
#include <vector>

#include <omp.h>

namespace {

void sequential_merge_ranges(const int* left, int n1, const int* right, int n2, int* out) {
    int i = 0;
    int j = 0;
    int k = 0;

    while (i < n1 && j < n2) {
        if (left[i] <= right[j]) {
            out[k++] = left[i++];
        } else {
            out[k++] = right[j++];
        }
    }

    while (i < n1) {
        out[k++] = left[i++];
    }

    while (j < n2) {
        out[k++] = right[j++];
    }
}

std::pair<int, int> merge_path_partition(const int* left, int n1, const int* right, int n2, int diagonal) {
    int low = std::max(0, diagonal - n2);
    int high = std::min(diagonal, n1);

    while (low <= high) {
        const int i = (low + high) / 2;
        const int j = diagonal - i;

        const bool left_ok = (i == 0) || (j == n2) || (left[i - 1] <= right[j]);
        const bool right_ok = (j == 0) || (i == n1) || (right[j - 1] < left[i]);

        if (left_ok && right_ok) {
            return {i, j};
        }

        if (!left_ok) {
            high = i - 1;
        } else {
            low = i + 1;
        }
    }

    return {low, diagonal - low};
}

void parallel_copy_internal(int* dst, const int* src, int total, int granularity) {
    if (total <= granularity) {
        for (int i = 0; i < total; ++i) {
            dst[i] = src[i];
        }
        return;
    }

    const int team_size = std::max(1, omp_get_num_threads());
    const int partitions = std::max(1, std::min(team_size * 4, (total + granularity - 1) / granularity));

    if (partitions == 1) {
        for (int i = 0; i < total; ++i) {
            dst[i] = src[i];
        }
        return;
    }

    #pragma omp taskgroup
    for (int part = 0; part < partitions; ++part) {
        const int begin = (part * total) / partitions;
        const int end = ((part + 1) * total) / partitions;

        #pragma omp task firstprivate(begin, end) shared(dst, src)
        for (int i = begin; i < end; ++i) {
            dst[i] = src[i];
        }
    }
}

void parallel_merge_ranges_internal(const int* left, int n1, const int* right, int n2, int* out, int granularity) {
    const int total = n1 + n2;
    if (total <= granularity) {
        sequential_merge_ranges(left, n1, right, n2, out);
        return;
    }

    const int team_size = std::max(1, omp_get_num_threads());
    const int partitions = std::max(1, std::min(team_size * 4, (total + granularity - 1) / granularity));

    if (partitions == 1) {
        sequential_merge_ranges(left, n1, right, n2, out);
        return;
    }

    std::vector<int> left_limits(partitions + 1);
    std::vector<int> right_limits(partitions + 1);

    for (int part = 0; part <= partitions; ++part) {
        const int diagonal = (part * total) / partitions;
        const auto cut = merge_path_partition(left, n1, right, n2, diagonal);
        left_limits[part] = cut.first;
        right_limits[part] = cut.second;
    }

    #pragma omp taskgroup
    for (int part = 0; part < partitions; ++part) {
        const int left_begin = left_limits[part];
        const int left_end = left_limits[part + 1];
        const int right_begin = right_limits[part];
        const int right_end = right_limits[part + 1];
        const int out_begin = (part * total) / partitions;

        #pragma omp task firstprivate(left_begin, left_end, right_begin, right_end, out_begin) shared(left, right, out)
        sequential_merge_ranges(
            left + left_begin,
            left_end - left_begin,
            right + right_begin,
            right_end - right_begin,
            out + out_begin
        );
    }
}

}  // namespace

void parallel_merge(int* A, int l, int mid, int r, int granularity) {
    if (l >= r) {
        return;
    }

    const int n1 = mid - l + 1;
    const int n2 = r - mid;
    const int total = n1 + n2;

    if (total <= granularity) {
        int i = 0;
        int j = 0;
        int k = l;

        std::vector<int> left(n1);
        std::vector<int> right(n2);

        for (int idx = 0; idx < n1; ++idx) {
            left[idx] = A[l + idx];
        }
        for (int idx = 0; idx < n2; ++idx) {
            right[idx] = A[mid + 1 + idx];
        }

        while (i < n1 && j < n2) {
            if (left[i] <= right[j]) {
                A[k++] = left[i++];
            } else {
                A[k++] = right[j++];
            }
        }

        while (i < n1) {
            A[k++] = left[i++];
        }

        while (j < n2) {
            A[k++] = right[j++];
        }

        return;
    }

    std::vector<int> left(n1);
    std::vector<int> right(n2);
    std::vector<int> merged(total);

    for (int idx = 0; idx < n1; ++idx) {
        left[idx] = A[l + idx];
    }
    for (int idx = 0; idx < n2; ++idx) {
        right[idx] = A[mid + 1 + idx];
    }

    if (omp_in_parallel()) {
        parallel_merge_ranges_internal(left.data(), n1, right.data(), n2, merged.data(), granularity);
        parallel_copy_internal(A + l, merged.data(), total, granularity);
        return;
    }

    #pragma omp parallel
    {
        #pragma omp single
        {
            parallel_merge_ranges_internal(left.data(), n1, right.data(), n2, merged.data(), granularity);
            parallel_copy_internal(A + l, merged.data(), total, granularity);
        }
    }
}
