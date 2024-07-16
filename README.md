= Benchmarks in order of best to worst performance =
* Pure C++ (updated) with atomic but using same strategy as in previous Cython code (pi_threaded.pyx) avoiding incrementing pi every iteration: 0.23s for 1E8 * 16 rounds
* Pytorch using Python/Cython on the GPU. This is about as fast as above, but In fact is faster if your CPU has less than ~16 cores (underclocked i9-9900k @ 3.5GHz vs GTX 3080; i9-9900k wins): 0.39s for 1E8 * 16 rounds
* Pytorch on the GPU using C++ (unfinished). Looks kinda promising, but it's possible the most expensive operations are yet to come: 0.028s for 1E8 * 16 rounds
* Cython (or C++ doesn't matter) for 1E8 * 8 rounds using 1 core: 0.8s
* Cython (or C++ doesn't matter) with C arrays to reduce the use of locks for 1E8 * 8 rounds using 8 cores (might be faster with more cores): 1.0s
* Pure C++ with atomic for 1E6 * 8 rounds updating std::atomic value every iteration using 8 cores: 0.6s (NB: 1E6 * 8 rounds, 100x less than other benchmarks)

= Conclusions =

As should be obvious the languages C++/C and Cython are equal in performance, but each code snippet here defines my journey through optimizing a pi cruncher/calculator.
* Pi.pyx is my first attempt, using only a single core
* Pi_threaded.pyx is my second attempt, this time multicore
* Cpp.cpp was my interest in testing out std::atomic and where I had my final breakthrough into fully optimizing the code. Pi_threaded can be optimized the same way and achieve the same result, to be clear.
* pytorch.pyx: mainly Python code running on the GPU - slightly slower than on a good CPU with many cores
* torch/test.cpp: upcoming champion? Code is unfinished, it might be as fast, faster, or somehow slower than pytorch.pyx

= To compile =

* g++ cpp.cpp -std=c++23 -O2

= Notes =

I also learned compiling Torch code on C++ for this project. Unless I've missed something it's unlikely that Torch will actually be any faster than this code even if it runs on the GPU according to my benchmarks, even if it's written in C++, though I could be wrong.
Hmmm, sort of confirmed. It's slightly slower than on the CPU if you have a good CPU (16 cores or more).
