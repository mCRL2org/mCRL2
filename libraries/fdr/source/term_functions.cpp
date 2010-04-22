#include "mcrl2/fdr/boolean_expression.h"
#include "mcrl2/fdr/common_expression.h"
#include "mcrl2/fdr/dotted_expression.h"
#include "mcrl2/fdr/expression.h"
#include "mcrl2/fdr/lambda_expression.h"
#include "mcrl2/fdr/numeric_expression.h"
#include "mcrl2/fdr/seq_expression.h"
#include "mcrl2/fdr/set_expression.h"
#include "mcrl2/fdr/tuple_expression.h"

namespace mcrl2 {

namespace fdr {

//--- start generated boolean expression class definitions ---//
    /// \brief Default constructor.
    inline
    true_::true_()
      : boolean_expression(fdr::detail::constructtrue())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    true_::true_(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_true(m_term));
    }

    /// \brief Default constructor.
    inline
    false_::false_()
      : boolean_expression(fdr::detail::constructfalse())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    false_::false_(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_false(m_term));
    }

    /// \brief Default constructor.
    inline
    and_::and_()
      : boolean_expression(fdr::detail::constructAnd())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    and_::and_(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_And(m_term));
    }

    /// \brief Constructor.
    inline
    and_::and_(const boolean_expression& left, const boolean_expression& right)
      : boolean_expression(fdr::detail::gsMakeAnd(left, right))
    {}

    inline
    boolean_expression and_::left() const
    {
      return atermpp::arg1(*this);
    }

    inline
    boolean_expression and_::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    inline
    or_::or_()
      : boolean_expression(fdr::detail::constructOr())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    or_::or_(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_Or(m_term));
    }

    /// \brief Constructor.
    inline
    or_::or_(const boolean_expression& left, const boolean_expression& right)
      : boolean_expression(fdr::detail::gsMakeOr(left, right))
    {}

    inline
    boolean_expression or_::left() const
    {
      return atermpp::arg1(*this);
    }

    inline
    boolean_expression or_::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    inline
    not_::not_()
      : boolean_expression(fdr::detail::constructNot())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    not_::not_(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_Not(m_term));
    }

    /// \brief Constructor.
    inline
    not_::not_(const boolean_expression& operand)
      : boolean_expression(fdr::detail::gsMakeNot(operand))
    {}

    inline
    boolean_expression not_::operand() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    inline
    null::null()
      : boolean_expression(fdr::detail::constructNull())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    null::null(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_Null(m_term));
    }

    /// \brief Constructor.
    inline
    null::null(const seq_expression& seq)
      : boolean_expression(fdr::detail::gsMakeNull(seq))
    {}

    inline
    seq_expression null::seq() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    inline
    elem::elem()
      : boolean_expression(fdr::detail::constructElem())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    elem::elem(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_Elem(m_term));
    }

    /// \brief Constructor.
    inline
    elem::elem(const expression& expr, const seq_expression& seq)
      : boolean_expression(fdr::detail::gsMakeElem(expr, seq))
    {}

    inline
    expression elem::expr() const
    {
      return atermpp::arg1(*this);
    }

    inline
    seq_expression elem::seq() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    inline
    member::member()
      : boolean_expression(fdr::detail::constructMember())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    member::member(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_Member(m_term));
    }

    /// \brief Constructor.
    inline
    member::member(const expression& expr, set_expression& set)
      : boolean_expression(fdr::detail::gsMakeMember(expr, set))
    {}

    inline
    expression member::expr() const
    {
      return atermpp::arg1(*this);
    }

    inline
    set_expression member::set() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    inline
    empty::empty()
      : boolean_expression(fdr::detail::constructEmpty())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    empty::empty(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_Empty(m_term));
    }

    /// \brief Constructor.
    inline
    empty::empty(const set_expression& set)
      : boolean_expression(fdr::detail::gsMakeEmpty(set))
    {}

    inline
    set_expression empty::set() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    inline
    equal::equal()
      : boolean_expression(fdr::detail::constructEqual())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    equal::equal(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_Equal(m_term));
    }

    /// \brief Constructor.
    inline
    equal::equal(const expression& left, const expression& right)
      : boolean_expression(fdr::detail::gsMakeEqual(left, right))
    {}

    inline
    expression equal::left() const
    {
      return atermpp::arg1(*this);
    }

    inline
    expression equal::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    inline
    notequal::notequal()
      : boolean_expression(fdr::detail::constructNotEqual())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    notequal::notequal(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_NotEqual(m_term));
    }

    /// \brief Constructor.
    inline
    notequal::notequal(const expression& left, const expression& right)
      : boolean_expression(fdr::detail::gsMakeNotEqual(left, right))
    {}

    inline
    expression notequal::left() const
    {
      return atermpp::arg1(*this);
    }

    inline
    expression notequal::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    inline
    less::less()
      : boolean_expression(fdr::detail::constructLess())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    less::less(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_Less(m_term));
    }

    /// \brief Constructor.
    inline
    less::less(const expression& left, const expression& right)
      : boolean_expression(fdr::detail::gsMakeLess(left, right))
    {}

    inline
    expression less::left() const
    {
      return atermpp::arg1(*this);
    }

    inline
    expression less::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    inline
    lessorequal::lessorequal()
      : boolean_expression(fdr::detail::constructLessOrEqual())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    lessorequal::lessorequal(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_LessOrEqual(m_term));
    }

    /// \brief Constructor.
    inline
    lessorequal::lessorequal(const expression& left, const expression& right)
      : boolean_expression(fdr::detail::gsMakeLessOrEqual(left, right))
    {}

    inline
    expression lessorequal::left() const
    {
      return atermpp::arg1(*this);
    }

    inline
    expression lessorequal::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    inline
    greater::greater()
      : boolean_expression(fdr::detail::constructGreater())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    greater::greater(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_Greater(m_term));
    }

    /// \brief Constructor.
    inline
    greater::greater(const expression& left, const expression& right)
      : boolean_expression(fdr::detail::gsMakeGreater(left, right))
    {}

    inline
    expression greater::left() const
    {
      return atermpp::arg1(*this);
    }

    inline
    expression greater::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    inline
    greaterorequal::greaterorequal()
      : boolean_expression(fdr::detail::constructGreaterOrEqual())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    greaterorequal::greaterorequal(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_GreaterOrEqual(m_term));
    }

    /// \brief Constructor.
    inline
    greaterorequal::greaterorequal(const expression& left, const expression& right)
      : boolean_expression(fdr::detail::gsMakeGreaterOrEqual(left, right))
    {}

    inline
    expression greaterorequal::left() const
    {
      return atermpp::arg1(*this);
    }

    inline
    expression greaterorequal::right() const
    {
      return atermpp::arg2(*this);
    }
