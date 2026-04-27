#include "kway.h"
#include <vector>
#include <algorithm>
#include "utils/utils.h"

struct Block {
    int l, r;
};

void kway_rec(int* A, int l, int r, int k, int* aux) {
    if (l >= r) return;

    int n = r - l + 1;
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

    for (int i = 0; i < parts; i++) {
        if (blocks[i].l <= blocks[i].r) {
            kway_rec(A, blocks[i].l, blocks[i].r, k, aux);
        }
    }

    // MERGE POR TORNEO (Consistente con versión paralela)
    std::vector<Block> current_blocks;
    for (const auto& b : blocks) if (b.l <= b.r) current_blocks.push_back(b);

    while (current_blocks.size() > 1) {
        std::vector<Block> next_blocks;
        for (size_t i = 0; i + 1 < current_blocks.size(); i += 2) {
            merge(A, current_blocks[i].l, current_blocks[i].r, current_blocks[i+1].r, aux);
            next_blocks.push_back({current_blocks[i].l, current_blocks[i+1].r});
        }
        if (current_blocks.size() % 2 == 1) next_blocks.push_back(current_blocks.back());
        current_blocks = next_blocks;
    }
}

void kway_mergesort(int* A, int n, int k) {
    std::vector<int> aux(n);
    kway_rec(A, 0, n - 1, k, aux.data());
}
