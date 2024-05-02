#!/usr/bin/env python3

import subprocess

subprocess.run(['mcrl22lps', '-v', 'food_package.mcrl2', 'food_package.lps'], shell=True, check=True)
subprocess.run(['lpssuminst', 'food_package.lps', 'food_package.lpssuminst.lps'], shell=True, check=True)
subprocess.run(['lps2pbes', '-v', '-f', 'sustained_delivery.mcf', 'food_package.lpssuminst.lps', 'sustained_delivery.pbes'], shell=True, check=True)
subprocess.run(['pbesconstelm', '-ve', 'sustained_delivery.pbes', 'sustained_delivery.pbesconstelm.pbes'], shell=True, check=True)

# We use -rjittyc is used below, which does work on linux and mac, and not on windows.
# Note that the generated bes is huge.
subprocess.run(['pbes2bool', '-v', '-zdepth-first', '-s3', 'sustained_delivery.pbesconstelm.pbes'], shell=True, check=True)