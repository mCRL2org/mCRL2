from sphinx.domains.std import ProductionList
from sphinx.util import logging
from docutils import nodes
import os

logger = logging.getLogger(__name__)

class DParserGrammarDirective(ProductionList):
  option_spec = {'opt': str}
  required_arguments = 1
  final_argument_whitespace = True

  def run(self):
    try:
      names = self.arguments[0].split()
      #rst = '\n'.join([str(_PROD[name]) for name in names])
      rst=''
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

  return {'parallel_read_safe': True, 'parallel_write_safe': True}