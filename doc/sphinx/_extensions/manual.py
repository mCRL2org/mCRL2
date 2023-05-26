import os
import subprocess
import lxml.etree as ET

from sphinx.util import logging
from sphinx.util.console import colorize, term_width_line

_LOG = logging.getLogger(__name__)
_PWD = os.path.dirname(__file__)

def log_nonl(msg):
  _LOG.info(term_width_line(msg), nonl = True)

def call(name, cmdline, stdin=None, return_err=False):
  '''Runs cmdline and pipes stdin to the process' stdin. The standard output of
  the process is returned as a string. Any errors and warnings will be logged
  using the name provided. Raises a RuntimeError if the return code of the
  process was nonzero.'''
  _LOG.debug('Running "{0}"'.format(' '.join(cmdline)))
  proc = subprocess.Popen(cmdline,
           stdin=subprocess.PIPE,
           stdout=subprocess.PIPE,
           stderr=subprocess.PIPE)
  out, err = proc.communicate(stdin)

  if proc.returncode:
    _LOG.error('{0} returned with non-zero status.'.format(name))
    _LOG.error('{0} output: {1}'.format(name, out.strip()))
    _LOG.error('{0} error output: {1}'.format(name, err.strip()))
    raise RuntimeError('{0} returned with non-zero status.'.format(name))
  
  if err and not return_err:
    _LOG.warning('{0} error output: {1}'.format(name, err.strip()))

  if return_err:
      return (out, err)
  
  return out

def generate_manpage(tool, rstfile, binpath):
  ''' Execute the given binary with --generate-xml  '''

  exe = os.path.join(binpath, tool)
  xml = call(tool, [exe, '--generate-xml'])

  dom = ET.fromstring(xml)
  xslt = ET.parse(os.path.join(_PWD, 'manual.xsl'))
  transform = ET.XSLT(xslt)
  result = transform(dom)
  
  with open(rstfile, 'w') as file:
    file.write(str(result))

def generate_tool_documentation(srcpath, dstpath, binpath, tools):
  for tool in os.listdir(srcpath):
    if tool in tools:
      log_nonl('generating user documentation... ' + colorize('darkgreen', tool))
      usr_rst = os.path.join(dstpath, tool + '.rst')
      man_rst = os.path.join(dstpath, 'man', tool + '.rst')

      # Writing RST fails if the target path does not exist
      if not os.path.exists(os.path.join(dstpath, 'man')):
        os.makedirs(os.path.join(dstpath, 'man'))

      generate_manpage(tool, man_rst, binpath)
      if os.path.exists(usr_rst):
        with open(usr_rst, 'a') as usr_rst_handle:
          usr_rst_handle.write(f'\n\n.. include:: man/{tool}.rst\n')
      elif os.path.exists(man_rst):
        _LOG.warning('No help available for {0}. Only man page will be available.'.format(tool))
        open(usr_rst, 'w+').write('.. index:: {0}\n\n.. _tool-{0}:\n\n{0}\n{1}\n\n.. include:: man/{0}.rst\n'.format(tool, '='*len(tool)))

    else:
      _LOG.warning('No documentation generated for {0}'.format(tool))


def generate_rst(srcpath, binpath, tools):
  dstdir=os.path.join(_PWD, '..', 'user_manual', 'tools')

  log_nonl('generating user documentation...')
  generate_tool_documentation(os.path.join(srcpath, 'tools', 'release'), os.path.join(dstdir, 'release'), binpath, tools)
  generate_tool_documentation(os.path.join(srcpath, 'tools', 'experimental'), os.path.join(dstdir, 'experimental'), binpath, tools)
  log_nonl('generated user documentation for all tools')
