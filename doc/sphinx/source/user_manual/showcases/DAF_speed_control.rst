:authors: Rob Schoren, Jan Friso Groote
:date: 13 March 2012

DAF Trucks Vehicle Function Architecture
========================================

.. image:: img/DAF.jpg
   :align: right
   :width: 250px

The designers at DAF Trucks N.V. model the software for their vehicle functions
in the Matlab Simulink environment. These models were translated to the mCRL2
modelling language in order to perform formal verification. As a first stage,
the cruise control system---that was implemented as a single Simulink 
block---was analysed. When this approach proved successful, the complete
Vehicle Function Architecture, a system that contains 9 interconnected Simulink
components, was translated to mCRL2 and analysed.

Technical Details
-----------------

*Type of verification*
   We performed formal verification on the two mCRL2 models using a set of safety properties provided by DAF designers,
   which we translated to modal formulae. For both models several requirements were unsatisfied, revealing previously
   unknown problems in the original Simulink models. These problems have been fixed in the simulink models.

*Data Size*
   The state space of the Cruise Control system is only 471 states, while it has 850.000 transitions.
   The VFA system has a state space of only 471.000 states, but over 113 million transitions. The translation
   to mCRL2 has been carefully engineered to keep the number of states small.

*Equipment*
   The verification and state space generation was performed on 56 Intel Xeon E5520 Processors with 1TB
   of shared main memory running a 64-bit Linux distribution.

*Organizational context*
   :Contact person:          Jan Friso Groote, Technische Universiteit Eindhoven, The Netherlands.
   :Other people involved:  Rob Schoren (Student, TU/e, Eindhoven)
   :Institution:             Technische Universiteit Eindhoven, The Netherlands.
   :Industrial partner:      DAF Trucks N.V., Eindhoven, The Netherlands
   :Time period:             September 2011 -- March 2012
