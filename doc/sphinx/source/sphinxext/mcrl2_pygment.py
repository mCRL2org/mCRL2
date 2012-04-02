from pygments.lexer import RegexLexer, include
from pygments.token import *

class mCRL2Lexer(RegexLexer):
  name = 'mCRL2'
  aliases = ['mcrl2']
  filenames = ['*.mcrl2']

  tokens = {
     'comment': [
       (r'%.*?$', Comment.Single)
     ],
     'whitespace': [
       (r'\s+', Whitespace)
     ],
     'decimal': [
       (r'[0]', Number),
       (r'[1-9][0-9]*', Number)
     ],
     'operator': [
       (r'\.', Operator),
       (r'\+', Operator),
       (r'\|', Operator),
       (r'&', Operator),
       (r'<', Operator),
       (r'>', Operator),
       (r':', Operator),
       (r';', Operator),
       (r'=', Operator),
       (r'#', Operator),
       (r'@', Operator),
       (r'\(', Operator),
       (r'\)', Operator),
       (r'{', Operator),
       (r'}', Operator),
       (r'\[', Operator),
       (r'\]', Operator),
       (r',', Operator),
       (r'!', Operator),
       (r'\*', Operator),
       (r'\?', Operator),
       (r'\\', Operator),
       (r'-', Operator),
       (r'\|\|_', Operator),
       (r'->', Operator)
     ],
     'builtin': [
       (r'sum\b', Operator.Word),
       (r'mod\b', Operator.Word),
       (r'div\b', Operator.Word),
       (r'hide\b', Operator.Word),
       (r'rename\b', Operator.Word),
       (r'allow\b', Operator.Word),
       (r'block\b', Operator.Word),
       (r'comm\b', Operator.Word),
       (r'delay\b', Operator.Word),
       (r'yaled\b', Operator.Word),
       (r'lambda\b', Operator.Word),
       (r'if\b', Operator.Word),
       (r'in\b', Operator.Word),
       (r'forall\b', Operator.Word),
       (r'exists\b', Operator.Word),
       (r'mu\b', Operator.Word),
       (r'nu\b', Operator.Word),
       (r'whr\b', Operator.Word),
       (r'end\b', Operator.Word)
     ],
     'constant': [
       (r'true\b', Name.Constant),
       (r'false\b', Name.Constant),
       (r'tau\b', Name.Constant),
       (r'nil\b', Name.Constant),
       (r'delta\b', Name.Constant)
     ],
     'keywords': [
       (r'map\b', Keyword.Declaration),
       (r'eqn\b', Keyword.Declaration),
       (r'cons\b', Keyword.Declaration),
       (r'glob\b', Keyword.Declaration),
       (r'proc\b', Keyword.Declaration),
       (r'pbes\b', Keyword.Declaration),
       (r'init\b', Keyword.Declaration),
       (r'var\b', Keyword.Declaration),
       (r'struct\b', Keyword.Declaration),
       (r'sort\b', Keyword.Declaration),
       (r'act\b', Keyword.Declaration),
       (r'val\b', Keyword.Declaration)
     ],
     'types': [
       (r'List\b', Keyword.Type),
       (r'Bag\b', Keyword.Type),
       (r'Set\b', Keyword.Type),
       (r'Bool\b', Keyword.Type),
       (r'Nat\b', Keyword.Type),
       (r'Pos\b', Keyword.Type),
       (r'Int\b', Keyword.Type),
       (r'Real\b', Keyword.Type)
     ],
     'identifier': [
       (r"[a-zA-z_'][a-zA-Z0-9_']*", String)
     ],
     'root': [
       include('keywords'),
       include('comment'),
       include('whitespace'),
       include('decimal'),
       include('operator'),
       include('builtin'),
       include('constant'),
       include('types'),
       include('identifier')
     ]
  }

def setup(app):
  from sphinx.highlighting import lexers
  lexers['mcrl2'] = mCRL2Lexer()


