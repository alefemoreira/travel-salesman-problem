#include "Solution.h"
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

extern Data *d;

std::vector<InsertionCost> *calculateInsertionCosts(std::vector<int> *s,
                                                    std::vector<int> *v) {
  int size = (s->size() - 1) * v->size();
  std::vector<InsertionCost> *costs = new std::vector<InsertionCost>(size);
  int l = 0;
  for (int a = 0, b = 1; a < s->size() - 1; a++, b++) {
    int i = (*s)[a];
    int j = (*s)[b];
    for (auto it = v->begin(); it < v->end(); it++) {
      InsertionCost cost;
      int k = *it;
      cost.node = k;
      cost.it = it;
      cost.edge = a;
      cost.cost =
          d->getDistance(i, k) + d->getDistance(k, j) - d->getDistance(i, j);
      (*costs)[l] = cost;
      l++;
    }
  }

  return costs;
}

Solution::Solution(bool build) {
  if (!build)
    return;

  this->sequence = std::vector<int>();
  // array of possibles vertices
  // d->getDimension() - 1 is because element '1' already is on solution
  std::vector<int> v(d->getDimension() - 1);
  std::iota(v.begin(), v.end(), 2);

  sequence.push_back(1);
  for (int i = 0; i < 3; i++) {
    int index = rand() % v.size();
    sequence.push_back(v[i]);
    v.erase(v.begin() + i);
  }
  sequence.push_back(1);

  while (!v.empty()) {
    std::vector<InsertionCost> *costs = calculateInsertionCosts(&sequence, &v);
    std::sort(costs->begin(), costs->end(),
              [](InsertionCost a, InsertionCost b) { return a.cost < b.cost; });

    // dis(gen) generates random number following normal distribuission
    double alpha = (double)rand() / RAND_MAX;
    int selectedIndex = rand() % ((int)ceil(alpha * costs->size()));
    InsertionCost selected = (*costs)[selectedIndex];

    sequence.insert(sequence.begin() + selected.edge + 1, selected.node);
    v.erase(selected.it);
    delete costs;
  }

  this->calculateCost();
}

Solution::Solution(Solution *s) {
  this->sequence = std::vector<int>(*s->getSequence());
  this->cost = s->cost;
}

double Solution::getCost() { return this->cost; }
void Solution::setCost(double c) { this->cost = c; }
void Solution::setSequence(std::vector<int> *v) { this->sequence = *v; }
std::vector<int> *Solution::getSequence() { return &this->sequence; }

void Solution::calculateCost() {
  this->cost = 0;
  for (int a = 0, b = 1; a < this->sequence.size() - 1; a++, b++) {
    int i = this->sequence[a];
    int j = this->sequence[b];
    this->cost += d->getDistance(i, j);
  }
}

void Solution::show() {
  for (int i = 0; i < this->sequence.size() - 1; i++)
    std::cout << this->sequence[i] << " -> ";

  std::cout << this->sequence.back() << std::endl;
}

