#include "mcrl2/fdr/any_expression.h"
#include "mcrl2/fdr/boolean_expression.h"
#include "mcrl2/fdr/check_expression.h"
#include "mcrl2/fdr/common_expression.h"
#include "mcrl2/fdr/comprehension_expression.h"
#include "mcrl2/fdr/definition_expression.h"
#include "mcrl2/fdr/dotted_expression.h"
#include "mcrl2/fdr/expression.h"
#include "mcrl2/fdr/failuremodel_expression.h"
#include "mcrl2/fdr/field_expression.h"
#include "mcrl2/fdr/lambda_expression.h"
#include "mcrl2/fdr/linkpar_expression.h"
#include "mcrl2/fdr/model_expression.h"
#include "mcrl2/fdr/numeric_expression.h"
#include "mcrl2/fdr/process_expression.h"
#include "mcrl2/fdr/seq_expression.h"
#include "mcrl2/fdr/set_expression.h"
#include "mcrl2/fdr/targ_expression.h"
#include "mcrl2/fdr/test_expression.h"
#include "mcrl2/fdr/testtype_expression.h"
#include "mcrl2/fdr/trname_expression.h"
#include "mcrl2/fdr/tuple_expression.h"
#include "mcrl2/fdr/type_expression.h"
#include "mcrl2/fdr/vartype_expression.h"

namespace mcrl2 {

namespace fdr {

//--- start generated boolean expression class definitions ---//
    /// \brief Default constructor.
    true_::true_()
      : boolean_expression(fdr::detail::constructtrue())
    {}

    /// \brief Constructor.
    /// \param term A term
    true_::true_(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_true(m_term));
    }

    /// \brief Default constructor.
    false_::false_()
      : boolean_expression(fdr::detail::constructfalse())
    {}

    /// \brief Constructor.
    /// \param term A term
    false_::false_(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_false(m_term));
    }

    /// \brief Default constructor.
    and_::and_()
      : boolean_expression(fdr::detail::constructAnd())
    {}

    /// \brief Constructor.
    /// \param term A term
    and_::and_(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_And(m_term));
    }

    /// \brief Constructor.
    and_::and_(const boolean_expression& left, const boolean_expression& right)
      : boolean_expression(fdr::detail::gsMakeAnd(left, right))
    {}

    boolean_expression and_::left() const
    {
      return atermpp::arg1(*this);
    }

    boolean_expression and_::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    or_::or_()
      : boolean_expression(fdr::detail::constructOr())
    {}

    /// \brief Constructor.
    /// \param term A term
    or_::or_(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_Or(m_term));
    }

    /// \brief Constructor.
    or_::or_(const boolean_expression& left, const boolean_expression& right)
      : boolean_expression(fdr::detail::gsMakeOr(left, right))
    {}

    boolean_expression or_::left() const
    {
      return atermpp::arg1(*this);
    }

    boolean_expression or_::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    not_::not_()
      : boolean_expression(fdr::detail::constructNot())
    {}

    /// \brief Constructor.
    /// \param term A term
    not_::not_(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_Not(m_term));
    }

    /// \brief Constructor.
    not_::not_(const boolean_expression& operand)
      : boolean_expression(fdr::detail::gsMakeNot(operand))
    {}

    boolean_expression not_::operand() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    null::null()
      : boolean_expression(fdr::detail::constructNull())
    {}

    /// \brief Constructor.
    /// \param term A term
    null::null(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_Null(m_term));
    }

    /// \brief Constructor.
    null::null(const seq_expression& seq)
      : boolean_expression(fdr::detail::gsMakeNull(seq))
    {}

    seq_expression null::seq() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    elem::elem()
      : boolean_expression(fdr::detail::constructElem())
    {}

    /// \brief Constructor.
    /// \param term A term
    elem::elem(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_Elem(m_term));
    }

    /// \brief Constructor.
    elem::elem(const expression& expr, const seq_expression& seq)
      : boolean_expression(fdr::detail::gsMakeElem(expr, seq))
    {}

    expression elem::expr() const
    {
      return atermpp::arg1(*this);
    }

    seq_expression elem::seq() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    member::member()
      : boolean_expression(fdr::detail::constructMember())
    {}

    /// \brief Constructor.
    /// \param term A term
    member::member(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_Member(m_term));
    }

    /// \brief Constructor.
    member::member(const expression& expr, set_expression& set)
      : boolean_expression(fdr::detail::gsMakeMember(expr, set))
    {}

    expression member::expr() const
    {
      return atermpp::arg1(*this);
    }

    set_expression member::set() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    empty::empty()
      : boolean_expression(fdr::detail::constructEmpty())
    {}

    /// \brief Constructor.
    /// \param term A term
    empty::empty(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_Empty(m_term));
    }

    /// \brief Constructor.
    empty::empty(const set_expression& set)
      : boolean_expression(fdr::detail::gsMakeEmpty(set))
    {}

    set_expression empty::set() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    equal::equal()
      : boolean_expression(fdr::detail::constructEqual())
    {}

    /// \brief Constructor.
    /// \param term A term
    equal::equal(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_Equal(m_term));
    }

    /// \brief Constructor.
    equal::equal(const expression& left, const expression& right)
      : boolean_expression(fdr::detail::gsMakeEqual(left, right))
    {}

    expression equal::left() const
    {
      return atermpp::arg1(*this);
    }

    expression equal::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    notequal::notequal()
      : boolean_expression(fdr::detail::constructNotEqual())
    {}

    /// \brief Constructor.
    /// \param term A term
    notequal::notequal(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_NotEqual(m_term));
    }

    /// \brief Constructor.
    notequal::notequal(const expression& left, const expression& right)
      : boolean_expression(fdr::detail::gsMakeNotEqual(left, right))
    {}

    expression notequal::left() const
    {
      return atermpp::arg1(*this);
    }

    expression notequal::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    less::less()
      : boolean_expression(fdr::detail::constructLess())
    {}

    /// \brief Constructor.
    /// \param term A term
    less::less(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_Less(m_term));
    }

    /// \brief Constructor.
    less::less(const expression& left, const expression& right)
      : boolean_expression(fdr::detail::gsMakeLess(left, right))
    {}

    expression less::left() const
    {
      return atermpp::arg1(*this);
    }

    expression less::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    lessorequal::lessorequal()
      : boolean_expression(fdr::detail::constructLessOrEqual())
    {}

    /// \brief Constructor.
    /// \param term A term
    lessorequal::lessorequal(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_LessOrEqual(m_term));
    }

    /// \brief Constructor.
    lessorequal::lessorequal(const expression& left, const expression& right)
      : boolean_expression(fdr::detail::gsMakeLessOrEqual(left, right))
    {}

    expression lessorequal::left() const
    {
      return atermpp::arg1(*this);
    }

    expression lessorequal::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    greater::greater()
      : boolean_expression(fdr::detail::constructGreater())
    {}

    /// \brief Constructor.
    /// \param term A term
    greater::greater(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_Greater(m_term));
    }

    /// \brief Constructor.
    greater::greater(const expression& left, const expression& right)
      : boolean_expression(fdr::detail::gsMakeGreater(left, right))
    {}

    expression greater::left() const
    {
      return atermpp::arg1(*this);
    }

    expression greater::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    greaterorequal::greaterorequal()
      : boolean_expression(fdr::detail::constructGreaterOrEqual())
    {}

    /// \brief Constructor.
    /// \param term A term
    greaterorequal::greaterorequal(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_GreaterOrEqual(m_term));
    }

    /// \brief Constructor.
    greaterorequal::greaterorequal(const expression& left, const expression& right)
      : boolean_expression(fdr::detail::gsMakeGreaterOrEqual(left, right))
    {}

    expression greaterorequal::left() const
    {
      return atermpp::arg1(*this);
    }

    expression greaterorequal::right() const
    {
      return atermpp::arg2(*this);
    }
//--- end generated boolean expression class definitions ---//

