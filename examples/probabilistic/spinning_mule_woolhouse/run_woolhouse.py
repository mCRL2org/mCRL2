import os

os.system('mcrl22lps spinning_mule_woolhouse.mcrl2 -v scratch/spinning_mule_woolhouse.lps')
os.system('lps2pres -fminimal_walking_distance.mcf scratch/spinning_mule_woolhouse.lps -v scratch/spinning_mule_woolhouse.pres')
os.system('presrewr -pquantifier-one-point scratch/spinning_mule_woolhouse.pres scratch/spinning_mule_woolhouse1.pres')
os.system('pressolve -v -rjitty -am scratch/spinning_mule_woolhouse1.pres -p30')
