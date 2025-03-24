import os
import lxml.etree as ET

from sphinx.util import logging
from sphinx.util.console import term_width_line
from docutils.parsers.rst.roles import register_canonical_role
from docutils import nodes

from library import call

_LOG = logging.getLogger(__name__)
_PWD = os.path.dirname(__file__)

_MCRL2_TOOL_PATH = os.environ.get("MCRL2_TOOL_PATH")
_MCRL2_TOOLS = os.environ.get("MCRL2_TOOLS")

def log_nonl(msg):
    _LOG.info(term_width_line(msg), nonl=True)

def generate_manpage(tool, binpath):
    """Execute the given binary with --generate-xml"""

    exe = os.path.join(binpath, tool)
    xml = call(tool, [exe, "--generate-xml"])

    dom = ET.fromstring(xml)
    xslt = ET.parse(os.path.join(_PWD, "manual.xsl"))
    transform = ET.XSLT(xslt)
    return transform(dom)

def mcrl2_manual_role(
    _name, _rawtext, text, lineno, inliner, _options=None, _content=None
):
    """A role to replace the mcrl2_manual by the output of the tools --generate-xml converted into RST"""
    toolname = text

    if toolname in _MCRL2_TOOLS:
        return inliner.parse(generate_manpage(toolname, _MCRL2_TOOL_PATH), lineno, inliner, parent=None)
    else:
        inliner.document.reporter.severe(f"Tool {toolname} is not part of the build configuration")
        return ([], [])

def setup(app):
    register_canonical_role("mcrl2_manual", mcrl2_manual_role)

    return {"parallel_read_safe": True, "parallel_write_safe": True}
