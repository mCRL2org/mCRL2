import os

os.system('mcrl22lps -f small_cube.mcrl2 temp.lps')
os.system('lpsparunfold -l temp.lps -v -n4 -s"List(List(Color))" | lpsconstelm -v | lpsrewr > temp1.lps')
os.system('lpsparunfold -l temp1.lps -v -n8 -s"List(Color)" | lpsconstelm -v | lpsrewr > temp2.lps')
os.system('lps2lts -asolved -t1 -v -rjitty temp2.lps')
os.system('tracepp temp2.lps_act_0_solved.trc')
