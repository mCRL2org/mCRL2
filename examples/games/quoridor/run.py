import os

# Create LPS from mCRL2 specification
os.system("mcrl22lps -v quoridor.mcrl2 quoridor.lps")

# Optimize LPS
os.system("lpssuminst -v -sBool quoridor.lps | lpsparunfold -v -l -n5 -sPosition | lpsrewr -v | lpsconstelm -v -c | "
          "lpsrewr -v quoridor-1.lps")

# Transform LPS into a LTS
os.system("lps2lts --cached -v -rjittyc --threads=16 quoridor-1.lps quoridor.lts")

# Reduce LTS modulo strong bisimulation
os.system("ltsconvert -v -ebisim quoridor.lts quoridor-1.lts")

# Verify whether player 1 has a winning strategy
os.system('lts2pbes -v -c -p -f"properties/winning_strategy_player_1.mcf" quoridor-1.lts | pbessolve -v --threads=16 -rjittyc '
          '-s1 --file=quoridor-1.lts --evidence-file=quoridor-player-1-evidence.lts')

# Verify whether player 2 has a winning strategy
os.system('lts2pbes -v -c -p -f"properties/winning_strategy_player_2.mcf" quoridor-1.lts | pbessolve -v --threads=16 -rjittyc '
          '-s1 --file=quoridor-1.lts --evidence-file=quoridor-player-2-evidence.lts')
