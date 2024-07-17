#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <torch/torch.h>

torch::TensorOptions options = torch::TensorOptions().dtype(torch::kDouble);

double microtime() {
  return (double(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count()) / double(1000000));
}

int main() {
  double tstart = microtime();
  uint_fast64_t memSize = 1E8 * 3;
  uint_fast64_t totalSize = 1E8 * 16;

  uint_fast64_t rounds = ceil(totalSize / memSize);
  uint_fast64_t i;
  bool lastAddition = false;
  double local_pi = 0.0;
  bool shouldSwitch = memSize % 2 == 1;
	bool theSwitch = true;
  uint_fast8_t startIndex;
  double pi = 0.0;

  if (torch::cuda::is_available())
    options = options.device(torch::kCUDA, 0);

  torch::Tensor t1 = torch::ones(memSize, options);
  torch::Tensor t2;
  torch::Tensor t3;
  for(i=0; i<rounds; i++) {
    t2 = torch::arange((i*memSize*2)+1, ((i+1)*memSize*2)+1, 2, options);
    if (shouldSwitch) {
      if (theSwitch)
        startIndex = 1;
      else
        startIndex = 0;
      theSwitch = !theSwitch;
    }
    else
      startIndex = 1;
    t2.index({torch::indexing::Slice(startIndex, torch::indexing::None, 2)}) = torch::negative(t2.index({torch::indexing::Slice(startIndex, torch::indexing::None, 2)}));
    t3 = torch::sum(t1.div(t2));
    pi += (t3*4).item<double>();
	}
  double tstop = microtime();
  std::cout << "t2[-1]: " << t2[-1].item<double>() << std::endl;
  printf("Execution time: %fs. Samples: %llu (%llu rounds, memSize %llu). Pi: %.40f\n", tstop-tstart, totalSize, i, memSize, pi);
}
