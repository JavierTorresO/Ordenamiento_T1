#ifndef MERGESORT_PARALELO_H
#define MERGESORT_PARALELO_H

void mergesort_parallel(int* A, int n, int granularity = 2048);
void kway_mergesort_parallel(int* A, int n, int k, int granularity = 2048);
void kway_mergesort_parallel_full(int* A, int n, int k, int granularity = 2048);

#endif
