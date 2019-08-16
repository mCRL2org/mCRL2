mcrl22lps -vb ertms-hl3.mcrl2 | lpssumelm -vc | lpsconstelm -vst | lpsparelm -v > ertms-hl3.lps

time lps2lts -vrjittyc --cached --timings=lps2lts_times.txt ertms-hl3.lps ertms-hl3.mod.aut

time ltsconvert --timings=ltsmin_times.txt --tau=break,split_train,enter,leave,extend_EoA,move,connect,disconnect -vedpbranching-bisim  ertms-hl3.mod.aut ertms-hl3.mod.min.aut

time ltsconvert -vl ertms-hl3.lps ertms-hl3.mod.min.aut ertms-hl3.mod.min.lts

echo
echo

echo 'Verifying strong determinacy'
time lts2pbes -vf strong_determinacy.mcf ertms-hl3.mod.min.lts -l ertms-hl3.lps ertms-hl3.mod.min.strong_determinacy.pbes
time pbessolve -v -s2 --timings=pbessolve_times.txt ertms-hl3.mod.min.strong_determinacy.pbes

echo 'Verifying termination'
time lts2pbes -vf termination.mcf ertms-hl3.mod.min.lts -l ertms-hl3.lps ertms-hl3.mod.min.termination.pbes
time pbessolve -v -s2 --timings=pbessolve_times.txt ertms-hl3.mod.min.termination.pbes


echo 'Verifying deterministic stabilisation'
time lts2pbes -vf deterministic_stabilisation.mcf ertms-hl3.mod.min.lts -l ertms-hl3.lps ertms-hl3.mod.min.deterministic_stabilisation.pbes
time pbessolve -v -s2 --timings=pbessolve_times.txt ertms-hl3.mod.min.deterministic_stabilisation.pbes
