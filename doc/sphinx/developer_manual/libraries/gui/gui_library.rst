The GUI library
=============================

The GUI library contains basic functionality for the graphical tools in the toolset.
The toolset is built using the Qt framework (https://www.qt.io).
The functionality of the GUI library resides in the namespace mcrl2::gui
and mcrl2::gui::qt.


.. toctree::
   :maxdepth: 1

   mcrl2ide


The qt_tool class
-----------------

The main class in the GUI library is the qt_tool class that allows to construct a new
graphical tool efficiently, using the tool construction framework of the mCRL2 toolset.


.. _gui-codeeditor:

The CodeEditor class
--------------------

``mcrl2::gui::qt::CodeEditor`` (``libraries/gui/include/mcrl2/gui/codeeditor.h``) is a text
editor widget shared by all graphical tools in the toolset that need to display or edit mCRL2
specifications or mu-calculus formulas.

It is built on top of ``QPlainTextEdit`` and bundles three cooperating classes:

``CodeHighlighter``
   A ``QSyntaxHighlighter`` subclass that applies keyword, operator, number and comment
   formatting rules to mCRL2 specification text or mu-calculus formula text. The active rule
   set and colour palette are selected via :func:`changeHighlightingRules`.

``LineNumberArea``
   A ``QWidget`` that is painted alongside the editor and displays the current line numbers.

``CodeEditor``
   The main widget. It creates a ``LineNumberArea``, instantiates the appropriate
   ``CodeHighlighter``, and implements standard editing operations: selecting, copying,
   cutting, pasting, undoing, redoing, zooming and syntax highlighting. The purpose of the
   editor (specification or formula) is set with :func:`setPurpose`, which also switches the
   highlighting rules accordingly.

Although it is used extensively by ``mcrl2ide``, the ``CodeEditor`` lives in the GUI library rather than in the tool itself, so that other GUI tools can share it.
