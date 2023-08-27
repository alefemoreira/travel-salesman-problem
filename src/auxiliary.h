#ifndef AUXILIARY_H
#define AUXILIARY_H

#include "Data.h"
#include <list>
#include <vector>

struct InsertionCost {
  int node;
  std::list<int>::iterator edge;
  double cost;
  std::vector<int>::iterator it;
};

std::vector<InsertionCost> *calculateInsertionCosts(std::list<int> *s,
                                                    std::vector<int> *v);

struct Subsequence {
  double T, C;
  int W;
  int first, last;

  inline static Subsequence concatenate(Subsequence &s1, Subsequence &s2) {
    Subsequence s;
    double temp = Data::instance->getDistance(s1.last, s2.first);
    s.W = s1.W + s2.W;
    s.T = s1.T + temp + s2.T;
    s.C = s1.C + s2.W * (s1.T + temp) + s2.C;
    s.first = s1.first;
    s.last = s2.last;
    return s;
  }
};

#endif