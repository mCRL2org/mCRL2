#!/usr/bin/env python
import sys
import os

def parse_file(f):
  transtbl = {}
  f.readline()
  cur = -1
  for l in f:
    lc = l.find(',')
    src = int(l[1:lc])
    if src <= cur:
      continue
    rc = l.rfind(',')
    dst = int(l[rc+1:-2])
    if dst <= src:
      continue
    lbl = l[lc+1:rc]
    cur = src
    transtbl[cur] = lbl, dst
  try:
    cur = 0
    while True:
      lbl, dst = transtbl[cur]
      del transtbl[cur]
      #print cur, lbl, dst
      print lbl.strip('"')
      cur = dst
  except KeyError:
    pass

def main():
  if len(sys.argv) != 2 or not os.path.isfile(sys.argv[1]):
    print('Usage: randomtrace.py FILE\n\n'
          'Reads a file in .aut format and produces a maximal trace from that file on the standard output.')
  else:
    parse_file(open(sys.argv[1]))

if __name__=='__main__':
  main()
