#include <omp.h>
#include <stdio.h>
#include <cstdlib>
#include <atomic>
#include <chrono>
#include <thread>
#include <vector>

double microtime() {
  return (double(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count()) / double(1000000));
}

std::atomic<double> pi = 1.0;
std::atomic<unsigned long long> counter = 0;
unsigned long long splitRounds = 0;
unsigned long long checkTotalRounds = 0;
double checkTimeout = 0.0;

void _calcPi(unsigned short coreid) {
  unsigned long long n;
  bool lastAddition = (coreid*splitRounds) % 2 == 0;
  unsigned long long start = 1 + (coreid*splitRounds);
  unsigned long long end = (coreid*splitRounds) + splitRounds;

  for (unsigned long long n=start; n<=end; n++) {
    double value = 1.0 / (n * 2.0 + 1.0);
    if (lastAddition)
      pi -= value;
    else
      pi += value;
    lastAddition = !lastAddition;
    counter++;
  }

  //printf("Coreid %d saying hi :3 Start %llu, end %llu, pi %f\n", coreid, start, end, pi.load());
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
  double start = microtime();
  calcPi(8, (unsigned long long)1E6);
  double stop = microtime();
  printf("Execution time: %fs. Counter %llu. Pi: %.20f\n", counter.load(), (stop-start), pi*4);
}
