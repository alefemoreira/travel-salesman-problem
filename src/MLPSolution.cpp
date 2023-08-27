#include "MLPSolution.h"
#include "Data.h"
#include "auxiliary.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <iostream>
#include <list>
#include <numeric>
#include <random>
#include <vector>

bool validateCost(MLPSolution *s) {
  double cost = s->getCost();
  s->calculateCost();
  double recalculatedCost = s->getCost();
  if (recalculatedCost != cost) {
    cout << '1' << endl;
    return false;
  }
  if (!s->isHamiltonTour()) {
    cout << '2' << endl;
    return false;
  }

  // cout << "Sequence: ";
  // s->show();
  // cout << "recalc cost: " << recalculatedCost << endl;
  // cout << "cost       : " << cost << endl;

  return true;
}

const std::vector<double> MLPSolution::alphas = {
    0.00, 0.01, 0.02, 0.03, 0.04, 0.05, 0.06, 0.07, 0.08,
    0.09, 0.10, 0.11, 0.12, 0.13, 0.14, 0.15, 0.16, 0.17,
    0.18, 0.19, 0.20, 0.21, 0.22, 0.23, 0.24, 0.25};

MLPSolution *MLPSolution::build(double alpha) {
  MLPSolution *mlp = new MLPSolution();
  int n = Data::instance->getDimension();

  list<int> *sequence = mlp->getSequence();

  sequence->push_back(1);

  vector<int> cl(n - 1);
  std::iota(cl.begin(), cl.end(), 2);

  int v = 1;

  while (cl.size() != 0) {
    std::sort(cl.begin(), cl.end(), [v](int a, int b) {
      return Data::instance->getDistance(v, a) >
             Data::instance->getDistance(v, b);
    });
    int rclSize = (int)ceil(alpha * cl.size());
    int selectedIndex = rclSize == 0 ? 0 : rand() % rclSize;
    int c = cl[selectedIndex];
    sequence->push_back(c);
    v = c;
    cl.erase(cl.begin() + selectedIndex);
  }

  sequence->push_back(1);

  mlp->calculateCost();
  mlp->updateAllSubsequences();

  return mlp;
}

MLPSolution::MLPSolution(MLPSolution *s) {
  this->sequence = std::list<int>(*s->getSequence());
  this->subsequences =
      std::vector<std::vector<Subsequence>>(*s->getSubsequences());
  this->cost = s->cost;
}

