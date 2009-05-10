"""
This file generates a file called strategy.cpp from the files 
format_*.fmt and preamble_*.mcrl2.

When adding or changing strategies for lysa2mcrl2, all you have 
to do is make those files and expand the strategies variable right
below this docblock. Then run strategy_converter.py from the 
tools/lysa2mcrl2 directory and rebuild the tool. The newly added
(or changed) strategies are now available via the -s switch.

@author Egbert Teeselink
"""

strategies = {
  'symbolic':        { 'makeSymbolic': True }, 
  'straightforward': { 'makeSymbolic': False }
}

def make_multiline_for_c(lines):
  s  = ""
  for line in lines:
    s += '\n"' + line[:-1].replace('"', '\\"') + '\\n"' #remove newline character, add newline literal, wrap in quotes, prepend newline character
  return s



def build_strategy_cpp(strats):
  
  s = """
#include "strategy.h"
#include <iostream>

using namespace std;
using namespace lysa;

"""
  
  #generate strategy classes
  for strat, settings in strats.iteritems():
    print "generating strategy '" + strat + "'..."
    preamble = open('preamble_' + strat + '.mcrl2', 'r')
    format   = open('format_'   + strat + '.fmt'  , 'r')
    
    s += '''
class Strategy_%(strat)s : public Strategy 
{ 
  std::string getFormatTemplate()    { return %(format)s; } 
  std::string getPreamble()          { return %(preamble)s; } 
  bool        makeSymbolicAttacker() { return %(makeSymbolic)s; } 
};

''' % {
      'strat'       : strat,
      'makeSymbolic': 'true' if settings['makeSymbolic'] else 'false',
      'format'      : make_multiline_for_c(format.readlines()),
      'preamble'    : make_multiline_for_c(preamble.readlines())
    }
    
  
  print "generating get method..."
  s += '''
Strategy* Strategy::get(string name)
{'''
  
  for strat, settings in strats.iteritems():
    s += '''
  if(name == "%(strat)s") { return new Strategy_%(strat)s(); }''' \
  % {'strat': strat}

  s += '''
  throw "Cannot find strategy '" + name + "'.";
}
'''
  
  print "writing to 'strategy.cpp'..."
  out = open('strategy.cpp', 'w')
  out.write(s)
  out.close()
  

build_strategy_cpp(strategies)