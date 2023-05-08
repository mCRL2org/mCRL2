# Peterson's mutual exclusion algorithm

This directory contains the models and associated properties for Peterson's mutual exclusion algorithm.

* Petersons_F_F contains the model in which both flags are initialised to false. This is the correct initialisation.
* Petersons_F_T contains the model in which the flag of the first process is initialised to false, and that of the second to true. This is the incorrect version that, for some time, was portrayed on Wikipedia. It does not allow process 0 to enter its CS a second time without cooperation of process 1.
* Petersons_T_T contains the model in which the flags of both processes are initialised to true. This models shows for both processes that one process cannot enter its CS a second time without cooperation of the other process.
