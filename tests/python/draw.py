#!/usr/bin/env python

#~ Copyright 2014 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import glob
import os
import os.path
import re
import sys
import yaml
import StringIO

def remove_ext(name):
  return re.sub(r'\.\w+$', '', name)

def generate_dotfile(ymlfile):
  out = StringIO.StringIO()
  out.write('digraph G {\n')
  f = open(ymlfile)
  data = yaml.safe_load(f)
  f.close()
  tools = data['tools']
  nodes = data['nodes']
  for node in sorted(nodes):
    out.write('  %s [label=%s];\n' % (remove_ext(node), nodes[node]))
  toolindex = 1
  for name in tools:
    tool = tools[name]
    toolname = 'tool%d' % toolindex
    toolindex = toolindex + 1
    out.write('  %s [shape=box, label="%s"];\n' % (toolname, name + ' ' + ' '.join(tool['args'])))
    for src in tool['input']:
      out.write('  %s -> %s;\n' % (remove_ext(src), toolname))
    for dest in tool['output']:
      out.write('  %s -> %s;\n' % (toolname, remove_ext(dest)))
  out.write('}\n')
  dotfile = remove_ext(ymlfile) + '.dot'
  pdffile = remove_ext(ymlfile) + '.pdf'
  print dotfile, pdffile
  with open(dotfile, 'w') as text_file:
    text_file.write(out.getvalue())
  os.system('dot -Tpdf %s -o %s' % (dotfile, pdffile))

if len(sys.argv) != 2:
  print('Usage: draw <path> where <path> is an yml file or a directory containing yml files')
if os.path.isdir(sys.argv[1]):
  files = glob.glob('tests/*.yml')
else:
  files = [sys.argv[1]]
for ymlfile in files:
  generate_dotfile(ymlfile)
if len(files) == 1:
  ymlfile = files[0]
  pdffile = remove_ext(ymlfile) + '.pdf'
  os.system('evince "%s"' % pdffile)
