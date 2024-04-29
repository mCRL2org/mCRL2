import os

os.system('mcrl22lps -n spinning_mule.mcrl2 -v scratch/spinning_mule.lps')
os.system('lps2pres -fminimal_walking_distance.mcf scratch/spinning_mule.lps -v scratch/spinning_mule.pres')
os.system('presrewr -pquantifier-inside scratch/spinning_mule.pres |  presrewr -vpquantifier-one-point > scratch/spinning_mule1.pres')
os.system('presrewr -pquantifier-all scratch/spinning_mule1.pres scratch/spinning_mule2.pres')
os.system('pressolve -v -rjitty -am scratch/spinning_mule2.pres -p30 --timings')
