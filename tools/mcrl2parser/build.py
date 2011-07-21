import os
import re
from path import *

MCRL2SYNTAXFILE = '../../doc/specs/mcrl2-syntax.g'

grammars = {
  'mcrl2spec' : 'mCRL2Spec' ,
  'besspec'   : 'BesSpec'   ,
  'pbesspec'  : 'PbesSpec'  ,
  'dataspec'  : 'DataSpec'  ,
  'besexpr'   : 'BesExpr'   ,
  'dataexpr'  : 'DataExpr'  ,
  'pbesexpr'  : 'PbesExpr'  ,
  'procexpr'  : 'ProcExpr'  ,
  'actfrm'    : 'ActFrm'    ,
  'regfrm'    : 'RegFrm'    ,
  'statefrm'  : 'StateFrm'  ,
}

def make_grammars(grammars):
  for key in grammars.keys():
    text = path(MCRL2SYNTAXFILE).text()
    text = 'Start: %s ;\n\n' % grammars[key] + text
    path('%s.g' % key).write_text(text)

def compile_grammars(grammars):
  for key in grammars.keys():
    os.system('../../3rd-party/dparser-1.26/make_dparser -i %s -o %s.g.d_parser.cpp %s.g' % (key, key, key))

make_grammars(grammars)
compile_grammars(grammars)