void Solution::localSearch() {
  // bool improved = false;
  // do {
  //   improved = this->bestImprovementOrOpt(3);
  // } while (improved);
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

Solution *Solution::disturbance(Solution *s) {
  Solution *solution = new Solution(s); // copia a solução
  int dimension = s->sequence.size() - 1;

  int lowerBound = 2;
  int upperBound = std::ceil(static_cast<double>(dimension) / 10);

  int sizeI = rand() % (upperBound - lowerBound + 1) + lowerBound;
  int sizeJ = rand() % (upperBound - lowerBound + 1) + lowerBound;

  // [1, dimension / 2 - sizeI]
  int i = rand() % (dimension / 2 - sizeI - 1) + 1;
  // // [dimension / 2 + 1, seq.size - 1]
  int j = dimension / 2 + rand() % (dimension / 2 - sizeJ - 1) + 1;

  std::vector<int> *seq = solution->getSequence();
  auto seqBegin = seq->begin();
  for (int c = 0; c < sizeI; c++) {
    int v = (*seq)[i];
    seq->erase(seqBegin + i);
    seq->insert(seqBegin + j - 1, v);
  }

  for (int c = 0; c < sizeJ; c++) {
    int v = (*seq)[j];
    seq->erase(seqBegin + j);
    seq->insert(seqBegin + i, v);
    j++;
    i++;
  }

  solution->calculateCost();

  return solution;
}

bool Solution::bestImprovementSwap() {
  double bestDelta = 0;
  vector<int>::iterator bestI, bestJ;
  vector<int>::iterator begin = this->sequence.begin();
  vector<int>::iterator back = prev(this->sequence.end(), 1);
  vector<int>::iterator stop = prev(back);

  for (auto i = next(begin); i != stop; i++) {
    int vi = *i;
    int viNext = *next(i);
    int viPrev = *prev(i);

    double costViViNext = d->getDistance(vi, viNext);
    double costViPrevVi = d->getDistance(viPrev, vi);

    vector<int>::iterator nextI = next(i);
    for (auto j = nextI; j != back; j++) {
      int vj = *j;
      int vjNext = *next(j);
      int vjPrev = *prev(j);

      double delta = d->getDistance(viPrev, vj) + d->getDistance(vj, viNext) +
                     d->getDistance(vjPrev, vi) + d->getDistance(vi, vjNext) -
                     costViPrevVi - costViViNext - d->getDistance(vjPrev, vj) -
                     d->getDistance(vj, vjNext);

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

bool Solution::bestImprovement2Opt() { // de lado por enquanto
  int bestDelta = 0;
  vector<int>::iterator bestI, bestJ;
  vector<int>::iterator begin = sequence.begin();
  vector<int>::iterator back = prev(sequence.end());
  vector<int>::iterator stop = prev(back, 3);

  for (auto i = next(begin); i != stop; i++) {
    int vi = *i;
    int viPrev = *prev(i);

    double distanceViPrevVi = d->getDistance(viPrev, vi);

    for (auto j = next(i, 3); j != back; j++) {
      int vj = *j;
      int vjNext = *next(j);

      double delta = d->getDistance(vi, vjNext) + d->getDistance(vj, viPrev) -
                     d->getDistance(vj, vjNext) - distanceViPrevVi;

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

void Solution::performTwoOptSwap(vector<int>::iterator i,
                                 vector<int>::iterator j) {
  std::reverse(i, next(j));
}

bool Solution::bestImprovementOrOpt(int size) {
  if (size < 1 || size > 3)
    return false;
  int distance = size - 1;
  double bestDelta = 0;
  vector<int>::iterator bestI, bestJ;
  vector<int>::iterator begin = sequence.begin();
  vector<int>::iterator back = prev(sequence.end());
  vector<int>::iterator stop = prev(back, size);

  for (auto i = next(begin); i != stop; i++) {
    int vi = *i;
    int viPrev = *prev(i);

    vector<int>::iterator i2 = next(i, distance);

    int vi2 = *i2;
    int vi2Next = *next(i2);

    double costViPrevVi = d->getDistance(viPrev, vi);
    double costVi2Vi2Next = d->getDistance(vi2, vi2Next);
    double costViPrevVi2Next = d->getDistance(viPrev, vi2Next);

    for (auto j = next(i, size); j != back; j++) {
      int vj = *j;
      int vjNext = *next(j);

      double delta = d->getDistance(vjNext, vi) + d->getDistance(vi2, vj) +
                     costViPrevVi2Next - costViPrevVi - costVi2Vi2Next -
                     d->getDistance(vj, vjNext);

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

void Solution::performOrOpt(std::vector<int>::iterator _i,
                            std::vector<int>::iterator _j, int size) {
  int i = std::distance(sequence.begin(), _i);
  int j = std::distance(sequence.begin(), _j);
  for (int k = 0; k < size; k++) {
    int v = sequence[i];
    sequence.erase(sequence.begin() + i);
    sequence.insert(sequence.begin() + j, v);
    j--;
  }
}
