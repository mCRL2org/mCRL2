import re
import string
from path import *
from mcrl2_classes import *

TRAVERSE_FUNCTION = r'''void operator()(const QUALIFIED_NODE& x)
{
  static_cast<Derived&>(*this).enter(x);VISIT_FUNCTIONS
  static_cast<Derived&>(*this).leave(x);
}
'''

def make_traverser_inc_file(filename, text):
    result = []
    classes = parse_classes(text)
    for c in classes:
        (aterm, constructor, description) = c
        f = FunctionDeclaration(constructor)
        print 'generating traverse functions for class', f.name()
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

    ctext = '\n'.join(result)

    #----------------------------------------------------------------------------------------#
    # N.B. THIS IS AN UGLY HACK to deal with the optional time function in some LPS classes
    # TODO: investigate if the time interface can be improved
    ctext = re.sub(r'static_cast<Derived&>\(\*this\)\(x.time\(\)\)', 'if (x.has_time()) static_cast<Derived&>(*this)(x.time());', ctext)   
    #----------------------------------------------------------------------------------------#

    text = path(filename).text()
    text = re.compile(
                 r'//--- start generated code ---//.*//--- end generated code ---//', re.S).sub(
                  '//--- start generated code ---//\n' + ctext + '//--- end generated code ---//',
                  text)
    path(filename).write_text(text)   

PROCESS_ADDITIONAL_CLASSES = '''
ActId | lps::action_label(const core::identifier_string& name, const data::sort_expression_list& sorts) | An action label
'''
  
make_traverser_inc_file('../../process/include/mcrl2/process/detail/traverser.inc', PROCESS_ADDITIONAL_CLASSES + PROCESS_EXPRESSION_CLASSES + PROCESS_CLASSES)
make_traverser_inc_file('../../lps/include/mcrl2/lps/detail/traverser.inc', LPS_CLASSES)
make_traverser_inc_file('../../pbes/include/mcrl2/pbes/detail/traverser.inc', PBES_EXPRESSION_CLASSES + PBES_CLASSES)
