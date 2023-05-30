import string
import os
import re

from manual import call
from sphinx.util import logging
from sphinx.util.console import colorize, term_width_line

# Some constants and abbreviations
_LOG = logging.getLogger(__name__)
_PWD = os.path.abspath(os.path.dirname(__file__))
_ARTICLEINDEXTEMPLATE= string.Template(
  open(os.path.join(_PWD, 'libraries_templates', 'articles.rst.template')).read())

def log_nonl(msg):
  _LOG.info(term_width_line(msg), nonl = True)

def log_generating_pdf(lib_dir):
  lib_dir = colorize('darkgray', f'({lib_dir})')
  log_nonl(f'compiling LaTeX documents in {lib_dir}...')

def makepdf(src):
  
  # For pdflatex we need to change the working directory to the directory
  # containing the source
  olddir = os.getcwd()
  os.chdir(os.path.dirname(src))

  try:
    title = re.search(r'\\title{(.*?)}', open(src).read(), re.DOTALL)
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
  finally:    
    os.chdir(olddir)
  return title

def generate_library_pdf(srcdir: str, dstdir: str):
  '''Search for LaTeX files in texdir, compile them and generate
  an index file called articles.rst.'''  

  log_generating_pdf(srcdir)
  titles = []

  # If there is no latex directory then skip
  if os.path.exists(srcdir):
    for f in os.listdir(srcdir):
      texdir = os.path.join(srcdir, f)

      if f.endswith('.tex'):
        basename = os.path.splitext(f)[0]

        # Turns out that sphinx only likes forward slashes
        log_nonl('Compiling pdf {}'.format(texdir))
        titles.append(':download:`{0} <latex/{1}.pdf>`'.format(makepdf(texdir), basename))

    # Write all the titles into the articles
    open(os.path.join(dstdir, 'articles.rst'), 'w+').write(
      _ARTICLEINDEXTEMPLATE.substitute(ARTICLES='* ' + '\n* '.join(titles))
    )

def generate_pdfs(): 
  docdir = os.path.join(_PWD, '..', 'developer_manual', 'libraries')

  for f in os.listdir(docdir):
    subdir = os.path.join(docdir, f, 'latex')
    generate_library_pdf(subdir, os.path.join(docdir, f))

  log_nonl('compiled LaTeX documents for all libraries')