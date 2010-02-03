#~ Copyright 2009, 2010 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import re
from path import *

def insert_text_in_file(filename, text, label):
    """
    Replaces the text between the strings '//--- start %s ---//' % label and '//--- end %s ---//' % label
    with text in the file named filename.
    """
    src = r'//--- start %s ---//.*//--- end %s ---//' % (label, label)
    dest = ('//--- start %s ---//\n' + text + '//--- end %s ---//') % (label, label)
    old_text = path(filename).text()
    new_text = re.compile(src, re.S).sub(dest, old_text)
    if old_text == new_text:
        print 'Warning: nothing has changed in file %s' % filename
    else:
        path(filename).write_text(new_text)
        print 'Updated file %s' % filename
