import os

os.system('mcrl22lps spinning_mule.mcrl2 -v scratch/temp1.lps')
os.system('mcrl22lps spinning_mule_optimized.mcrl2 -v scratch/temp2.lps')
os.system('lps2lts -v -rjittyc scratch/temp1.lps scratch/temp1.lts')
os.system('lps2lts -v -rjittyc scratch/temp2.lps scratch/temp2.lts')
os.system('ltspcompare -epbisim -v scratch/temp1.lts scratch/temp2.lts')
