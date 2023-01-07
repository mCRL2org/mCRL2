import os

os.system('mcrl22lps -v hopcroft.mcrl2 hopcroft.lps')
os.system('lps2lts -v hopcroft.lps --no-info hopcroft.lts')
os.system('ltsconvert -ebisim hopcroft.lts -v hopcroftred.lts')

