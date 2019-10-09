.. _showcase-load-balancer:

AIA ITP load-balancer
=====================

.. image:: ../../_static/showcases/ITP_Aia_r.png
   :align: right
   :width: 250px

The AIA company is one of the world leading companies if it comes to software
for print job distribution over document processors (high volume printers). The
core of the software consists of 7.5 thousand lines of C code. In order to
understand this job distribution process better, a large part of this software
system has been modeled and analyzed using mCRL2. Six critical issues were
discovered. Since the model was close to the code, all problems that were found
in the model, could be traced back to the actual code resulting in concrete
suggestions for improvement of the code. All in all, the analysis significantly
improved the quality of this system and led to its certification by the
Laboratory for Quality Software (`LaQuSo <http://www.laquso.com>`_).

Technical details
-----------------
Session Layer of the load-balancer implementation was modeled in mCRL2 based on
the C code. The underlying network socket administration layer and the upper application
layers were modeled in an abstract way.

*Type of verification*
   Deadlock and safety properties violations detection by explicit state-space
   generation (breadth-first search).

*Data size*
   For the case of 3 clients and 1 server configuration 1.9 billion states was
   generated.

*Equipment (computers, CPU, RAM)*
   Large experiments were performed on an 8-way AMD 64 bit machine with 128Gb
   RAM.

*Models*
   The model is available as an appendix of [EHSU07a]_.

*Organizational context*
   :Contact person: Yaroslav S. Usenko (LaQuSo Eindhoven)
   :Other people involved: - Marko van Eekelen (LaQuSo Nijmegen)
                           - Stefan ten Hoedt (AIA)
                           - René Schreurs (AIA)
   :Institution: The project was a cooperation between two branches of `LaQuSo
                 <http://www.laquso.com>`_: LaQuSo Nijmegen, a part of Radboud
                 Universiteit Nijmegen, and LaQuSo Eindhoven, a part of
                 Technische Universiteit Eindhoven
   :Industrial partner: `Aia Software B.V. <http://www.aia-itp.com>`_, a
                        mid-size software development company with headquaters
                        in Nijmegen, The Netherlands.
   :Project: The work was supported by `SenterNovem
             <http://www.senternovem.nl>`_ Innovation Voucher Inv053967.
   :Time period: Main project including the modeling and verification was the
                 first half of 2006. Later on verification of bigger instances
                 were performed.

Publications
------------

.. [EHSU07a] Y.S. Usenko, M. van Eekelen, S. ten Hoedt, R. Schreurs. Testing of inter-process communication and synchronization of ITP LoadBalancer software via model-checking. TUE Computer Science Reports 07-04. 2007.
