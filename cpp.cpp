#include <iostream>
#include <vector>
#include <atomic>
#include <chrono>
#include <thread>
#include <vector>

std::atomic<double> pi = 1.0;
std::atomic<unsigned long long> counter = 0;
unsigned long long splitRounds = 0;
unsigned long long checkTotalRounds = 0;
double checkTimeout = 0.0;

double microtime() {
  return (double(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count()) / double(1000000));
}

void _calcPi(unsigned short coreid) {
  unsigned long long n;
  bool lastAddition = (coreid*splitRounds) % 2 == 0;
  unsigned long long start = 1 + (coreid*splitRounds);
  unsigned long long end = (coreid*splitRounds) + splitRounds;
  double local_pi = 0.0;

  double tstart = microtime();
  for (unsigned long long n=start; n<=end; n++) {
    double value = 1.0 / (n * 2 + 1); // this is basically the only cost; 0.23s for 1E9 entries
    if (lastAddition)
      local_pi -= value;
    else
      local_pi += value;
    lastAddition = !lastAddition;
  }

  pi += local_pi;
  counter += end-start+1;
  double tstop = microtime();

  printf("Coreid %d saying hi :3 Start %llu, end %llu, pi %f, execution time %fs\n", coreid, start, end, pi.load(), tstop-tstart);
}

double calcPi(unsigned short cores, unsigned long long totalRounds=0, double timeout=0.0) {
  std::vector<std::thread> threads(cores);
  if ((totalRounds == 0 || totalRounds*cores == 0) && !timeout) {
    printf("Either totalRounds or timeout must be specified or else the program runs forever\n");
    exit(1);
  }
  if (totalRounds) {
    splitRounds = checkTotalRounds = totalRounds;
    checkTotalRounds *= cores;
  }
  if (timeout)
    checkTimeout = microtime() + timeout;
  for (unsigned short coreid=0; coreid<cores; coreid++) {
    threads[coreid] = std::thread(_calcPi, coreid);
  }
  for (std::thread& t : threads)
    t.join();
  return pi;
}

int main() {
	double start, stop;

	start = microtime();
	calcPi(16, (unsigned long long)1E8);
	stop = microtime();
	
	printf("Execution time: %fs. Counter: %llu. Pi: %.20f\n", stop-start, counter.load(), pi.load()*4);
}
