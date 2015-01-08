#!/usr/bin/env python
import argparse
import Queue
import sys

class SCCDecomp(object):
  def __init__(self):
    self.graph = {}
    self.__index = None
    self.__sccs = None
    self.__low = None
    self.__S = None

  def load(self, f):
    self.graph = {}
    self.filename = f
    f = open(f) if f is not None else sys.stdin
    f.readline()
    cur = None
    print "Loading...",
    for l in f:
      src, l = l.split(',', 1)
      src = int(src.strip(' ('))
      lbl, dst = l.rsplit(',', 1)
      cur = src
      dst = int(dst.strip(' )\n'))
      lbl = lbl.strip(' "')
      self.graph.setdefault(cur, []).append((lbl, dst))
      self.graph.setdefault(dst, [])
    if len(self.graph) > sys.getrecursionlimit():
      sys.setrecursionlimit(len(self.graph))
    print "Done."

  def findSCCs(self, genpath):
    self.__sccs = {}
    self.__low = {}
    self.__index = 0
    self.__S = []
    self.__N = 0
    self.__genpath = genpath
    print "SCC Decomposition...",
    for node in self.graph:
      if node not in self.__sccs:
        self.traverse(node)
    print "Done."
  
  def traverse(self, node):
    self.__sccs[node] = self.__index
    self.__low[node] = self.__index
    self.__index += 1
    self.__S.append(node)
    
    for lbl, dst in self.graph[node]:
      if dst not in self.__sccs:
        self.traverse(dst)
        self.__low[node] = min(self.__low[node], self.__low[dst])
      elif dst in self.__S:
        self.__low[node] = min(self.__low[node], self.__sccs[dst])

    if self.__low[node] == self.__sccs[node]:
      SCC = []
      v = self.__S.pop()
      SCC.append(v)
      while v != node:
        v = self.__S.pop()
        SCC.append(v)
      self.report(SCC)      

  def report(self, SCC):
    if len(SCC) > 1 or any([dst == SCC[0] 
                           and not lbl.startswith('startup_failed')
                           and not lbl == 'FLAG' for lbl, dst in self.graph[SCC[0]]]):
      rename = {}
      ntrans = 0
      aut = ''
      print "Found SCC."
      if self.__genpath:
        path = self.pathto(target=SCC)
        src = 0
        for lbl, dst in path:
          ntrans += 1
          aut += '({0}, "{1}", {2})\n'.format(rename.setdefault(src, len(rename)), lbl, rename.setdefault(dst, len(rename)))
          src = dst
      for i, v in enumerate(SCC):
        for lbl, dst in self.graph[v]:
          if dst in SCC:
            ntrans += 1
            aut += '({0}, "{1}", {2})\n'.format(rename.setdefault(v, len(rename)), lbl, rename.setdefault(dst, len(rename)))

      open(((self.filename + '.') if self.filename is not None else '')
           + 'scc{0}.aut'.format(self.__N), 'w+').write('des (0,{0},{1})\n{2}'.format(ntrans, len(rename), aut))
      self.__N += 1

  def pathto(self, target={}, label='', deadlock=False, from_=0):
    paths = [[('@start', from_)]]
    reach = set([0])
    while paths:
      newpaths = []
      for path in paths:
        fanout = self.graph[path[-1][1]]
        if not fanout and deadlock:
          return path[1:]
        for lbl, dst in fanout:
          if dst in target or lbl == label:
            return path[1:] + [(lbl, dst)]
          elif dst not in reach:
            newpaths.append(path + [(lbl, dst)])
            reach.add(dst)
      paths = newpaths
    return []

  def findFLAG(self, action):
    print '\n'.join([lbl for lbl, dst in self.pathto(label=action)])

def main():
  parser = argparse.ArgumentParser(description='Perform an SCC decomposition of a .aut file. The SCCs are written to separate .aut files.')
  parser.add_argument('--path', action='store_true',
                      help='Also store a path from the initial state with each SCC.')
  parser.add_argument('--find', type=str, metavar='ACTION',
                      help='Do not do SCC composition. Instead, search for an action and print a trace to it.')
  parser.add_argument('file', metavar='FILE', nargs='?', type=str, 
                      help='The .aut file to read from (omit to use STDIN).')
  args = parser.parse_args()

  alg = SCCDecomp()
  alg.load(args.file)
  if args.find is not None:
    alg.findFLAG(args.find)
  else:
    alg.findSCCs(args.path)
  
if __name__ == '__main__':
  main()
