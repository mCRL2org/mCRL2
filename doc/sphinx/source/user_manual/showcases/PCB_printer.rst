PCB Printer
===========

.. image:: img/Oce_Arizona_250-web.jpg
   :align: right
   :width: 250px

The Lunaris is an Etch Resist Printer, intended to operate in the
manufacturing of printed circuit boards (PCBs). In current PCB production
processes, the substrate is laminated with a photo resist and using a
lithographic process the desired photo mask is created on the substrate. With the
development of the Lunaris, it is possible to skip the expensive task of creating
the mask that is required for illuminating the photo resist.
By directly printing the resist in the desired pattern, it is possible to
create customized and individual PCBs at lower costs.

At controller level, the Lunaris consists of 245 multi-threaded tasks (running in parallel)
that are implemented in C#. The tasks specify behaviour for amongst others logging and error handling. In total
170.000 lines of code are needed to implement the behaviour. The code is distributed over 120 classes in 40 files.

The method used in this case study, consists of creating an over-approximation of the behaviour by abstracting from the values of program variables. The derived model, consisting of interface calls between processes,
is checked for various safety properties with the mCRL2 toolset.

Technical details
-----------------
The communication layer of the controller was modelled in mCRL2 based on 
C# code after a rigorous abstraction method. 
 
*Type of verification*
  Deadlock and safety properties violations detection by solving PBES equations.
 
*Equipment (computers, CPU, RAM)*
  All the experiments were performed on an Pentium D930 with 2Gb RAM. 

*Organizational context*
  :Contact person: Frank Stappers, Technische Universiteit Eindhoven, The 
                   Netherlands.
  :Other people involved: - Michel Reniers (TU/e Eindhoven)
                          - Marc Vanmaris (NBG)
                          - Tjeu Naus (NBG)
  :Institution: Technische Universiteit Eindhoven, The Netherlands.
  :Industrial partner: `NBG <http://www.nbg-industrial.nl>`_: a mid-size hard 
                       and software developer for industrial or medical
                       applications, located in Nederweert, The Netherlands.
  :Project: The work was supported by ITEA `TWINS <http://www.twins-itea.org>`_ 
            project, project 05004.
  :Time period: Main project including the modeling and verification took place
                between 2007-11 -- 2008-12 (One day a week).

Publications
------------

.. [SR09] "Verification of safety requirements for program code using data abstraction",
   F.P.M. Stappers and M.A.Reniers. Proc. Ninth International Workshop on Automated Verification of Critical Systems (AVoCS 2009),
   Swansea, UK, September 23-25, 2009.
   `(DOI) <http://eceasst.cs.tu-berlin.de/index.php/eceasst/article/view/311>`_

