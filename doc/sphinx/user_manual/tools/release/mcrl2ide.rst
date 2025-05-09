.. index:: mcrl2ide

.. _tool-mcrl2ide:

mcrl2ide
========

mcrl2ide is a graphical integrated development environment aimed at new users to use the mCRL2 tool-set and its language.
With this tool the user can

* write and validate an mCRL2 specification
* simulate the mCRL2 specification
* create, reduce and view the state space of the mCRL2 specification
* define mu-calculus and equivalence properties and verify them on the mCRL2 specification
* create and view the evidence for mu-calculus properties

.. note::

   Although mcrl2ide is marked as a release tool, it is still in development.


Projects
^^^^^^^^

To use the capabilities of mcrl2ide one needs to have a project first.
To create a new project use the "Create New Project" button in the "File" menu or on the toolbar.
This will create a project folder in the chosen destination.
To open a project, use the "Open Project" button in the "File" menu or on the toolbar.
One can also open a project at startup by adding the path to the project folder as command line argument.

Each project consists of one mCRL2 specification and zero or more properties.
The mCRL2 specification can be edited in the editor in the center of the main window.
To save the mCRL2 specification to the project folder, use the "Save Project" button in the "File" menu or on the toolbar.
To parse the mCRL2 specification, use the "Parse Specification" button in the "Tools" menu or on the toolbar.

To save the project folder under a new name, use the "Save Project As" button in the "File" menu.


Simulation and state space generation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To simulate the current specification, use the "Simulate Specification" button in the "Tools" menu or on the toolbar.
This will open the dialog of lpsxsim in which one can select actions to simulate traces in the given specification.

To create and show the state space of the current specification, use the "Show State Space" button in the "Tools" menu or on the toolbar.
This will open the dialog of ltsgraph in which one can see and interact with the state space of the given specification in the form of a labelled transition system.
One can also choose to create and show a reduced state space according to a given equivalence relation.
This can be done using the the "Show Reduced State Space" button in the "Tools" menu or on the toolbar.


Properties
^^^^^^^^^^

To add a property to the project, use the "Add Property" button in the "File" menu or on the toolbar, which opens a property dialog.
Each property is required to have a name, which can be defined in the topmost input field in this dialog.
There are two types of properties: mu-calculus properties and equivalence properties.

To create a mu-calculus property the "Mu-calculus" tab should be active in the property dialog.
This tab contains a text field to fill in a mu-calculus property.
Verifying this property means verifying whether the mu-calculus property is true on the current specification.

To create an equivalence property the "Equivalence" tab should be active in the property dialog.
This tab contains two text fields for process expressions and one field for choosing an equivalence relation.
Any references used in the process expressions, such as actions and process variables, should be defined in the mCRL2 specification.
Verifying this property means verifying whether the two given process expressions are equivalent under the given equivalence relation.

To parse the property, use the "Parse" button at the bottom of the property dialog.
To save the property to the project, use the "Save" button at the bottom of the property dialog.
An entry for this property will then appear in the properties dock, situated at the right-hand side of the main window by default.
Properties can also be added to the project by importing them from another project by using the "Import Properties" button in the "File" menu.

Each entry in the properties dock consists of the name of the property and three buttons.
The first button in the entry with a green "play" icon verifies the property on the current specification.
While the verification is running, it changes to an "Abort" button with a red "stop" icon which can be used to abort the verification.
When the property has been verified, the button changes to a "Show witness" button in case it is true and a "Show counterexample" button in case it is false.
Additionally, the background of the entry becomes green in case the property is true and red in case the property is false.
For mu-calculus properties, the "Show witness" and "Show counterexample" buttons can be used to show why the property is true or false in the form of a labelled transition system.
For equivalence properties this has not been implemented yet.
The second button in the entry with a blue "pencil" icon opens the property dialog so that the property can be edited.
The last button in the entry with a red "trash can" icon removes the property from the project.

To verify all properties in the project one can use the "Verify all Properties" button in the "Tools" menu or on the toolbar.


Running tools
^^^^^^^^^^^^^

To enable most of the functionality described above mcrl2ide uses the tools :ref:`tool-mcrl22lps`, :ref:`tool-lpsxsim`, :ref:`tool-lps2lts`, :ref:`tool-ltsconvert`, :ref:`tool-ltscompare`, :ref:`tool-ltsgraph`, :ref:`tool-lps2pbes` and :ref:`tool-pbessolve`.
Buttons that spawn a process that uses a subset of these tools can usually be identified by having an icon with a green "play" icon in it.
When a process is running it can be aborted by the same button used to run the process.
This is indicated in the icon of the button when the "play" icon changes to a red "stop" icon.

The tools used may provide output to indicate progress or results.
This output is shown in the console dock, situated at the bottom of the main window by default.
This console dock has multiple tabs for different types of processes and also shows the progress of the running process.
Only one process can be run concurrently per process type.

Most processes create intermediate files (lps, lts or pbes files) that are produced by one tool and used by another.
These files are by default placed outside the project in a temporary directory.
In the "Options" menu one can select intermediate files to be saved to the project folder instead.


Known issues
^^^^^^^^^^^^

Known issues in the 201808.0 release version:

* For Mac users: mcrl2ide uses relative paths to find tools.
  Due to this, mcrl2ide cannot find the other tools when starting it via Finder.
  It does work when mcrl2ide is started from the terminal.
* Adding and editing properties does not work well.
  "Parse" always saves the property to file, "Add"/"Edit" does not save the property to file and intermediate property files may not be deleted.
  This may lead to situations where the state of the IDE is different from the state in the file system, which may lead to confusing results when using these properties.
  To keep the state of the IDE consistent with the file system always press "Parse" at least once just before pressing "Add"/"Edit" and do not close the dialog without pressing "Add"/"Edit".
  One can also reopen the project to reload the properties in the IDE from the file system.

These issues have been fixed in the latest 201908.0 release version

.. mcrl2_manual:: mcrl2ide