//--- start generated common expression class definitions ---//
    /// \brief Default constructor.
    conditional::conditional()
      : common_expression(fdr::detail::constructConditional())
    {}

    /// \brief Constructor.
    /// \param term A term
    conditional::conditional(atermpp::aterm_appl term)
      : common_expression(term)
    {
      assert(fdr::detail::check_term_Conditional(m_term));
    }

    /// \brief Constructor.
    conditional::conditional(const boolean_expression& guard, const any_expression& thenpart, const any_expression& elsepart)
      : common_expression(fdr::detail::gsMakeConditional(guard, thenpart, elsepart))
    {}

    boolean_expression conditional::guard() const
    {
      return atermpp::arg1(*this);
    }

    any_expression conditional::thenpart() const
    {
      return atermpp::arg2(*this);
    }

    any_expression conditional::elsepart() const
    {
      return atermpp::arg3(*this);
    }

    /// \brief Default constructor.
    identifier::identifier()
      : common_expression(fdr::detail::constructName())
    {}

    /// \brief Constructor.
    /// \param term A term
    identifier::identifier(atermpp::aterm_appl term)
      : common_expression(term)
    {
      assert(fdr::detail::check_term_Name(m_term));
    }

    /// \brief Constructor.
    identifier::identifier(const name& id)
      : common_expression(fdr::detail::gsMakeName(id))
    {}

    name identifier::id() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    lambdaappl::lambdaappl()
      : common_expression(fdr::detail::constructLambdaAppl())
    {}

    /// \brief Constructor.
    /// \param term A term
    lambdaappl::lambdaappl(atermpp::aterm_appl term)
      : common_expression(term)
    {
      assert(fdr::detail::check_term_LambdaAppl(m_term));
    }

    /// \brief Constructor.
    lambdaappl::lambdaappl(const lambda_expression& lambda, const expression_list& exprs)
      : common_expression(fdr::detail::gsMakeLambdaAppl(lambda, exprs))
    {}

    lambda_expression lambdaappl::lambda() const
    {
      return atermpp::arg1(*this);
    }

    expression_list lambdaappl::exprs() const
    {
      return atermpp::list_arg2(*this);
    }

    /// \brief Default constructor.
    localdef::localdef()
      : common_expression(fdr::detail::constructLocalDef())
    {}

    /// \brief Constructor.
    /// \param term A term
    localdef::localdef(atermpp::aterm_appl term)
      : common_expression(term)
    {
      assert(fdr::detail::check_term_LocalDef(m_term));
    }

    /// \brief Constructor.
    localdef::localdef(const definition_expression_list& defs, const any_expression& within)
      : common_expression(fdr::detail::gsMakeLocalDef(defs, within))
    {}

    definition_expression_list localdef::defs() const
    {
      return atermpp::list_arg1(*this);
    }

    any_expression localdef::within() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    bracketed::bracketed()
      : common_expression(fdr::detail::constructBracketed())
    {}

    /// \brief Constructor.
    /// \param term A term
    bracketed::bracketed(atermpp::aterm_appl term)
      : common_expression(term)
    {
      assert(fdr::detail::check_term_Bracketed(m_term));
    }

    /// \brief Constructor.
    bracketed::bracketed(const any_expression& operand)
      : common_expression(fdr::detail::gsMakeBracketed(operand))
    {}

    any_expression bracketed::operand() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    pattern::pattern()
      : common_expression(fdr::detail::constructPattern())
    {}

    /// \brief Constructor.
    /// \param term A term
    pattern::pattern(atermpp::aterm_appl term)
      : common_expression(term)
    {
      assert(fdr::detail::check_term_Pattern(m_term));
    }

    /// \brief Constructor.
    pattern::pattern(const any_expression& left, const any_expression& right)
      : common_expression(fdr::detail::gsMakePattern(left, right))
    {}

    any_expression pattern::left() const
    {
      return atermpp::arg1(*this);
    }

    any_expression pattern::right() const
    {
      return atermpp::arg2(*this);
    }
//--- end generated common expression class definitions ---//

//--- start generated comprehension expression class definitions ---//
    /// \brief Default constructor.
    nil::nil()
      : comprehension_expression(fdr::detail::constructNil())
    {}

    /// \brief Constructor.
    /// \param term A term
    nil::nil(atermpp::aterm_appl term)
      : comprehension_expression(term)
    {
      assert(fdr::detail::check_term_Nil(m_term));
    }

    /// \brief Default constructor.
    bcomprehension::bcomprehension()
      : comprehension_expression(fdr::detail::constructBComprehension())
    {}

    /// \brief Constructor.
    /// \param term A term
    bcomprehension::bcomprehension(atermpp::aterm_appl term)
      : comprehension_expression(term)
    {
      assert(fdr::detail::check_term_BComprehension(m_term));
    }

    /// \brief Constructor.
    bcomprehension::bcomprehension(const boolean_expression& operand)
      : comprehension_expression(fdr::detail::gsMakeBComprehension(operand))
    {}

    boolean_expression bcomprehension::operand() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    ecomprehension::ecomprehension()
      : comprehension_expression(fdr::detail::constructEComprehension())
    {}

    /// \brief Constructor.
    /// \param term A term
    ecomprehension::ecomprehension(atermpp::aterm_appl term)
      : comprehension_expression(term)
    {
      assert(fdr::detail::check_term_EComprehension(m_term));
    }

    /// \brief Constructor.
    ecomprehension::ecomprehension(const expression& left, const expression& right)
      : comprehension_expression(fdr::detail::gsMakeEComprehension(left, right))
    {}

    expression ecomprehension::left() const
    {
      return atermpp::arg1(*this);
    }

    expression ecomprehension::right() const
    {
      return atermpp::arg2(*this);
    }
//--- end generated comprehension expression class definitions ---//

//--- start generated dotted expression class definitions ---//
    /// \brief Default constructor.
    dot::dot()
      : dotted_expression(fdr::detail::constructDot())
    {}

    /// \brief Constructor.
    /// \param term A term
    dot::dot(atermpp::aterm_appl term)
      : dotted_expression(term)
    {
      assert(fdr::detail::check_term_Dot(m_term));
    }

    /// \brief Constructor.
    dot::dot(const expression& left, expression& right)
      : dotted_expression(fdr::detail::gsMakeDot(left, right))
    {}

    expression dot::left() const
    {
      return atermpp::arg1(*this);
    }

    expression dot::right() const
    {
      return atermpp::arg2(*this);
    }
//--- end generated dotted expression class definitions ---//

//--- start generated  expression class definitions ---//
    /// \brief Default constructor.
    numb::numb()
      : expression(fdr::detail::constructNumb())
    {}

    /// \brief Constructor.
    /// \param term A term
    numb::numb(atermpp::aterm_appl term)
      : expression(term)
    {
      assert(fdr::detail::check_term_Numb(m_term));
    }

    /// \brief Constructor.
    numb::numb(const numeric_expression& operand)
      : expression(fdr::detail::gsMakeNumb(operand))
    {}

    numeric_expression numb::operand() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    bool_::bool_()
      : expression(fdr::detail::constructBool())
    {}

    /// \brief Constructor.
    /// \param term A term
    bool_::bool_(atermpp::aterm_appl term)
      : expression(term)
    {
      assert(fdr::detail::check_term_Bool(m_term));
    }

    /// \brief Constructor.
    bool_::bool_(const boolean_expression& operand)
      : expression(fdr::detail::gsMakeBool(operand))
    {}

    boolean_expression bool_::operand() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    Set::Set()
      : expression(fdr::detail::constructSet())
    {}

    /// \brief Constructor.
    /// \param term A term
    Set::Set(atermpp::aterm_appl term)
      : expression(term)
    {
      assert(fdr::detail::check_term_Set(m_term));
    }

    /// \brief Constructor.
    Set::Set(const set_expression& operand)
      : expression(fdr::detail::gsMakeSet(operand))
    {}

    set_expression Set::operand() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    Seq::Seq()
      : expression(fdr::detail::constructSeq())
    {}

    /// \brief Constructor.
    /// \param term A term
    Seq::Seq(atermpp::aterm_appl term)
      : expression(term)
    {
      assert(fdr::detail::check_term_Seq(m_term));
    }

    /// \brief Constructor.
    Seq::Seq(const seq_expression& operand)
      : expression(fdr::detail::gsMakeSeq(operand))
    {}

    seq_expression Seq::operand() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    tuple::tuple()
      : expression(fdr::detail::constructTuple())
    {}

    /// \brief Constructor.
    /// \param term A term
    tuple::tuple(atermpp::aterm_appl term)
      : expression(term)
    {
      assert(fdr::detail::check_term_Tuple(m_term));
    }

    /// \brief Constructor.
    tuple::tuple(const tuple_expression& operand)
      : expression(fdr::detail::gsMakeTuple(operand))
    {}

    tuple_expression tuple::operand() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    dotted::dotted()
      : expression(fdr::detail::constructDotted())
    {}

    /// \brief Constructor.
    /// \param term A term
    dotted::dotted(atermpp::aterm_appl term)
      : expression(term)
    {
      assert(fdr::detail::check_term_Dotted(m_term));
    }

    /// \brief Constructor.
    dotted::dotted(const dotted_expression& operand)
      : expression(fdr::detail::gsMakeDotted(operand))
    {}

    dotted_expression dotted::operand() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    lambda::lambda()
      : expression(fdr::detail::constructLambda())
    {}

    /// \brief Constructor.
    /// \param term A term
    lambda::lambda(atermpp::aterm_appl term)
      : expression(term)
    {
      assert(fdr::detail::check_term_Lambda(m_term));
    }

    /// \brief Constructor.
    lambda::lambda(const lambda_expression& operand)
      : expression(fdr::detail::gsMakeLambda(operand))
    {}

    lambda_expression lambda::operand() const
    {
      return atermpp::arg1(*this);
    }
//--- end generated  expression class definitions ---//

//--- start generated lambda expression class definitions ---//
    /// \brief Default constructor.
    lambdaexpr::lambdaexpr()
      : lambda_expression(fdr::detail::constructLambdaExpr())
    {}

    /// \brief Constructor.
    /// \param term A term
    lambdaexpr::lambdaexpr(atermpp::aterm_appl term)
      : lambda_expression(term)
    {
      assert(fdr::detail::check_term_LambdaExpr(m_term));
    }

    /// \brief Constructor.
    lambdaexpr::lambdaexpr(const expression_list& exprs, const any_expression& function)
      : lambda_expression(fdr::detail::gsMakeLambdaExpr(exprs, function))
    {}

    expression_list lambdaexpr::exprs() const
    {
      return atermpp::list_arg1(*this);
    }

    any_expression lambdaexpr::function() const
    {
      return atermpp::arg2(*this);
    }
