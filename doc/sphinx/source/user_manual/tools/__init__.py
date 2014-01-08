import os
import logging
from ... import call

_LOG = logging.getLogger('user_manual')
_PWD = os.path.dirname(__file__)
_TRUNK = os.path.abspath(os.path.join(_PWD, '..', '..', '..', '..', '..'))
_TOOLS = None
_RSTTAG = None

def setvars(temppath, outpath):
  global _TOOLS
  global _RSTTAG
  _TOOLS = os.path.join(temppath, 'rst', 'user_manual', 'tools')
  _RSTTAG = os.path.join(temppath, 'man.cached') 

def xsltproc(xml, transform, dst):
  rst = call('xsltproc', 
       ['xsltproc', transform, '-'], xml)
  open(dst, 'w+').write(rst)

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

def generate_rst(temppath, outpath, binpath):
  setvars(temppath, outpath)

  if not os.path.exists(_RSTTAG):
    open(_RSTTAG, 'w+').close()
    for tool in os.listdir(os.path.join(_TRUNK, 'tools')):
      if tool.startswith('.'):
        continue
      _LOG.warning(tool)
      usr_rst = tool + '.rst'
      man_rst = os.path.join('man', tool + '.txt')
      usr_rst = os.path.join(_TOOLS, usr_rst)
      man_rst = os.path.join(_TOOLS, man_rst)
      # Writing RST fails if the target path does not exist
      if not os.path.exists(os.path.join(_TOOLS, 'man')):
        os.makedirs(os.path.join(_TOOLS, 'man'))
      generate_manpage(tool, man_rst, binpath)
      if os.path.exists(os.path.join(_PWD, tool + '.rst')):
        open(usr_rst, 'a').write('\n\n.. include:: man/{0}.txt'.format(tool))
      else:
        _LOG.warning('No help available for {0}. Only man page will be available.'.format(tool))
        open(usr_rst, 'w+').write('.. index:: {0}\n\n.. _tool-{0}:\n\n{0}\n{1}\n\n.. include:: man/{0}.txt'.format(tool, '='*len(tool)))