//--- end generated boolean expression class definitions ---//

//--- start generated common expression class definitions ---//
    /// \brief Default constructor.
    inline
    conditional::conditional()
      : common_expression(fdr::detail::constructConditional())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    conditional::conditional(atermpp::aterm_appl term)
      : common_expression(term)
    {
      assert(fdr::detail::check_term_Conditional(m_term));
    }

    /// \brief Constructor.
    inline
    conditional::conditional(const boolean_expression& guard, const any& thenpart, const any& elsepart)
      : common_expression(fdr::detail::gsMakeConditional(guard, thenpart, elsepart))
    {}

    inline
    boolean_expression conditional::guard() const
    {
      return atermpp::arg1(*this);
    }

    inline
    any conditional::thenpart() const
    {
      return atermpp::arg2(*this);
    }

    inline
    any conditional::elsepart() const
    {
      return atermpp::arg3(*this);
    }

    /// \brief Default constructor.
    inline
    identifier::identifier()
      : common_expression(fdr::detail::constructName())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    identifier::identifier(atermpp::aterm_appl term)
      : common_expression(term)
    {
      assert(fdr::detail::check_term_Name(m_term));
    }

    /// \brief Constructor.
    inline
    identifier::identifier(const name& id)
      : common_expression(fdr::detail::gsMakeName(id))
    {}

    inline
    name identifier::id() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    inline
    lambdaappl::lambdaappl()
      : common_expression(fdr::detail::constructLambdaAppl())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    lambdaappl::lambdaappl(atermpp::aterm_appl term)
      : common_expression(term)
    {
      assert(fdr::detail::check_term_LambdaAppl(m_term));
    }

    /// \brief Constructor.
    inline
    lambdaappl::lambdaappl(const lambda_expression& lambda, const expression_list& exprs)
      : common_expression(fdr::detail::gsMakeLambdaAppl(lambda, exprs))
    {}

    inline
    lambda_expression lambdaappl::lambda() const
    {
      return atermpp::arg1(*this);
    }

    inline
    expression_list lambdaappl::exprs() const
    {
      return atermpp::list_arg2(*this);
    }

    /// \brief Default constructor.
    inline
    localdef::localdef()
      : common_expression(fdr::detail::constructLocalDef())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    localdef::localdef(atermpp::aterm_appl term)
      : common_expression(term)
    {
      assert(fdr::detail::check_term_LocalDef(m_term));
    }

    /// \brief Constructor.
    inline
    localdef::localdef(const definition_list& defs, const any& within)
      : common_expression(fdr::detail::gsMakeLocalDef(defs, within))
    {}

    inline
    definition_list localdef::defs() const
    {
      return atermpp::list_arg1(*this);
    }

    inline
    any localdef::within() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    inline
    bracketed::bracketed()
      : common_expression(fdr::detail::constructBracketed())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    bracketed::bracketed(atermpp::aterm_appl term)
      : common_expression(term)
    {
      assert(fdr::detail::check_term_Bracketed(m_term));
    }

    /// \brief Constructor.
    inline
    bracketed::bracketed(const any& operand)
      : common_expression(fdr::detail::gsMakeBracketed(operand))
    {}

    inline
    any bracketed::operand() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    inline
    pattern::pattern()
      : common_expression(fdr::detail::constructPattern())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    pattern::pattern(atermpp::aterm_appl term)
      : common_expression(term)
    {
      assert(fdr::detail::check_term_Pattern(m_term));
    }

    /// \brief Constructor.
    inline
    pattern::pattern(const any& left, const any& right)
      : common_expression(fdr::detail::gsMakePattern(left, right))
    {}

    inline
    any pattern::left() const
    {
      return atermpp::arg1(*this);
    }

    inline
    any pattern::right() const
    {
      return atermpp::arg2(*this);
    }
