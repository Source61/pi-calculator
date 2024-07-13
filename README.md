= Benchmarks in order of best to worst performance =
* Cython (or C++ doesn't matter) for 1E8 * 8 rounds using 1 core: 0.8s
* Cython (or C++ doesn't matter) with C arrays to reduce the use of locks for 1E8 * 8 rounds using 8 cores (might be faster with more cores): 1.0s
* Pure C++ with atomic for 1E6 * 8 rounds using 8 cores: 0.6s
