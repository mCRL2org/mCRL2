from sphinx.domains.std import ProductionList
from sphinx.util import logging
from docutils import nodes

import tpg

import os

class DParserParser(tpg.Parser):
  r"""
    separator space: '\s+' ;
    separator comments: '//.*' ;
    separator option: '\$\{.*\}' ;

    token operator: '[+?*]' ;
    token literal: '\'[\S]*\'' ; 
    token literal2: '"[\S\s]*"' ; 
    token identifier: '[a-zA-Z\d]*' ;

    START/rules ->      $ rules = {}
      (
        RULE/r          $ (i, c) = r; rules[i] = '\n | '.join(c)
      )*
      ;

    RULE/r ->           
      identifier/i
      ':'               
      CASES/c      
      ';'               $ r = (i, c)
      ;

    CASES/cases ->      $ cases = []
      CASE/c            $ cases.append(c)
      (
        '\|'
        CASE/c          $ cases.append(c)
      )*
    ;

    CASE/c  ->                             $ tmp = []
      (                                    $ val = None; op = None
        (                                  
          '\(' CASE/d '\$[a-z_]*\s*\d*\)'  $ val = d
        | '\(' CASES/d '\)'                $ val = '({})'.format(' | '.join(d))
        | '\$[a-z_]* \s*-?\d*'
        | ATOM/a                           $ val = a
        )
        operator/op?                       $ if val: tmp.append(val + op) if op else tmp.append(val)
      )*                                   $ c = ' '.join(tmp)
    ;

      ATOM/a ->
          identifier/a
        | literal/a
        | literal2/a
      ;

   """
  
# Global variable since DParserGrammarDirective cannot have a constructor, blerg.
rules = {}
  
class DParserGrammarDirective(ProductionList):
  option_spec = {'opt': str}
  required_arguments = 1
  final_argument_whitespace = True

  def run(self):
    try:
      
      # For every production rule get its right hand side as a string
      names = self.arguments[0].split()
      rst = '\n'.join([str(rules[name]) for name in names])
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
  with open(os.path.join(os.environ['CMAKE_SOURCE_DIR'], 'libraries', 'core', 'source', 'mcrl2_syntax.g'), 'r') as file:
    global rules

    text = file.read()
    rules = DParserParser().parse('START', text)
    
    app.add_directive('dparser', DParserGrammarDirective)  
    
  return {'parallel_read_safe': False, 'parallel_write_safe': False}