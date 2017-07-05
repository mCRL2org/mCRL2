#!/usr/bin/env python

import os
import sys
import json
import string
import codecs
import argparse
import subprocess

class Testset(object):
  def __init__(self):
    self.__template = None

  @property
  def template(self):
    if self.__template is None:
      self.__template = string.Template(codecs.open('startup.mcrl2', encoding='utf-8').read())
    return self.__template

  def specs(self):
    default = None
    for c in json.load(open('testset.json')):
      if default is None:
        default = c
        continue
      spec = default.copy()
      spec.update(c)
      yield (str(spec['name']), spec['method'] + '.sh', self.template.substitute(spec))
      
  def run_tool(self, cmdline):
      try:
          print('* {}'.format(cmdline))
          subprocess.check_call(cmdline, shell=True)
      except Exception as e:
          print('* Error: {}'.format(e))
          raise RuntimeError()

  def run(self, stage):
    if not os.path.isdir('data'):
      os.mkdir('data')
    try:
      for name, script, spec in self.specs():
        name = name.replace(' ', '_')
        specfile = os.path.join('data', name + '.mcrl2')
        f = codecs.open(specfile, 'w+', encoding='utf-8')
        f.write(spec)
        f.close()
        print('***')
        print('*** Starting {}'.format(name))
        print('***')
        self.run_tool('{0} preprocessor.py data/{1}.mcrl2 > data/{1}.expanded.mcrl2'.format(
          sys.executable, name))
        if stage > 1:
          print('*** Linearizing {}...'.format(name))
          self.run_tool('mcrl22lps -Dfcb data/{0}.expanded.mcrl2 data/{0}.lps'.format(name))
          for mcf in os.listdir('mucalc'):
            if not mcf.endswith('.mcf') or stage < 3:
              continue
            propname = mcf[:-4]
            pbesbase = name + '.' + propname
            print('*** Generating PBES for {} on {}'.format(propname, name))          
            self.run_tool('lps2pbes -f mucalc/{} data/{}.lps data/{}.pbes'.format(mcf, name, pbesbase))
            self.run_tool('pbesrewr data/{0}.pbes data/{0}.rw.pbes'.format(pbesbase))
            self.run_tool('pbesconstelm data/{0}.rw.pbes data/{0}.rw.ce.pbes'.format(pbesbase))
            self.run_tool('pbesparelm data/{0}.rw.ce.pbes data/{0}.rw.ce.pe.pbes'.format(pbesbase))
            self.run_tool('pbesrewr data/{0}.rw.ce.pe.pbes data/{0}.rw.ce.pe.rw.pbes'.format(pbesbase))
            if stage > 3:
              print('*** Solving {0}.rw.ce.pe.rw.pbes'.format(pbesbase))
              self.run_tool('pbespgsolve -srecursive -vrjittyc data/{0}.rw.ce.pe.rw.pbes > data/{0}.txt'.format(pbesbase))
    except RuntimeError:
      print('*** Aborted.')

def main():
  parser = argparse.ArgumentParser(description='Analyse the FlexRay startup protocol.')
  parser.add_argument('--mcrl2', dest='mcrl2', action='store_true',
                      help='Only expand startup.mcrl2 for all instances, do not analyse anything.')
  parser.add_argument('--lps', dest='lps', action='store_true',
                      help='Linearize the specification for all instance, but do not analyse.')
  parser.add_argument('--pbes', action='store_true',
                      help='Generate all PBESs, but do not solve them.')
  args = parser.parse_args()
  stage = 4
  if args.mcrl2:
    stage = 1
  if args.lps:
    stage = 2
  if args.pbes:
    stage = 3
  Testset().run(stage)

if __name__ == '__main__':
  main()
