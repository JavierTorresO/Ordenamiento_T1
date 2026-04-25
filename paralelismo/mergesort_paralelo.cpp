#include "mergesort_paralelo.h"

#include <algorithm>
#include <vector>

#include <omp.h>

#include "paralelismo/merge_paralelo.h"
#include "utils/utils.h"

namespace {

struct Block {
    int l;
    int r;
};

void mergesort_serial_range(int* A, int l, int r) {
    if (l >= r) {
        return;
    }

    const int mid = l + (r - l) / 2;
    mergesort_serial_range(A, l, mid);
    mergesort_serial_range(A, mid + 1, r);
    merge(A, l, mid, r);
}

void kway_serial_range(int* A, int l, int r, int k) {
    if (l >= r) {
        return;
    }

    const int n = r - l + 1;
    const int parts = std::max(2, k);
    const int base = n / parts;
    const int remainder = n % parts;

    std::vector<Block> blocks(parts);
    int current = l;

    for (int i = 0; i < parts; ++i) {
        const int size = base + (i < remainder ? 1 : 0);
        blocks[i] = {current, current + size - 1};
        current += size;
    }

    for (const Block& block : blocks) {
        if (block.l <= block.r) {
            kway_serial_range(A, block.l, block.r, parts);
        }
    }

    std::vector<Block> current_blocks;
    current_blocks.reserve(blocks.size());
    for (const Block& block : blocks) {
        if (block.l <= block.r) {
            current_blocks.push_back(block);
        }
    }

    while (current_blocks.size() > 1) {
        std::vector<Block> next_blocks;
        next_blocks.reserve((current_blocks.size() + 1) / 2);

        for (std::size_t i = 0; i + 1 < current_blocks.size(); i += 2) {
            merge(A, current_blocks[i].l, current_blocks[i].r, current_blocks[i + 1].r);
            next_blocks.push_back({current_blocks[i].l, current_blocks[i + 1].r});
        }

        if (current_blocks.size() % 2 == 1) {
            next_blocks.push_back(current_blocks.back());
        }

        current_blocks.swap(next_blocks);
    }
}

std::vector<Block> split_blocks(int l, int r, int k) {
    const int n = r - l + 1;
    const int parts = std::max(2, std::min(k, n));
    const int base = n / parts;
    const int remainder = n % parts;

    std::vector<Block> blocks(parts);
    int current = l;

    for (int i = 0; i < parts; ++i) {
        const int size = base + (i < remainder ? 1 : 0);
        blocks[i] = {current, current + size - 1};
        current += size;
    }

    return blocks;
}

void merge_rounds(int* A, const std::vector<Block>& blocks, int granularity, bool use_parallel_merge) {
    std::vector<Block> current_blocks;
    current_blocks.reserve(blocks.size());

    for (const Block& block : blocks) {
        if (block.l <= block.r) {
            current_blocks.push_back(block);
        }
    }

    while (current_blocks.size() > 1) {
        const std::size_t pair_count = current_blocks.size() / 2;
        std::vector<Block> next_blocks((current_blocks.size() + 1) / 2);

        for (std::size_t i = 0; i < pair_count; ++i) {
            next_blocks[i] = {current_blocks[2 * i].l, current_blocks[2 * i + 1].r};
        }

        if (current_blocks.size() % 2 == 1) {
            next_blocks.back() = current_blocks.back();
        }

        #pragma omp taskgroup
        for (std::size_t i = 0; i < pair_count; ++i) {
            const Block left = current_blocks[2 * i];
            const Block right = current_blocks[2 * i + 1];
            const int merged_size = right.r - left.l + 1;

            #pragma omp task firstprivate(left, right, merged_size, granularity, use_parallel_merge) shared(A) if(merged_size > granularity)
            {
                if (use_parallel_merge) {
                    parallel_merge(A, left.l, left.r, right.r, granularity);
                } else {
                    merge(A, left.l, left.r, right.r);
                }
            }
        }

        current_blocks.swap(next_blocks);
    }
}

void mergesort_parallel_rec(int* A, int l, int r, int granularity) {
    if (l >= r) {
        return;
    }

    const int n = r - l + 1;
    if (n <= granularity) {
        mergesort_serial_range(A, l, r);
        return;
    }

    const int mid = l + (r - l) / 2;

    #pragma omp task shared(A) firstprivate(l, mid, granularity) if(mid - l + 1 > granularity)
    mergesort_parallel_rec(A, l, mid, granularity);

    #pragma omp task shared(A) firstprivate(mid, r, granularity) if(r - mid > granularity)
    mergesort_parallel_rec(A, mid + 1, r, granularity);

    #pragma omp taskwait
    merge(A, l, mid, r);
}

void kway_parallel_rec(int* A, int l, int r, int k, int granularity, bool use_parallel_merge) {
    if (l >= r) {
        return;
    }

    const int n = r - l + 1;
    if (n <= granularity) {
        kway_serial_range(A, l, r, k);
        return;
    }

    const std::vector<Block> blocks = split_blocks(l, r, k);

    #pragma omp taskgroup
    for (const Block& block : blocks) {
        if (block.l > block.r) {
            continue;
        }

        const int block_size = block.r - block.l + 1;

        #pragma omp task shared(A) firstprivate(block, k, granularity, use_parallel_merge, block_size) if(block_size > granularity)
        kway_parallel_rec(A, block.l, block.r, k, granularity, use_parallel_merge);
    }

    merge_rounds(A, blocks, granularity, use_parallel_merge);
}

}  // namespace

void mergesort_parallel(int* A, int n, int granularity) {
    if (n <= 1) {
        return;
    }

    #pragma omp parallel
    {
        #pragma omp single
        mergesort_parallel_rec(A, 0, n - 1, granularity);
    }
}

void kway_mergesort_parallel(int* A, int n, int k, int granularity) {
    if (n <= 1) {
        return;
    }

    #pragma omp parallel
    {
        #pragma omp single
        kway_parallel_rec(A, 0, n - 1, std::max(2, k), granularity, false);
    }
}

void kway_mergesort_parallel_full(int* A, int n, int k, int granularity) {
    if (n <= 1) {
        return;
    }

    #pragma omp parallel
    {
        #pragma omp single
        kway_parallel_rec(A, 0, n - 1, std::max(2, k), granularity, true);
    }
}
