# Configuration file for the Sphinx documentation builder.
# https://www.sphinx-doc.org/en/master/usage/configuration.html


# -- Imports -----------------------------------------------------------------

import datetime
import os
# used to parse mCRL2 version information from CMake file
from pathlib import Path
import re
# used to add paths for extensions
import sys


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
from man import call
src_path = Path(__file__).parent / '../..'
olddir = os.getcwd()
try:
    os.chdir(src_path)
    out = call('CMake', ['cmake', '-P', 'build/cmake/MCRL2Version.cmake']).decode('utf-8')
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
master_doc = 'user_manual/index'

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
    'dparser_grammar',
    'mcrl2_pygment',
    'sphinx.ext.graphviz',
    'sphinx.ext.ifconfig',
    'sphinx.ext.mathjax',
    'sphinx.ext.todo',
    'sphinx.ext.viewcode',
    'tikz'
]

# Extension configuration
tikz_latex_preamble = r'''
  \tikzstyle{every state}=[
    draw,
    shape=circle,
    inner sep=1pt,
    minimum size=5pt,
    final/.style={double,minimum size=6pt},
    initial text=]
'''

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = ['.svn', '.git', 'Thumbs.db', '.DS_Store']


# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
#
html_theme = 'mcrl2'
html_theme_path = ['_templates']
html_logo = '_templates/mcrl2/static/mcrl2.png'
html_sidebars = {
   '**': ['localtoc.html', 'relations.html', 'sourcelink.html', 'searchbox.html'],
}

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['_static']

# Tweaking how the "last updated" is displayed
today_fmt = '%d-%m-%Y'

# Suppress warnings about unreferenced citations (there are many on the publications page).
suppress_warnings = ['ref.citation']


# -- App setup - executed before the build process starts --------------------

def setup(app):
    # Generate rst files from man pages
    import libraries
    import man

    temppath = f'{app.outdir}/cache'
    os.makedirs(temppath, mode = 0o755, exist_ok = True)
    olddir = os.getcwd()
    try:
        os.chdir(temppath)
        libraries.generate_rst(temppath)
        man.generate_rst(temppath, f'{app.outdir}/../stage/bin/')
    finally:
        os.chdir(olddir)
