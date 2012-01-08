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
       (r'sum', Operator.Word),
       (r'mod', Operator.Word),
       (r'hide', Operator.Word),
       (r'allow', Operator.Word),
       (r'block', Operator.Word),
       (r'comm', Operator.Word),
       (r'lambda', Operator.Word),
       (r'if', Operator.Word),
       (r'in', Operator.Word),
       (r'forall', Operator.Word),
       (r'exists', Operator.Word),
       (r'mu', Operator.Word),
       (r'nu', Operator.Word),
       (r'whr', Operator.Word),
       (r'end', Operator.Word)
     ],
     'constant': [
       (r'tau', Keyword.Constant),
       (r'delta', Keyword.Constant)
     ],
     'keywords': [
       (r'map', Keyword.Declaration),
       (r'eqn', Keyword.Declaration),
       (r'glob', Keyword.Declaration),
       (r'proc', Keyword.Declaration),
       (r'pbes', Keyword.Declaration),
       (r'init', Keyword.Declaration),
       (r'var', Keyword.Declaration),
       (r'struct', Keyword.Declaration),
       (r'sort', Keyword.Declaration),
       (r'act', Keyword.Declaration),
       (r'val', Keyword.Declaration)
     ],
     'types': [
       (r'Bool', Keyword.Type),
       (r'Nat', Keyword.Type),
       (r'Pos', Keyword.Type),
       (r'Int', Keyword.Type),
       (r'Real', Keyword.Type)
     ],
     'boolean': [
       (r'true', Name.Constant),
       (r'false', Name.Constant)
     ],
     'identifier': [
       (r'[a-zA-z_][a-zA-Z0-9_]*', String)
     ],
     'root': [
       include('comment'),
       include('whitespace'),
       include('decimal'),
       include('operator'),
       include('builtin'),
       include('constant'),
       include('keywords'),
       include('types'),
       include('boolean'),
       include('identifier')
     ]
  }

def setup(app):
  from sphinx.highlighting import lexers
  lexers['mcrl2'] = mCRL2Lexer()


