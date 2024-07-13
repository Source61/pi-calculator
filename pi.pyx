from libcpp cimport bool
import time

cdef:
	double pi = 1.0
	unsigned int divisor
	bool lastAddition = True
	unsigned long long n
	double value

start = time.time()
for n in range(1, (int(1E8)*8)+1):
	value = 1 / (n * 2 + 1)
	pi -= value if lastAddition else -value
	lastAddition = not lastAddition
stop = time.time()

print("Execution time: %fs. Pi: %.20f. Counter: %d" % ((stop-start), pi*4, n))
