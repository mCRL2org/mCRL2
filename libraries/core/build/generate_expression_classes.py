import re
import string
from path import *
from mcrl2_classes import *

# Example of generated class:
#
# /// \brief The or operator for state formulas
# class or_: public state_formula
# {
#   public:
#     /// \brief Constructor.
#     /// \param term A term
#     or_(atermpp::aterm_appl term)
#       : state_formula(term)
#     {
#       assert(core::detail::check_term_StateOr(m_term));
#     }
# 
#     /// \brief Constructor.
#     /// \param left A process expression
#     /// \param d A data expression
#     or_(const state_formula& left, const state_formula& right)
#       : or_(core::detail::gsMakeStateOr(left, right))
#     {}
# };
CLASS_DEFINITION = r'''/// \\brief DESCRIPTION
class CLASSNAME: public SUPERCLASS
{
  public:
    /// \\brief Constructor.
    /// \\param term A term
    CLASSNAME(atermpp::aterm_appl term)
      : SUPERCLASS(term)
    {
      assert(core::detail::check_term_ATERM(m_term));
    }

    /// \\brief Constructor.
    CONSTRUCTOR
      : SUPERCLASS(core::detail::gsMakeATERM(PARAMETERS))
    {}MEMBER_FUNCTIONS
};'''

# generates the actual code for expression classses, by substituting values in the
# template CLASS_DEFINITION
#
# example input:
#
# ActTrue   | true_()  | The value true for action formulas  
# ActFalse  | false_() | The value false for action formulas 
#
# returns a sequence of class definitions
def generate_classes(text, superclass):
    result = []
    classes = parse_classes(text)
    for c in classes:
        (aterm, constructor, description) = c
        f = FunctionDeclaration(constructor)
        if f.qualified_name().find('::') != -1:
            continue
        classname = f.name()
        print 'generating class', classname

        member_functions = f.class_member_functions()
        mtext = '\n\n'.join(member_functions)
        if mtext != '':
            mtext = '\n\n' + mtext

        parameters = [p.name() for p in f.parameters()]
        ptext = ', '.join(parameters)

        ctext = CLASS_DEFINITION
        ctext = re.sub('DESCRIPTION'     , description, ctext)
        ctext = re.sub('CLASSNAME'       , classname  , ctext)
        ctext = re.sub('ATERM'           , aterm      , ctext)
        ctext = re.sub('CONSTRUCTOR'     , constructor, ctext)
        ctext = re.sub('PARAMETERS'      , ptext      , ctext)
        ctext = re.sub('SUPERCLASS'      , superclass , ctext)
        ctext = re.sub('MEMBER_FUNCTIONS', mtext, ctext)
        result.append(ctext)
    return result
                                                                                          
def make_expression_classes(filename, class_text, class_name):
    classes = generate_classes(class_text, class_name)
    ctext = '\n\n'.join(classes) + '\n'
    text = path(filename).text()
    text = re.compile(r'//--- start generated expression classes ---//.*//--- end generated expression classes ---//', re.S).sub(
                  '//--- start generated expression classes ---//\n' + ctext + '//--- end generated expression classes ---//',
                  text)
    path(filename).write_text(text)   

make_expression_classes('../../lps/include/mcrl2/modal_formula/state_formula.h', STATE_FORMULA_CLASSES, 'state_formula')
make_expression_classes('../../lps/include/mcrl2/modal_formula/action_formula.h', ACTION_FORMULA_CLASSES, 'action_formula')
make_expression_classes('../../process/include/mcrl2/process/process_expression.h', PROCESS_EXPRESSION_CLASSES, 'process_expression')
make_expression_classes('../../pbes/include/mcrl2/pbes/pbes_expression.h', PBES_EXPRESSION_CLASSES, 'pbes_expression')