//--- end generated lambda expression class definitions ---//

//--- start generated linkpar expression class definitions ---//
    /// \brief Default constructor.
    links::links()
      : linkpar_expression(fdr::detail::constructLinks())
    {}

    /// \brief Constructor.
    /// \param term A term
    links::links(atermpp::aterm_appl term)
      : linkpar_expression(term)
    {
      assert(fdr::detail::check_term_Links(m_term));
    }

    /// \brief Constructor.
    links::links(const link_list& linkpars)
      : linkpar_expression(fdr::detail::gsMakeLinks(linkpars))
    {}

    link_list links::linkpars() const
    {
      return atermpp::list_arg1(*this);
    }

    /// \brief Default constructor.
    linksgens::linksgens()
      : linkpar_expression(fdr::detail::constructLinksGens())
    {}

    /// \brief Constructor.
    /// \param term A term
    linksgens::linksgens(atermpp::aterm_appl term)
      : linkpar_expression(term)
    {
      assert(fdr::detail::check_term_LinksGens(m_term));
    }

    /// \brief Constructor.
    linksgens::linksgens(const link_list& linkpars, const comprehension_expression_list& comprs)
      : linkpar_expression(fdr::detail::gsMakeLinksGens(linkpars, comprs))
    {}

    link_list linksgens::linkpars() const
    {
      return atermpp::list_arg1(*this);
    }

    comprehension_expression_list linksgens::comprs() const
    {
      return atermpp::list_arg2(*this);
    }
//--- end generated linkpar expression class definitions ---//

//--- start generated numeric expression class definitions ---//
    /// \brief Default constructor.
    number::number()
      : numeric_expression(fdr::detail::constructNumber())
    {}

    /// \brief Constructor.
    /// \param term A term
    number::number(atermpp::aterm_appl term)
      : numeric_expression(term)
    {
      assert(fdr::detail::check_term_Number(m_term));
    }

    /// \brief Constructor.
    number::number(const number& operand)
      : numeric_expression(fdr::detail::gsMakeNumber(operand))
    {}

    number number::operand() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    card::card()
      : numeric_expression(fdr::detail::constructCard())
    {}

    /// \brief Constructor.
    /// \param term A term
    card::card(atermpp::aterm_appl term)
      : numeric_expression(term)
    {
      assert(fdr::detail::check_term_Card(m_term));
    }

    /// \brief Constructor.
    card::card(const set_expression& set)
      : numeric_expression(fdr::detail::gsMakeCard(set))
    {}

    set_expression card::set() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    length::length()
      : numeric_expression(fdr::detail::constructLength())
    {}

    /// \brief Constructor.
    /// \param term A term
    length::length(atermpp::aterm_appl term)
      : numeric_expression(term)
    {
      assert(fdr::detail::check_term_Length(m_term));
    }

    /// \brief Constructor.
    length::length(const seq_expression& seq)
      : numeric_expression(fdr::detail::gsMakeLength(seq))
    {}

    seq_expression length::seq() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    plus::plus()
      : numeric_expression(fdr::detail::constructPlus())
    {}

    /// \brief Constructor.
    /// \param term A term
    plus::plus(atermpp::aterm_appl term)
      : numeric_expression(term)
    {
      assert(fdr::detail::check_term_Plus(m_term));
    }

    /// \brief Constructor.
    plus::plus(const numeric_expression& left, const numeric_expression& right)
      : numeric_expression(fdr::detail::gsMakePlus(left, right))
    {}

    numeric_expression plus::left() const
    {
      return atermpp::arg1(*this);
    }

    numeric_expression plus::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    minus::minus()
      : numeric_expression(fdr::detail::constructMinus())
    {}

    /// \brief Constructor.
    /// \param term A term
    minus::minus(atermpp::aterm_appl term)
      : numeric_expression(term)
    {
      assert(fdr::detail::check_term_Minus(m_term));
    }

    /// \brief Constructor.
    minus::minus(const numeric_expression& left, const numeric_expression& right)
      : numeric_expression(fdr::detail::gsMakeMinus(left, right))
    {}

    numeric_expression minus::left() const
    {
      return atermpp::arg1(*this);
    }

    numeric_expression minus::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    times::times()
      : numeric_expression(fdr::detail::constructTimes())
    {}

    /// \brief Constructor.
    /// \param term A term
    times::times(atermpp::aterm_appl term)
      : numeric_expression(term)
    {
      assert(fdr::detail::check_term_Times(m_term));
    }

    /// \brief Constructor.
    times::times(const numeric_expression& left, const numeric_expression& right)
      : numeric_expression(fdr::detail::gsMakeTimes(left, right))
    {}

    numeric_expression times::left() const
    {
      return atermpp::arg1(*this);
    }

    numeric_expression times::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    div::div()
      : numeric_expression(fdr::detail::constructDiv())
    {}

    /// \brief Constructor.
    /// \param term A term
    div::div(atermpp::aterm_appl term)
      : numeric_expression(term)
    {
      assert(fdr::detail::check_term_Div(m_term));
    }

    /// \brief Constructor.
    div::div(const numeric_expression& left, const numeric_expression& right)
      : numeric_expression(fdr::detail::gsMakeDiv(left, right))
    {}

    numeric_expression div::left() const
    {
      return atermpp::arg1(*this);
    }

    numeric_expression div::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    mod::mod()
      : numeric_expression(fdr::detail::constructMod())
    {}

    /// \brief Constructor.
    /// \param term A term
    mod::mod(atermpp::aterm_appl term)
      : numeric_expression(term)
    {
      assert(fdr::detail::check_term_Mod(m_term));
    }

    /// \brief Constructor.
    mod::mod(const numeric_expression& left, const numeric_expression& right)
      : numeric_expression(fdr::detail::gsMakeMod(left, right))
    {}

    numeric_expression mod::left() const
    {
      return atermpp::arg1(*this);
    }

    numeric_expression mod::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    min::min()
      : numeric_expression(fdr::detail::constructMin())
    {}

    /// \brief Constructor.
    /// \param term A term
    min::min(atermpp::aterm_appl term)
      : numeric_expression(term)
    {
      assert(fdr::detail::check_term_Min(m_term));
    }

    /// \brief Constructor.
    min::min(const numeric_expression& operand)
      : numeric_expression(fdr::detail::gsMakeMin(operand))
    {}

    numeric_expression min::operand() const
    {
      return atermpp::arg1(*this);
    }
//--- end generated numeric expression class definitions ---//

//--- start generated seq expression class definitions ---//
    /// \brief Default constructor.
    cat::cat()
      : seq_expression(fdr::detail::constructCat())
    {}

    /// \brief Constructor.
    /// \param term A term
    cat::cat(atermpp::aterm_appl term)
      : seq_expression(term)
    {
      assert(fdr::detail::check_term_Cat(m_term));
    }

    /// \brief Constructor.
    cat::cat(const seq_expression& left, const seq_expression& right)
      : seq_expression(fdr::detail::gsMakeCat(left, right))
    {}

    seq_expression cat::left() const
    {
      return atermpp::arg1(*this);
    }

    seq_expression cat::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    concat::concat()
      : seq_expression(fdr::detail::constructConcat())
    {}

    /// \brief Constructor.
    /// \param term A term
    concat::concat(atermpp::aterm_appl term)
      : seq_expression(term)
    {
      assert(fdr::detail::check_term_Concat(m_term));
    }

    /// \brief Constructor.
    concat::concat(const seq_expression& seq)
      : seq_expression(fdr::detail::gsMakeConcat(seq))
    {}

    seq_expression concat::seq() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    head::head()
      : seq_expression(fdr::detail::constructHead())
    {}

    /// \brief Constructor.
    /// \param term A term
    head::head(atermpp::aterm_appl term)
      : seq_expression(term)
    {
      assert(fdr::detail::check_term_Head(m_term));
    }

    /// \brief Constructor.
    head::head(const seq_expression& seq)
      : seq_expression(fdr::detail::gsMakeHead(seq))
    {}

    seq_expression head::seq() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    tail::tail()
      : seq_expression(fdr::detail::constructTail())
    {}

    /// \brief Constructor.
    /// \param term A term
    tail::tail(atermpp::aterm_appl term)
      : seq_expression(term)
    {
      assert(fdr::detail::check_term_Tail(m_term));
    }

    /// \brief Constructor.
    tail::tail(const seq_expression& seq)
      : seq_expression(fdr::detail::gsMakeTail(seq))
    {}

    seq_expression tail::seq() const
    {
      return atermpp::arg1(*this);
    }
//--- end generated seq expression class definitions ---//

