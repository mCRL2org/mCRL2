from sphinx.domains.std import ProductionList
from sphinx.util.compat import make_admonition
from docutils import nodes
import os

_PROD = {}

def lstr(l):
  if isinstance(l, (list, tuple)):
    if len(l) == 1:
      return lstr(l[0])
    else:
      return ' '.join([lstr(x) for x in l])
  return str(l)

class Rule(object):
  def __init__(self, t):
    if len(t[0]):
      self.elts = t[0][0][0] + [[t[0][0][1], t[0][0][2]]]
    else:
      self.elts = []
  def __repr__(self):
    return ' '.join([' '.join([lstr(x) for x in [elt] + modifiers])
                     for elt, modifiers in self.elts])

class Production(object):
  def __init__(self, name, t):
    self.name = name
    self.rules = [t[0]] + [x[1] for x in t[1]]
  def __repr__(self):
    return self.name + ' : ' + (' |\n' + ' '*len(self.name) + ' : ').join([str(r) for r in self.rules])

class IdRef(object):
  def __init__(self, t):
    self.id = t[0][0]
  def __repr__(self):
    return '`{0}`'.format(self.id)

def d_parse_grammar(t):
  return t

def d_parse_id_ref(t):
  '''identifier_ref : identifier'''
  return IdRef(t)

def d_parse_rule(t):
  '''rule : (((element (element_modifier*))*) simple_element (element_modifier*))? 
            rule_modifier* 
            rule_code '''
  return Rule(t)
    

def d_parse_production(t):
  '''production : identifier ':' rules ';' 
                | identifier '::=' rules ';'
                | ';' '''
  if len(t) == 4:
    global _PROD
    _PROD[t[0][0]] = Production(t[0][0], t[2])

def getGrammar(filename):
  from dparser import Parser
  gramgram = open(os.path.join(os.path.dirname(__file__), 'grammargrammar.txt')).read()
  d_parse_grammar.__doc__ = gramgram
  parser = Parser()
  return parser.parse(open(filename).read()).getStructure()

class DParserGrammarDirective(ProductionList):
  option_spec = {'opt': str}
  required_arguments = 1
  final_argument_whitespace = True

  def run(self):
    try:
      names = self.arguments[0].split()
      rst = '\n'.join([str(_PROD[name]) for name in names])
      self.arguments = [rst]
      p = nodes.compound()
      p['classes'] += ['dparser', 'admonition', 'collapse']
      title = nodes.paragraph()
      title['classes'] += ['first', 'admonition-title']
      title += nodes.Text(' '.join(names))
      body = ProductionList.run(self)[0]
      body['classes'] += ['last']
      p += [title, body]
      return [p]
    except KeyError as e:
      self.state.document.reporter.severe("Unknown nonterminal: " + str(e))
      return []

def setup(app):
  app.add_directive('dparser', DParserGrammarDirective)
  try:
    getGrammar(os.path.join(os.path.dirname(__file__), 
                                  '..', '..', '..', 'specs', 'mcrl2-syntax.g'))
  except Exception as e:
    app.warn('DParser Python module not found. The dparser RST directive will '
             'not be processed correctly.')
    app.warn('While loading {}'.format(os.path.join(os.path.dirname(__file__),
                                  '..', '..', '..', 'specs', 'mcrl2-syntax.g')))
    app.warn('Exception: {}'.format(e))

if __name__ == '__main__':
  g = getGrammar(os.path.join(os.path.dirname(__file__), 
                                '..', '..', '..', 'specs', 'mcrl2-syntax.g'))
  print '!!!'
  print _PROD['SortExpr']
