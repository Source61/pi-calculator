import time
import gmpy2

start = time.time()

cdef:
  unsigned long long rounds = int(1E6)
  unsigned long long precision = 256

pi = gmpy2.mpfr(0, precision=precision)
cdef bint lastAddition = False
d = gmpy2.mpfr(0, precision=precision)
v1 = gmpy2.mpfr(1, precision=precision)
for n in range(rounds):
  v2 = gmpy2.mpfr(-(n * 2 + 1) if lastAddition else (n * 2 + 1), precision=precision)
  d = gmpy2.div(v1, v2)
  lastAddition = not lastAddition
  pi = gmpy2.add(pi, d)
stop = time.time()
pi = gmpy2.mul(pi, 4)

print("Execution time: %fs. Pi: %.20f" % ((stop-start), pi))

print(pi.digits(10, precision), pi.precision)
print(gmpy2.const_pi(precision=precision))