//--- start generated set expression class definitions ---//
    /// \brief Default constructor.
    chanset::chanset()
      : set_expression(fdr::detail::constructChanSet())
    {}

    /// \brief Constructor.
    /// \param term A term
    chanset::chanset(atermpp::aterm_appl term)
      : set_expression(term)
    {
      assert(fdr::detail::check_term_ChanSet(m_term));
    }

    /// \brief Constructor.
    chanset::chanset(const targ_expression& argument)
      : set_expression(fdr::detail::gsMakeChanSet(argument))
    {}

    targ_expression chanset::argument() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    union_::union_()
      : set_expression(fdr::detail::constructunion())
    {}

    /// \brief Constructor.
    /// \param term A term
    union_::union_(atermpp::aterm_appl term)
      : set_expression(term)
    {
      assert(fdr::detail::check_term_union(m_term));
    }

    /// \brief Constructor.
    union_::union_(const set_expression& left, const set_expression& right)
      : set_expression(fdr::detail::gsMakeunion(left, right))
    {}

    set_expression union_::left() const
    {
      return atermpp::arg1(*this);
    }

    set_expression union_::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    inter::inter()
      : set_expression(fdr::detail::constructinter())
    {}

    /// \brief Constructor.
    /// \param term A term
    inter::inter(atermpp::aterm_appl term)
      : set_expression(term)
    {
      assert(fdr::detail::check_term_inter(m_term));
    }

    /// \brief Constructor.
    inter::inter(const set_expression& left, const set_expression& right)
      : set_expression(fdr::detail::gsMakeinter(left, right))
    {}

    set_expression inter::left() const
    {
      return atermpp::arg1(*this);
    }

    set_expression inter::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    diff::diff()
      : set_expression(fdr::detail::constructdiff())
    {}

    /// \brief Constructor.
    /// \param term A term
    diff::diff(atermpp::aterm_appl term)
      : set_expression(term)
    {
      assert(fdr::detail::check_term_diff(m_term));
    }

    /// \brief Constructor.
    diff::diff(const set_expression& left, const set_expression& right)
      : set_expression(fdr::detail::gsMakediff(left, right))
    {}

    set_expression diff::left() const
    {
      return atermpp::arg1(*this);
    }

    set_expression diff::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    Union::Union()
      : set_expression(fdr::detail::constructUnion())
    {}

    /// \brief Constructor.
    /// \param term A term
    Union::Union(atermpp::aterm_appl term)
      : set_expression(term)
    {
      assert(fdr::detail::check_term_Union(m_term));
    }

    /// \brief Constructor.
    Union::Union(const set_expression& set)
      : set_expression(fdr::detail::gsMakeUnion(set))
    {}

    set_expression Union::set() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    Inter::Inter()
      : set_expression(fdr::detail::constructInter())
    {}

    /// \brief Constructor.
    /// \param term A term
    Inter::Inter(atermpp::aterm_appl term)
      : set_expression(term)
    {
      assert(fdr::detail::check_term_Inter(m_term));
    }

    /// \brief Constructor.
    Inter::Inter(const set_expression& set)
      : set_expression(fdr::detail::gsMakeInter(set))
    {}

    set_expression Inter::set() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    set::set()
      : set_expression(fdr::detail::constructset())
    {}

    /// \brief Constructor.
    /// \param term A term
    set::set(atermpp::aterm_appl term)
      : set_expression(term)
    {
      assert(fdr::detail::check_term_set(m_term));
    }

    /// \brief Constructor.
    set::set(const seq_expression& seq)
      : set_expression(fdr::detail::gsMakeset(seq))
    {}

    seq_expression set::seq() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    extensions::extensions()
      : set_expression(fdr::detail::constructextensions())
    {}

    /// \brief Constructor.
    /// \param term A term
    extensions::extensions(atermpp::aterm_appl term)
      : set_expression(term)
    {
      assert(fdr::detail::check_term_extensions(m_term));
    }

    /// \brief Constructor.
    extensions::extensions(const expression& expr)
      : set_expression(fdr::detail::gsMakeextensions(expr))
    {}

    expression extensions::expr() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    productions::productions()
      : set_expression(fdr::detail::constructproductions())
    {}

    /// \brief Constructor.
    /// \param term A term
    productions::productions(atermpp::aterm_appl term)
      : set_expression(term)
    {
      assert(fdr::detail::check_term_productions(m_term));
    }

    /// \brief Constructor.
    productions::productions(const expression& expr)
      : set_expression(fdr::detail::gsMakeproductions(expr))
    {}

    expression productions::expr() const
    {
      return atermpp::arg1(*this);
    }
//--- end generated set expression class definitions ---//

//--- start generated tuple expression class definitions ---//
    /// \brief Default constructor.
    exprs::exprs()
      : tuple_expression(fdr::detail::constructExprs())
    {}

    /// \brief Constructor.
    /// \param term A term
    exprs::exprs(atermpp::aterm_appl term)
      : tuple_expression(term)
    {
      assert(fdr::detail::check_term_Exprs(m_term));
    }

    /// \brief Constructor.
    exprs::exprs(const expression_list& elements)
      : tuple_expression(fdr::detail::gsMakeExprs(elements))
    {}

    expression_list exprs::elements() const
    {
      return atermpp::list_arg1(*this);
    }
//--- end generated tuple expression class definitions ---//

