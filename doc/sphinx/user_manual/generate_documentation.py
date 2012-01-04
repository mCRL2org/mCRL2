#!/usr/bin/python

import os

def run(command):
  print "Running command {0}".format(command)
  ret = os.system(command)
  if ret <> 0:
    raise Exception("Execution of command {0} failed with exit code {1}".format(command, ret))

def manpages():
  run("python print_manual_pages.py -m ../../../ -g")

  for t in os.listdir("tools"):
    print t
    if t.find(".") > 0:
      continue
    if (not os.path.exists("tools/{0}/manual.xml".format(t))) or os.stat("tools/{0}/manual.xml".format(t))[6]==0:
      continue


    run("xsltproc xslt/manual.xsl tools/{0}/manual.xml > tools/{0}/manual.rst".format(t))

def sphinx():
  os.system("sphinx-build -bhtml -c . . _build")

def main():
  manpages()
  sphinx()

if __name__ == "__main__":
  main()