//--- end generated common expression class definitions ---//

//--- start generated dotted expression class definitions ---//
    /// \brief Default constructor.
    inline
    dot::dot()
      : dotted_expression(fdr::detail::constructDot())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    dot::dot(atermpp::aterm_appl term)
      : dotted_expression(term)
    {
      assert(fdr::detail::check_term_Dot(m_term));
    }

    /// \brief Constructor.
    inline
    dot::dot(const expression& left, expression& right)
      : dotted_expression(fdr::detail::gsMakeDot(left, right))
    {}

    inline
    expression dot::left() const
    {
      return atermpp::arg1(*this);
    }

    inline
    expression dot::right() const
    {
      return atermpp::arg2(*this);
    }
//--- end generated dotted expression class definitions ---//

//--- start generated expression class definitions ---//
    /// \brief Default constructor.
    inline
    numb::numb()
      : expression(fdr::detail::constructNumb())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    numb::numb(atermpp::aterm_appl term)
      : expression(term)
    {
      assert(fdr::detail::check_term_Numb(m_term));
    }

    /// \brief Constructor.
    inline
    numb::numb(const numeric_expression& operand)
      : expression(fdr::detail::gsMakeNumb(operand))
    {}

    inline
    numeric_expression numb::operand() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    inline
    bool_::bool_()
      : expression(fdr::detail::constructBool())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    bool_::bool_(atermpp::aterm_appl term)
      : expression(term)
    {
      assert(fdr::detail::check_term_Bool(m_term));
    }

    /// \brief Constructor.
    inline
    bool_::bool_(const boolean_expression& operand)
      : expression(fdr::detail::gsMakeBool(operand))
    {}

    inline
    boolean_expression bool_::operand() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    inline
    set::set()
      : expression(fdr::detail::constructSet())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    set::set(atermpp::aterm_appl term)
      : expression(term)
    {
      assert(fdr::detail::check_term_Set(m_term));
    }

    /// \brief Constructor.
    inline
    set::set(const set_expression& operand)
      : expression(fdr::detail::gsMakeSet(operand))
    {}

    inline
    set_expression set::operand() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    inline
    seq::seq()
      : expression(fdr::detail::constructSeq())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    seq::seq(atermpp::aterm_appl term)
      : expression(term)
    {
      assert(fdr::detail::check_term_Seq(m_term));
    }

    /// \brief Constructor.
    inline
    seq::seq(const seq_expression& operand)
      : expression(fdr::detail::gsMakeSeq(operand))
    {}

    inline
    seq_expression seq::operand() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    inline
    tuple::tuple()
      : expression(fdr::detail::constructTuple())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    tuple::tuple(atermpp::aterm_appl term)
      : expression(term)
    {
      assert(fdr::detail::check_term_Tuple(m_term));
    }

    /// \brief Constructor.
    inline
    tuple::tuple(const tuple_expression& operand)
      : expression(fdr::detail::gsMakeTuple(operand))
    {}

    inline
    tuple_expression tuple::operand() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    inline
    dotted::dotted()
      : expression(fdr::detail::constructDotted())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    dotted::dotted(atermpp::aterm_appl term)
      : expression(term)
    {
      assert(fdr::detail::check_term_Dotted(m_term));
    }

    /// \brief Constructor.
    inline
    dotted::dotted(const dotted_expression& operand)
      : expression(fdr::detail::gsMakeDotted(operand))
    {}

    inline
    dotted_expression dotted::operand() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    inline
    lambda::lambda()
      : expression(fdr::detail::constructLambda())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    lambda::lambda(atermpp::aterm_appl term)
      : expression(term)
    {
      assert(fdr::detail::check_term_Lambda(m_term));
    }

    /// \brief Constructor.
    inline
    lambda::lambda(const lambda_expression& operand)
      : expression(fdr::detail::gsMakeLambda(operand))
    {}

    inline
    lambda_expression lambda::operand() const
    {
      return atermpp::arg1(*this);
    }
