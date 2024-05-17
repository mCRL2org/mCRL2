import os

os.system('mcrl22lps spinning_mule_optimized.mcrl2 -v scratch/spinning_mule_optimized.lps')
os.system('lps2pres -fminimal_walking_distance.mcf scratch/spinning_mule_optimized.lps -v scratch/spinning_mule_optimized.pres')
os.system('resrewr -pquantifier-inside scratch/spinning_mule_optimized.pres |  presrewr -vpquantifier-one-point > scratch/spinning_mule_optimized1.pres')
os.system('presrewr -pquantifier-all scratch/spinning_mule_optimized1.pres scratch/spinning_mule_optimized2.pres')
os.system('pressolve -v -rjitty -am scratch/spinning_mule_optimized2.pres -p30 --timings')
