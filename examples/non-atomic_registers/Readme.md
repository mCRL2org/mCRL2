# Non-atomic registers 

This directory contains the models of multi-writer multi-reader safe, regular and atomic registers and their applications in the verification of several mutual exclusion algorithms, as described in a forthcoming article by Myrthe Spronck and Bas Luttik.

In Register_model, an mCRL2 specification can be found with basic versions of safe, regular and atomic registers. To use this specification, only the first section of the specification needs to be modified:
* define the sort RID (for Register ID), with appropriate initial, minimum and maximal values per register id;
* define an appropriate Thread process for the algorithm to be verified;
* define an appropriate initial process with every required register as parallel component and the desired number of threads 
* adapt MAX_TID to the number of threads minus 1.

The basic register model is combined with several mutual exclusion algorithms. We refer to Algorithms_pseudocode.pdf for the pseudocode of the respective algorithms, for references to the originating literature, and for additional explanations. 

* Aravind_BLRU
* Attiya-Welch
* Attiya-Welch_alternate
* Dekker
* Dijkstra
* Knuth
* Lamport_3bit
* Peterson
* Szymanski_flag
* Szymanski_flag_with_bits
* Szymanski_3bit_linear_wait

In addition there are three more mCRL2 models, not described in Algorithms_pseudocode.pdf:

* Lamport_3bit_incorrect_z gives an interpretation of Lamport's 3bit algorithm that appears to be reasonable at first sight, but turns out to be incorrect. This models illustrates that one must be very careful when implementing pseudocode.
* Szymanski_3bit_linear_wait_with_semaphore, abbreviated as Szymanski_3bitlw_sem, was used to produce a counterexample showing that Szymanski_3bit_linear_wait does not guarantee mutual exclusion for three threads and atomic registers.
* Szymanski_flag_with_bits_protected_exit, abbreviated as Szymanski_fwb_pe, does guarantee mutual exclusion for three threads and atomic registers, but does not guarantee mutual exclusion for three threads with regular or safe registers.

