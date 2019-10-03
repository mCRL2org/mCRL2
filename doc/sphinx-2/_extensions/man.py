import os
import subprocess

from sphinx.util import logging
from sphinx.util.console import colorize, term_width_line


_LOG = logging.getLogger(__name__)
_PWD = os.path.dirname(__file__)
_TRUNK = os.path.abspath(os.path.join(_PWD, '..', '..', '..'))
_TOOLS = None
_RSTTAG = None


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


def setvars(temppath):
  global _TOOLS
  global _RSTTAG
  _TOOLS = os.path.join(_PWD, '..', 'user_manual', 'tools')
  _RSTTAG = os.path.join(temppath, 'man.cached')


def xsltproc(xml, transform, dst):
  rst = call('xsltproc', ['xsltproc', transform, '-'], xml)
  open(dst, 'w+').write(rst.decode('utf-8'))


def generate_manpage(tool, rstfile, binpath):
  exe = tool
  if binpath:
    exe = os.path.join(binpath, tool) # If this is a stage bin dir, it can be found in binpath directly
    if not os.path.exists(exe): # Otherwise, assume binpath is the root of the CMAKE binary dir
      exe = os.path.join(binpath, 'tools', tool, tool)
  try:
    xml = call(tool, [exe, '--generate-xml'])
    xsltproc(xml, os.path.join(_PWD, 'manual.xsl'), rstfile)
  except Exception as inst:
    _LOG.error('Could not generate man page for {0} in reStructuredText. Caught exception {1} with arguments {2}'.format(exe, type(inst), inst))


def generate_tool_documentation(temppath, binpath, tool_directory_path, tool_sub_directory_path):
  for tool in os.listdir(os.path.join(_TRUNK, tool_directory_path, tool_sub_directory_path)):
    if tool.startswith('.'):
      continue
    if tool=='CMakeLists.txt':
      continue
    log_nonl('generating user documentation... ' + colorize('darkgreen', tool))
    usr_rst = os.path.join(_TOOLS, tool_sub_directory_path, tool + '.rst')
    man_rst = os.path.join(_TOOLS, tool_sub_directory_path, 'man', tool + '.txt')
    # Writing RST fails if the target path does not exist
    if not os.path.exists(os.path.join(_TOOLS, tool_sub_directory_path, 'man')):
      os.makedirs(os.path.join(_TOOLS, tool_sub_directory_path, 'man'))
    generate_manpage(tool, man_rst, binpath)
    if os.path.exists(usr_rst):
      with open(usr_rst, 'r+') as usr_rst_handle:
        for line in usr_rst_handle:
          if f'include:: man/{tool}.txt' in line:
            break
        else: # not found
          usr_rst_handle.write(f'\n\n.. include:: man/{tool}.txt\n')
    elif os.path.exists(man_rst):
      _LOG.warning('No help available for {0}. Only man page will be available.'.format(tool))
      open(usr_rst, 'w+').write('.. index:: {0}\n\n.. _tool-{0}:\n\n{0}\n{1}\n\n.. include:: man/{0}.txt\n'.format(tool, '='*len(tool)))
    else:
      _LOG.warning('No documentation generated for {0}'.format(tool))


def generate_rst(temppath, binpath):
  setvars(temppath)

  if not os.path.exists(_RSTTAG):
    log_nonl('generating user documentation...')
    open(_RSTTAG, 'w+').close()
    generate_tool_documentation(temppath, binpath, 'tools', 'release')
    generate_tool_documentation(temppath, binpath, 'tools', 'experimental')
    log_nonl('generated user documentation for all tools')
    _LOG.info('')
