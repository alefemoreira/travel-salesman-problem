#ifndef SOLUTION_H
#define SOLUTION_H

#include "Data.h"
#include <iterator>
#include <vector>

struct InsertionCost {
  int node;
  int edge;
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
  std::vector<int> *getSequence();

  void setSequence(std::vector<int> *v);
  void setCost(double c);
  void calculateCost();
  void show();
  void localSearch();

  static Solution *disturbance(Solution *s);

private:
  std::vector<int> sequence;
  double cost;
  int lastI, lastJ;

  bool bestImprovementSwap();
  bool bestImprovement2Opt();
  bool bestImprovementOrOpt(int size);
  void performTwoOptSwap(vector<int>::iterator i, vector<int>::iterator j);
  void performOrOpt(vector<int>::iterator i, vector<int>::iterator j, int size);
};

#endif