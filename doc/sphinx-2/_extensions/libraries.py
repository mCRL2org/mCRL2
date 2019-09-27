import subprocess
import string
import os
import re

from man import call
from sphinx.util import logging
from sphinx.util.console import colorize, term_width_line

#
# Some constants and abbreviations
#
_LOG = logging.getLogger(__name__)
_PWD = os.path.abspath(os.path.dirname(__file__))
_TRUNK = os.path.abspath(os.path.join(_PWD, '..', '..', '..'))
_XML = None
_RST = None
_DOXYTAG = None
_RSTTAG = None
_PDFTAG = None
_LIBRARIES = {
  'atermpp': 'ATerm++',  'bes': 'BES',                      'core': 'Core',
  'data': 'Data',        'gui': 'GUI',                      'lps': 'LPS',
  'lts': 'LTS',          'modal_formula': 'Modal Formula',  'smt': 'SMT',
  'pbes': 'PBES',        'process': 'Process',
  'trace': 'Trace',      'utilities': 'Utilities'
}
_DOXYTEMPLATE = string.Template(
  open(os.path.join(_PWD, 'libraries_templates', 'doxy.template')).read())
_REFINDEXTEMPLATE = string.Template(
  open(os.path.join(_PWD, 'libraries_templates', 'reference.rst.template')).read())
_ARTICLEINDEXTEMPLATE= string.Template(
  open(os.path.join(_PWD, 'libraries_templates', 'articles.rst.template')).read())


def log_nonl(msg):
  _LOG.info(term_width_line(msg), nonl = True)

def log_generated(filename):
  # log only the last two components of the path (library/file)
  filename = colorize('darkgreen', str.join('/', filename.split('/')[-2:]))
  log_nonl(f'transforming Doxygen XML into reST {filename}')

def log_generating_pdf(lib_dir, lib_name):
  lib_name = colorize('darkgreen', lib_name)
  lib_dir = colorize('darkgray', f'({lib_dir})')
  log_nonl(f'compiling LaTeX documents for library {lib_name} {lib_dir}...')

def log_generating_xml(lib_dir, lib_name):
  lib_name = colorize('darkgreen', lib_name)
  lib_dir = colorize('darkgray', f'({lib_dir})')
  log_nonl(f'generating Doxygen XML for library {lib_name} {lib_dir}...')


def setvars(temppath):
  global _XML
  global _RST
  global _DOXYTAG
  global _RSTTAG
  global _PDFTAG
  _XML = os.path.join(temppath, 'xml')
  _RST = os.path.join(_PWD, '..', 'developer_manual')
  _DOXYTAG = os.path.join(temppath, 'doxy.cached')
  _RSTTAG = os.path.join(temppath, 'doxyrst.cached')
  _PDFTAG = os.path.join(temppath, 'libpdf.cached')


#
# Generation functions
#
def doxygen(**kwargs):
  '''Run Doxygen. The configuration used is generated by applying the keyword
  arguments to this function to _DOXYTEMPLATE.'''
  call('Doxygen', ['doxygen', '-'],
       _DOXYTEMPLATE.substitute(kwargs).encode('utf-8'), return_err = True)


def xsltproc(src, transform, dst, xmldir):
  rst = call('xsltproc',
       ['xsltproc', '--param', 'dir', xmldir, transform, src])
  open(dst, 'w+').write(rst.decode('utf-8'))


def makepdf(src):
  title = re.search(r'\\title{(.*?)}', open(src + '.tex').read(), re.DOTALL)
  title = title.group(1) if title else os.path.basename(src)
  title = ' '.join(title.splitlines())
  if '{' in title or '\\' in title:
    title = os.path.basename(src)
  call('pdflatex', ['pdflatex', src])
# Only apply bibtex if a "source.bib" file exists in the directory.
  if os.path.isfile(src + '.bib'):
    try:
      call('bibtex', ['bibtex', src])
      call('pdflatex', ['pdflatex', src])
    except RuntimeError:
      pass
  call('pdflatex', ['pdflatex', src])
  return title


