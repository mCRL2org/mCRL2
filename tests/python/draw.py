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

def remove_extension(name):
    return re.sub(r'\.\w+$', '', name)

def add_vertex(out, name, attributes):
    out.write('  {0} [{1}];\n'.format(name, ', '.join(attributes)))

def add_edge(out, src, dest):
    out.write('  {0} -> {1};\n'.format(src, dest))

# returns the node labels that appear in result_text
def result_nodes(nodes, result_text):
    result = []
    for label in nodes:
        if re.search(r'\b{0}\b'.format(label), result_text):
            result.append(label)
    return result

def generate_dotfile(ymlfile):
    out = StringIO.StringIO()
    out.write('digraph G {\n')
    f = open(ymlfile)
    data = yaml.safe_load(f)
    f.close()

    # draw the nodes
    nodes = data['nodes']
    for node in sorted(nodes):
        attributes = ['label="{0}: {1}"'.format(node, nodes[node]['type'])]
        add_vertex(out, node, attributes)

    # draw the tools
    tools = data['tools']
    for name in tools:
        tool = tools[name]
        label_attribute = 'label="{0}"'.format(name + ': ' + tool['name'] + ' ' + ' '.join(tool['args']))
        add_vertex(out, name, ['shape=box', label_attribute])
        for src in tool['input']:
            add_edge(out, src, name)
        for dest in tool['output']:
            add_edge(out, name, dest)

    # draw the result node
    result_text = data['result']
    name = 'result'
    label_attribute = 'label="{0}"'.format(result_text)
    add_vertex(out, name, ['shape=box', label_attribute])
    for label in result_nodes(nodes, result_text):
        add_edge(out, label, name)

    out.write('}\n')
    dotfile = remove_extension(ymlfile) + '.dot'
    pdffile = remove_extension(ymlfile) + '.pdf'
    with open(dotfile, 'w') as text_file:
        text_file.write(out.getvalue())
    os.system('dot -Tpdf {0} -o {1}'.format(dotfile, pdffile))

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
  pdffile = remove_extension(ymlfile) + '.pdf'
  os.system('evince "{0}"'.format(pdffile))
