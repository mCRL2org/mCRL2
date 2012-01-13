Automatic Document Feeder
=========================

.. image:: img/ADFCopier.jpg
   :align: right
   :width: 250px

An Automatic Document Feeder (ADF) is an important component of a copier
machine. Its task is to feed a document to the scanner automatically, one sheet
at a time. We have analyzed the embedded software of an ADF prototype. We
constructed a model of the ADF in µCRL and expressed seven requirements in
the modal µ-calculus. Next, we used the µCRL and CADP toolsets to check
whether the system meets its requirements. This analysis revealed two errors in
the ADF and we proposed solutions to these problems. We showed that the system
that incorporates these solutions, meets all of the requirements. Also, we
showed that some requirements were too strict. We presented slightly weaker
versions of these requirements and showed that these do hold. In this sense, in
addition to finding errors in the ADF, our analysis also led to a better
understanding of the behaviour the system.

Technical details
-----------------

Type of verification
^^^^^^^^^^^^^^^^^^^^
Six safety properties and one liveness property were checked on the state space.
Four violations of properties were found, of which two indicated errors in the
system.

Data size
^^^^^^^^^
The original specification resulted in a state space of around 358.000 states.
The specification that incorporates the solutions to the errors found during
verification, has a state space of around 79.000 states.

Equipment (computers, CPU, RAM)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
The state space was generated in about 2 minutes on a Pentium 4, 3 GHz machine
with 1 GB of RAM. Model checking of the properties was done on the same machine.

Models
^^^^^^
The model is available as an appendix to [PS06]_.

Organizational context
----------------------

.. list-table:: 

  * - **Contact person**
    - Bas Ploeger, Technische Universiteit Eindhoven, The Netherlands.
  * - **Other people involved**
    - Lou Somers, Technische Universiteit Eindhoven, The Netherlands.
  * - **Institution**
    - Technische Universiteit Eindhoven, The Netherlands.
  * - **Industrial partner**
    - Océ Technologies B.V., Venlo, The Netherlands
  * - **Time period**
    - May 2005 -- August 2005

Publications
------------

.. [PS06] Bas Ploeger and Lou Somers. Analysis and Verification of an Automatic Document Feeder.
   CS-Report 06-25, Technische Universiteit Eindhoven (2006).
   `(PDF) <http://alexandria.tue.nl/extra1/wskrap/publichtml/200625.pdf>`_

.. [PS07] Bas Ploeger and Lou Somers. Analysis and Verification of an Automatic Document Feeder.
   In: "Proceedings of the 2007 ACM Symposium on Applied Computing (ACMSAC'07)", pages 1499-1505, Seoul, Korea. ACM, March 2007.
   `(DOI) <http://doi.acm.org/10.1145/1244002.1244324>`_

Links
-----
Another description of this showcase is available on the `CADP homepage <http://www.inrialpes.fr/vasy/cadp/case-studies/06-b-adf.html>`_.

