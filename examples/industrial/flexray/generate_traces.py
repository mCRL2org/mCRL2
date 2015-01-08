#!/usr/bin/env python
import os
import sys
import subprocess

if not os.path.isdir('traces'):
  os.mkdir('traces')

def run(cmdline):
  try:
    print('* {}'.format(cmdline))
    subprocess.check_call(cmdline, shell=True)
  except Exception as e:
    print('* Error: {}'.format(e))
    raise RuntimeError

def main():
  if not (os.path.isfile('data/3_Deaf_follower.lps') and
          os.path.isfile('data/3_Ideal_trace.lps') and
          os.path.isfile('data/3_Resetting_leader.lps') and
          os.path.isfile('data/Big_Deaf_follower.lps')):
    print('Please run "generate.py --lps" before running this script.')
  else:
    try:
      print('** Deaf follower')
      run('lps2lts --cached -vrjittyc -m"is_integrating(2)|is_integrating(3)" -t1 data/3_Deaf_follower.lps')
      run('tracepp "data/3_Deaf_follower.lps_act_0_is_integrating(2)|is_integrating(3).trc" | {} trace.py --pdf traces/3_Deaf_follower'.format(sys.executable))
      print('** Ideal trace')
      run('lps2lts --cached -vrjittyc data/3_Ideal_trace.lps data/3_Ideal_trace.aut')
      run('{0} randomtrace.py data/3_Ideal_trace.aut | {0} trace.py --pdf traces/3_Ideal_trace'.format(sys.executable))
      print('** Resetting leader')
      run('lps2lts --cached -vrjittyc -m"is_integrating(2)|is_integrating(3)" -t1 data/3_Resetting_leader.lps')
      run('tracepp "data/3_Resetting_leader.lps_act_0_is_integrating(2)|is_integrating(3).trc" | {} trace.py --pdf traces/3_Resetting_leader'.format(sys.executable))
      print('** Deaf follower (repaired)')
      run('lps2lts --cached -vrjittyc -m"is_integrating(2)|is_integrating(3)" -t1 data/Big_Deaf_follower.lps')
      run('tracepp "data/Big_Deaf_follower.lps_act_0_is_integrating(2)|is_integrating(3).trc" | {} trace.py --pdf traces/Big_deaf_follower'.format(sys.executable))
    except RuntimeError:
      print('** Aborted')

if __name__ == '__main__':
  main()
