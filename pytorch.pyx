import torch, time

dev = 'cuda' if torch.cuda.is_available() else 'cpu'
dtype = torch.float64

from libc.math cimport ceil

cdef:
  # Config
  unsigned long long memSize = int(1E8)*3
  unsigned long long totalSize = int(1E8)*16
	
  # Don't touch
  unsigned long long i
  double pi = 0
  bint shouldSwitch = memSize % 2 == 1
  bint switch = True
  bint indexStart

start = time.time()
for i in range(<unsigned long long>ceil(totalSize/memSize)):
  print(i, i*memSize, i*memSize*2+1, (i+2)*memSize+1)
  t1 = torch.ones(memSize, dtype=dtype, device=dev)
  t2 = torch.arange(i*memSize*2+1, (i+1)*memSize*2+1, 2, dtype=dtype, device=dev)
  if shouldSwitch:
    if switch:
      t2[1::2] = torch.neg(t2[1::2])
    else:
      t2[0::2] = torch.neg(t2[0::2])
    switch = not switch
  else:
    t2[1::2] = torch.neg(t2[1::2])
  t3 = t1.div(t2)
  t3 = torch.sum(t3)
  pi += t3.item()*4
stop = time.time()

print("Execution time: %fs" % ((stop-start,)))
print(pi)
