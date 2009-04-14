from path import *
import re

LICENSE = '''// Author(s): %(author)s
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \\file %(filename)s
/// \\brief Add your file description here.

'''

LICENSE_LINE_0 = LICENSE.split('\n')[0]

def add_license(text, filename, year, author):
    if re.search('Software License', text):
        return text
    else:
        return LICENSE % {
            'filename' : filename,
            #'year'     : year,
            'author'   : author
        } + text

def remove_license(text):
    while True:
        paragraphs = re.split('\n\s*\n', text)
        if len(paragraphs) <= 1:
            break
        lines = paragraphs[0].split('\n')
        is_license_paragraph = False
        for line in lines:
            if not line.startswith('//'):
                break
            if re.search(r'\\file', text) != None or re.search('Software License', text) != None:
                is_license_paragraph = True
        if is_license_paragraph:
            text = text[len(paragraphs[0]):].strip() + '\n'
        else:
            break
    return text

def replace_include_guards(text, file):
    filename = file.splitext()[0]
    filename = re.sub(r'[/\\]', '_', filename)
    filename = re.sub(r'.*mcrl2', 'mcrl2', filename)
    guard = filename.upper() + '_H'
    m = re.compile(r'#ifndef\s*(\w*)_H\s*#define\s*(\w*)_H[\w\W]*#endif\s*//\s*(\w*)_H\s*$', re.M).match(text)
    if m != None and (m.group(1) == m.group(2) == m.group(3)):
        text = re.sub(m.group(1) + '_H', guard, text)
    return text

def replace_data(text, filename, year = None, author = None):
    text = re.sub(r'///\s*\\file', r'/// \\file ' + filename, text)
    if year != None:
        text = re.sub(r'^//  Copyright\s*([\d,]*)\s*(.*)', 'aap', text)

    if re.search('Software License', text):
        return text
    else:
        return LICENSE % {
            'filename' : filename,
            'year'     : year,
            'author'   : author
        } + text

def process_dir(year, author, dir, ext):
    for file in path(dir).walkfiles(ext):
        old_text = file.text()
        assert(file.startswith(dir))
        filename = re.sub(r'\\', r'/', file[len(dir)+1:])

        print filename
        text = old_text
        text = remove_license(text)
        text = replace_include_guards(text, file)
        text = add_license(text, filename, year, author)
        if text != old_text:
            file.write_text(text)

year = '2007'
author = 'Wieger Wesselink'
#process_dir(year, author, '../include', '*.h')
#process_dir(year, author, '../source', '*.cpp')
#process_dir(year, author, '../example', '*.cpp')
#process_dir(year, author, '../example', '*.h')
#process_dir(year, author, '../test', '*.cpp')
#process_dir(year, author, '../../atermpp/include', '*.h')
#process_dir(year, author, '../../atermpp/example', '*.cpp')
#process_dir(year, author, '../../atermpp/example', '*.h')
#process_dir(year, author, '../../atermpp/test', '*.cpp')
#process_dir(year, author, '../../lps/include/mcrl2/data/detail', '*.h')
#process_dir(year, author, '../../pbes/include/mcrl2/pbes/detail', '*.h')
process_dir(year, author, '../../lps/include/mcrl2/lps/detail', '*.h')

raw_input('done')