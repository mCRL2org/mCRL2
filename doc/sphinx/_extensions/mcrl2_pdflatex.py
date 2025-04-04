import os
import re
from pathlib import Path

from sphinx.util import logging
from sphinx.util.console import colorize, term_width_line
from docutils.parsers.rst.roles import register_canonical_role

from library import call

# Some constants and abbreviations
_LOG = logging.getLogger(__name__)
_PWD = os.path.abspath(os.path.dirname(__file__))


def log_nonl(msg):
    _LOG.info(term_width_line(msg), nonl=True)

def makepdf(src):
    """For pdflatex we need to change the working directory to the directory containing the source"""
    # Turns out that sphinx only likes forward slashes
    olddir = os.getcwd()
    os.chdir(os.path.dirname(src))
    
    src_colored = colorize("darkgray", f"({src})")
    log_nonl(f"compiling LaTeX document {src_colored}...")

    try:
        call("latexmk", ["latexmk", "-f", "-pdf", src])
        
    finally:
        os.chdir(olddir)

# Regex to obtain "title <path>" from the role.
TEX_PATH = re.compile(r"(.*) <(.*)>")

def mcrl2_pdflatex_role(
    _name, _rawtext, text, lineno, inliner, _options=None, _content=None
):
    current_rst_file_path = os.path.dirname(inliner.document.settings._source)

    result = TEX_PATH.match(text)
    if result is not None:
        title = result.group(1)
        srctex = result.group(2)

        srctex = os.path.join(current_rst_file_path, srctex)
        if not os.path.exists(srctex):
            raise FileNotFoundError(f"File {srctex} not found")
        
        # Change the extension to .pdf
        p = Path(srctex)
        src = p.with_suffix(".pdf")
                
        makepdf(os.path.splitext(srctex)[0])
        return inliner.parse(f":download:`{title} </{src}>`", lineno, inliner, parent=None)
    else:
        inliner.document.reporter.severe(f"Invalid syntax for :mcrl2_pdflatex: role: {text}")
        return ([], [])

def setup(app):
    register_canonical_role("mcrl2_pdflatex", mcrl2_pdflatex_role)

    return {"parallel_read_safe": False, "parallel_write_safe": True}
