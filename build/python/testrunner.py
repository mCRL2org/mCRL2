import sys
import argparse

class TestRunner(object):
  def main(self):
    parser = argparse.ArgumentParser(description="Interface between CMake and a Python test runner.")
    parser.add_argument('--name', dest='name', metavar='N', type=int, action='store', 
                        help='Retrieve the name of test N, or exit with return value 1 if N is too large.')
    parser.add_argument('--run', dest='run', metavar='N', type=int, action='store',
                        help='Run test N, or exit with return value 1 if N is too large.')
    args = parser.parse_args()
    if args.name is not None:
      result = self.name(args.name)
      if result is None: 
        return 1
      else:
        print result
    elif args.run is not None:
      try:
        self.run(args.run)
      except:
        return 1
    return 0

  def run(self, testnum):
    '''Runs the name of the test with number testnum.'''
    raise NotImplemented

  def name(self, testnum):
    '''Returns the name of the test with number testnum. If testnum
    does not specify a valid test, returns None.'''
    return
