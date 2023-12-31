#include "TSPSolution.h"
#include "Data.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <list>
#include <numeric>
#include <random>
#include <type_traits>
#include <vector>

// extern Data *d;

Data *TSPSolution::reader = nullptr;

void TSPSolution::setReader(Data *r) { reader = r; }

bool validateCost(TSPSolution *s) {
  double cost = s->getCost();
  s->calculateCost();
  double recalculatedCost = s->getCost();
  if (recalculatedCost != cost)
    return false;
  if (!s->isHamiltonTour())
    return false;

  cout << "Sequence: ";
  s->show();
  cout << "recalc cost: " << recalculatedCost << endl;
  cout << "cost       : " << cost << endl;

  return true;
}

std::vector<InsertionCost> *calculateInsertionCosts(std::list<int> *s,
                                                    std::vector<int> *v) {
  int size = (s->size() - 1) * v->size();
  std::vector<InsertionCost> *costs = new std::vector<InsertionCost>(size);
  int l = 0;
  list<int>::iterator a = s->begin();
  list<int>::iterator b = next(a);
  for (; b != s->end(); a++, b++) {
    int i = *a;
    int j = *b;
    for (auto it = v->begin(); it < v->end(); it++) {
      InsertionCost cost;
      int k = *it;
      cost.node = k;
      cost.it = it;
      cost.edge = a;
      cost.cost = TSPSolution::reader->getDistance(i, k) +
                  TSPSolution::reader->getDistance(k, j) -
                  TSPSolution::reader->getDistance(i, j);
      (*costs)[l] = cost;
      l++;
    }
  }

  return costs;
}

TSPSolution::TSPSolution(bool build) {
  if (!build)
    return;

  this->sequence = std::list<int>();
  // array of possibles vertices
  // TSPSolution::reader->getDimension() - 1 is because element '1' already is
  // on solution
  std::vector<int> v(TSPSolution::reader->getDimension() - 1);
  std::iota(v.begin(), v.end(), 2);

  sequence.push_back(1);
  for (int i = 0; i < 3; i++) {
    int index = rand() % v.size();
    sequence.push_back(v[i]);
    v.erase(v.begin() + i);
  }
  sequence.push_back(1);

  // initialize random generator; Uses Normal Distribution

  while (!v.empty()) {
    std::vector<InsertionCost> *costs = calculateInsertionCosts(&sequence, &v);
    std::sort(costs->begin(), costs->end(),
              [](InsertionCost a, InsertionCost b) { return a.cost < b.cost; });

    // dis(gen) generates random number following normal distribuission
    double alpha = (double)rand() / RAND_MAX;
    int selectedIndex = rand() % ((int)ceil(alpha * costs->size()));
    InsertionCost selected = (*costs)[selectedIndex];

    sequence.insert(next(selected.edge), selected.node);
    v.erase(selected.it);
    delete costs;
  }

  this->calculateCost();
}

TSPSolution::TSPSolution(TSPSolution *s) {
  this->sequence = std::list<int>(*s->getSequence());
  this->cost = s->cost;
}

bool TSPSolution::isHamiltonTour() {
  int size = this->sequence.size();
  list<int>::iterator begin = sequence.begin();
  list<int>::iterator pback = prev(sequence.end());

  if (*begin != *pback)
    return false;

  vector<int> bucket(size, 0);

  for (auto it = begin; it != pback; it++) {
    bucket[*it]++;
    if (bucket[*it] > 1)
      return false;
  }

  return true;
}

double TSPSolution::getCost() { return this->cost; }
void TSPSolution::setCost(double c) { this->cost = c; }
void TSPSolution::setSequence(std::list<int> *v) { this->sequence = *v; }
std::list<int> *TSPSolution::getSequence() { return &this->sequence; }

void TSPSolution::calculateCost() {
  this->cost = 0;

  list<int>::iterator a = sequence.begin();
  list<int>::iterator b = next(a);
  for (; b != sequence.end(); a++, b++)
    this->cost += TSPSolution::reader->getDistance(*a, *b);
}

void TSPSolution::show() {
  list<int>::iterator back = prev(sequence.end());
  for (auto it = sequence.begin(); it != back; it++)
    std::cout << *it << " -> ";

  std::cout << this->sequence.back() << std::endl;
}

void TSPSolution::localSearch() {
  std::vector<int> NL = {1, 2, 3, 4, 5};
  bool improved = false;

  while (!NL.empty()) {
    int n = rand() % NL.size();
    switch (NL[n]) {
    case 1:
      improved = this->bestImprovementSwap();
      break;
    case 2:
      improved = this->bestImprovementOrOpt(1);
      break;
    case 3:
      improved = this->bestImprovementOrOpt(2);
      break;
    case 4:
      improved = this->bestImprovementOrOpt(3);
      break;
    case 5:
      improved = this->bestImprovement2Opt();
      break;
    }
    if (improved) {
      NL = {1, 2, 3, 4, 5};
    } else {
      NL.erase(NL.begin() + n);
    }
  };
}

