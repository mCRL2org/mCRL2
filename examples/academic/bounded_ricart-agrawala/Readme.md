## Bounded Ricart-Agrawala

This directory contains the models made by Myrthe Spronck during her seminar project on the Ricart-Agrawala mutual exclusion algorithm, supervised by Jeroen Keiren.

RA_original contains a model of the Ricart-Agrawala algorithm with bounded sequence numbers, as presented in *An Optimal Algorithm for Mutual Exclusion in Computer Networks* by Glenn Ricart and Ashok Agrawala (https://doi.org/10.1145/358527.358537). 

In the seminar project, two problems with this version of the algorithm were found:
- A deadlock is possible. Changing the placement of the semaphore calls to match the presentation of the algorithm from *Formal Verification of the Ricart-Agrawala Algorithm* by Ekaterina Sedletsky, Amir Pnueli and Mordechai Ben-Ari (https://doi.org/10.1007/3-540-44450-5_26) eliminates this problem.
- A mutual exclusion violation is possible. This can be fixed by incrementing the variable *highest* when a new REQUEST is made.

RA_fixed contains both fixes, and moves the *noncrit* action to allow verification of starvation freedom without making explicit fairness assumptions.

Both models can be checked for 2 or 3 nodes. However, with 3 nodes the state space becomes very large. Two reductions are included.
- RA_fixed+broadcast allows the sending of REQUEST messages to occur as a broadcast.
- RA_fixed+reduced includes the broadcasts as well as several other reductions, further specified in the file itself.

The seminar report is also included in this directory. There are two known erros in this report:
- The text refers to channels being modelled as bags of messages, but the appendices show sets being used. In the case of this algorithm, there is no difference in behaviour. However, for the sake of consistency the models included in this directory have been modified to use bags instead. This does not affect the results of the verification, or the size of the state spaces.
- The fourth reference should be called *On mutual exclusion in computer networks* not *An mutual exclusion in computer networks*. Additionally, this text was written by Osvaldo Carvalho and Gerard Roucairol, not by Carvalho alone.