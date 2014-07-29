Stella Solar Car
================

.. image:: img/Stella.jpg
   :align: right
   :width: 250px

In 2012 a group of students at Eindhoven University of Technology
decided to participate in the Michelin Cruiser Class of the Bridgestone World
Solar Challenge 2013 to be held in Australia. The goal was to build a solar powered 
family car capable to transport four persons and some
luggage in a comfortable way. The car was named Stella. 
The challenge took place from Darwin to Adelaide,
approximately 3000 kilometers straight through the Australian desert. 
The winner was determined by a complex set of criteria, based on
the travel time, the number of passengers, the extra electricity
required during the race, and the practicality of the car. In a field with quite fierce competition
the team from Eindhoven stood out by winning the challenge.

The approach to the software was quite unique for this kind of projects.
Directly from the beginning a formal design of the architecture of the
software was written down in mCRL2 and its correct functioning was 
established by verifying a large set of modal properties. 

This had two effects. The first one was that it forced those groups
responsible for the hardware (batteries, motor, solar-cells, console) to
think about their software interfaces in an early phase of the project.
The second effect was that when it came to the actual programming of
the software, this was a relatively effortless undertaking and finished 
on time. No serious problems were encountered in the software
after finishing it.

An interesting observation was that other cars required reset buttons
to overcome the unreliability of their software, which were not needed in Stella. There
was also a minor incident with melting electrical wires. Due to the
software this was reported immediately to the trailing car, and
the problem could be resolved on the go. As one of the team members
observed: "It is hard to know whether the quality of the
software was crucial in winning this race, but it was certainly a contributing factor".


Technical details
-----------------

A model of the behavioural software architecture was described in mCRL2. A set of essential
correctness requirements were formulated which were translated in the 
modal mu-Calculus with data. Both the formulas and the architecture went through
a series of adaptations and improvements until the model became stable. 
The final model had 50 levels, 553.616 states and 4.829.764 transitions. 
After reduction modulo branching bisimulation, this reduced to only 14.192 states
and 130.016 transitions.


*Type of verification*
   Deadlock, reachability and safety properties were either validated by explicit state-space 
   generation (breadth-first search) or translated to modal formulae which were subsequently verified.
 
*Equipment (computers, CPU, RAM)*
  The verification was done on a linux machine with 1Tbyte of main memory (Mammoth at
  Eindhoven University of Technology). State space generation could be done on regular
  8Gbyte main memory laptop.

*Models*
  The models and the architecture are confidential (as they might be interesting to
  the competition in the next World Solar Challenge). 

*Organizational context*
  :Contact person: Maikel Leemans, Student of the Master Computer Science and Engineering, 
                   Eindhoven University of Technology, The Netherlands.
  :Institution: Eindhoven University of Technology, The Netherlands.
  :Time period: The project was started in October 2012 and finished in Juli 2013.

