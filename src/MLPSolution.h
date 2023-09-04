#ifndef MLP_SOLUTION_H
#define MLP_SOLUTION_H

#include "Data.h"
#include "auxiliary.h"
#include <algorithm>
#include <list>
#include <vector>

class MLPSolution {
public:
  MLPSolution() {
    int n = Data::instance->getDimension();
    this->sequence = list<int>();
    this->subsequences = std::vector<std::vector<Subsequence>>(
        n + 1, std::vector<Subsequence>(n + 1));
  }
  MLPSolution(bool build);     // equivalente ao método construção
  MLPSolution(MLPSolution *s); // Copia uma solução já existente
  static const std::vector<double> alphas;

  double getCost();
  std::list<int> *getSequence();
  std::vector<std::vector<Subsequence>> *getSubsequences();

  void setSequence(std::list<int> *v);
  void setSubsequences(std::vector<std::vector<Subsequence>> *ss);
  void setCost(double c);

  void show();

  void calculateCost();
  void localSearch();
  bool isHamiltonTour();

  static MLPSolution *disturbance(MLPSolution *s);
  static MLPSolution *build(double alpha);
  static double randomAlpha() {
    int index = rand() % MLPSolution::alphas.size();
    return MLPSolution::alphas[index];
  }
  void updateAllSubsequences();

  bool bestImprovement2Opt();
  bool bestImprovementSwap();
  bool bestImprovementOrOpt(int size);

private:
  std::list<int> sequence;
  std::vector<std::vector<Subsequence>> subsequences;
  double cost;

  void updateSubsequences(int _i, list<int>::iterator _it, int _j,
                          list<int>::iterator _jt);

  void performTwoOptSwap(std::list<int>::iterator i,
                         std::list<int>::iterator j) {
    std::reverse(i, next(j));
  }

  void performOrOpt(std::list<int>::iterator i, std::list<int>::iterator j,
                    int size) {
    sequence.splice(next(j), sequence, i, next(i, size));
    if (size >= 2) {
      iter_swap(next(j), next(j, size));
    }
  }
};

bool validateCost(MLPSolution *s);

#endif