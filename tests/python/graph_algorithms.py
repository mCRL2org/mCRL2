#!/usr/bin/env python

#~ Copyright 2015 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

# Implementation of topological sorting, based on http://en.wikipedia.org/wiki/Topological_sorting

def has_incoming_edges(G, n):
    return G[n][0]

def remove_edge(G, m, n):
    G[m][1].remove(n)
    G[n][0].remove(m)

def insert_edge(G, m, n):
    G[m][1].add(n)
    G[n][0].add(m)

def has_edges(G):
    for n in G:
        if G[n][0] or G[n][1]:
            return True
    return False

def topological_sort(G):
    # Empty list that will contain the sorted elements
    L = []

    # S <- Set of all nodes with no incoming edges
    S = set([n for n in G if not has_incoming_edges(G, n)])

    while S:
        n = S.pop()
        L.append(n)
        for m in G:
            if n in G[m][0]: # (n, m) is an edge in G
                remove_edge(G, n, m)
                if not has_incoming_edges(G, m):
                    S.add(m)
    if has_edges(G):
        raise RuntimeError('topological_sort: the graph has at least one cycle')
    return L

if __name__ == "__main__":
    nodes = [0, 1, 2, 3, 4]
    edges = [ (0, 1), (0, 2), (4, 1), (2, 3) ]
    G = {}
    for n in nodes:
        G[n] = (set(), set())
    for e in edges:
        insert_edge(G, e[0], e[1])
    L = topological_sort(G)
    print 'L =', L
