# Author(s): Wieger Wesselink
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
# 
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
# 
#  \file print_authors.py
#  \brief Print the authors of tools

import os
import os.path
import re
from path import *

FILENAME = 'authors.txt'

# Align the lines on the first occurrence of separator, starting from index 'start'
# The alignment is done by inserting spaces.
def align(lines, separator, start):
  maxindex = -1
  for line in lines:
    index = line.find(separator, start)
    if index > maxindex:
        maxindex = index
  for i, line in enumerate(lines):
    index = line.find(separator, start)
    if index > -1:
      lines[i] = line[:index] + ' '*(maxindex - index) + line[index:]
  return maxindex

# Align the lines in text on all occurrences of separator.
# The alignment is done by inserting spaces.
def align_text(text, separator = '|'):
  lines = text.split('\n')
  index = 0
  while index >= 0:
    index = align(lines, separator, index + 1)
  return '\n'.join(lines)

if os.path.exists(FILENAME):
  os.remove(FILENAME)

for dir in path('../../tools').dirs():
    tool = dir.basename()
    if tool == '.svn':
        continue
    tool = path('../../build/stage/%s' % tool).normcase()
    print tool.basename()
    if os.path.exists(tool) or os.path.exists(tool + '.exe'):
        os.system("echo %s >> authors.txt" % tool.basename())
        os.system("%s --generate-wiki-page | grep 'Written by' >> authors.txt" % tool)

lines = path(FILENAME).text().split('\n')
text = ''
for i, line in enumerate(lines):
  if i % 2 == 0:
    text = text + '\n'
  text = text + line
text = align_text(text, 'Written')
path(FILENAME).write_text(text)