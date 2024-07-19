= Benchmarks in order of best to worst performance =
* Pure C++ (updated) with atomic but using same strategy as in previous Cython code (pi_threaded.pyx) avoiding incrementing pi every iteration: 0.23s for 1E8 * 16 rounds after being fully optimized
* Pytorch on the GPU using C++. This is almost exactly as fast as above, but in fact is faster if your CPU has less than ~16 cores (underclocked i9-9900k @ 3.5GHz vs GTX 3080; i9-9900k/above results wins): 0.24s for 1E8 * 16 rounds after being fully optimized
* Pytorch using mostly Python on the GPU: 0.30s for 1E8 * 16 rounds after being fully optimized
* Cython (or C++ doesn't matter) for 1E8 * 8 rounds using 1 core: 1.09s
* Cython (or C++ doesn't matter) with C arrays to reduce the use of locks for 1E8 * 16 rounds using 16 cores: 3.2s
* Pure C++ using GMP on CPU using multithreading for 1E6 * 16 rounds: 0.4s (NB: 1E6 * 16 rounds, 100x less than the other benchmarks above)
* Pure C++ with atomic for 1E6 * 8 rounds updating std::atomic value every iteration using 8 cores: 0.6s (NB: 1E6 * 8 rounds, 100x less than other benchmarks above)
* GMP implementation in Python (allows for arbitrary float precision) on single core: 1.6s (NB: 1E6 * 8 rounds as well, 100x less than the previous benchmarks above)

= Conclusions =

As should be obvious the languages C++/C and Cython are equal in performance, but each code snippet here defines my journey through optimizing a pi cruncher/calculator.
* Pi.pyx is my first attempt, using only a single core
* Pi_threaded.pyx is my second attempt, this time multicore
* Cpp.cpp was my interest in testing out std::atomic and where I had my final breakthrough into fully optimizing the code (NOT thanks to std::atomic). Pi_threaded can be optimized the same way and achieve the same result, to be clear.
* pytorch.pyx: mainly Python code running on the GPU - slightly slower than on a good CPU with many cores
* torch/test.cpp: upcoming champion? Code is unfinished, it might be as fast, faster, or somehow slower than pytorch.pyx
* Added/tried GMP implementation in Python (slow, but arbitrary float precision!)
* Added/tried GMP implementation in C++ (slow, but quite faster than Python + multithreaded)

= To compile CPU multithreaded without GMP =

* g++ cpp.cpp -std=c++23 -O2

= Notes =

I also learned compiling Torch code on C++ for this project. Unless I've missed something it's unlikely that Torch will actually be any faster than this code even if it runs on the GPU according to my benchmarks, even if it's written in C++, though I could be wrong.
Hmmm, sort of confirmed. It's slightly slower than on the CPU if you have a good CPU (16 cores or more), but not by much.