TSPSolution *TSPSolution::disturbance(TSPSolution *s) {
  TSPSolution *solution = new TSPSolution(s); // copia a solução
  int dimension = s->sequence.size() - 1;

  int lowerBound = 2;
  int upperBound = std::ceil(static_cast<double>(dimension) / 10);

  int sizeI = rand() % (upperBound - lowerBound + 1) + lowerBound;
  int sizeJ = rand() % (upperBound - lowerBound + 1) + lowerBound;

  // [1, dimension / 2 - sizeI]
  int i = rand() % (dimension / 2 - sizeI - 1) + 1;
  // // [dimension / 2 + 1, seq.size - 1]
  int j = dimension / 2 + rand() % (dimension / 2 - sizeJ - 1) + 1;

  std::list<int> *seq = solution->getSequence();
  auto seqBegin = seq->begin();

  list<int>::iterator startI = next(seqBegin, i);
  list<int>::iterator endI = next(startI, sizeI);

  list<int>::iterator startJ = next(seqBegin, j);
  list<int>::iterator endJ = next(startJ, sizeJ);

  seq->splice(endJ, *seq, startI, endJ);
  seq->splice(endI, *seq, startJ, endJ);

  solution->calculateCost();

  return solution;
}

bool TSPSolution::bestImprovementSwap() {
  double bestDelta = 0;
  list<int>::iterator bestI, bestJ;
  list<int>::iterator begin = this->sequence.begin();
  list<int>::iterator pback = prev(this->sequence.end(), 2);
  list<int>::iterator stop = prev(pback);

  for (auto i = next(begin); i != stop; i++) {
    int vi = *i;
    int viNext = *next(i);
    int viPrev = *prev(i);

    double costViViNext = TSPSolution::reader->getDistance(vi, viNext);
    double costViPrevVi = TSPSolution::reader->getDistance(viPrev, vi);

    list<int>::iterator nextI = next(i);
    for (auto j = nextI; j != pback; j++) {
      int vj = *j;
      int vjNext = *next(j);
      int vjPrev = *prev(j);

      double delta = TSPSolution::reader->getDistance(viPrev, vj) +
                     TSPSolution::reader->getDistance(vj, viNext) +
                     TSPSolution::reader->getDistance(vjPrev, vi) +
                     TSPSolution::reader->getDistance(vi, vjNext) -
                     costViPrevVi - costViViNext -
                     TSPSolution::reader->getDistance(vjPrev, vj) -
                     TSPSolution::reader->getDistance(vj, vjNext);

      if (j == nextI) {
        delta += 2 * costViViNext;
      }

      if (delta < bestDelta) {
        bestDelta = delta;
        bestI = i;
        bestJ = j;
      }
    }
  }

  if (bestDelta < 0) {
    std::iter_swap(bestI, bestJ);
    this->cost += bestDelta;
    return true;
  }
  return false;
}

bool TSPSolution::bestImprovement2Opt() { // de lado por enquanto
  int bestDelta = 0;
  list<int>::iterator bestI, bestJ;
  list<int>::iterator begin = sequence.begin();
  list<int>::iterator pback =
      prev(sequence.end(), 2); // pback mean previous back
  list<int>::iterator stop = prev(pback, 3);

  for (auto i = next(begin); i != stop; i++) {
    int vi = *i;
    int viPrev = *prev(i);

    double distanceViPrevVi = TSPSolution::reader->getDistance(viPrev, vi);

    for (auto j = next(i, 3); j != pback; j++) {
      int vj = *j;
      int vjNext = *next(j);

      double delta = TSPSolution::reader->getDistance(vi, vjNext) +
                     TSPSolution::reader->getDistance(vj, viPrev) -
                     TSPSolution::reader->getDistance(vj, vjNext) -
                     distanceViPrevVi;

      if (delta < bestDelta) {
        bestDelta = delta;
        bestI = i;
        bestJ = j;
      }
    }
  }
  if (bestDelta < 0) {
    this->performTwoOptSwap(bestI, bestJ);
    this->cost += bestDelta;
    return true;
  }
  return false;
}

bool TSPSolution::bestImprovementOrOpt(int size) {
  if (size < 1 || size > 3)
    return false;
  int distance = size - 1;
  double bestDelta = 0;
  list<int>::iterator bestI, bestJ;
  list<int>::iterator begin = sequence.begin();
  list<int>::iterator pback = prev(sequence.end());
  list<int>::iterator stop = prev(pback, size);

  for (auto i = next(begin); i != stop; i++) {
    int vi = *i;
    int viPrev = *prev(i);

    list<int>::iterator i2 = next(i, distance);

    int vi2 = *i2;
    int vi2Next = *next(i2);

    double costViPrevVi = TSPSolution::reader->getDistance(viPrev, vi);
    double costVi2Vi2Next = TSPSolution::reader->getDistance(vi2, vi2Next);
    double costViPrevVi2Next =
        TSPSolution::reader->getDistance(viPrev, vi2Next);

    for (auto j = next(i, size); j != pback; j++) {
      int vj = *j;
      int vjNext = *next(j);

      double delta = TSPSolution::reader->getDistance(vjNext, vi) +
                     TSPSolution::reader->getDistance(vi2, vj) +
                     costViPrevVi2Next - costViPrevVi - costVi2Vi2Next -
                     TSPSolution::reader->getDistance(vj, vjNext);

      if (delta < bestDelta) {
        bestDelta = delta;
        bestI = i;
        bestJ = j;
      }
    }
  }

  if (bestDelta < 0) {
    this->performOrOpt(bestI, bestJ, size);
    this->cost += bestDelta;
    return true;
  }

  return false;
}

void TSPSolution::performTwoOptSwap(list<int>::iterator i,
                                    list<int>::iterator j) {
  std::reverse(i, next(j));
}

void TSPSolution::performOrOpt(list<int>::iterator i, list<int>::iterator j,
                               int size) {
  sequence.splice(next(j), sequence, i, next(i, size));
  if (size >= 2) {
    iter_swap(next(j), next(j, size));
  }
}
