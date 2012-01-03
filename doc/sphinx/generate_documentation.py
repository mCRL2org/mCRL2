#!/usr/bin/python

import os

def run(command):
  ret = os.system(command)
  if ret <> 0:
    raise Exception("Execution of command {0} failed with exit code {1}".format(command, ret))

def doxygen(library = "lts"):
  referencePath = "libraries/{0}/reference".format(library)
  doxygenXmlPath = "libraries/{0}/xml".format(library)

  if not os.path.exists(referencePath):
    os.makedirs(referencePath)

  pwd = os.getcwd()
  os.chdir("../../")
  # For now do LTS library only
  run("./doc/sphinx/generate_libref_website.sh -l6")
  os.chdir(pwd)

  listing = []
  # Transform XML into RST
  for f in os.listdir(doxygenXmlPath):
    relXmlPath = "{0}/{1}".format(doxygenXmlPath, f)
    absDoxygenXmlPath = "{0}/{1}".format(pwd, doxygenXmlPath)
    relRstPath = "{0}/{1}.rst".format(referencePath, f[:-4])
    
    if f == "index.xml":
      run("xsltproc --param dir \"\'{0}\'\" xslt/refindex.xsl {1} > {2}".format(absDoxygenXmlPath, relXmlPath, relRstPath))
      
    elif f.endswith(".xml") and ((f.startswith("class") and not f.startswith("classstd")) or f.endswith("_8h.xml")):
      run("xsltproc --param dir \"\'{0}\'\" xslt/compound.xsl {1} > {2}".format(absDoxygenXmlPath, relXmlPath, relRstPath))
      listing.append("   {0}".format(f[:-4]))
      
  f = open("{0}/listing.rst".format(referencePath), 'w')
  f.write("\n".join([".. toctree::", "   :hidden:", "\n"]))
  f.write("\n".join(sorted(listing)))
  f.close()

def sphinx():
  os.system("sphinx-build -bhtml -c . . _build")

def main():
  doxygen()
  sphinx()

if __name__ == "__main__":
  main()

