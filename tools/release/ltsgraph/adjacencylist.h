//
// Created by s152717 on 1-8-2020.
//

#ifndef MCRL2_ADJACENCYLIST_H
#define MCRL2_ADJACENCYLIST_H

#include <vector>

namespace Graph
{
class AdjacencyList
{
  /// locator maps each node to a start index in edgeList
  std::vector<size_t> locator;
  std::vector<size_t> edgeList;

public:
  AdjacencyList() = default;

  AdjacencyList(std::vector<std::vector<size_t>>& mapping);

  void operator=(AdjacencyList&& other);

  /// returns the pointer to the index of the first neighbour of elt.
  /// all elements up to prtTo(elt + 1) will be neighbours of elt.
  inline size_t* ptrTo(size_t elt)
  {
    return &edgeList[locator[elt]];
  }

  /// creates a vector of the elements connected to elt
  template<typename T>
  std::vector<T> getConnectionsOf(size_t elt, std::vector<T> list)
  {
    std::vector<T> result;

    for (size_t* i_ptr = ptrTo(elt); i_ptr < ptrTo(elt + 1); ++i_ptr)
    {
      size_t& index = *i_ptr;
      result.push_back(list[index]);
    }

    return result;
  }

  /// returns the edgeNr-th neighbour of the given node
  size_t getEdge(size_t node, int edgeNr);

  /// returns the number of neighbours of the given element
  int nrNeighbours(size_t elt);

  std::size_t nrNodes() const
  {
    return locator.size();
  }

  std::size_t nrEdges() const
  {
    return edgeList.size();
  }

  void clear();
};
}


#endif //MCRL2_ADJACENCYLIST_H
