#!/bin/bash

mcrl22lps -n 4.mcrl2 4.lps

echo
lts2pbes -f nodeadlock.mcf -l4.lps 4.aut -c 4.pbes
echo '****Testing whether it works with the included .aut file****'
pbesinfo 4.pbes

echo
lts2pbes -f nodeadlock.mcf -l4.lps 4.lts -c 4.pbes
echo '****Testing whether it works with the included .lts file****'
pbesinfo 4.pbes

