import re
from path import *

lines = path('../../../mcrl22lps/lin_std.cpp').lines()
for line in lines:
    if re.match('\\w', line) and re.search('[()]', line) == None and re.match('typedef', line) == None:
        print line.strip()
