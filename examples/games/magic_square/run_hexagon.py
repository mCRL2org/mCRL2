import os 

os.system('mcrl22lps -vo magic_hexagon.mcrl2 temp.lps')
os.system('lpssumelm -v temp.lps temp1.lps --timings')
os.system('lps2lts -v -rjittyc temp1.lps temp.aut')
