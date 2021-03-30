# Mutual exclusion algorithms

This directory contains the models and associated properties for a set of mutual exclusion algorithms described in the DisCoTec tutorial in 2021 (reference to be added).

* Mutex-naive contains a naive attempt at mutual exclusion, that does not satisfy the mutual exclusion property.
* Improved-mutex-naive fixes the algorithm to satisfy mutual exclusion, but does not yet satisfy eventual access.
* Dekker contains contains the model and properties of Dekker's mutual exclusion algorithm. Eventual access is still not satisfied due to fairness issues.
* Petersons contains a variant of Peterson's process for mutual exclusion that is almost identical to the one in ../software_models/Petersons_T_T. All four relevant properties are satisfied here.
* Petersons-3 contains a model of Peterson's filter-lock generalization for 3 processes, and shows that mutual exclusion is satisfied. Eventual access does not hold because of fairness issues.
