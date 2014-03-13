import os
import sys
sys.path += [os.path.join(os.path.dirname(__file__), '..', '..', 'build', 'python')]
import testrunner

class TestRunner(testrunner.TestRunner):
  def name(self, testnum):
    if testnum < 10:
      return 'regressiontest_{0}'.format(testnum)

  def run(self, testnum):
    if testnum < 10:
      print 'regressiontest_{0}'.format(testnum)
    else:
      raise RuntimeError('Invalid test number')  

if __name__ == "__main__":
  sys.exit(TestRunner().main())
