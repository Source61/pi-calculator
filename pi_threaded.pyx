from libc.stdio cimport printf
from libcpp cimport bool
from libc.stdlib cimport calloc, free
from libc.string cimport memset
from openmp cimport *
from cython.parallel cimport prange
from cython.operator cimport dereference as deref
import os, time

cdef:
  double pi = 1.0

  unsigned short cores = os.cpu_count()
  #unsigned short cores = max(int(os.cpu_count()/2), 1)
  unsigned short corenum
  omp_lock_t lock
  
  unsigned long long counter = 0
  unsigned long long dest = int(1E9) * cores
  unsigned long long destSplit = int(dest / cores)
  unsigned long long vectorCacheSize = int(1E8)
  
  void calcPiArray(double* array, unsigned long long size) noexcept nogil:
    global pi, counter
    cdef:
      double d
      double local_pi = 0.0
      unsigned long long n
    #for d in deref(array)[:size]:
    for n in range(size):
      d = array[n]
      local_pi += d
    omp_set_lock(&lock)
    pi -= local_pi
    counter += size
    omp_unset_lock(&lock)
    memset(&array, 0, sizeof(double)*size)

  double calcPi(unsigned short coreid) nogil:
    cdef:
      unsigned long long n
      double value
      bool lastAddition = (coreid*destSplit) % 2 == 0
      unsigned long long start = 1 + (coreid*destSplit)
      unsigned long long end = (coreid*destSplit) + destSplit + 1
      double* cachedValues = <double*>calloc(vectorCacheSize+1, sizeof(double))
      unsigned long long cachedValueIndex = 0

    printf("Coreid %llu, start %llu, end %llu\n", coreid, start, end)

    for n in range(start, end):
      value = 1 / (n * 2 + 1)
      cachedValues[cachedValueIndex] = value if lastAddition else -value
      #pi -= value if lastAddition else -value
      #printf("coreid %d, value %f, lastAddition %d, pi %.20f\n", coreid, value, lastAddition, pi)
      lastAddition = not lastAddition
      cachedValueIndex += 1
      if cachedValueIndex >= vectorCacheSize:
        calcPiArray(cachedValues, cachedValueIndex)
        cachedValueIndex = 0

    if cachedValueIndex >= 0:
      calcPiArray(cachedValues, cachedValueIndex)

    #printf("Coreid %d done: start %d, end %d, counter %d, pi %f\n", coreid, start, end, counter, pi)
    free(cachedValues)

omp_init_lock(&lock)

start = time.time()
for corenum in prange(cores, nogil=True):
  calcPi(corenum)
stop = time.time()

print("Execution time: %fs. Pi: %.20f. counter: %d" % ((stop-start), pi*4, counter))
