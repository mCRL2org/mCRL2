import os

os.system('mcrl22lps -v bridge_crossing.mcrl2 bridge_crossing.lps')
os.system('lps2lts -v -a ready bridge_crossing.lps -t5')
os.system('tracepp bridge_crossing.lps_act_2_ready.trc');
