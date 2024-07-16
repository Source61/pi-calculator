#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <torch/torch.h>

// UNFINISHED

torch::TensorOptions options = torch::TensorOptions().dtype(torch::kDouble).device(torch::kCUDA, 0);

double microtime() {
  return (double(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count()) / double(1000000));
}

void oldTestCode() {
  bool lastAddition = false;
	std::vector<double> v;
	v.reserve(1E9);
  //v.resize(1E9);
	for(unsigned long long i=0; i<1E9; i++) {
		if (lastAddition)
			v[i] = -i; // 0.7s
		else
			v[i] = i;
	}
  printf("v.capacity(): %d\n", v.capacity());
	double start = microtime();
	torch::Tensor tensor = torch::from_blob((double*)(v.data()), v.capacity());
	torch::Tensor res = torch::sum(tensor);
	double stop = microtime();
	std::cout << res << std::endl;
	printf("%f\n", stop-start);
}

void newTestCode() {
  unsigned int memSize = 1E8;
  unsigned long long totalSize = 1E8 * 16;

  unsigned long long rounds = ceil(totalSize / memSize);
  unsigned long long i;
  bool lastAddition = false;
  double* v1 = (double*)calloc(1, sizeof(double));
  double* v2 = (double*)calloc(1, sizeof(double));
  double local_pi = 0.0;

  //torch::Tensor t1 = torch::from_blob((double*)(v1), 1).clone().to(torch::kCUDA);
  //torch::Tensor t2 = torch::from_blob((double*)(v2), 1).clone().to(torch::kCUDA);
  torch::Tensor t1 = torch::ones({memSize}, options);
  torch::Tensor t2 = torch::empty({memSize}, options);
  torch::Tensor t3 = torch::empty({memSize}, options);
  double start = microtime();
	for(i=0; i<rounds; i++) {
    t1 = torch::ones({memSize}, options);
    t2 = torch::arange(memSize, options).mul(2.0).add(1);
    // Todo: make t2[::2] = 0 - t2[::2]
    t3 = torch::sum(t1.div(t2));
    //t1.add(t2);
    //t1[0] = t2[0] = 1.0;
    //torch::add(t1, t2);
    // double value = 1.0 / (i * 2.0 + 1.0);
		//if (lastAddition)
    //  tensor[0] = (double)-i;
		//else
    //  tensor[0] = (double)i;
	}
  double stop = microtime();
  printf("Execution time: %fs. Rounds: %d.\n", stop-start, i);
  std::cout << "T1: " << t1[10].item() << ", T2: " << t2[10].item() << ", T3: " << t3.item() << std::endl;
  free(v1);
  free(v2);
}

int main() {
  //oldTestCode();
  newTestCode();
}
