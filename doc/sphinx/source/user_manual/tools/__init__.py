import os
import logging
from ... import call

_LOG = logging.getLogger('user_manual')
_PWD = os.path.dirname(__file__)
_TRUNK = os.path.abspath(os.path.join(_PWD, '..', '..', '..', '..', '..'))
_TOOLS = os.path.join(_TRUNK, 'doc', 'sphinx', '_temp', 'rst', 'user_manual', 'tools')
_SRC = os.path.join(_TRUNK, 'doc', 'sphinx', 'source', 'user_manual', 'tools')

def xsltproc(xml, transform, dst):
  rst = call('xsltproc', 
       ['xsltproc', transform, '-'], xml)
  open(dst, 'w+').write(rst)

def generate_manpage(tool, rstfile, binpath):
  try:
    exe = os.path.join(binpath, tool) if binpath else tool
    xml = call(tool, [exe, '--generate-xml'])
    xsltproc(xml, os.path.join(_PWD, 'manual.xsl'), rstfile)
  except:
    _LOG.error('Could not generate man page for {0} in reStructuredText'.format(tool))

def generate_rst(binpath):
  if not os.path.exists(os.path.join(_TOOLS, 'man')):
    os.makedirs(os.path.join(_TOOLS, 'man'))
  for tool in os.listdir(os.path.join(_TRUNK, 'tools')):
    if tool.startswith('.'):
      continue
    _LOG.warning(tool)
    usr_rst = tool + '.rst'
    man_rst = os.path.join('man', usr_rst)
    usr_rst = os.path.join(_TOOLS, usr_rst)
    man_rst = os.path.join(_TOOLS, man_rst)
    generate_manpage(tool, man_rst, binpath)
    if os.path.exists(os.path.join(_SRC, tool + '.rst')):
      open(usr_rst, 'a').write('\n\n.. include:: man/{0}.rst'.format(tool))
    else:
      _LOG.warning('No help available for {0}. Only man page will be available.'.format(tool))
      open(usr_rst, 'w+').write('{0}\n{1}\n.. include:: man/{0}.rst'.format(tool, '='*len(tool)))
