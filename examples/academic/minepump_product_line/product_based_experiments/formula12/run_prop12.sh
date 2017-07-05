#!/bin/bash

rm timings
rm wallclock

echo 'Linearising'
echo 'Wallclock time for generating and solving PBES:' > wallclock

echo 'mcrl22lps:' >> wallclock

{ time mcrl22lps -nf --timings=timings minepump.mcrl2 | lpssumelm > minepump.lps ; } 2>> wallclock

echo 'lps2pbes:' >> wallclock

echo 'Converting to PBES'

{ time lps2pbes --timings=timings -f prop12.mcf minepump.lps  minepump.pbes ; } 2>> wallclock

echo 'Solving PBES'

echo 'pbespgsolve:' >> wallclock

{ time pbespgsolve --timings=timings -rjittyc -srecursive minepump.pbes ; }  2>> wallclock

rm -rf *.pbes
rm -rf *.lps