//--- end generated expression class definitions ---//

//--- start generated lambda expression class definitions ---//
    /// \brief Default constructor.
    inline
    lambdaexpr::lambdaexpr()
      : lambda_expression(fdr::detail::constructLambdaExpr())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    lambdaexpr::lambdaexpr(atermpp::aterm_appl term)
      : lambda_expression(term)
    {
      assert(fdr::detail::check_term_LambdaExpr(m_term));
    }

    /// \brief Constructor.
    inline
    lambdaexpr::lambdaexpr(const expression_list& exprs, const any& function)
      : lambda_expression(fdr::detail::gsMakeLambdaExpr(exprs, function))
    {}

    inline
    expression_list lambdaexpr::exprs() const
    {
      return atermpp::list_arg1(*this);
    }

    inline
    any lambdaexpr::function() const
    {
      return atermpp::arg2(*this);
    }
//--- end generated lambda expression class definitions ---//

//--- start generated numeric expression class definitions ---//
    /// \brief Default constructor.
    inline
    number::number()
      : numeric_expression(fdr::detail::constructNumber())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    number::number(atermpp::aterm_appl term)
      : numeric_expression(term)
    {
      assert(fdr::detail::check_term_Number(m_term));
    }

    /// \brief Constructor.
    inline
    number::number(const number& operand)
      : numeric_expression(fdr::detail::gsMakeNumber(operand))
    {}

    inline
    number number::operand() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    inline
    card::card()
      : numeric_expression(fdr::detail::constructCard())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    card::card(atermpp::aterm_appl term)
      : numeric_expression(term)
    {
      assert(fdr::detail::check_term_Card(m_term));
    }

    /// \brief Constructor.
    inline
    card::card(const set_expression& set)
      : numeric_expression(fdr::detail::gsMakeCard(set))
    {}

    inline
    set_expression card::set() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    inline
    length::length()
      : numeric_expression(fdr::detail::constructLength())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    length::length(atermpp::aterm_appl term)
      : numeric_expression(term)
    {
      assert(fdr::detail::check_term_Length(m_term));
    }

    /// \brief Constructor.
    inline
    length::length(const seq_expression& seq)
      : numeric_expression(fdr::detail::gsMakeLength(seq))
    {}

    inline
    seq_expression length::seq() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    inline
    plus::plus()
      : numeric_expression(fdr::detail::constructPlus())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    plus::plus(atermpp::aterm_appl term)
      : numeric_expression(term)
    {
      assert(fdr::detail::check_term_Plus(m_term));
    }

    /// \brief Constructor.
    inline
    plus::plus(const numeric_expression& left, const numeric_expression& right)
      : numeric_expression(fdr::detail::gsMakePlus(left, right))
    {}

    inline
    numeric_expression plus::left() const
    {
      return atermpp::arg1(*this);
    }

    inline
    numeric_expression plus::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    inline
    minus::minus()
      : numeric_expression(fdr::detail::constructMinus())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    minus::minus(atermpp::aterm_appl term)
      : numeric_expression(term)
    {
      assert(fdr::detail::check_term_Minus(m_term));
    }

    /// \brief Constructor.
    inline
    minus::minus(const numeric_expression& left, const numeric_expression& right)
      : numeric_expression(fdr::detail::gsMakeMinus(left, right))
    {}

    inline
    numeric_expression minus::left() const
    {
      return atermpp::arg1(*this);
    }

    inline
    numeric_expression minus::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    inline
    times::times()
      : numeric_expression(fdr::detail::constructTimes())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    times::times(atermpp::aterm_appl term)
      : numeric_expression(term)
    {
      assert(fdr::detail::check_term_Times(m_term));
    }

    /// \brief Constructor.
    inline
    times::times(const numeric_expression& left, const numeric_expression& right)
      : numeric_expression(fdr::detail::gsMakeTimes(left, right))
    {}

    inline
    numeric_expression times::left() const
    {
      return atermpp::arg1(*this);
    }

    inline
    numeric_expression times::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    inline
    div::div()
      : numeric_expression(fdr::detail::constructDiv())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    div::div(atermpp::aterm_appl term)
      : numeric_expression(term)
    {
      assert(fdr::detail::check_term_Div(m_term));
    }

    /// \brief Constructor.
    inline
    div::div(const numeric_expression& left, const numeric_expression& right)
      : numeric_expression(fdr::detail::gsMakeDiv(left, right))
    {}

    inline
    numeric_expression div::left() const
    {
      return atermpp::arg1(*this);
    }

    inline
    numeric_expression div::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    inline
    mod::mod()
      : numeric_expression(fdr::detail::constructMod())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    mod::mod(atermpp::aterm_appl term)
      : numeric_expression(term)
    {
      assert(fdr::detail::check_term_Mod(m_term));
    }

    /// \brief Constructor.
    inline
    mod::mod(const numeric_expression& left, const numeric_expression& right)
      : numeric_expression(fdr::detail::gsMakeMod(left, right))
    {}

    inline
    numeric_expression mod::left() const
    {
      return atermpp::arg1(*this);
    }

    inline
    numeric_expression mod::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    inline
    min::min()
      : numeric_expression(fdr::detail::constructMin())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    min::min(atermpp::aterm_appl term)
      : numeric_expression(term)
    {
      assert(fdr::detail::check_term_Min(m_term));
    }

    /// \brief Constructor.
    inline
    min::min(const numeric_expression& operand)
      : numeric_expression(fdr::detail::gsMakeMin(operand))
    {}

    inline
    numeric_expression min::operand() const
    {
      return atermpp::arg1(*this);
    }
