This directory contains specifications of the safety layer of the automated
parking garage as described in [1,2]. Also some trace files are provided to
give more insight in the specifications.

Each specification (.mcrl2 file) can be linearised to an LPS (.lps file) using
mcrl22lps. After that, the possibilities are listed in the following table.

  LPS                xsim    traces    lps2lts
  --------------------------------------------
  garage              no       no         no
  garage-r1          yes       no         no
  garage-r2          yes     yes/no*      no
  garage-r2-error    yes      yes         no
  garage-r3          yes       no        yes
  garage-ver         yes       no        yes

* For this LPS, the error traces do not work. This means that the errors cannot
  occur in the LPS.

For the LPS's where traces can be used, an xsim plugin can be used that
represents the visualization of the floorplan of the garage. After opening
xsim, it can be loaded by choosing the "Views" menu, then "Load Plugin..."  and
then "libxsim_garage_view.so".


References
----------

[1] Aad Mathijssen and A. Johannes Pretorius. Verified Design of an Automated
    Parking Garage. In Proc. FMICS and PDMC 2006, LNCS 4346, pp 165-180,
    Springer-Verlag, 2007.

[2] Aad Mathijssen and A. Johannes Pretorius. Specification, Analysis and
    Verification of an Automated Parking Garage. Computer Science Report 05-25,
    Eindhoven University of Technology (TU/e), 2005.
