import os
os.system('mcrl22lps sokoban.mcrl2 -m temp.lps -v')
os.system('lpssuminst -v temp.lps temp1.lps')
os.system('lpsparunfold -s Board -n 8 -lv temp1.lps | lpsconstelm -vt >temp2.lps')
os.system('lpsparunfold -s Row -n 11 -lv temp2.lps | lpsconstelm -vt > temp3.lps')
os.system('lps2lts -vrjittyc -awin -t1 --todo-max=100000 temp3.lps') 

