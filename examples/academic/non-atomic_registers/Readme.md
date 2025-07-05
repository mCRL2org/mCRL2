# Non-atomic registers 

This directory contains models of multi-writer multi-reader safe, regular and atomic registers and their applications in the verification of several mutual exclusion algorithms.
These models are described in *Just Verification of Mutual Exclusion Algorithms* by Rob van Glabbeek, Bas Luttik and Myrthe Spronck (2025), and extend models previously described in *Process-Algebraic Models of Multi-Writer Multi-Reader Non-Atomic Registers* by Myrthe Spronck and Bas Luttik (2023).
The previous versions of these models can be found in commit 972629b.

Results reported in the 2025 paper were obtained using mCRL2 version 202407.1.700c1fb726M (Release).

In the folder *properties*, the following properties are given as modal mu-calculus formulae:
* Absence of deadlocks from the LTS (no_dead.mcf)
* Mutual exclusion (mutex.mcf)
* Reachability of the critical section (reach.mcf)
* Deadlock freedom under the justness assumption, using concurrency relations T, S, I and A respectively (df_just_T.mcf, df_just_S.mcf, df_just_I.mcf, df_just_A.mcf)
* Starvation freedom under the justness assumption, using concurrency relations T, S, I and A respectively (sf_just_T.mcf, sf_just_S.mcf, sf_just_I.mcf, sd_just_A.mcf)

In the folder *models*, the following mutual exclusion algorithms are modelled:
* Anderson's algorithm (Anderson.mcrl2)
* Aravind's BLRU algorithm, both in its original presentation (Aravind_BLRU.mcrl2) and a variant with minor modifications (Aravind_BLRU_alt.mcrl2)
* Four different versions Attiya-Welch's algorithm: the variant originally presented (Attiya-Welch_orig.mcrl2), a minor modification of the former (Attiya-Welch_orig_alt.mcrl2), a variant presented in a later paper (Attiya-Welch_var.mcrl2), and a modification of that variant as well (Attiya-Welch_var_alt.mcrl2)
* The Burns-Lynch algorithm (Burns-Lynch.mcrl2)
* Four variants of Dekker's algorithm: the original presentation (Dekker.mcrl2), a minor modification of the original presentation (Dekker_alt.mcrl2), a variant presentation to make the algorithm RW-safe (Dekker_RW-safe.mcrl2), and the application of the RW-safe version as a component of a starvation-free algorithm (Dekker_RW-safe_DFtoSF.mcrl2)
* Dijkstra's algorithm (Dijkstra.mcrl2)
* Kessels' algorithm (Kessels.mcrl2)
* Knuth's algorithm (Knuth.mcrl2)
* Lamport's 1-bit algorithm, both as originally presented (Lamport_1-bit.mcrl2) and as a component of a starvation-free algorithm (Lamport_1-bit_DFtoSF.mcrl2)
* Lamport's 3-bit algorithm (Lamport_3-bit.mcrl2), and a version where the z variable is implemented incorrectly (Lamport_3-bit_incorrect_z.mcrl2)
* Peterson's algorithm (Peterson.mcrl2)
* Szymanski's 3-bit linear wait algorithm, both its original presentation (Szymanski_3-bit_lin_wait.mcrl2) and a variant with minor modifications (Szymanski_3-bit_lin_wait_alt.mcrl2)
* Szymanski's flag algorithm, both the variant with integers (Szymanski_flag.mcrl2) and the variant with Booleans (Szymanski_flag_bit.mcrl2 with the original exit protocol and Szymanski_flag_bit_altexit.mcrl2 with the alternative exit protocol)

We refer to the full version of *Just Verification of Mutual Exclusion Algorithms* for the pseudocode of the modelled algorithms, for references to the originating literature, and for additional explanations.

To add further algorithms, any of the above .mcrl2 files can be modified as follows:
* adapt the sort RID (for Register ID) to capture the correct registers, and assign each the appropriate minimum and maximum values
* replace the Thread process with the appropriate specification for the algorithm to be verified
* adapt the initial process to initialise the right registers and their initial values, and place the correct number of threads in parallel
* adapt NUM to the desired number of threads

To run a verification:
* The models are all given with all registers being safe. To verify with regular or atomic registers, replace Safe_Register(...) with Regular_Register(...) or Atomic_Register(...) everywhere it is used in the initial process.
* Then use the run.py script. It requires two arguments, which must be given in order: the name of the model (without .mcrl2 extension) and the name of the property (without .mcf extension)

Note that some of these verifications may take a long time to run.

In addition to the models described above, there are two mCRL2 IDE projects for models that are only referenced in *Process-Algebraic Models of Multi-Writer Multi-Reader Non-Atomic Registers*:
* Szymanski_3bit_linear_wait_with_semaphore, abbreviated as Szymanski_3bitlw_sem, was used to produce a counterexample showing that Szymanski_3bit_linear_wait does not guarantee mutual exclusion for three threads and atomic registers.
* Szymanski_flag_with_bits_protected_exit, abbreviated as Szymanski_fwb_pe, does guarantee mutual exclusion for three threads and atomic registers, but does not guarantee mutual exclusion for three threads with regular or safe registers.
  
Since these models use actions that the models and properties presented in *Just Verification of Mutual Exclusion Algorithms* do not account for, these are left in their previous form.




