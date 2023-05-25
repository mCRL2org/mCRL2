import string
import os
import re

from manual import call
from sphinx.util import logging
from sphinx.util.console import colorize, term_width_line

#
# Some constants and abbreviations
#
_LOG = logging.getLogger(__name__)
_PWD = os.path.abspath(os.path.dirname(__file__))
_LIBRARIES = {
  'atermpp': 'ATerm++',  'bes': 'BES',                      'core': 'Core',
  'data': 'Data',        'gui': 'GUI',                      'lps': 'LPS',
  'lts': 'LTS',          'modal_formula': 'Modal Formula',  'smt': 'SMT',
  'pbes': 'PBES',        'process': 'Process',
  'trace': 'Trace',      'utilities': 'Utilities'
}

_ARTICLEINDEXTEMPLATE= string.Template(
  open(os.path.join(_PWD, 'libraries_templates', 'articles.rst.template')).read())

def log_nonl(msg):
  _LOG.info(term_width_line(msg), nonl = True)

def log_generating_pdf(lib_dir, lib_name):
  lib_name = colorize('darkgreen', lib_name)
  lib_dir = colorize('darkgray', f'({lib_dir})')
  log_nonl(f'compiling LaTeX documents for library {lib_name} {lib_dir}...')

def makepdf(src):
  title = re.search(r'\\title{(.*?)}', open(src + '.tex').read(), re.DOTALL)
  title = title.group(1) if title else os.path.basename(src)
  title = ' '.join(title.splitlines())
  if '{' in title or '\\' in title:
    title = os.path.basename(src)
  call('pdflatex', ['pdflatex', src])
  
  # Only apply bibtex if a "source.bib" file exists in the directory.
  if os.path.isfile(src + '.bib'):
    call('bibtex', ['bibtex', src])
    call('pdflatex', ['pdflatex', src])

  call('pdflatex', ['pdflatex', src])
  return title

def generate_library_pdf(lib_dir, lib_name):
  '''Search for LaTeX files in lib_dir/latex, compile them and generate
  an index file called articles.rst.'''  
  _RST = os.path.join(_PWD, '..', 'developer_manual')
  subdir = os.path.join('libraries', lib_dir, 'latex')
  texdir = os.path.join(_RST, subdir)

  if os.path.exists(texdir):
    olddir = os.getcwd()
    os.chdir(texdir)
    log_generating_pdf(lib_dir, lib_name)
    titles = []
    try:
      for f in os.listdir('.'):
        if f.endswith('.tex'):
          fn = os.path.splitext(f)[0]
          titles.append(':download:`{0} <{2}/{1}.pdf>`'.format(makepdf(fn), fn, subdir))
      open(os.path.join('..', 'articles.rst'), 'w+').write(
        _ARTICLEINDEXTEMPLATE.substitute(ARTICLES='* ' + '\n* '.join(titles))
      )
    finally:
      os.chdir(olddir)

def generate_pdfs(temppath): 
  for dirname, name in _LIBRARIES.items():
    generate_library_pdf(dirname, name)

  log_nonl('compiled LaTeX documents for all libraries')