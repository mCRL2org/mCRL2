//
// Created by s152717 on 1-8-2020.
//

#include "adjacencylist.h"

Graph::AdjacencyList::AdjacencyList(std::vector<std::vector<size_t>>& mapping)
{
  for (int i = 0; i < mapping.size(); ++i)
  {
    std::vector<size_t>& map_to = mapping[i];

    // start index of the edges of this node
    locator.push_back(edgeList.size());

    for (int j = 0; j < map_to.size(); ++j)
    {
      edgeList.push_back(map_to[j]);
    }
  }
}

void Graph::AdjacencyList::operator=(Graph::AdjacencyList&& other)
{
  std::swap(locator, other.locator);
  std::swap(edgeList, other.edgeList);
}

size_t Graph::AdjacencyList::getEdge(size_t node, int edgeNr)
{
  size_t& index = locator[node];
  return edgeList[index + edgeNr];
}

int Graph::AdjacencyList::nrNeighbours(size_t elt)
{
  size_t startIndex = locator[elt];

  bool isLast = (elt == locator.size() - 1);
  size_t endIndex = isLast ? edgeList.size() : locator[elt + 1];

  return (int) (startIndex - endIndex);
}

void Graph::AdjacencyList::clear()
{
  locator.clear();
  edgeList.clear();
}
