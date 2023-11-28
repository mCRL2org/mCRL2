#!/usr/bin/env python3

#~ Copyright 2014-2016 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import argparse
import os
import os.path
import re
import traceback
import yaml
import io

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
    out = io.StringIO()
    out.write('digraph G {\n')
    f = open(ymlfile)
    data = yaml.safe_load(f)
    f.close()

    # draw the nodes
    nodes = data['nodes']
    for node in sorted(nodes):
        attributes = ['label="{0}: {1}"'.format(node, nodes[node]['type']), 'style=filled', 'fillcolor=papayawhip']
        add_vertex(out, node, attributes)

    # draw the tools
    tools = data['tools']
    for name in tools:
        tool = tools[name]
        label_attribute = 'label="{0}"'.format(name + ': ' + tool['name'] + ' ' + ' '.join(tool['args']))
        add_vertex(out, name, ['shape=box', 'style=filled', 'fillcolor=tan1', label_attribute])
        for src in tool['input']:
            add_edge(out, src, name)
        for dest in tool['output']:
            add_edge(out, name, dest)

    # draw the result node
    result_text = data['result']
    name = 'result'
    label_attribute = 'label="{0}"'.format(result_text)
    add_vertex(out, name, ['shape=box', 'style=filled', 'fillcolor=lightblue', label_attribute])
    for label in result_nodes(list(nodes.keys()) + list(tools.keys()), result_text):
        add_edge(out, label, name)

    out.write('}\n')
    dotfile = remove_extension(ymlfile) + '.dot'
    pdffile = remove_extension(ymlfile) + '.pdf'
    with open(dotfile, 'w') as text_file:
        text_file.write(out.getvalue())
    os.system('dot -Tpdf {0} -o {1}'.format(dotfile, pdffile))

def main():
    cmdline_parser = argparse.ArgumentParser()
    cmdline_parser.add_argument('ymlfile', metavar='INFILE', type=str, help='a .yml file containing a test')
    args = cmdline_parser.parse_args()
    ymlfile = args.ymlfile
    generate_dotfile(ymlfile)
    pdffile = remove_extension(ymlfile) + '.pdf'
    os.system('evince "{0}"'.format(pdffile))

if __name__ == '__main__':
    try:
        main()
    except Exception as e:
        print('ERROR:', e)
        traceback.print_exc()
