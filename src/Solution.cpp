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

  // initialize random generator; Uses Normal Distribution
  std::random_device rd;
  std::mt19937 gen(rd());
  std::normal_distribution<> dis(0.5, 0.1);

  while (!v.empty()) {
    std::vector<InsertionCost> *costs = calculateInsertionCosts(&sequence, &v);
    std::sort(costs->begin(), costs->end(),
              [](InsertionCost a, InsertionCost b) { return a.cost < b.cost; });

    // dis(gen) generates random number following normal distribuission
    double alpha = dis(gen);

    int selected_index = rand() % ((int)ceil(alpha * costs->size()));
    InsertionCost selected = (*costs)[selected_index];

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
  std::vector<int> NL = {1, 2, 3, 4, 5};
  bool improved = false;

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis;

  while (!NL.empty()) {
    int n = dis(gen) % NL.size();
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
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(2, ceil((float)dimension / 10));

  int sizeI = dis(gen);
  int sizeJ = dis(gen);

  dis = std::uniform_int_distribution<>(1, dimension / 2 - sizeI);
  int i = dis(gen);
  dis = std::uniform_int_distribution<>(dimension / 2, dimension - sizeJ);
  int j = dis(gen);

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
  int bestI = 0, bestJ = 0;

  for (int i = 1; i < this->sequence.size() - 1; i++) {
    int vi = this->sequence[i];
    int viNext = this->sequence[i + 1];
    int viPrev = this->sequence[i - 1];
    for (int j = i + 2; j < this->sequence.size() - 1; j++) {
      int vj = this->sequence[j];
      int vjNext = this->sequence[j + 1];
      int vjPrev = this->sequence[j - 1];

      double delta = d->getDistance(viPrev, vj) + d->getDistance(vj, viNext) +
                     d->getDistance(vjPrev, vi) + d->getDistance(vi, vjNext) -
                     d->getDistance(viPrev, vi) - d->getDistance(vi, viNext) -
                     d->getDistance(vjPrev, vj) - d->getDistance(vj, vjNext);
      if (delta < bestDelta) {
        bestDelta = delta;
        bestI = i;
        bestJ = j;
      }
    }
  }

  if (bestDelta < 0) {
    std::swap(this->sequence[bestI], this->sequence[bestJ]);
    this->cost += bestDelta;
    return true;
  }
  return false;
}

bool Solution::bestImprovement2Opt() { // de lado por enquanto
  int bestDelta = 0;
  int bestI = 0, bestJ = 0;

  for (int i = 1; i < this->sequence.size() - 2; i++) {
    int vi = this->sequence[i];
    int viNext = this->sequence[i + 1];
    // + 2, because cannot be adjacents
    for (int j = i + 2; j < this->sequence.size() - 1; j++) {
      int vj = this->sequence[j];
      int vjNext = this->sequence[j + 1];

      double delta = d->getDistance(viNext, vjNext) + d->getDistance(vj, vj) -
                     d->getDistance(vi, viNext) - d->getDistance(vj, vjNext);

      if (delta < bestDelta) {
        bestDelta = delta;
        bestI = i;
        bestJ = j;
      }
    }
  }
  if (bestDelta < 0 && bestI != lastI && bestJ != lastJ) {
    lastI = bestI;
    lastJ = bestJ;
    this->twoOptSwap(bestI, bestJ);
    this->cost += bestDelta;
    return true;
  }
  return false;
}

void Solution::twoOptSwap(int i, int j) {
  while (i < j) {
    std::swap(this->sequence[i], this->sequence[j]);
    i++;
    j--;
  }
}

bool Solution::bestImprovementOrOpt(int size) {
  if (size < 1 || size > 3)
    return false;
  size--;
  double bestDelta = 0;
  int bestI = 0, bestJ = 0;

  for (int i = 1; i < sequence.size() - 2 - size; i++) {
    int vi = sequence[i];
    int viPrev = sequence[i - 1];
    int vi2 = sequence[i + size];
    int vi2Next = sequence[i + size + 1];

    for (int j = i + size + 1; j < sequence.size() - 1; j++) {
      int vj = sequence[j];
      int vjNext = sequence[j + 1];

      double delta = d->getDistance(vjNext, vi) + d->getDistance(vi2, vj) +
                     d->getDistance(viPrev, vi2Next) -
                     d->getDistance(viPrev, vi) - d->getDistance(vi2, vi2Next) -
                     d->getDistance(vj, vjNext);

      if (delta < bestDelta) {
        bestDelta = delta;
        bestI = i;
        bestJ = j;
      }
    }
  }

  if (bestDelta < 0) {
    this->orOpt(bestI, bestJ, size);
    this->cost += bestDelta;
    return true;
  }

  return false;
}

void Solution::orOpt(int i, int j, int size) {
  for (int k = 0; k < size + 1; k++) {
    int v = sequence[i];
    sequence.erase(sequence.begin() + i);
    sequence.insert(sequence.begin() + j, v);
    j--;
  }
}