//--- start generated process expression class definitions ---//
    /// \brief Default constructor.
    stop::stop()
      : process_expression(fdr::detail::constructSTOP())
    {}

    /// \brief Constructor.
    /// \param term A term
    stop::stop(atermpp::aterm_appl term)
      : process_expression(term)
    {
      assert(fdr::detail::check_term_STOP(m_term));
    }

    /// \brief Default constructor.
    skip::skip()
      : process_expression(fdr::detail::constructSKIP())
    {}

    /// \brief Constructor.
    /// \param term A term
    skip::skip(atermpp::aterm_appl term)
      : process_expression(term)
    {
      assert(fdr::detail::check_term_SKIP(m_term));
    }

    /// \brief Default constructor.
    chaos::chaos()
      : process_expression(fdr::detail::constructCHAOS())
    {}

    /// \brief Constructor.
    /// \param term A term
    chaos::chaos(atermpp::aterm_appl term)
      : process_expression(term)
    {
      assert(fdr::detail::check_term_CHAOS(m_term));
    }

    /// \brief Constructor.
    chaos::chaos(const set_expression& set)
      : process_expression(fdr::detail::gsMakeCHAOS(set))
    {}

    set_expression chaos::set() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    prefix::prefix()
      : process_expression(fdr::detail::constructPrefix())
    {}

    /// \brief Constructor.
    /// \param term A term
    prefix::prefix(atermpp::aterm_appl term)
      : process_expression(term)
    {
      assert(fdr::detail::check_term_Prefix(m_term));
    }

    /// \brief Constructor.
    prefix::prefix(const dotted_expression& dotted, const field_expression_list& fields, const process_expression& proc)
      : process_expression(fdr::detail::gsMakePrefix(dotted, fields, proc))
    {}

    dotted_expression prefix::dotted() const
    {
      return atermpp::arg1(*this);
    }

    field_expression_list prefix::fields() const
    {
      return atermpp::list_arg2(*this);
    }

    process_expression prefix::proc() const
    {
      return atermpp::arg3(*this);
    }

    /// \brief Default constructor.
    externalchoice::externalchoice()
      : process_expression(fdr::detail::constructExternalChoice())
    {}

    /// \brief Constructor.
    /// \param term A term
    externalchoice::externalchoice(atermpp::aterm_appl term)
      : process_expression(term)
    {
      assert(fdr::detail::check_term_ExternalChoice(m_term));
    }

    /// \brief Constructor.
    externalchoice::externalchoice(const process_expression& left, const process_expression& right)
      : process_expression(fdr::detail::gsMakeExternalChoice(left, right))
    {}

    process_expression externalchoice::left() const
    {
      return atermpp::arg1(*this);
    }

    process_expression externalchoice::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    internalchoice::internalchoice()
      : process_expression(fdr::detail::constructInternalChoice())
    {}

    /// \brief Constructor.
    /// \param term A term
    internalchoice::internalchoice(atermpp::aterm_appl term)
      : process_expression(term)
    {
      assert(fdr::detail::check_term_InternalChoice(m_term));
    }

    /// \brief Constructor.
    internalchoice::internalchoice(const process_expression& left, const process_expression& right)
      : process_expression(fdr::detail::gsMakeInternalChoice(left, right))
    {}

    process_expression internalchoice::left() const
    {
      return atermpp::arg1(*this);
    }

    process_expression internalchoice::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    sequentialcomposition::sequentialcomposition()
      : process_expression(fdr::detail::constructSequentialComposition())
    {}

    /// \brief Constructor.
    /// \param term A term
    sequentialcomposition::sequentialcomposition(atermpp::aterm_appl term)
      : process_expression(term)
    {
      assert(fdr::detail::check_term_SequentialComposition(m_term));
    }

    /// \brief Constructor.
    sequentialcomposition::sequentialcomposition(const process_expression& left, const process_expression& right)
      : process_expression(fdr::detail::gsMakeSequentialComposition(left, right))
    {}

    process_expression sequentialcomposition::left() const
    {
      return atermpp::arg1(*this);
    }

    process_expression sequentialcomposition::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    interrupt::interrupt()
      : process_expression(fdr::detail::constructInterrupt())
    {}

    /// \brief Constructor.
    /// \param term A term
    interrupt::interrupt(atermpp::aterm_appl term)
      : process_expression(term)
    {
      assert(fdr::detail::check_term_Interrupt(m_term));
    }

    /// \brief Constructor.
    interrupt::interrupt(const process_expression& left, const process_expression& right)
      : process_expression(fdr::detail::gsMakeInterrupt(left, right))
    {}

    process_expression interrupt::left() const
    {
      return atermpp::arg1(*this);
    }

    process_expression interrupt::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    hiding::hiding()
      : process_expression(fdr::detail::constructHiding())
    {}

    /// \brief Constructor.
    /// \param term A term
    hiding::hiding(atermpp::aterm_appl term)
      : process_expression(term)
    {
      assert(fdr::detail::check_term_Hiding(m_term));
    }

    /// \brief Constructor.
    hiding::hiding(const process_expression& proc, const set_expression& set)
      : process_expression(fdr::detail::gsMakeHiding(proc, set))
    {}

    process_expression hiding::proc() const
    {
      return atermpp::arg1(*this);
    }

    set_expression hiding::set() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    interleave::interleave()
      : process_expression(fdr::detail::constructInterleave())
    {}

    /// \brief Constructor.
    /// \param term A term
    interleave::interleave(atermpp::aterm_appl term)
      : process_expression(term)
    {
      assert(fdr::detail::check_term_Interleave(m_term));
    }

    /// \brief Constructor.
    interleave::interleave(const process_expression& left, const process_expression& right)
      : process_expression(fdr::detail::gsMakeInterleave(left, right))
    {}

    process_expression interleave::left() const
    {
      return atermpp::arg1(*this);
    }

    process_expression interleave::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    sharing::sharing()
      : process_expression(fdr::detail::constructSharing())
    {}

    /// \brief Constructor.
    /// \param term A term
    sharing::sharing(atermpp::aterm_appl term)
      : process_expression(term)
    {
      assert(fdr::detail::check_term_Sharing(m_term));
    }

    /// \brief Constructor.
    sharing::sharing(const process_expression& left, const process_expression& right, const set_expression& set)
      : process_expression(fdr::detail::gsMakeSharing(left, right, set))
    {}

    process_expression sharing::left() const
    {
      return atermpp::arg1(*this);
    }

    process_expression sharing::right() const
    {
      return atermpp::arg2(*this);
    }

    set_expression sharing::set() const
    {
      return atermpp::arg3(*this);
    }

    /// \brief Default constructor.
    alphaparallel::alphaparallel()
      : process_expression(fdr::detail::constructAlphaParallel())
    {}

    /// \brief Constructor.
    /// \param term A term
    alphaparallel::alphaparallel(atermpp::aterm_appl term)
      : process_expression(term)
    {
      assert(fdr::detail::check_term_AlphaParallel(m_term));
    }

    /// \brief Constructor.
    alphaparallel::alphaparallel(const process_expression& left, const process_expression& right, const set_expression& left_set, const set_expression& right_set)
      : process_expression(fdr::detail::gsMakeAlphaParallel(left, right, left_set, right_set))
    {}

    process_expression alphaparallel::left() const
    {
      return atermpp::arg1(*this);
    }

    process_expression alphaparallel::right() const
    {
      return atermpp::arg2(*this);
    }

    set_expression alphaparallel::left_set() const
    {
      return atermpp::arg3(*this);
    }

    set_expression alphaparallel::right_set() const
    {
      return atermpp::arg4(*this);
    }

    /// \brief Default constructor.
    repexternalchoice::repexternalchoice()
      : process_expression(fdr::detail::constructRepExternalChoice())
    {}

    /// \brief Constructor.
    /// \param term A term
    repexternalchoice::repexternalchoice(atermpp::aterm_appl term)
      : process_expression(term)
    {
      assert(fdr::detail::check_term_RepExternalChoice(m_term));
    }

    /// \brief Constructor.
    repexternalchoice::repexternalchoice(const setgen& gen, const process_expression& proc)
      : process_expression(fdr::detail::gsMakeRepExternalChoice(gen, proc))
    {}

    setgen repexternalchoice::gen() const
    {
      return atermpp::arg1(*this);
    }

    process_expression repexternalchoice::proc() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    repinternalchoice::repinternalchoice()
      : process_expression(fdr::detail::constructRepInternalChoice())
    {}

    /// \brief Constructor.
    /// \param term A term
    repinternalchoice::repinternalchoice(atermpp::aterm_appl term)
      : process_expression(term)
    {
      assert(fdr::detail::check_term_RepInternalChoice(m_term));
    }

    /// \brief Constructor.
    repinternalchoice::repinternalchoice(const setgen& gen, const process_expression& proc)
      : process_expression(fdr::detail::gsMakeRepInternalChoice(gen, proc))
    {}

    setgen repinternalchoice::gen() const
    {
      return atermpp::arg1(*this);
    }

    process_expression repinternalchoice::proc() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    repsequentialcomposition::repsequentialcomposition()
      : process_expression(fdr::detail::constructRepSequentialComposition())
    {}

    /// \brief Constructor.
    /// \param term A term
    repsequentialcomposition::repsequentialcomposition(atermpp::aterm_appl term)
      : process_expression(term)
    {
      assert(fdr::detail::check_term_RepSequentialComposition(m_term));
    }

    /// \brief Constructor.
    repsequentialcomposition::repsequentialcomposition(const seqgen& gen, const process_expression& proc)
      : process_expression(fdr::detail::gsMakeRepSequentialComposition(gen, proc))
    {}

    seqgen repsequentialcomposition::gen() const
    {
      return atermpp::arg1(*this);
    }

    process_expression repsequentialcomposition::proc() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    repinterleave::repinterleave()
      : process_expression(fdr::detail::constructRepInterleave())
    {}

    /// \brief Constructor.
    /// \param term A term
    repinterleave::repinterleave(atermpp::aterm_appl term)
      : process_expression(term)
    {
      assert(fdr::detail::check_term_RepInterleave(m_term));
    }

    /// \brief Constructor.
    repinterleave::repinterleave(const setgen& gen, const process_expression& proc)
      : process_expression(fdr::detail::gsMakeRepInterleave(gen, proc))
    {}

    setgen repinterleave::gen() const
    {
      return atermpp::arg1(*this);
    }

    process_expression repinterleave::proc() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    repsharing::repsharing()
      : process_expression(fdr::detail::constructRepSharing())
    {}

    /// \brief Constructor.
    /// \param term A term
    repsharing::repsharing(atermpp::aterm_appl term)
      : process_expression(term)
    {
      assert(fdr::detail::check_term_RepSharing(m_term));
    }

    /// \brief Constructor.
    repsharing::repsharing(const setgen& gen, const process_expression& proc, const set_expression& set)
      : process_expression(fdr::detail::gsMakeRepSharing(gen, proc, set))
    {}

    setgen repsharing::gen() const
    {
      return atermpp::arg1(*this);
    }

    process_expression repsharing::proc() const
    {
      return atermpp::arg2(*this);
    }

    set_expression repsharing::set() const
    {
      return atermpp::arg3(*this);
    }

    /// \brief Default constructor.
    repalphaparallel::repalphaparallel()
      : process_expression(fdr::detail::constructRepAlphaParallel())
    {}

    /// \brief Constructor.
    /// \param term A term
    repalphaparallel::repalphaparallel(atermpp::aterm_appl term)
      : process_expression(term)
    {
      assert(fdr::detail::check_term_RepAlphaParallel(m_term));
    }

    /// \brief Constructor.
    repalphaparallel::repalphaparallel(const setgen& gen, const process_expression& proc, const set_expression& set)
      : process_expression(fdr::detail::gsMakeRepAlphaParallel(gen, proc, set))
    {}

    setgen repalphaparallel::gen() const
    {
      return atermpp::arg1(*this);
    }

    process_expression repalphaparallel::proc() const
    {
      return atermpp::arg2(*this);
    }

    set_expression repalphaparallel::set() const
    {
      return atermpp::arg3(*this);
    }

    /// \brief Default constructor.
    untimedtimeout::untimedtimeout()
      : process_expression(fdr::detail::constructUntimedTimeOut())
    {}

    /// \brief Constructor.
    /// \param term A term
    untimedtimeout::untimedtimeout(atermpp::aterm_appl term)
      : process_expression(term)
    {
      assert(fdr::detail::check_term_UntimedTimeOut(m_term));
    }

    /// \brief Constructor.
    untimedtimeout::untimedtimeout(const process_expression& left, const process_expression& right)
      : process_expression(fdr::detail::gsMakeUntimedTimeOut(left, right))
    {}

    process_expression untimedtimeout::left() const
    {
      return atermpp::arg1(*this);
    }

    process_expression untimedtimeout::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    boolguard::boolguard()
      : process_expression(fdr::detail::constructBoolGuard())
    {}

    /// \brief Constructor.
    /// \param term A term
    boolguard::boolguard(atermpp::aterm_appl term)
      : process_expression(term)
    {
      assert(fdr::detail::check_term_BoolGuard(m_term));
    }

    /// \brief Constructor.
    boolguard::boolguard(const boolean_expression& guard, const process_expression& proc)
      : process_expression(fdr::detail::gsMakeBoolGuard(guard, proc))
    {}

    boolean_expression boolguard::guard() const
    {
      return atermpp::arg1(*this);
    }

    process_expression boolguard::proc() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    linkedparallel::linkedparallel()
      : process_expression(fdr::detail::constructLinkedParallel())
    {}

    /// \brief Constructor.
    /// \param term A term
    linkedparallel::linkedparallel(atermpp::aterm_appl term)
      : process_expression(term)
    {
      assert(fdr::detail::check_term_LinkedParallel(m_term));
    }

    /// \brief Constructor.
    linkedparallel::linkedparallel(const process_expression& left, const process_expression& right, const linkpar& linked)
      : process_expression(fdr::detail::gsMakeLinkedParallel(left, right, linked))
    {}

    process_expression linkedparallel::left() const
    {
      return atermpp::arg1(*this);
    }

    process_expression linkedparallel::right() const
    {
      return atermpp::arg2(*this);
    }

    linkpar linkedparallel::linked() const
    {
      return atermpp::arg3(*this);
    }

    /// \brief Default constructor.
    replinkedparallel::replinkedparallel()
      : process_expression(fdr::detail::constructRepLinkedParallel())
    {}

    /// \brief Constructor.
    /// \param term A term
    replinkedparallel::replinkedparallel(atermpp::aterm_appl term)
      : process_expression(term)
    {
      assert(fdr::detail::check_term_RepLinkedParallel(m_term));
    }

    /// \brief Constructor.
    replinkedparallel::replinkedparallel(const seqgen& gen, const process_expression& proc, const linkpar& linked)
      : process_expression(fdr::detail::gsMakeRepLinkedParallel(gen, proc, linked))
    {}

    seqgen replinkedparallel::gen() const
    {
      return atermpp::arg1(*this);
    }

    process_expression replinkedparallel::proc() const
    {
      return atermpp::arg2(*this);
    }

    linkpar replinkedparallel::linked() const
    {
      return atermpp::arg3(*this);
    }
