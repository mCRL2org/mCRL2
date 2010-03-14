#~ Copyright 2009, 2010 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import re
from path import *
from mcrl2_classes import *
from mcrl2_utility import *

TRAVERSE_FUNCTION = r'''void operator()(const QUALIFIED_NODE& x)
{
  static_cast<Derived&>(*this).enter(x);VISIT_FUNCTIONS
  static_cast<Derived&>(*this).leave(x);
}
'''

def make_traverser_inc_file(filename, class_text, expression_class = None, expression_text = None):
    result = []
    classes = parse_classes(class_text)
    for c in classes:
        print 'generating traverse functions for class', c.name()
        f = c.constructor
        visit_functions = []       
        for p in f.parameters():
            #----------------------------------------------------------------------------------------#
            # N.B. The data traverser skips data_specification, so it needs to be done here too!
            # TODO: investigate why it is skipped
            if p.type().find('data_specification') != -1:
                continue
            visit_functions.append('\n  static_cast<Derived&>(*this)(x.%s());' % p.name())
        vtext = ''.join(visit_functions)
        ctext = TRAVERSE_FUNCTION
        ctext = re.sub('QUALIFIED_NODE', f.qualified_name(), ctext)
        ctext = re.sub('VISIT_FUNCTIONS', vtext, ctext)
        if f.is_template():
            ctext = 'template <typename ' + ', typename '.join(f.template_arguments()) + '>\n' + ctext
        result.append(ctext)

    if expression_class != None:
        ctext = TRAVERSE_FUNCTION
        ctext = re.sub('QUALIFIED_NODE', expression_class, ctext)
        classes = parse_classes(expression_text)
        visit_functions = []
        for c in classes:
            f = c.constructor
            visit_functions.append('if (%sis_%s(x)) { static_cast<Derived&>(*this)(%s(x)); }' % (f.qualifier(), f.name(), f.qualified_name()))
        vtext = '\n  ' + '\n  else '.join(visit_functions)
        ctext = re.sub('VISIT_FUNCTIONS', vtext, ctext)
        result.append(ctext)
    ctext = '\n'.join(result)

    #----------------------------------------------------------------------------------------#
    # N.B. THIS IS AN UGLY HACK to deal with the optional time function in some LPS classes
    # TODO: investigate if the time interface can be improved
    ctext = re.sub(r'static_cast<Derived&>\(\*this\)\(x.time\(\)\)', 'if (x.has_time()) static_cast<Derived&>(*this)(x.time());', ctext)   
    #----------------------------------------------------------------------------------------#

    insert_text_in_file(filename, ctext, 'generated code')

PROCESS_ADDITIONAL_CLASSES = '''
ActId | lps::action_label(const core::identifier_string& name, const data::sort_expression_list& sorts) | An action label
'''

if __name__ == "__main__":
    make_traverser_inc_file('../../process/include/mcrl2/process/detail/traverser.inc.h', PROCESS_ADDITIONAL_CLASSES + PROCESS_EXPRESSION_CLASSES + PROCESS_CLASSES, 'process_expression', PROCESS_EXPRESSION_CLASSES)
    make_traverser_inc_file('../../lps/include/mcrl2/lps/detail/traverser.inc.h', LPS_CLASSES)
    make_traverser_inc_file('../../pbes/include/mcrl2/pbes/detail/traverser.inc.h', PBES_EXPRESSION_CLASSES + PBES_CLASSES)
    make_traverser_inc_file('../../lps/include/mcrl2/modal_formula/detail/traverser.inc.h', STATE_FORMULA_CLASSES)
