#!/bin/bash

rm timings
rm wallclock

echo 'Linearising'
echo 'Wallclock time for generating and solving PBES:' > wallclock

echo 'mcrl22lps:' >> wallclock

{ time mcrl22lps -nf --timings=timings minepump.mcrl2 | lpssumelm > minepump.lps ; } 2>> wallclock

echo 'lps2pbes:' >> wallclock

echo 'Converting to PBES'

{ time lps2pbes --timings=timings -f prop1.mcf minepump.lps  |pbesconstelm --timings=timings | pbesparelm --timings=timings > minepump.pbes ; } 2>> wallclock

echo 'Solving PBES'

echo 'pbespgsolve:' >> wallclock

{ time pbespgsolve -rjittyc  --timings=timings -srecursive minepump.pbes ; } 2>> wallclock

rm *.pbes
rm *.lps


