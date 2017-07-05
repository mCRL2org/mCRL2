import os
import sys
import argparse

class TestRunner(object):
    def __init__(self):
        '''
        Constructs a TestRunner, and initializes the _source_dir and _stage_dir attributes
        with sensible default values.
        Unless indicated otherwise by command-line parameters, we assume that the current file
        is located in <_source_path>/build/python, and that the mCRL2 executables are located in
        <_source_path>/stage/bin (an in-source build). For out-of-source build trees, the location
        of the executables must be passed as a command-line option.
        '''
        self._args = self._get_commandline_parser().parse_args()
        self._source_path = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
        self._tool_path = self._args.toolpath or os.path.join(self._source_path, 'stage', 'bin')

    def _get_commandline_parser(self):
        '''
        Returns an argparse.ArgumentParser. May be overridden in derived classes to add arguments.
        '''
        parser = argparse.ArgumentParser(description="Interface between CMake and a Python test runner.")
        parser.add_argument('--names', dest='names', action='store_true',
                            help='Retrieve the test names defined by this test runner.')
        parser.add_argument('--run', dest='run', metavar='N', type=int, action='store',
                            help='Run test N, or exit with return value 1 if N is too large.')
        parser.add_argument('--toolpath', dest='toolpath', metavar='DIR', type=str, action='store',
                            help='Assume that the mCRL2 executables are located in this directory.')
        return parser

    def main(self):
        if self._args.names:
            for name in self.names():
                print(name)
        elif self._args.run is not None:
            try:
                self.run(self._args.run)
            except Exception as e:
                sys.exit(str(e))

    def run(self, testnum):
        '''Runs the name of the test with number testnum.'''
        raise NotImplemented

    def names(self):
        '''Iterates through the test names defined by this test runner.'''
        return []
