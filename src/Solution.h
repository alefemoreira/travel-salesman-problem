#ifndef SOLUTION_H
#define SOLUTION_H

#include "Data.h"
#include <iterator>
#include <list>
#include <vector>

struct InsertionCost {
  int node;
  std::list<int>::iterator edge;
  double cost;
  std::vector<int>::iterator it;
};

class Solution {
public:
  Solution() {}
  Solution(bool build);  // equivalente ao método construção
  Solution(Solution *s); // Copia uma solução já existente
  // ~Solution();

  double getCost();
  std::list<int> *getSequence();

  void setSequence(std::list<int> *v);
  void setCost(double c);
  void calculateCost();
  void show();
  void localSearch();

  static Solution *disturbance(Solution *s);

private:
  std::list<int> sequence;
  double cost;
  list<int>::iterator lastI, lastJ;

  bool bestImprovementSwap();
  bool bestImprovement2Opt();
  bool bestImprovementOrOpt(int size);
  void twoOptSwap(list<int>::iterator i, list<int>::iterator j);
  void orOpt(list<int>::iterator i, list<int>::iterator j, int size);
};

#endif