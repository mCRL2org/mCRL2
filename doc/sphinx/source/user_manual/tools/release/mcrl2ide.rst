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

.. note::

   Although mcrl2ide is marked as a release tool, it is still in developement.

.. note::

   Known issues in the 201808.0 release version:

   * For Mac users: mcrl2ide uses relative paths to find tools. Due to this,
     mcrl2ide cannot find the other tools when starting it via Finder. It does
     work when mcrl2ide is started from the terminal.
   * Adding and editing properties does not work well. "Parse" always saves the
     property to file, "Add"/"Edit" does not save the property to file and
     intermediate property files may not be deleted. This may lead to
     situations where the state of the IDE is different from the state in the
     file system, which may lead to confusing results when using these
     properties. To keep the state of the IDE consistent with the file system
     always press "Parse" at least once just before pressing "Add"/"Edit" and do
     not close the dialog without pressing "Add"/"Edit". One can also reopen the
     project to reload the properties in the IDE from the file system.

   These issues have been fixed in the latest version
