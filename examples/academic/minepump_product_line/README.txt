The FTS model we use is the one that closely follows the Promela code distributed with SNIP, i.e. the 
one used in the IEEE TSE paper, which is slightly different from the one described (also graphically)
in both the TR and in Classen's PhD thesis: the difference is the distinction of the two sub-features 
MethaneAlarm and MethaneQuery (put in an or-relation) of the (optional) Methane (detection or sensor) 
feature, leading to 128 rather than 64 products. 

The state space of the FTS is: 582 states and 1375 transitions.

There are some more subtle differences, like the skip's from states 8, 13, and 20, and a methane test
in the highlevel path, and a setStopped loop in s1. Furthermore, the three loops have been removed: 
methaneRise - methaneLower in METHANELEVEL, pumpRunning between PUMP and WATERLEVEL and pumpRunning - 
waterRise in WATERLEVEL.

Sets of products are represented by means of a datastructure representing OBDDs. These data structures
are essentially based on the rewriting rules by Van de Pol & Zantema (JLAP 2001).

This case study is discussed in the paper 'Family-Based Model Checking with mCRL2' by Maurice H. ter Beek, 
Erik P. de Vink and Tim A.C. Willemse (submitted for publication).

----

* The family_based_experiments subdirectory contains a number of subdirectories, each of which is
  dedicated to the verification of a single property. Such a subdirectory contains a formula which
  is verified on the mCRL2 file contained in that directory. 
  
  The mCRL2 files in these subdirectories are mildly adapted versions of the FTS as it can be found 
  in this directory. More specifically, actions not relevant to the property are mapped to the 
  (featured) action 'skip', allowing for reducing the number of modalities in the formula that is 
  verified.
  
  Each property file contains (if applicable) a formula and its complement which are verified for
  sets of product families satisfying the property and those product families satisfying its complement.
  This permits confirming that the partition (P+,P-) described this way indeed is such that P+
  satisfies the property and P- does not satisfy the property.

  Each subdirectory contains a run file that performs the verification as it was performed for the FASE 
  paper.


* The product_based_experiments subdirectory contains a number of subdirectories, each of which is
  dedicated to the verification of a single property. Such a subdirectory contains a formula which
  is verified on the mCRL2 file contained in that directory.

  The mCRL2 files in these subdirectories are the exact copy of the versions of the semi-FTS following
  the regime of ter Beek & de Vink (SPLC 2014), where the mCRL2 process essentially explicitly describes 
  all possible product LTSs underlying the FTS.

  Each property file contains (if applicable) a formula and its complement which are verified for
  sets of product families satisfying the property and those product families satisfying its complement.
  This permits confirming that the partition (P+,P-) described this way indeed is such that P+
  satisfies the property and P- does not satisfy the property. 

  Each subdirectory contains a run file that performs the verification as it was performed for the FASE 
  paper.
