mcrl22lps -vnb ertms-hl3.announce.mcrl2 | lpssumelm -vc | lpssuminst -f | lpsconstelm -vst | lpsparelm -v > ertms-hl3.announce.lps

time lps2lts -vrjittyc --cached  --timings=lps2lts_announce_times.txt ertms-hl3.announce.lps ertms-hl3.announce.mod.aut

time ltsconvert --timings=ltsmin_times.announce.txt -vedpbranching-bisim  ertms-hl3.announce.mod.aut ertms-hl3.announce.mod.min.aut

time ltsconvert -vl ertms-hl3.announce.lps ertms-hl3.announce.mod.min.aut ertms-hl3.announce.mod.min.lts

echo
echo

echo 'Verifying collision'
time lts2pbes -vf no_collision.mcf ertms-hl3.announce.mod.min.lts -l ertms-hl3.announce.lps ertms-hl3.announce.mod.min.no_collision.pbes
time pbessolve -v -s2 --timings=pbessolve_times.announce.txt ertms-hl3.announce.mod.min.no_collision.pbes