bool MLPSolution::isHamiltonTour() {
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

double MLPSolution::getCost() { return this->cost; }
void MLPSolution::setCost(double c) { this->cost = c; }
void MLPSolution::setSequence(std::list<int> *v) { this->sequence = *v; }
void MLPSolution::setSubsequences(std::vector<std::vector<Subsequence>> *ss) {
  this->subsequences = *ss;
}
std::vector<std::vector<Subsequence>> *MLPSolution::getSubsequences() {
  return &this->subsequences;
}
std::list<int> *MLPSolution::getSequence() { return &this->sequence; }

void MLPSolution::calculateCost() {
  this->cost = 0;
  auto begin = sequence.begin();

  for (auto it = begin; it != sequence.end(); it++) {
    list<int>::iterator a = begin;
    list<int>::iterator b = next(a);
    for (; a != it; a++, b++)
      this->cost += Data::instance->getDistance(*a, *b);
  }
}

void MLPSolution::show() {
  list<int>::iterator back = prev(sequence.end());
  for (auto it = sequence.begin(); it != back; it++)
    std::cout << *it << " -> ";

  std::cout << this->sequence.back() << std::endl;
}

void MLPSolution::localSearch() {
  // bool improved = false;
  // do {
  //   improved = this->bestImprovement2Opt();
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

MLPSolution *MLPSolution::disturbance(MLPSolution *s) {
  MLPSolution *solution = new MLPSolution(s); // copia a solução
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
  solution->updateAllSubsequences();

  return solution;
}

void MLPSolution::updateSubsequences(int _i, list<int>::iterator _it, int _j,
                                     list<int>::iterator _jt) {
  list<int>::iterator begin = this->sequence.begin();
  list<int>::iterator back = prev(this->sequence.end());

  int i = _i;
  for (auto it = _it; it != next(_jt); it++) {
    this->subsequences[i][i].W = (i > 0);
    this->subsequences[i][i].C = 0;
    this->subsequences[i][i].T = 0;
    this->subsequences[i][i].first = *it;
    this->subsequences[i][i].last = *it;

    i++;
  }

  int n = this->sequence.size();

  for (int i = 0; i <= _j; i++) {
    int j = _i;
    if (i >= _i)
      j = i + 1;
    for (; j < n; j++)
      this->subsequences[i][j] = Subsequence::concatenate(
          this->subsequences[i][j - 1], this->subsequences[j][j]);
  }

  for (int i = n - 1; i >= _i; i--) {
    int j = _j;
    if (i <= _j)
      j = i - 1;
    for (; j >= 0; j--)
      this->subsequences[i][j] = Subsequence::concatenate(
          this->subsequences[i][j + 1], this->subsequences[j][j]);
  }

  cout << subsequences[0][n - 1].C << endl;
}

void MLPSolution::updateAllSubsequences() {
  list<int>::iterator begin = this->sequence.begin();
  list<int>::iterator back = this->sequence.end();

  int i = 0;
  for (auto it = begin; it != back; it++) {
    this->subsequences[i][i].W = (i > 0);
    this->subsequences[i][i].C = 0;
    this->subsequences[i][i].T = 0;
    this->subsequences[i][i].first = *it;
    this->subsequences[i][i].last = *it;
    i++;
  }

  int n = this->sequence.size();

  for (int i = 0; i < n; i++)
    for (int j = i + 1; j < n; j++)
      this->subsequences[i][j] = Subsequence::concatenate(
          this->subsequences[i][j - 1], this->subsequences[j][j]);

  for (int i = n - 1; i >= 0; i--)
    for (int j = i - 1; j >= 0; j--)
      this->subsequences[i][j] = Subsequence::concatenate(
          this->subsequences[i][j + 1], this->subsequences[j][j]);

  // this->cost = subsequences[0][n - 1].C;
}

bool MLPSolution::bestImprovementSwap() {
  list<int>::iterator bestI, bestJ;
  int bestIndexI, bestIndexJ;
  list<int>::iterator begin = this->sequence.begin();
  list<int>::iterator back = prev(this->sequence.end());
  list<int>::iterator stop = prev(back);

  double bestCost = this->cost;
  int end = this->sequence.size() - 1;

  int i = 1;
  for (auto it = next(begin); it != stop; it++) {
    list<int>::iterator nextI = next(it);
    int j = i + 1;
    for (auto jt = nextI; jt != back; jt++) {
      Subsequence sigma =
          Subsequence::concatenate(subsequences[0][i - 1], subsequences[j][j]);
      if (j != i + 1) { // caso da adjacencia;
        sigma = Subsequence::concatenate(sigma, subsequences[i + 1][j - 1]);
      }
      sigma = Subsequence::concatenate(sigma, subsequences[i][i]);
      sigma = Subsequence::concatenate(sigma, subsequences[j + 1][end]);

      if (sigma.C < bestCost) {
        bestCost = sigma.C;
        bestIndexI = i;
        bestIndexJ = j;
        bestI = it;
        bestJ = jt;
      }
      j++;
    }
  }

  if (bestCost < this->cost) {
    std::iter_swap(bestI, bestJ);
    this->cost = bestCost;
    // this->updateAllSubsequences();
    this->updateSubsequences(bestIndexI, bestI, bestIndexJ, bestJ);
    bool isValid = validateCost(this);
    if (!isValid) {
      printf("SOLUCAO INVALIDA\n");
      printf("custo: %f\n", bestCost);
      printf("bestImprovement SWAP\n");
      printf("(i, j) = (%d, %d)\n", bestIndexI, bestIndexJ);
      printf("-----\n");
    }
    return true;
  }
  return false;
}

bool MLPSolution::bestImprovement2Opt() { // de lado por enquanto
  int bestDelta = 0;
  int end = this->sequence.size() - 1;
  double bestCost = this->subsequences[0][end].C;

  int bestIndexI, bestIndexJ;
  list<int>::iterator bestI, bestJ;
  list<int>::iterator begin = sequence.begin();
  list<int>::iterator back = prev(sequence.end());
  list<int>::iterator stop = prev(back, 3);

  int i = 1;
  for (auto it = next(begin); it != stop; it++) {
    int j = i + 3;
    for (auto jt = next(it, 3); jt != back; jt++) {
      Subsequence sigma1 =
          Subsequence::concatenate(subsequences[0][i - 1], subsequences[j][i]);
      Subsequence sigma2 =
          Subsequence::concatenate(sigma1, subsequences[j + 1][end]);

      if (sigma2.C < bestCost) {
        bestI = it;
        bestJ = jt;
        bestIndexI = i;
        bestIndexJ = j;
        bestCost = sigma2.C;
      }
      j++;
    }
    i++;
  }
  if (bestCost < this->cost) {
    this->performTwoOptSwap(bestI, bestJ);
    this->cost = bestCost;
    // this->updateAllSubsequences();
    this->updateSubsequences(bestIndexI, bestI, bestIndexJ, bestJ);
    bool isValid = validateCost(this);
    if (!isValid) {
      printf("SOLUCAO INVALIDA\n");
      printf("custo: %f\n", bestCost);
      printf("bestImprovement 2 Opt\n");
      printf("(i, j) = (%d, %d)\n", bestIndexI, bestIndexJ);
      printf("-----\n");
    }
    return true;
  }
  return false;
}

bool MLPSolution::bestImprovementOrOpt(int size) {
  if (size < 1 || size > 3)
    return false;
  int distance = size - 1;
  int end = this->sequence.size() - 1;

  double bestCost = this->cost;

  int bestIndexI, bestIndexJ;
  list<int>::iterator bestI, bestJ;
  list<int>::iterator begin = sequence.begin();
  list<int>::iterator back = prev(sequence.end());
  list<int>::iterator stop = prev(back, size);

  int i = 1;
  for (auto it = next(begin); it != stop; it++) {

    int j = i + size;
    for (auto jt = next(it, size); jt != back; jt++) {

      // [0, i-1][i+size, j][i + size - 1, i][j+1, end]
      Subsequence sigma1 = Subsequence::concatenate(subsequences[0][i - 1],
                                                    subsequences[i + size][j]);
      Subsequence sigma2 =
          Subsequence::concatenate(sigma1, subsequences[i + distance][i]);
      Subsequence sigma3 =
          Subsequence::concatenate(sigma2, subsequences[j + 1][end]);

      if (sigma3.C < bestCost) {
        bestCost = sigma3.C;
        bestI = it;
        bestJ = jt;
        bestIndexI = i;
        bestIndexJ = j;
      }
      j++;
    }

    i++;
  }

  if (bestCost < this->cost) {
    this->performOrOpt(bestI, bestJ, size);
    this->cost = bestCost;
    // this->updateAllSubsequences();
    this->updateSubsequences(bestIndexI, bestI, bestIndexJ, bestJ);
    bool isValid = validateCost(this);
    if (!isValid) {
      printf("SOLUCAO INVALIDA\n");
      printf("custo: %f\n", bestCost);
      printf("bestImprovement Or Opt %d\n", size);
      printf("(i, j) = (%d, %d)\n", bestIndexI, bestIndexJ);
      printf("-----\n");
    }
    return true;
  }

  return false;
}