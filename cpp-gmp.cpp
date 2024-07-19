#include <iostream>
#include <vector>
#include <atomic>
#include <chrono>
#include <thread>
#include <vector>
#include <gmp.h>

mpf_t pi;
std::atomic<uint_fast64_t> counter = 0;
uint_fast64_t splitRounds = 0;
uint_fast64_t checkTotalRounds = 0;

double microtime() {
  return (double(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count()) / double(1000000));
}

void calcPi(unsigned short threadid) {
  uint_fast64_t n;
  bool lastAddition = (threadid*splitRounds) % 2 == 0;
  uint_fast64_t start = 1 + (threadid*splitRounds);
  uint_fast64_t end = (threadid*splitRounds) + splitRounds;
  mpf_t local_pi;
  mpf_t value;
  mpf_t divisor;
  mpf_t one;
  mpf_t nptr;

  mpf_init(local_pi);
  mpf_init(value);
  mpf_init(divisor);
  mpf_init2(one, 53);
  mpf_set_d(one, 1.0);
  mpf_init2(nptr, 53);

  for (unsigned long long n=start; n<=end; n++) {
    mpf_set_d(value, 1.0);
    mpf_set_d(divisor, 2.0);
    mpf_set_d(nptr, n);

    mpf_mul(divisor, divisor, nptr); // divisor = 2 * n 
    mpf_add(divisor, divisor, one); // divisor = 2 * n + 1
    mpf_div(value, value, divisor); // value = 1 / (2 * n + 1)

    if (lastAddition)
      mpf_sub(local_pi, local_pi, value);
    else
      mpf_add(local_pi, local_pi, value);
    lastAddition = !lastAddition;
  }

  mpf_add(pi, pi, local_pi);

  counter += end-start+1;
}

void run(unsigned short threadsCount, unsigned long long totalRounds) {
  std::vector<std::thread> threads(threadsCount);
  if ((totalRounds == 0 || threadsCount == 0)) {
    printf("Either totalRounds or timeout must be specified or else the program runs forever\n");
    exit(1);
  }
  if (totalRounds) {
    splitRounds = checkTotalRounds = totalRounds;
    checkTotalRounds *= threadsCount;
  }
  for (unsigned short threadid=0; threadid<threadsCount; threadid++) {
    threads[threadid] = std::thread(calcPi, threadid);
  }
  for (std::thread& t : threads)
    t.join();
}

int main() {
  mpf_set_default_prec(128);
  mpf_init_set_d(pi, 1.0);
  mpf_t four;
  mpf_init_set_d(four, 4);

	double start = microtime();
	run(16, 1E6);
  mpf_mul(pi, pi, four);
	double stop = microtime();
	
  mp_exp_t exp;
  std::string pi_str = mpf_get_str(NULL, &exp, 10, 500, pi);

  if (exp > 0) {
    if (exp < pi_str.length()) {
      pi_str.insert(exp, ".");
    } else {
      pi_str.append(exp - pi_str.length(), '0');
      pi_str.append(".0");
      }
  } else {
    pi_str.insert(0, "0.");
    pi_str.insert(2, -exp, '0');
  }

	printf("Execution time: %fs. Counter: %llu. Pi: %s\n", stop-start, counter.load(), pi_str.c_str());
}
