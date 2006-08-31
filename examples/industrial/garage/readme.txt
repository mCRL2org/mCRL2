This directory contains specifications of the safety layer of the automated
parking garage as described in [1,2]. Also some trace files are provided to
give more insight in the specifications.

Each specification (.mcrl2 file) can be linearised to an LPE (.lpe file) using
mcrl22lpe. After that, the possibilities are listed in the following table.

  LPE                xsim    traces    lpe2lts
  --------------------------------------------
  garage              no       no         no
  garage-r1          yes       no         no
  garage-r2          yes     yes/no*      no
  garage-r2-error    yes      yes         no
  garage-r3          yes       no        yes
  garage-ver         yes       no        yes

* For this LPE, the error traces do not work. This means that the errors cannot
  occur in the LPE.

For the LPE's where traces can be used, an xsim plugin can be used that
represents the visualization of the floorplan of the garage. After opening
xsim, it can be loaded by choosing the "Views" menu, then "Load Plugin..."  and
then "libxsim_garage_view.so".


References
----------

[1] Aad Mathijssen and A. Johannes Pretorius. Verified Design of an Automated
    Parking Garage. In Proc. 11th Int'l Workshop on Formal Methods for
    Industrial Critical Systems (FMICS), LNCS (to appear), Springer-Verlag,
    2006.

[2] Aad Mathijssen and A. Johannes Pretorius. Specification, Analysis and
    Verification of an Automated Parking Garage. Computer Science Report 05-25,
    Eindhoven University of Technology (TU/e), 2005.
