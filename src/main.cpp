#include "Data.h"
#include "Solution.h"
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <ratio>
#include <vector>

using namespace std;
Data *d;
Solution *ILS(int maxIter, int maxIterIls) {
  Solution *bestOfAll = new Solution();
  Solution *best, *s;
  bestOfAll->setCost(INFINITY);

  for (int i = 0; i < maxIter; i++) {
    s = new Solution(true); // new Solution(true') is equivalent to
    best = s;

    int iterIls = 0;
    while (iterIls <= maxIterIls) {
      s->localSearch();
      if (s->getCost() < best->getCost()) {
        best = s;
        iterIls = 0;
      } else if (s != best) {
        delete s;
      }
      s = Solution::disturbance(best);
      iterIls++;
    }

    if (s != best) {
      delete s;
    }

    if (best->getCost() < bestOfAll->getCost()) {
      bestOfAll = best;
    } else {
      delete best;
    }
  }
  return bestOfAll;
}

int main(int argc, char **argv) {
  srand(time(0));
  d = new Data(argc, argv[1]);
  d->read();
  size_t n = d->getDimension();

  int maxIterIls = n;

  if (n > 150) {
    maxIterIls /= 2;
  }

  using std::chrono::duration;
  using std::chrono::duration_cast;
  using std::chrono::high_resolution_clock;
  using std::chrono::milliseconds;

  auto start = std::chrono::high_resolution_clock::now();
  Solution *s = ILS(50, maxIterIls);
  auto end = std::chrono::high_resolution_clock::now();
  duration<double, std::milli> duration_ = end - start;

  cout << "Solução: ";
  s->show();

  cout << "Custo  : " << s->getCost() << endl;
  cout << "Tempo  : " << duration_.count() / 1000 << " segundos" << endl;

  // cout << d->getInstanceName() << ';' << s->getCost() << ';'
  //      << duration_.count() / 1000 << endl;

  return 0;
}