//--- end generated numeric expression class definitions ---//

//--- start generated seq expression class definitions ---//
    /// \brief Default constructor.
    inline
    cat::cat()
      : seq_expression(fdr::detail::constructCat())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    cat::cat(atermpp::aterm_appl term)
      : seq_expression(term)
    {
      assert(fdr::detail::check_term_Cat(m_term));
    }

    /// \brief Constructor.
    inline
    cat::cat(const seq_expression& left, const seq_expression& right)
      : seq_expression(fdr::detail::gsMakeCat(left, right))
    {}

    inline
    seq_expression cat::left() const
    {
      return atermpp::arg1(*this);
    }

    inline
    seq_expression cat::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    inline
    concat::concat()
      : seq_expression(fdr::detail::constructConcat())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    concat::concat(atermpp::aterm_appl term)
      : seq_expression(term)
    {
      assert(fdr::detail::check_term_Concat(m_term));
    }

    /// \brief Constructor.
    inline
    concat::concat(const seq_expression& seq)
      : seq_expression(fdr::detail::gsMakeConcat(seq))
    {}

    inline
    seq_expression concat::seq() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    inline
    head::head()
      : seq_expression(fdr::detail::constructHead())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    head::head(atermpp::aterm_appl term)
      : seq_expression(term)
    {
      assert(fdr::detail::check_term_Head(m_term));
    }

    /// \brief Constructor.
    inline
    head::head(const seq_expression& seq)
      : seq_expression(fdr::detail::gsMakeHead(seq))
    {}

    inline
    seq_expression head::seq() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    inline
    tail::tail()
      : seq_expression(fdr::detail::constructTail())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    tail::tail(atermpp::aterm_appl term)
      : seq_expression(term)
    {
      assert(fdr::detail::check_term_Tail(m_term));
    }

    /// \brief Constructor.
    inline
    tail::tail(const seq_expression& seq)
      : seq_expression(fdr::detail::gsMakeTail(seq))
    {}

    inline
    seq_expression tail::seq() const
    {
      return atermpp::arg1(*this);
    }
