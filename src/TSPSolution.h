#ifndef TSP_SOLUTION_H
#define TSP_SOLUTION_H

#include "Data.h"
#include <algorithm>
#include <iterator>
#include <list>
#include <vector>

class TSPSolution {
public:
  TSPSolution() {}
  TSPSolution(bool build);     // equivalente ao método construção
  TSPSolution(TSPSolution *s); // Copia uma solução já existente
  // ~Solution();

  double getCost();
  std::list<int> *getSequence();

  void setSequence(std::list<int> *v);
  void setCost(double c);
  void calculateCost();
  void show();
  void localSearch();
  bool isHamiltonTour();

  static TSPSolution *disturbance(TSPSolution *s);

private:
  std::list<int> sequence;
  double cost;

  bool bestImprovementSwap();
  bool bestImprovement2Opt();
  bool bestImprovementOrOpt(int size);
  void performTwoOptSwap(std::list<int>::iterator i,
                         std::list<int>::iterator j) {
    std::reverse(i, next(j));
  }
  void performOrOpt(std::list<int>::iterator i, std::list<int>::iterator j,
                    int size) {
    sequence.splice(next(j), sequence, i, next(i, size));
    // if (size >= 2) {
    //   iter_swap(next(j), next(j, size));
    // }
  }
};

bool validateCost(TSPSolution *s);

#endif