//--- end generated process expression class definitions ---//

//--- start generated definition expression class definitions ---//
    /// \brief Default constructor.
    assign::assign()
      : definition_expression(fdr::detail::constructAssign())
    {}

    /// \brief Constructor.
    /// \param term A term
    assign::assign(atermpp::aterm_appl term)
      : definition_expression(term)
    {
      assert(fdr::detail::check_term_Assign(m_term));
    }

    /// \brief Constructor.
    assign::assign(const any_expression& left, const any_expression& right)
      : definition_expression(fdr::detail::gsMakeAssign(left, right))
    {}

    any_expression assign::left() const
    {
      return atermpp::arg1(*this);
    }

    any_expression assign::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    channel::channel()
      : definition_expression(fdr::detail::constructChannel())
    {}

    /// \brief Constructor.
    /// \param term A term
    channel::channel(atermpp::aterm_appl term)
      : definition_expression(term)
    {
      assert(fdr::detail::check_term_Channel(m_term));
    }

    /// \brief Constructor.
    channel::channel(const name_list& names, const type_expression& type_name)
      : definition_expression(fdr::detail::gsMakeChannel(names, type_name))
    {}

    name_list channel::names() const
    {
      return atermpp::list_arg1(*this);
    }

    type_expression channel::type_name() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    channel::channel()
      : definition_expression(fdr::detail::constructSimpleChannel())
    {}

    /// \brief Constructor.
    /// \param term A term
    channel::channel(atermpp::aterm_appl term)
      : definition_expression(term)
    {
      assert(fdr::detail::check_term_SimpleChannel(m_term));
    }

    /// \brief Constructor.
    channel::channel(const name_list& names)
      : definition_expression(fdr::detail::gsMakeSimpleChannel(names))
    {}

    name_list channel::names() const
    {
      return atermpp::list_arg1(*this);
    }

    /// \brief Default constructor.
    nametype::nametype()
      : definition_expression(fdr::detail::constructNameType())
    {}

    /// \brief Constructor.
    /// \param term A term
    nametype::nametype(atermpp::aterm_appl term)
      : definition_expression(term)
    {
      assert(fdr::detail::check_term_NameType(m_term));
    }

    /// \brief Constructor.
    nametype::nametype(const name& id, const type_expression& type_name)
      : definition_expression(fdr::detail::gsMakeNameType(id, type_name))
    {}

    name nametype::id() const
    {
      return atermpp::arg1(*this);
    }

    type_expression nametype::type_name() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    datatype::datatype()
      : definition_expression(fdr::detail::constructDataType())
    {}

    /// \brief Constructor.
    /// \param term A term
    datatype::datatype(atermpp::aterm_appl term)
      : definition_expression(term)
    {
      assert(fdr::detail::check_term_DataType(m_term));
    }

    /// \brief Constructor.
    datatype::datatype(const name& id, const vartype_expression_list& vartypes)
      : definition_expression(fdr::detail::gsMakeDataType(id, vartypes))
    {}

    name datatype::id() const
    {
      return atermpp::arg1(*this);
    }

    vartype_expression_list datatype::vartypes() const
    {
      return atermpp::list_arg2(*this);
    }

    /// \brief Default constructor.
    subtype::subtype()
      : definition_expression(fdr::detail::constructSubType())
    {}

    /// \brief Constructor.
    /// \param term A term
    subtype::subtype(atermpp::aterm_appl term)
      : definition_expression(term)
    {
      assert(fdr::detail::check_term_SubType(m_term));
    }

    /// \brief Constructor.
    subtype::subtype(const name& id, const vartype_expression_list& vartypes)
      : definition_expression(fdr::detail::gsMakeSubType(id, vartypes))
    {}

    name subtype::id() const
    {
      return atermpp::arg1(*this);
    }

    vartype_expression_list subtype::vartypes() const
    {
      return atermpp::list_arg2(*this);
    }

    /// \brief Default constructor.
    external::external()
      : definition_expression(fdr::detail::constructExternal())
    {}

    /// \brief Constructor.
    /// \param term A term
    external::external(atermpp::aterm_appl term)
      : definition_expression(term)
    {
      assert(fdr::detail::check_term_External(m_term));
    }

    /// \brief Constructor.
    external::external(const name_list& names)
      : definition_expression(fdr::detail::gsMakeExternal(names))
    {}

    name_list external::names() const
    {
      return atermpp::list_arg1(*this);
    }

    /// \brief Default constructor.
    transparent::transparent()
      : definition_expression(fdr::detail::constructTransparent())
    {}

    /// \brief Constructor.
    /// \param term A term
    transparent::transparent(atermpp::aterm_appl term)
      : definition_expression(term)
    {
      assert(fdr::detail::check_term_Transparent(m_term));
    }

    /// \brief Constructor.
    transparent::transparent(const trname_expression_list& trnames)
      : definition_expression(fdr::detail::gsMakeTransparent(trnames))
    {}

    trname_expression_list transparent::trnames() const
    {
      return atermpp::list_arg1(*this);
    }

    /// \brief Default constructor.
    assert_::assert_()
      : definition_expression(fdr::detail::constructAssert())
    {}

    /// \brief Constructor.
    /// \param term A term
    assert_::assert_(atermpp::aterm_appl term)
      : definition_expression(term)
    {
      assert(fdr::detail::check_term_Assert(m_term));
    }

    /// \brief Constructor.
    assert_::assert_(const check_expression& chk)
      : definition_expression(fdr::detail::gsMakeAssert(chk))
    {}

    check_expression assert_::chk() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    print::print()
      : definition_expression(fdr::detail::constructPrint())
    {}

    /// \brief Constructor.
    /// \param term A term
    print::print(atermpp::aterm_appl term)
      : definition_expression(term)
    {
      assert(fdr::detail::check_term_Print(m_term));
    }

    /// \brief Constructor.
    print::print(const expression& expr)
      : definition_expression(fdr::detail::gsMakePrint(expr))
    {}

    expression print::expr() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    include::include()
      : definition_expression(fdr::detail::constructInclude())
    {}

    /// \brief Constructor.
    /// \param term A term
    include::include(atermpp::aterm_appl term)
      : definition_expression(term)
    {
      assert(fdr::detail::check_term_Include(m_term));
    }

    /// \brief Constructor.
    include::include(const filename& file)
      : definition_expression(fdr::detail::gsMakeInclude(file))
    {}

    filename include::file() const
    {
      return atermpp::arg1(*this);
    }
//--- end generated definition expression class definitions ---//

//--- start generated failuremodel expression class definitions ---//
    /// \brief Default constructor.
    f::f()
      : failuremodel_expression(fdr::detail::constructF())
    {}

    /// \brief Constructor.
    /// \param term A term
    f::f(atermpp::aterm_appl term)
      : failuremodel_expression(term)
    {
      assert(fdr::detail::check_term_F(m_term));
    }

    /// \brief Default constructor.
    fd::fd()
      : failuremodel_expression(fdr::detail::constructFD())
    {}

    /// \brief Constructor.
    /// \param term A term
    fd::fd(atermpp::aterm_appl term)
      : failuremodel_expression(term)
    {
      assert(fdr::detail::check_term_FD(m_term));
    }
