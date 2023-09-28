#include "Data.h"
#include "MLPSolution.h"
#include "TSPSolution.h"
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <ratio>
#include <vector>

using namespace std;

TSPSolution *ILS_TSP(int maxIter, int maxIterIls) {
  TSPSolution *bestOfAll = new TSPSolution();
  TSPSolution *best, *s;
  bestOfAll->setCost(INFINITY);

  for (int i = 0; i < maxIter; i++) {
    s = new TSPSolution(true); // new TSPSolution(true') is equivalent to
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
      s = TSPSolution::disturbance(best);
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

MLPSolution *ILS_MLP(int maxIter, int maxIterIls) {
  MLPSolution *bestOfAll = new MLPSolution();
  MLPSolution *best, *s;
  bestOfAll->setCost(INFINITY);

  for (int i = 0; i < maxIter; i++) {
    double alpha = MLPSolution::randomAlpha();
    s = MLPSolution::build(alpha);
    best = s;

    int iterIls = 0;
    while (iterIls < maxIterIls) {
      s->localSearch();
      if (s->getCost() < best->getCost()) {
        best = s;
        iterIls = 0;
      } else if (s != best) {
        delete s;
      }
      s = MLPSolution::disturbance(best);
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

void tsp(int n) {
  int maxIterIls = n;

  if (n > 150) {
    maxIterIls /= 2;
  }

  using std::chrono::duration;
  using std::chrono::duration_cast;
  using std::chrono::high_resolution_clock;
  using std::chrono::milliseconds;

  auto start = std::chrono::high_resolution_clock::now();
  TSPSolution *s = ILS_TSP(50, maxIterIls);
  auto end = std::chrono::high_resolution_clock::now();
  duration<double, std::milli> duration_ = end - start;

  // cout << "Solução: ";
  // s->show();

  // cout << "Custo  : " << s->getCost() << endl;
  // s->calculateCost();
  // cout << "Custo recalculado  : " << s->getCost() << endl;
  // cout << "Tempo  : " << duration_.count() / 1000 << " segundos" << endl;

  cout << Data::instance->getInstanceName() << ';' << s->getCost() << ';'
       << duration_.count() / 1000 << endl;
}

void mlp(int n) {
  int maxIterIls = 100;

  if (n < 100) {
    maxIterIls = n;
  }

  using std::chrono::duration;
  using std::chrono::duration_cast;
  using std::chrono::high_resolution_clock;
  using std::chrono::milliseconds;

  auto start = std::chrono::high_resolution_clock::now();
  MLPSolution *s = ILS_MLP(10, maxIterIls);
  auto end = std::chrono::high_resolution_clock::now();
  duration<double, std::milli> duration_ = end - start;

  cout << Data::instance->getInstanceName() << ';' << s->getCost() << ';'
       << duration_.count() / 1000 << endl;
  // cout << "Custo  : " << s->getCost() << endl;
  // s->calculateCost();
  // cout << "Custo recalculado  : " << s->getCost() << endl;
  // cout << "Tempo  : " << duration_.count() / 1000 << " segundos" << endl;
}

int main(int argc, char **argv) {
  cout << fixed << std::setprecision(2);
  int seed = atoi(argv[2]);
  srand(seed);

  Data::create(argc, argv[1]);
  Data *d = Data::getInstance();
  d->read();

  size_t n = d->getDimension();

  int problem = atoi(argv[3]);

  switch (problem) {
  case 1:
    tsp(n);
    break;
  case 2:
    mlp(n);
    break;
  }

  // cout << d->getInstanceName() << ';' << s->getCost() << ';'
  //      << duration_.count() / 1000 << endl;

  return 0;
}
