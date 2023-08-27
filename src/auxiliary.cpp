#include "auxiliary.h"
#include "Data.h"
#include <list>
#include <vector>

using namespace std;

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
      cost.cost = Data::instance->getDistance(i, k) +
                  Data::instance->getDistance(k, j) -
                  Data::instance->getDistance(i, j);
      (*costs)[l] = cost;
      l++;
    }
  }

  return costs;
}