//--- end generated failuremodel expression class definitions ---//

//--- start generated targ expression class definitions ---//
    /// \brief Default constructor.
    nil::nil()
      : targ_expression(fdr::detail::constructNil())
    {}

    /// \brief Constructor.
    /// \param term A term
    nil::nil(atermpp::aterm_appl term)
      : targ_expression(term)
    {
      assert(fdr::detail::check_term_Nil(m_term));
    }

    /// \brief Default constructor.
    closedrange::closedrange()
      : targ_expression(fdr::detail::constructClosedRange())
    {}

    /// \brief Constructor.
    /// \param term A term
    closedrange::closedrange(atermpp::aterm_appl term)
      : targ_expression(term)
    {
      assert(fdr::detail::check_term_ClosedRange(m_term));
    }

    /// \brief Constructor.
    closedrange::closedrange(const numeric_expression& begin, const numeric_expression& end)
      : targ_expression(fdr::detail::gsMakeClosedRange(begin, end))
    {}

    numeric_expression closedrange::begin() const
    {
      return atermpp::arg1(*this);
    }

    numeric_expression closedrange::end() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    openrange::openrange()
      : targ_expression(fdr::detail::constructOpenRange())
    {}

    /// \brief Constructor.
    /// \param term A term
    openrange::openrange(atermpp::aterm_appl term)
      : targ_expression(term)
    {
      assert(fdr::detail::check_term_OpenRange(m_term));
    }

    /// \brief Constructor.
    openrange::openrange(const numeric_expression& begin)
      : targ_expression(fdr::detail::gsMakeOpenRange(begin))
    {}

    numeric_expression openrange::begin() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    compr::compr()
      : targ_expression(fdr::detail::constructCompr())
    {}

    /// \brief Constructor.
    /// \param term A term
    compr::compr(atermpp::aterm_appl term)
      : targ_expression(term)
    {
      assert(fdr::detail::check_term_Compr(m_term));
    }

    /// \brief Constructor.
    compr::compr(const expression& expr, const comprehension_expression_list& comprs)
      : targ_expression(fdr::detail::gsMakeCompr(expr, comprs))
    {}

    expression compr::expr() const
    {
      return atermpp::arg1(*this);
    }

    comprehension_expression_list compr::comprs() const
    {
      return atermpp::list_arg2(*this);
    }
//--- end generated targ expression class definitions ---//

//--- start generated field expression class definitions ---//
    /// \brief Default constructor.
    simpleinput::simpleinput()
      : field_expression(fdr::detail::constructSimpleInput())
    {}

    /// \brief Constructor.
    /// \param term A term
    simpleinput::simpleinput(atermpp::aterm_appl term)
      : field_expression(term)
    {
      assert(fdr::detail::check_term_SimpleInput(m_term));
    }

    /// \brief Constructor.
    simpleinput::simpleinput(const expression& expr)
      : field_expression(fdr::detail::gsMakeSimpleInput(expr))
    {}

    expression simpleinput::expr() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    input::input()
      : field_expression(fdr::detail::constructInput())
    {}

    /// \brief Constructor.
    /// \param term A term
    input::input(atermpp::aterm_appl term)
      : field_expression(term)
    {
      assert(fdr::detail::check_term_Input(m_term));
    }

    /// \brief Constructor.
    input::input(const expression& expr, const set_expression& restriction)
      : field_expression(fdr::detail::gsMakeInput(expr, restriction))
    {}

    expression input::expr() const
    {
      return atermpp::arg1(*this);
    }

    set_expression input::restriction() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    output::output()
      : field_expression(fdr::detail::constructOutput())
    {}

    /// \brief Constructor.
    /// \param term A term
    output::output(atermpp::aterm_appl term)
      : field_expression(term)
    {
      assert(fdr::detail::check_term_Output(m_term));
    }

    /// \brief Constructor.
    output::output(const expression& expr)
      : field_expression(fdr::detail::gsMakeOutput(expr))
    {}

    expression output::expr() const
    {
      return atermpp::arg1(*this);
    }
//--- end generated field expression class definitions ---//

//--- start generated testtype expression class definitions ---//
    /// \brief Default constructor.
    deterministic::deterministic()
      : testtype_expression(fdr::detail::constructdeterministic())
    {}

    /// \brief Constructor.
    /// \param term A term
    deterministic::deterministic(atermpp::aterm_appl term)
      : testtype_expression(term)
    {
      assert(fdr::detail::check_term_deterministic(m_term));
    }

    /// \brief Default constructor.
    deadlock_free::deadlock_free()
      : testtype_expression(fdr::detail::constructdeadlock_free())
    {}

    /// \brief Constructor.
    /// \param term A term
    deadlock_free::deadlock_free(atermpp::aterm_appl term)
      : testtype_expression(term)
    {
      assert(fdr::detail::check_term_deadlock_free(m_term));
    }

    /// \brief Default constructor.
    livelock_free::livelock_free()
      : testtype_expression(fdr::detail::constructlivelock_free())
    {}

    /// \brief Constructor.
    /// \param term A term
    livelock_free::livelock_free(atermpp::aterm_appl term)
      : testtype_expression(term)
    {
      assert(fdr::detail::check_term_livelock_free(m_term));
    }
//--- end generated testtype expression class definitions ---//

//--- start generated vartype expression class definitions ---//
    /// \brief Default constructor.
    simplebranch::simplebranch()
      : vartype_expression(fdr::detail::constructSimpleBranch())
    {}

    /// \brief Constructor.
    /// \param term A term
    simplebranch::simplebranch(atermpp::aterm_appl term)
      : vartype_expression(term)
    {
      assert(fdr::detail::check_term_SimpleBranch(m_term));
    }

    /// \brief Constructor.
    simplebranch::simplebranch(const name& id)
      : vartype_expression(fdr::detail::gsMakeSimpleBranch(id))
    {}

    name simplebranch::id() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    branch::branch()
      : vartype_expression(fdr::detail::constructBranch())
    {}

    /// \brief Constructor.
    /// \param term A term
    branch::branch(atermpp::aterm_appl term)
      : vartype_expression(term)
    {
      assert(fdr::detail::check_term_Branch(m_term));
    }

    /// \brief Constructor.
    branch::branch(const name& id, const type_expression& type_name)
      : vartype_expression(fdr::detail::gsMakeBranch(id, type_name))
    {}

    name branch::id() const
    {
      return atermpp::arg1(*this);
    }

    type_expression branch::type_name() const
    {
      return atermpp::arg2(*this);
    }
//--- end generated vartype expression class definitions ---//

//--- start generated type expression class definitions ---//
    /// \brief Default constructor.
    typeproduct::typeproduct()
      : type_expression(fdr::detail::constructTypeProduct())
    {}

    /// \brief Constructor.
    /// \param term A term
    typeproduct::typeproduct(atermpp::aterm_appl term)
      : type_expression(term)
    {
      assert(fdr::detail::check_term_TypeProduct(m_term));
    }

    /// \brief Constructor.
    typeproduct::typeproduct(const type_expression& left, const type_expression& right)
      : type_expression(fdr::detail::gsMakeTypeProduct(left, right))
    {}

    type_expression typeproduct::left() const
    {
      return atermpp::arg1(*this);
    }

    type_expression typeproduct::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    typetuple::typetuple()
      : type_expression(fdr::detail::constructTypeTuple())
    {}

    /// \brief Constructor.
    /// \param term A term
    typetuple::typetuple(atermpp::aterm_appl term)
      : type_expression(term)
    {
      assert(fdr::detail::check_term_TypeTuple(m_term));
    }

    /// \brief Constructor.
    typetuple::typetuple(const type_expression_list& types)
      : type_expression(fdr::detail::gsMakeTypeTuple(types))
    {}

    type_expression_list typetuple::types() const
    {
      return atermpp::list_arg1(*this);
    }

    /// \brief Default constructor.
    typeset::typeset()
      : type_expression(fdr::detail::constructTypeSet())
    {}

    /// \brief Constructor.
    /// \param term A term
    typeset::typeset(atermpp::aterm_appl term)
      : type_expression(term)
    {
      assert(fdr::detail::check_term_TypeSet(m_term));
    }

    /// \brief Constructor.
    typeset::typeset(const set_expression& set)
      : type_expression(fdr::detail::gsMakeTypeSet(set))
    {}

    set_expression typeset::set() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    simpletypename::simpletypename()
      : type_expression(fdr::detail::constructSympleTypeName())
    {}

    /// \brief Constructor.
    /// \param term A term
    simpletypename::simpletypename(atermpp::aterm_appl term)
      : type_expression(term)
    {
      assert(fdr::detail::check_term_SympleTypeName(m_term));
    }

    /// \brief Constructor.
    simpletypename::simpletypename(const name& id)
      : type_expression(fdr::detail::gsMakeSympleTypeName(id))
    {}

    name simpletypename::id() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    typename::typename()
      : type_expression(fdr::detail::constructTypeName())
    {}

    /// \brief Constructor.
    /// \param term A term
    typename::typename(atermpp::aterm_appl term)
      : type_expression(term)
    {
      assert(fdr::detail::check_term_TypeName(m_term));
    }

    /// \brief Constructor.
    typename::typename(const name& id, const type_expression& type_name)
      : type_expression(fdr::detail::gsMakeTypeName(id, type_name))
    {}

    name typename::id() const
    {
      return atermpp::arg1(*this);
    }

    type_expression typename::type_name() const
    {
      return atermpp::arg2(*this);
    }
