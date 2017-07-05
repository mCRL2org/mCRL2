#!/bin/bash

rm timings
rm wallclock

echo 'Linearising'
echo 'Wallclock time for generating and solving PBES:' > wallclock

echo 'mcrl22lps:' >> wallclock

{ time mcrl22lps -nf --timings=timings mp_fts_prop1.mcrl2 | lpssumelm > mp_fts_prop1.lps ; } 2>> wallclock

echo 'lps2pbes:' >> wallclock

echo 'Converting to PBES'

{ time lps2pbes --timings=timings -f prop1.mcf mp_fts_prop1.lps  |pbesconstelm --timings=timings | pbesparelm --timings=timings > mp_fts_prop1.pbes ; } 2>> wallclock

echo 'Solving PBES'

echo 'pbespgsolve:' >> wallclock

{ time pbespgsolve -rjittyc  --timings=timings -srecursive mp_fts_prop1.pbes ; } 2>> wallclock

rm *.pbes
rm *.lps


