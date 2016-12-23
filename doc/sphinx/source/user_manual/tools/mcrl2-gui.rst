.. index:: mcrl2-gui

.. _tool-mcrl2-gui:

mcrl2-gui
=========

mcrl2-gui is a simple graphical interface that allows to run the most commonly used tools
in the mCRL2 toolset. 

At the left of the start-up menu there is a file browser. When right-clicking on a file,
a menu will pop-up. This menu contains standard operations such as deleting or renaming the file, and
offers the possibility to create new files or folders. If the extension of the file is meaningful
to a tool in the toolset, it can be selected under the **Analysis** or **Transformation** items.
In some cases there is also an option to start dedicated editors using **Editing**.
Typical files types for which tools exist are mcrl2 specification files (extionsion: .mcrl2), 
files with linear processes (extension: .lps), files with labelled transition systems (extensions:
.aut, .lts, .fsm), files with parameterised boolean equation systems (extension: .pbes).

When one of the tools is chosen a tabpage will pop up presenting a list of options to start the 
tool. After selecting the appropriate options the tool can be started using the run option. 
The command line equivalent of starting the tool is listed in the window at the bottom.
New files that are created occur in the file browser at the left. 

Note that multiple tools can be started simultaneously and note that tools can be stopped
to press the cancel button. Also note that a tool can easily be rerun by pressing run again.
If necessary this can be done with changed parameters, which can be set in the configuration 
window. 


.. warning:: 
   
   If the startup screen is blank the file browser may have been closed. Use **Reset Perspective**
   under **View** in the main menu to bring the file browser back. 

