#!/usr/bin/env python3

# Copyright 2009, 2010 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import re

# TODO: put these functions in a central place
def read_text(filename):
    with open(filename, 'r') as f:
        return f.read()

def write_text(filename, text):
    with open(filename, 'w') as f:
        f.write(text)

# searches for sections of text between pairs of lines like this, and returns them as a mapping
#
# //--- start user section X ---//
# //--- end user section X ---//
def extract_user_sections(text):
    regex = re.compile(r"//--- start user section (\w*) ---//(.*)//--- end user section \1 ---//", re.DOTALL)
    result = {}
    for item in regex.findall(text):
        result[item[0]] = item[1]
    return result

# inserts user sections in text
#
# user_sections: a mapping containing user sections
# labels: the set of keys of user_sections must be equal to labels
def insert_user_sections(text, labels, user_sections):
    for label in labels:
        src  = r'//--- start user section %s ---//.*//--- end user section %s ---//' % (label, label)
        if not label in user_sections:
            raise Exception("User section with label '//--- start user section %s ---//' not found!" % label)
        if re.compile(src, re.DOTALL).findall(text) == None:
            raise Exception("User section with label '//--- start user section %s ---//' missing!" % label)
        dest = r'//--- start user section %s ---//%s//--- end user section %s ---//' % (label, user_sections[label], label)
        # protect backslashes...
        dest = re.sub(r'\\', '\\\\\\\\', dest)
        text = re.compile(src, re.DOTALL).sub(dest, text)
    return text

def insert_text_in_file(filename, text, label, handle_user_sections = False):
    text = '\n' + text.strip() + '\n'
    """
    Replaces the text between the strings '//--- start %s ---//' % label and '//--- end %s ---//' % label
    with text in the file named filename.
    """
    src = r'//--- start %s ---//.*//--- end %s ---//' % (label, label)
    dest = ('//--- start %s ---//' + text + '//--- end %s ---//') % (label, label)
    try:
        old_text = read_text(filename)
        new_text = re.compile(src, re.S).sub(lambda m: dest, old_text)   # lambda prevents interpretation of Escapes (e.g. \p) in dest.
        if handle_user_sections:
            labels = list(extract_user_sections(new_text).keys())
            user_sections = extract_user_sections(old_text)
            new_text = insert_user_sections(new_text, labels, user_sections)
        if old_text == new_text:
            if re.search(src, old_text, re.S) == None:
                print(("Error: label '//--- start %s ---//' not found in file %s" % (label, filename)))
                return False
            else:
                print(('Warning: nothing has changed in file %s' % filename))
        else:
            write_text(filename, new_text)
            print(('Updated file %s' % filename))
        return True
    except IOError as e:
        print(('Error: unable to open file ' + filename + ' ', e))
        return False

def indent_text(text, indent):
    lines = []
    for line in text.split('\n'):
        if len(line) == 0:
            lines.append(line)
        else:
            lines.append(indent + line)
    return '\n'.join(lines)

if __name__ == "__main__":
    text = '''
    data::data_expression_list parameters() const
    {
      return atermpp::list_arg2(*this);
    }
//--- start user section propositional_variable_instantiation ---//
//--- end user section propositional_variable_instantiation ---//
};
'''
    user_sections = { 'propositional_variable_instantiation': 'abc' }
    labels = list(user_sections.keys())
    print((extract_user_sections(text)))
    print((insert_user_sections(text, labels, user_sections)))
