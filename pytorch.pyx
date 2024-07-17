import torch, time

dev = 'cuda' if torch.cuda.is_available() else 'cpu'
dtype = torch.double

from libc.math cimport ceil

cdef:
  # Config
  unsigned long long memSize = int(1E8)*3
  unsigned long long totalSize = int(1E10)*16
	
  # Don't touch
  unsigned long long i
  double pi = 0
  bint shouldSwitch = memSize % 2 == 1
  bint switch = True
  bint indexStart

start = time.time()
for i in range(<unsigned long long>ceil(totalSize/memSize)):
  print(i, i*memSize, (i+1)*memSize)
  t1 = torch.ones(memSize, dtype=dtype, device=dev)
  t2 = torch.arange(i*memSize, (i+1)*memSize, dtype=dtype, device=dev).mul(2).add(1)
  if shouldSwitch:
    if switch:
      t2[1::2] = 0 - t2[1::2]
    else:
      t2[0::2] = 0 - t2[0::2]
    switch = not switch
  else:
    t2[1::2] = 0 - t2[1::2]
  t3 = torch.sum(t1.div(t2))
  pi += t3.item()*4
stop = time.time()

print("Execution time: %fs" % ((stop-start,)))
print(pi)
