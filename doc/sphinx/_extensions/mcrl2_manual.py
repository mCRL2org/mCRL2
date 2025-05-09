import os
import lxml.etree as ET

from sphinx.util.console import term_width_line
from sphinx.util.docutils import SphinxDirective
from sphinx.util import logging

from docutils.parsers.rst import Parser

from library import call

_LOG = logging.getLogger(__name__)
_PWD = os.path.dirname(__file__)

_MCRL2_TOOL_PATH = os.environ.get("MCRL2_TOOL_PATH")
_MCRL2_TOOLS = os.environ.get("MCRL2_TOOLS")


def log_nonl(msg):
    _LOG.info(term_width_line(msg), nonl=True)


def generate_manpage(tool, binpath):
    """Execute the given binary with --generate-xml and process the output with the manual.xsl stylesheet"""
    log_nonl(f"Generating manpage for {tool}...")

    exe = os.path.join(binpath, tool)
    xml = call(tool, [exe, "--generate-xml"])

    dom = ET.fromstring(xml)
    xslt = ET.parse(os.path.join(_PWD, "manual.xsl"))
    transform = ET.XSLT(xslt)

    return str(transform(dom))


class MCRL2ManualDirective(SphinxDirective):
    """A role to replace the mcrl2_manual by the output of the tools --generate-xml converted into RST"""

    required_arguments = 1
    final_argument_whitespace = True

    def run(self):
        toolname = self.arguments[0]

        if toolname in _MCRL2_TOOLS:
            try:
                return self.parse_text_to_nodes(
                    generate_manpage(toolname, _MCRL2_TOOL_PATH),
                    allow_section_headings=True,
                )
            except Exception as e:
                self.state.document.reporter.severe(
                    f"Failed to generate manual for {toolname}: {e}"
                )
        else:
            self.state.document.reporter.severe(
                f"Tool {toolname} is not part of the build configuration"
            )
        return []


def setup(app):
    app.add_directive("mcrl2_manual", MCRL2ManualDirective)

    return {"parallel_read_safe": True, "parallel_write_safe": True}
