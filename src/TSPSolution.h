#ifndef TSP_SOLUTION_H
#define TSP_SOLUTION_H

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
  static void setReader(Data *reader);
  static Data *reader;

private:
  std::list<int> sequence;
  double cost;

  bool bestImprovementSwap();
  bool bestImprovement2Opt();
  bool bestImprovementOrOpt(int size);
  void performTwoOptSwap(list<int>::iterator i, list<int>::iterator j);
  void performOrOpt(list<int>::iterator i, list<int>::iterator j, int size);
};

bool validateCost(TSPSolution *s);

#endif