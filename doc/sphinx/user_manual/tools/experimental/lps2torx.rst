.. index:: lps2torx

.. _tool-besconvert:

lps2torx
==========

lps2torx is intended to do model based testing using the JTorx environment.
It can be invoked (on a Windows environment) by::

   "lps2torx.exe" "your-mcrl2-model.lps"

You should note the following

#. Ensure that lps2torx.exe is in your system path.

#. Start up JTorX with the "jtorx" shell script or with "jtorx.bat".
   Navigate to the "Config Items" tab (should be active by default). 
   Set the “Model” field to the location of the helper file (see Listing 1), e.g. “D:/jtorx-helper.tx.bat”.

#. Set the “Implementation” field to “real program, comm. labels via tcp, JTorX is server”. Set the text field below to the address of your SUT, e.g. “127.0.0.1:5555”.

#. Set “Timeout” appropriately.

#. Set “Interpretation” to “action names below”.

#. Set “Trace kind” to “Straces”.

The picture below shows the setup screen of JTorx. 
.. image:: _pictures/jtorx-setup.png

