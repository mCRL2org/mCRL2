#!/usr/bin/env python
import argparse
import subprocess
import sys
import re

class lifeline(object):
  symbols = {'NONE': ' ', 'FIRST_HEADER_BIT': 'H', 'DATA_BIT': 'D', 'NOISE': 'X', 'CAS_BIT': 'C'}  

  def __init__(self, name):
    self.__name = name
    self.__values = [' ']
    self.__events = [[]]
    self.__start = []
  def __len__(self):
    return len(self.__values)
  def hasevents(self):
    return any([x not in ' U' for x in self.__values])

  def setvalue(self, val):
    self.__values[-1] = lifeline.symbols[val]
  def setevent(self, val):
    print val
    if val == 'attempt_startup':
      self.__start.append(len(self.__values) - 1)
      val = ''
    self.__events[-1].append(val)
  def bit(self):
    self.__values.append('U')
    self.__events.append([])
  def __str__(self):
    return '.' * max(0, self.__start) + ''.join(self.__values[self.__start:])
  def tikz(self, show_events):
    self.__start = self.__start or [-1]
    doc = r'\draw (-5, 0) node[left]  {{\textsc{{{0}}}}};'.format(self.__name)
    doc += '\path[draw] ({0}, 0) -- ({1}, 0);\n'.format(max(0, self.__start[0]), len(self.__values))

    pos = 'below'
    dir1 = ''
    dir2 = '-'
    start = self.__start[0]
    self.__start = self.__start[1:]
    c = 7 - start - 1 - 3 # 84 - start - 1 - 24 # 
    C = 7 # 84
    for i, v in enumerate(self.__events):
      c += 1
      if 'abort' in v:
        c = C + 1
        if self.__start:
          start = (self.__start[0] + 1) % C
          self.__start = self.__start[2:]
          c = (i - start) % C
      if c == C and self.__name != 'bus':
        doc += r'\path[draw,gray] ({0}, 0.1) -- ({0}, -0.1);'.format(i)
        c = 0
      if v and show_events:
        pos = 'below' if pos == 'above' else 'above'
        dir1, dir2 = dir2, dir1
        doc += r'\path[draw] ({0}, {1}0.5) -- ({0}, {2}0.5)'.format(i, dir1, dir2)
        doc += 'node[{1},font=\scriptsize] {{{0}}}'.format(r'\begin{{tabular}}{{c}}{0}\end{{tabular}}'.format(',\\\\'.join([e.replace('_', '\\_') for e in v if e != 'attempt_startup'])), pos)
        doc += ';\n'

    start = -1
    startf = -1
    sym = ' '
    H = 1 # 6
    h = 0
    for i, v in enumerate(self.__values):
      if v != ' ' and start < 0:
        start = i
        startf = i if sym == ' ' else startf
        sym = v
      elif v != sym and start > 0 and (v != 'D' or i - startf >= H):
        doc += r'\draw[{0}] ({2}, 0.3) rectangle({1}, -0.3);'.format('sym' + sym, i, start)
        start = i if v != ' ' else -1
        if v == ' ':
          doc += r'\draw[frame] ({2}, 0.3) rectangle({1}, -0.3);'.format('sym' + sym, i, startf)
        else:
          startf = i if sym == ' ' else startf
        sym = v

    return doc

def gen_tikz(parties, show_events):
  scale = 9.0 / len(parties[0])
  doc = r'\tikzstyle{symC}=[rectangle, draw=none, fill=black]' \
        r'\tikzstyle{symH}=[rectangle, draw=none, fill=black!50]' \
        r'\tikzstyle{symD}=[rectangle, draw=none, fill=black!30]' \
        r'\tikzstyle{symX}=[rectangle, draw=none, fill=white]' \
        r'\tikzstyle{symU}=[rectangle, draw=none, fill=red]' \
        r'\tikzstyle{frame}=[rectangle, draw=black, fill=none]' \
        r'\begin{tikzpicture}[x=%fcm]'%scale
  i = 0.0
  for p in parties:
    doc += r'\begin{{scope}}[yshift=-{0}cm,y=0.5cm]'.format(i)
    doc += p.tikz(show_events)
    doc += r'\end{scope}'
    i += 0.5
  return r'''
\documentclass{{article}}
\usepackage{{tikz}}
\usepackage[active,tightpage]{{preview}}
\begin{{document}}
\begin{{preview}}
{0}
\end{{preview}}
\end{{document}}'''.format(doc + r'\end{tikzpicture}')

def gen_pdf(doc, pdfname):
  latex = subprocess.Popen(['pdflatex', '-jobname={}'.format(pdfname)], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
  out, err = latex.communicate(doc)
  if (latex.returncode):
    print(out)
  else:
    print('OK.')

def parse_file(f, pdfname, show_events):
  parties = (lifeline('node 1'), lifeline('node 2'), lifeline('node 3'), lifeline('node 4'), lifeline('bus'), lifeline('noise'))

  for l in f:
    m = re.search('(Put|bus|abort|enter_operation|init_sched|startup_failed|attempt_startup)\(([^\(\)]*)', l)
    if m:
      if m.group(1) == 'Put':
        i, v = [x.strip() for x in m.group(2).split(',')]
        i = int(i)
        parties[i-1].setvalue(v)
      elif m.group(1) == 'bus':
        parties[-2].setvalue(m.group(2))
        for party in parties:
          party.bit()
      else:
        i = int(m.group(2))
        if i < len(parties):
          parties[i-1].setevent(m.group(1))
        else:
          print ils

  while not parties[-1].hasevents():
    parties = parties[:-1]
  doc = gen_tikz(parties, show_events)
  if pdfname is not None:
    gen_pdf(doc, pdfname)
  else:
    print doc

def main():
  parser = argparse.ArgumentParser(description='Generate a Tikz script or PDF file from a textual trace provided on STDIN.')
  parser.add_argument('--pdf', metavar='NAME', type=str, 
                      help='Create a PDF file (requires pdflatex to be available in the PATH).')
  parser.add_argument('--events', action='store_true',
                      help='Annotate the trace with events that would otherwise be invisible.')
  args = parser.parse_args()
  parse_file(sys.stdin, args.pdf, args.events)

if __name__ == '__main__':
  main()
