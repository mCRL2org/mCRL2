import os

def run_dining(specification_version = "dining3"):
  os.system('mcrl22lps -v %s.mcrl2 %s.lps' % (specification_version, specification_version))

  os.system('lps2pbes -v -f nodeadlock.mcf %s.lps %s.nodeadlock.pbes' % (specification_version, specification_version))
  os.system('pbes2bool -v %s.nodeadlock.pbes' % (specification_version))

  os.system('lps2pbes -v -f nostuffing.mcf %s.lps %s.nostuffing.pbes' % (specification_version, specification_version))
  os.system('pbes2bool -v %s.nostuffing.pbes' % (specification_version))

  os.system('lps2pbes -v -f nostarvation.mcf %s.lps %s.nostarvation.pbes' % (specification_version, specification_version))
  os.system('pbes2bool -v %s.nostarvation.pbes' % (specification_version))

run_dining("dining3")
run_dining("dining3_cs")
run_dining("dining3_cs_seq")
run_dining("dining3_ns")
run_dining("dining3_ns_seq")
run_dining("dining3_schedule")
run_dining("dining3_schedule_seq")