//--- end generated type expression class definitions ---//

//--- start generated check expression class definitions ---//
    /// \brief Default constructor.
    bcheck::bcheck()
      : check_expression(fdr::detail::constructBCheck())
    {}

    /// \brief Constructor.
    /// \param term A term
    bcheck::bcheck(atermpp::aterm_appl term)
      : check_expression(term)
    {
      assert(fdr::detail::check_term_BCheck(m_term));
    }

    /// \brief Constructor.
    bcheck::bcheck(const boolean_expression& expr)
      : check_expression(fdr::detail::gsMakeBCheck(expr))
    {}

    boolean_expression bcheck::expr() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    rcheck::rcheck()
      : check_expression(fdr::detail::constructRCheck())
    {}

    /// \brief Constructor.
    /// \param term A term
    rcheck::rcheck(atermpp::aterm_appl term)
      : check_expression(term)
    {
      assert(fdr::detail::check_term_RCheck(m_term));
    }

    /// \brief Constructor.
    rcheck::rcheck(const process_expression& left, const process_expression& right, const refined& refinement)
      : check_expression(fdr::detail::gsMakeRCheck(left, right, refinement))
    {}

    process_expression rcheck::left() const
    {
      return atermpp::arg1(*this);
    }

    process_expression rcheck::right() const
    {
      return atermpp::arg2(*this);
    }

    refined rcheck::refinement() const
    {
      return atermpp::arg3(*this);
    }

    /// \brief Default constructor.
    tcheck::tcheck()
      : check_expression(fdr::detail::constructTCheck())
    {}

    /// \brief Constructor.
    /// \param term A term
    tcheck::tcheck(atermpp::aterm_appl term)
      : check_expression(term)
    {
      assert(fdr::detail::check_term_TCheck(m_term));
    }

    /// \brief Constructor.
    tcheck::tcheck(const process_expression& proc, const test_expression& operand)
      : check_expression(fdr::detail::gsMakeTCheck(proc, operand))
    {}

    process_expression tcheck::proc() const
    {
      return atermpp::arg1(*this);
    }

    test_expression tcheck::operand() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    notcheck::notcheck()
      : check_expression(fdr::detail::constructNotCheck())
    {}

    /// \brief Constructor.
    /// \param term A term
    notcheck::notcheck(atermpp::aterm_appl term)
      : check_expression(term)
    {
      assert(fdr::detail::check_term_NotCheck(m_term));
    }

    /// \brief Constructor.
    notcheck::notcheck(const check_expression& chk)
      : check_expression(fdr::detail::gsMakeNotCheck(chk))
    {}

    check_expression notcheck::chk() const
    {
      return atermpp::arg1(*this);
    }
//--- end generated check expression class definitions ---//

//--- start generated model expression class definitions ---//
    /// \brief Default constructor.
    nil::nil()
      : model_expression(fdr::detail::constructNil())
    {}

    /// \brief Constructor.
    /// \param term A term
    nil::nil(atermpp::aterm_appl term)
      : model_expression(term)
    {
      assert(fdr::detail::check_term_Nil(m_term));
    }

    /// \brief Default constructor.
    t::t()
      : model_expression(fdr::detail::constructT())
    {}

    /// \brief Constructor.
    /// \param term A term
    t::t(atermpp::aterm_appl term)
      : model_expression(term)
    {
      assert(fdr::detail::check_term_T(m_term));
    }
//--- end generated model expression class definitions ---//

//--- start generated test expression class definitions ---//
    /// \brief Default constructor.
    divergence_free::divergence_free()
      : test_expression(fdr::detail::constructdivergence_free())
    {}

    /// \brief Constructor.
    /// \param term A term
    divergence_free::divergence_free(atermpp::aterm_appl term)
      : test_expression(term)
    {
      assert(fdr::detail::check_term_divergence_free(m_term));
    }

    /// \brief Default constructor.
    test::test()
      : test_expression(fdr::detail::constructTest())
    {}

    /// \brief Constructor.
    /// \param term A term
    test::test(atermpp::aterm_appl term)
      : test_expression(term)
    {
      assert(fdr::detail::check_term_Test(m_term));
    }

    /// \brief Constructor.
    test::test(const testtype_expression& tt, const failuremodel_expression& fm)
      : test_expression(fdr::detail::gsMakeTest(tt, fm))
    {}

    testtype_expression test::tt() const
    {
      return atermpp::arg1(*this);
    }

    failuremodel_expression test::fm() const
    {
      return atermpp::arg2(*this);
    }
//--- end generated test expression class definitions ---//

//--- start generated trname expression class definitions ---//
    /// \brief Default constructor.
    normal::normal()
      : trname_expression(fdr::detail::constructnormal())
    {}

    /// \brief Constructor.
    /// \param term A term
    normal::normal(atermpp::aterm_appl term)
      : trname_expression(term)
    {
      assert(fdr::detail::check_term_normal(m_term));
    }

    /// \brief Default constructor.
    normalise::normalise()
      : trname_expression(fdr::detail::constructnormalise())
    {}

    /// \brief Constructor.
    /// \param term A term
    normalise::normalise(atermpp::aterm_appl term)
      : trname_expression(term)
    {
      assert(fdr::detail::check_term_normalise(m_term));
    }

    /// \brief Default constructor.
    normalize::normalize()
      : trname_expression(fdr::detail::constructnormalize())
    {}

    /// \brief Constructor.
    /// \param term A term
    normalize::normalize(atermpp::aterm_appl term)
      : trname_expression(term)
    {
      assert(fdr::detail::check_term_normalize(m_term));
    }

    /// \brief Default constructor.
    sbsim::sbsim()
      : trname_expression(fdr::detail::constructsbsim())
    {}

    /// \brief Constructor.
    /// \param term A term
    sbsim::sbsim(atermpp::aterm_appl term)
      : trname_expression(term)
    {
      assert(fdr::detail::check_term_sbsim(m_term));
    }

    /// \brief Default constructor.
    tau_loop_factor::tau_loop_factor()
      : trname_expression(fdr::detail::constructtau_loop_factor())
    {}

    /// \brief Constructor.
    /// \param term A term
    tau_loop_factor::tau_loop_factor(atermpp::aterm_appl term)
      : trname_expression(term)
    {
      assert(fdr::detail::check_term_tau_loop_factor(m_term));
    }

    /// \brief Default constructor.
    diamond::diamond()
      : trname_expression(fdr::detail::constructdiamond())
    {}

    /// \brief Constructor.
    /// \param term A term
    diamond::diamond(atermpp::aterm_appl term)
      : trname_expression(term)
    {
      assert(fdr::detail::check_term_diamond(m_term));
    }

    /// \brief Default constructor.
    model_compress::model_compress()
      : trname_expression(fdr::detail::constructmodel_compress())
    {}

    /// \brief Constructor.
    /// \param term A term
    model_compress::model_compress(atermpp::aterm_appl term)
      : trname_expression(term)
    {
      assert(fdr::detail::check_term_model_compress(m_term));
    }

    /// \brief Default constructor.
    explicate::explicate()
      : trname_expression(fdr::detail::constructexplicate())
    {}

    /// \brief Constructor.
    /// \param term A term
    explicate::explicate(atermpp::aterm_appl term)
      : trname_expression(term)
    {
      assert(fdr::detail::check_term_explicate(m_term));
    }
//--- end generated trname expression class definitions ---//

//--- start generated any expression class definitions ---//
    /// \brief Default constructor.
    expr::expr()
      : any_expression(fdr::detail::constructExpr())
    {}

    /// \brief Constructor.
    /// \param term A term
    expr::expr(atermpp::aterm_appl term)
      : any_expression(term)
    {
      assert(fdr::detail::check_term_Expr(m_term));
    }

    /// \brief Constructor.
    expr::expr(const expression& operand)
      : any_expression(fdr::detail::gsMakeExpr(operand))
    {}

    expression expr::operand() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    proc::proc()
      : any_expression(fdr::detail::constructProc())
    {}

    /// \brief Constructor.
    /// \param term A term
    proc::proc(atermpp::aterm_appl term)
      : any_expression(term)
    {
      assert(fdr::detail::check_term_Proc(m_term));
    }

    /// \brief Constructor.
    proc::proc(const process_expression& operand)
      : any_expression(fdr::detail::gsMakeProc(operand))
    {}

    process_expression proc::operand() const
    {
      return atermpp::arg1(*this);
    }
//--- end generated any expression class definitions ---//

} // namespace fdr

} // namespace mcrl2
