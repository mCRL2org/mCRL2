IEEE 1394 link layer
====================


.. image:: img/FireWire_cables.jpg
   :align: right
   :width: 250px

The behavior of the Link Layer in asynchronous mode is specified in &mu;CRL.
This layer is the middle layer of the three-layered Firewire protocol,
responsible for construction of packets, the transmission of these over a serial
(one-bit) line to other parties, and the computation and verification of
checksums. Moreover, a Bus-process is specified, describing the external
behavior of the underlying physical components according to IEEE 1394, in order
to be able to simulate the situation where a number of Link Layers communicate
asynchronously.

Technical details
-----------------
Asynchronous mode of the Link Layer of IEEE 1394 was modeled in &mu;CRL based on the documentation.
 
Type of verification
^^^^^^^^^^^^^^^^^^^^
Deadlock, safety and liveness properties checking.

Models
^^^^^^
The &mu;CRL model is available as a part of the &mu;CRL toolset distribution. 
A translation to mCRL2, performed by Jan Friso Groote, is available as a part
of the mCRL2 distribution.

Organizational context
----------------------

.. list-table:: 

  * - **Contact person**
    - Bas Luttik, Technische Universiteit Eindhoven, The Netherlands.
  * - **Institution**
    - Centrum voor Wiskunde en Informatica (CWI), Amsterdam
  * - **Time period**
    - The model was written in 1997, translation to mCRL2 in 2005.

Publications
------------
.. [Lut97] Description and formal specification of the Link Layer of P1394.
   S. P. Luttik. In: Ignac Lovrek, editor, Proceedings of the 2nd International
   Workshop on Applied Formal Methods in System Design, University of Zagreb,
   Croatia. Also available as Report SEN-R9706, CWI, The Netherlands.