def generate_library_xml(lib_dir, lib_name):
  '''Generate Doxygen XML for a single library.'''
  lib_path = os.path.join(_TRUNK, 'libraries', lib_dir)
  log_generating_xml(lib_dir, lib_name)
  xml_path = os.path.join(_XML, 'libraries', lib_dir)
  if not os.path.exists(xml_path):
    os.makedirs(xml_path)
  doxygen(
    INPUT='{0}/include {0}/source'.format(lib_path),
    PROJECT_NAME=lib_name,
    PROJECT_NUMBER='unknown',
    STRIP_FROM_PATH=lib_path,
    STRIP_FROM_INC_PATH='{0} {1}'.format(
      os.path.join(lib_path, 'include'),
      os.path.join(lib_path, 'source')
    ),
    XML_OUTPUT=xml_path
  )


def generate_xml():
  '''Generate doxygen XML for all libraries.'''
  for dirname, name in _LIBRARIES.items():
    generate_library_xml(dirname, name)
  log_nonl('generated Doxygen XML for all libraries')
  _LOG.info('')


def generate_library_pdf(lib_dir, lib_name):
  '''Search for LaTeX files in lib_dir/latex, compile them and generate
  an index file called articles.rst.'''
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
      open(os.path.join('..', 'articles.txt'), 'w+').write(
        _ARTICLEINDEXTEMPLATE.substitute(ARTICLES='* ' + '\n* '.join(titles))
      )
    finally:
      os.chdir(olddir)


def generate_library_rst(lib_dir):
  '''Generate reStructuredText documentation for a single library.'''
  xml_path = os.path.join(_XML, 'libraries', lib_dir)
  rst_path = os.path.join(_RST, 'libraries', lib_dir)
  refindex = os.path.join(rst_path, 'reference.rst')
  transform = os.path.join(_PWD, 'libraries_templates', 'compound.xsl')
  if not os.path.exists(rst_path):
    os.makedirs(rst_path)
  classrst = []
  headerrst = []
  for f in os.listdir(xml_path):
    base, ext = os.path.splitext(f)
    if ext == '.xml' and base != "index":
      src = os.path.join(xml_path, f)
      dst = os.path.join(rst_path, base + '.rst')
      if base.startswith('class') and not base.startswith('classstd') and not base.endswith('_8h'):
        classrst.append(base)
        xsltproc(src, transform, dst, "'{0}'".format(xml_path))
        log_generated(dst)
      elif base.endswith('_8h'):
        headerrst.append(base)
        xsltproc(src, transform, dst, "'{0}'".format(xml_path))
        log_generated(dst)
  open(refindex, 'w+').write(_REFINDEXTEMPLATE.substitute(
    CLASSES='\n   '.join(sorted(classrst)),
    FILES='\n   '.join(sorted(headerrst))
  ))
  log_generated(refindex)


def generate_rst(temppath):
  '''Generate reStructuredText documentation for all libraries.'''
  setvars(temppath)

  doxychanged = False

  if os.path.exists(_DOXYTAG):
    _LOG.info('assuming Doxygen XML is up-to-date')
  else:
    open(_DOXYTAG, 'w+').close()
    doxychanged = True
    generate_xml()
  if os.path.exists(_RSTTAG) and not doxychanged:
      _LOG.info('assuming Doxygen reST is up-to-date')
  else:
    open(_RSTTAG, 'w+').close()
    for dirname in _LIBRARIES:
      generate_library_rst(dirname)
    log_nonl('transformed all library XML into reST')
    _LOG.info('')
  if os.path.exists(_PDFTAG):
      _LOG.info('assuming generated PDF is up-to-date')
  else:
    open(_PDFTAG, 'w+').close()
    for dirname, name in _LIBRARIES.items():
      generate_library_pdf(dirname, name)
    log_nonl('compiled LaTeX documents for all libraries')
    _LOG.info('')
