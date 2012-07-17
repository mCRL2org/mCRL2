// Author(s): A.J. (Hannes) pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./node.cpp

#include "node.h"

void Node::swapTupleVal(size_t idx1, size_t idx2)
{
  double temp = tuple[idx1];
  tuple[idx1] = tuple[idx2];
  tuple[idx2] = temp;
}

void Node::moveTupleVal(size_t idxFr, size_t idxTo)
{
  double temp = tuple[idxFr];

  if (idxFr < idxTo)
  {
    for (size_t i = idxFr; i < idxTo; ++i)
    {
      tuple[i] = tuple[i+1];
    }
    tuple[idxTo] = temp;
  }
  else if (idxTo < idxFr)
  {
    for (size_t i = idxFr; i > idxTo; --i)
    {
      tuple[i] = tuple[i-1];
    }
    tuple[idxTo] = temp;
  }
}

void Node::moveTupleVals(std::map<size_t, size_t> &idcsFrTo)
{
  std::vector<double> tupleNew(idcsFrTo.size(), -1);

  for (size_t i = 0; i < idcsFrTo.size(); ++i)
  {
    tupleNew[idcsFrTo[i]] = tuple[i];
  }

  tuple = tupleNew;
}
