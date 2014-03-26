#!/usr/bin/env python

# Copyright 2009, 2010 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import re
from path import *

class Substitution(dict):
    def __call__(self, match):
        label = match.group(2).strip()
        if label in self:
            return '{0}{1}{2}'.format(match.group(1), self[label], match.group(4))
        return match.group(0)

class GeneratedCodeFile(object):
    __USER_BLOCK_PATTERN = re.compile(r'(//--- start user section\s+(.*?)\s*---//)(.*?)(//--- end user section\s*\2\s*---//)', re.DOTALL)
    __BLOCK_PATTERN = re.compile(r'(//--- start\s+(.*?)\s*---//)(.*?)(//--- end\s+\2\s*---//)', re.DOTALL)

    def __init__(self, filename):
        self.__filename = filename
        self.__text = None
        self.__changed = False
    def __enter__(self):
        self.__text = path(self.__filename).text()
        return self
    def __exit__(self, exc_type, exc_value, traceback):
        if self.__changed and exc_type is None and exc_value is None and traceback is None:
            path(self.__filename).write_text(self.__text)
    def substitute(self, label, text, handle_user_sections=False):
        blocksubst = Substitution({label: '\n{0}\n'.format(text.strip())})
        if handle_user_sections:
            usersubst = Substitution((match.group(2), match.group(3)) for match in GeneratedCodeFile.__USER_BLOCK_PATTERN.finditer(self.__text))
            new_text, count = GeneratedCodeFile.__BLOCK_PATTERN.subn(blocksubst, self.__text)
            new_text = GeneratedCodeFile.__USER_BLOCK_PATTERN.sub(usersubst, new_text)            
        else:
            new_text, count = GeneratedCodeFile.__BLOCK_PATTERN.subn(blocksubst, self.__text)
        if count == 0:
            raise RuntimeError('Substitution requested for non-existent block "{0}" in {1}'.format(label, self.__filename))
        self.__changed, self.__text = self.__text != new_text, new_text
    @property
    def changed(self):
        return self.__changed

def indent_text(text, indent):
    lines = []
    for line in text.split('\n'):
        if len(line) == 0:
            lines.append(line)
        else:
            lines.append(indent + line)
    return '\n'.join(lines)

if __name__ == "__main__":
    import os
    oldtext = '''
    void function_before_block() { }
    //--- start test ---//
    void generated_function()
    {
    }
    //--- start user section propositional_variable_instantiation ---//
    void user_function()
    {
    }
    //--- end user section propositional_variable_instantiation ---//
    //--- end test ---//
    void function_after_block() { }
    \n'''
    newtext = '''
    void generated_function()
    {
    }
    //--- start user section propositional_variable_instantiation ---//
    //--- end user section propositional_variable_instantiation ---//
    '''
    if not os.path.exists('/tmp/test.txt'):
        open('/tmp/test.txt', 'w+').write(oldtext)
    with GeneratedCodeFile('/tmp/test.txt') as f:
        f.substitute('test', newtext, True)
        if not f.changed:
            print('Warning: No change made!')            
  
