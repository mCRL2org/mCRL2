.. index:: mcrl2ide

.. _tool-mcrl2ide:

mcrl2ide
=========

mcrl2ide is a graphical integrated development environment aimed at new users to
use the mCRL2 tool-set and its language. With this tool the user can

* write and validate an mCRL2 specification
* simulate the mCRL2 specification
* create, reduce and view the state space of the mCRL2 specification
* define properties and verify them on the mCRL2 specification
* create and view the evidence for these properties

..note::

   Although mcrl2ide is marked as a release tool, it is still in developement.

..note::

   Known issues in the 201808.0 release version:
   
   * For Mac users: mcrl2ide uses relative paths to find tools. Due to this,
     mcrl2ide cannot find the other tools when starting it via Finder. It does
     work when mcrl2ide is started from the terminal.
   * When pressing "Add" or "Edit" when adding or editing a property, the
     property is not saved to file. It is saved when pressing "Parse", so you
     should parse a property before pressing "Add" or "Edit". For this the
     result of parsing does not matter.
