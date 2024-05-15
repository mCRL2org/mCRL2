#!/usr/bin/env python3

import subprocess
import os

from sys import argv

# Change working dir to the script path
os.chdir(os.path.dirname(os.path.abspath(__file__)))


model="MLV"

subprocess.run

mcrl22lps -vn "${model}.mcrl2" | lpsconstelm -v | lpsparelm -v > "${model}.lps"
#lps2lts -vrjittyc --cached "${model}.lps" "${model}.lts"

lpsreach -v --saturation --cached --chaining --groups=simple "${model}.lps"

for mcfpath in properties/*.mcf; do
  mcffile="`basename "${mcfpath}"`"
  property=`basename "${mcffile}" .mcf`

  echo "================="
  echo "REQUIREMENT ${property}"
  echo "================="
  lps2pbes -v -f "properties/${property}.mcf" "${model}.lps" "${model}.${property}.pbes"
  pbessolvesymbolic -v -c --saturation --cached --chaining --groups=simple "${model}.${property}.pbes"

  echo
  echo
done
