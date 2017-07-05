import os

os.system('mcrl22lps -nfTv ball_game.mcrl2 ball_game.lps')
os.system('lpsuntime -v ball_game.lps ball_gameu.lps')
os.system('lpsrealelm --max=11 -v ball_gameu.lps ball_gamer.lps')
os.system('lps2lts -v ball_gamer.lps ball_game.lts')

