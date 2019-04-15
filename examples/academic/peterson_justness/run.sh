#!/bin/bash

mcrl22lps mutex.mcrl2 mutex.lps
lps2pbes -f justlive.mcf mutex.lps mutex.justlive.pbes
pbessolve mutex.justlive.pbes

rm *.lps
rm *.pbes
