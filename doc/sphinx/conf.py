# Configuration file for the Sphinx documentation builder.
# https://www.sphinx-doc.org/en/master/usage/configuration.html


# -- Imports -----------------------------------------------------------------

import datetime
import os
import re
import sys
import textwrap

# used to parse mCRL2 version information from CMake file
from pathlib import Path

import sphinx.errors

# The ReadTheDocs theme.
import sphinx_rtd_theme

_CMAKE_SOURCE_DIR = os.environ['CMAKE_SOURCE_DIR']
_MCRL2_TOOL_PATH = os.environ['MCRL2_TOOL_PATH']
_MCRL2_TOOLS = os.environ['MCRL2_TOOLS']
_DOXYGEN_EXECUTABLE = os.environ['DOXYGEN_EXECUTABLE']

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here.
sys.path.insert(0, (Path(__file__).parent / '_extensions').as_posix())

# -- Project information -----------------------------------------------------
project = 'mCRL2'
author = 'Technische Universiteit Eindhoven'
release = '' # populated below
version = ''

# run CMake on the version file to obtain the current version of mCRL2
from manual import call
olddir = os.getcwd()
try:
    os.chdir(_CMAKE_SOURCE_DIR)
    out = call('CMake', ['cmake', '-P',
        f'{_CMAKE_SOURCE_DIR}/build/cmake/MCRL2Version.cmake']).decode('utf-8')
    for line in iter(out.splitlines()):
        matches = re.findall(r'MCRL2_MAJOR_VERSION ([\S]+)', line)
        if matches:
            release = matches[0]
        matches = re.findall(r'MCRL2_MINOR_VERSION ([\S]+)', line)
        if matches:
            version = matches[0]
finally:
    os.chdir(olddir)
version = f'{release}.{version}'

# update copyright from current version
copyright = f'2011-{datetime.datetime.now().year}, {author}'


# -- General configuration ---------------------------------------------------

# The master document is in the user_manual directory.
master_doc = 'index'

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
    'mcrl2_pygment',
    'sphinx.ext.graphviz',
    'sphinx.ext.ifconfig',
    'sphinx.ext.mathjax',
    'sphinx.ext.todo',
    'sphinx.ext.viewcode',
    'sphinx_rtd_theme',
]

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = ['.git']

# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
#
html_theme = 'sphinx_rtd_theme'
html_theme_options = {
    'collapse_navigation': False,
    'logo_only': True,
    'style_external_links': True
}
html_logo = '_static/mcrl2.png'
html_sidebars = {
   '**': ['localtoc.html', 'relations.html', 'sourcelink.html', 'searchbox.html'],
}

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['_static']

html_css_files = [
    'css/mcrl2.css'
]

html_js_files = [
    'js/mcrl2.js'
]

# Tweaking how the "last updated" is displayed
today_fmt = '%d-%m-%Y'

# Suppress warnings about unreferenced citations (there are many on the publications page).
suppress_warnings = ['ref.citation']

# This should be illegal, but apparently you cannot pass a custom path for the
# doxygen executable to breathe (which is required for Windows where cmake can
# find Doxygen without it being in PATH)
os.environ["PATH"] += os.pathsep + os.path.dirname(_DOXYGEN_EXECUTABLE)

# First initialize the data structures for breathe and exhale. They are
# populated below.
breathe_projects = {"mCRL2": "./_doxygen/xml"}
breathe_default_project = "mCRL2"

# Setup the exhale extension
exhale_args = {
    # These arguments are required
    "containmentFolder":     "./exhale/",
    "rootFileName":          "mcrl2_reference.rst",
    "doxygenStripFromPath":  "..",
    # Heavily encouraged optional argument (see docs)
    "rootFileTitle":         "Source code reference",
    # Suggested optional arguments
    "createTreeView":        True,
    "exhaleExecutesDoxygen": False,
}

if tags.has('build_doxygen'):
    extensions.append('breathe')
    extensions.append('exhale')
    
# -- App setup - executed before the build process starts --------------------
def setup(app):
    import manual
    import pdflatex

    temppath = f'{app.outdir}/../temp'
    os.makedirs(temppath, mode = 0o755, exist_ok = True)
    olddir = os.getcwd()
    try:
        os.chdir(temppath)

        if tags.has('build_pdflatex'):
            pdflatex.generate_pdfs()

        if tags.has('build_manual'):
            manual.generate_rst(_CMAKE_SOURCE_DIR, _MCRL2_TOOL_PATH, _MCRL2_TOOLS)
    finally:
        os.chdir(olddir)