//--- end generated seq expression class definitions ---//

//--- start generated set expression class definitions ---//
    /// \brief Default constructor.
    inline
    chanset::chanset()
      : set_expression(fdr::detail::constructChanSet())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    chanset::chanset(atermpp::aterm_appl term)
      : set_expression(term)
    {
      assert(fdr::detail::check_term_ChanSet(m_term));
    }

    /// \brief Constructor.
    inline
    chanset::chanset(const targ& argument)
      : set_expression(fdr::detail::gsMakeChanSet(argument))
    {}

    inline
    targ chanset::argument() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    inline
    union::union()
      : set_expression(fdr::detail::constructunion())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    union::union(atermpp::aterm_appl term)
      : set_expression(term)
    {
      assert(fdr::detail::check_term_union(m_term));
    }

    /// \brief Constructor.
    inline
    union::union(const set_expression& left, const set_expression& right)
      : set_expression(fdr::detail::gsMakeunion(left, right))
    {}

    inline
    set_expression union::left() const
    {
      return atermpp::arg1(*this);
    }

    inline
    set_expression union::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    inline
    inter::inter()
      : set_expression(fdr::detail::constructinter())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    inter::inter(atermpp::aterm_appl term)
      : set_expression(term)
    {
      assert(fdr::detail::check_term_inter(m_term));
    }

    /// \brief Constructor.
    inline
    inter::inter(const set_expression& left, const set_expression& right)
      : set_expression(fdr::detail::gsMakeinter(left, right))
    {}

    inline
    set_expression inter::left() const
    {
      return atermpp::arg1(*this);
    }

    inline
    set_expression inter::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    inline
    diff::diff()
      : set_expression(fdr::detail::constructdiff())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    diff::diff(atermpp::aterm_appl term)
      : set_expression(term)
    {
      assert(fdr::detail::check_term_diff(m_term));
    }

    /// \brief Constructor.
    inline
    diff::diff(const set_expression& left, const set_expression& right)
      : set_expression(fdr::detail::gsMakediff(left, right))
    {}

    inline
    set_expression diff::left() const
    {
      return atermpp::arg1(*this);
    }

    inline
    set_expression diff::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    inline
    union::union()
      : set_expression(fdr::detail::constructUnion())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    union::union(atermpp::aterm_appl term)
      : set_expression(term)
    {
      assert(fdr::detail::check_term_Union(m_term));
    }

    /// \brief Constructor.
    inline
    union::union(const set_expression& set)
      : set_expression(fdr::detail::gsMakeUnion(set))
    {}

    inline
    set_expression union::set() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    inline
    inter::inter()
      : set_expression(fdr::detail::constructInter())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    inter::inter(atermpp::aterm_appl term)
      : set_expression(term)
    {
      assert(fdr::detail::check_term_Inter(m_term));
    }

    /// \brief Constructor.
    inline
    inter::inter(const set_expression& set)
      : set_expression(fdr::detail::gsMakeInter(set))
    {}

    inline
    set_expression inter::set() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    inline
    set::set()
      : set_expression(fdr::detail::constructset())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    set::set(atermpp::aterm_appl term)
      : set_expression(term)
    {
      assert(fdr::detail::check_term_set(m_term));
    }

    /// \brief Constructor.
    inline
    set::set(const seq_expression& seq)
      : set_expression(fdr::detail::gsMakeset(seq))
    {}

    inline
    seq_expression set::seq() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    inline
    set::set()
      : set_expression(fdr::detail::constructSet())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    set::set(atermpp::aterm_appl term)
      : set_expression(term)
    {
      assert(fdr::detail::check_term_Set(m_term));
    }

    /// \brief Constructor.
    inline
    set::set(const set_expression& set)
      : set_expression(fdr::detail::gsMakeSet(set))
    {}

    inline
    set_expression set::set() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    inline
    seq::seq()
      : set_expression(fdr::detail::constructSeq())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    seq::seq(atermpp::aterm_appl term)
      : set_expression(term)
    {
      assert(fdr::detail::check_term_Seq(m_term));
    }

    /// \brief Constructor.
    inline
    seq::seq(const seq_expression& seq)
      : set_expression(fdr::detail::gsMakeSeq(seq))
    {}

    inline
    seq_expression seq::seq() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    inline
    extensions::extensions()
      : set_expression(fdr::detail::constructextensions())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    extensions::extensions(atermpp::aterm_appl term)
      : set_expression(term)
    {
      assert(fdr::detail::check_term_extensions(m_term));
    }

    /// \brief Constructor.
    inline
    extensions::extensions(const expression& expr)
      : set_expression(fdr::detail::gsMakeextensions(expr))
    {}

    inline
    expression extensions::expr() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    inline
    productions::productions()
      : set_expression(fdr::detail::constructproductions())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    productions::productions(atermpp::aterm_appl term)
      : set_expression(term)
    {
      assert(fdr::detail::check_term_productions(m_term));
    }

    /// \brief Constructor.
    inline
    productions::productions(const expression& expr)
      : set_expression(fdr::detail::gsMakeproductions(expr))
    {}

    inline
    expression productions::expr() const
    {
      return atermpp::arg1(*this);
    }
//--- end generated set expression class definitions ---//

//--- start generated tuple expression class definitions ---//
    /// \brief Default constructor.
    inline
    exprs::exprs()
      : tuple_expression(fdr::detail::constructExprs())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    exprs::exprs(atermpp::aterm_appl term)
      : tuple_expression(term)
    {
      assert(fdr::detail::check_term_Exprs(m_term));
    }

    /// \brief Constructor.
    inline
    exprs::exprs(const expression_list& elements)
      : tuple_expression(fdr::detail::gsMakeExprs(elements))
    {}

    inline
    expression_list exprs::elements() const
    {
      return atermpp::list_arg1(*this);
    }
//--- end generated tuple expression class definitions ---//

} // namespace fdr

} // namespace mcrl2
