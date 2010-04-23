#include "mcrl2/fdr/boolean_expression.h"
#include "mcrl2/fdr/common_expression.h"
#include "mcrl2/fdr/dotted_expression.h"
#include "mcrl2/fdr/expression.h"
#include "mcrl2/fdr/lambda_expression.h"
#include "mcrl2/fdr/numeric_expression.h"
#include "mcrl2/fdr/process.h"
#include "mcrl2/fdr/seq_expression.h"
#include "mcrl2/fdr/set_expression.h"
#include "mcrl2/fdr/tuple_expression.h"

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
    conditional::conditional(const boolean_expression& guard, const any& thenpart, const any& elsepart)
      : common_expression(fdr::detail::gsMakeConditional(guard, thenpart, elsepart))
    {}

    boolean_expression conditional::guard() const
    {
      return atermpp::arg1(*this);
    }

    any conditional::thenpart() const
    {
      return atermpp::arg2(*this);
    }

    any conditional::elsepart() const
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
    localdef::localdef(const definition_list& defs, const any& within)
      : common_expression(fdr::detail::gsMakeLocalDef(defs, within))
    {}

    definition_list localdef::defs() const
    {
      return atermpp::list_arg1(*this);
    }

    any localdef::within() const
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
    bracketed::bracketed(const any& operand)
      : common_expression(fdr::detail::gsMakeBracketed(operand))
    {}

    any bracketed::operand() const
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
    pattern::pattern(const any& left, const any& right)
      : common_expression(fdr::detail::gsMakePattern(left, right))
    {}

    any pattern::left() const
    {
      return atermpp::arg1(*this);
    }

    any pattern::right() const
    {
      return atermpp::arg2(*this);
    }
//--- end generated common expression class definitions ---//

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

//--- start generated expression class definitions ---//
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
//--- end generated expression class definitions ---//

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
    lambdaexpr::lambdaexpr(const expression_list& exprs, const any& function)
      : lambda_expression(fdr::detail::gsMakeLambdaExpr(exprs, function))
    {}

    expression_list lambdaexpr::exprs() const
    {
      return atermpp::list_arg1(*this);
    }

    any lambdaexpr::function() const
    {
      return atermpp::arg2(*this);
    }
//--- end generated lambda expression class definitions ---//

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
    chanset::chanset(const targ& argument)
      : set_expression(fdr::detail::gsMakeChanSet(argument))
    {}

    targ chanset::argument() const
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
      : process(fdr::detail::constructSTOP())
    {}

    /// \brief Constructor.
    /// \param term A term
    stop::stop(atermpp::aterm_appl term)
      : process(term)
    {
      assert(fdr::detail::check_term_STOP(m_term));
    }

    /// \brief Default constructor.
    skip::skip()
      : process(fdr::detail::constructSKIP())
    {}

    /// \brief Constructor.
    /// \param term A term
    skip::skip(atermpp::aterm_appl term)
      : process(term)
    {
      assert(fdr::detail::check_term_SKIP(m_term));
    }

    /// \brief Default constructor.
    chaos::chaos()
      : process(fdr::detail::constructCHAOS())
    {}

    /// \brief Constructor.
    /// \param term A term
    chaos::chaos(atermpp::aterm_appl term)
      : process(term)
    {
      assert(fdr::detail::check_term_CHAOS(m_term));
    }

    /// \brief Constructor.
    chaos::chaos(const set_expression& set)
      : process(fdr::detail::gsMakeCHAOS(set))
    {}

    set_expression chaos::set() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    prefix::prefix()
      : process(fdr::detail::constructPrefix())
    {}

    /// \brief Constructor.
    /// \param term A term
    prefix::prefix(atermpp::aterm_appl term)
      : process(term)
    {
      assert(fdr::detail::check_term_Prefix(m_term));
    }

    /// \brief Constructor.
    prefix::prefix(const dotted_expression& dotted, const field_list& fields, const process& proc)
      : process(fdr::detail::gsMakePrefix(dotted, fields, proc))
    {}

    dotted_expression prefix::dotted() const
    {
      return atermpp::arg1(*this);
    }

    field_list prefix::fields() const
    {
      return atermpp::list_arg2(*this);
    }

    process prefix::proc() const
    {
      return atermpp::arg3(*this);
    }

    /// \brief Default constructor.
    externalchoice::externalchoice()
      : process(fdr::detail::constructExternalChoice())
    {}

    /// \brief Constructor.
    /// \param term A term
    externalchoice::externalchoice(atermpp::aterm_appl term)
      : process(term)
    {
      assert(fdr::detail::check_term_ExternalChoice(m_term));
    }

    /// \brief Constructor.
    externalchoice::externalchoice(const process& left, const process& right)
      : process(fdr::detail::gsMakeExternalChoice(left, right))
    {}

    process externalchoice::left() const
    {
      return atermpp::arg1(*this);
    }

    process externalchoice::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    internalchoice::internalchoice()
      : process(fdr::detail::constructInternalChoice())
    {}

    /// \brief Constructor.
    /// \param term A term
    internalchoice::internalchoice(atermpp::aterm_appl term)
      : process(term)
    {
      assert(fdr::detail::check_term_InternalChoice(m_term));
    }

    /// \brief Constructor.
    internalchoice::internalchoice(const process& left, const process& right)
      : process(fdr::detail::gsMakeInternalChoice(left, right))
    {}

    process internalchoice::left() const
    {
      return atermpp::arg1(*this);
    }

    process internalchoice::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    sequentialcomposition::sequentialcomposition()
      : process(fdr::detail::constructSequentialComposition())
    {}

    /// \brief Constructor.
    /// \param term A term
    sequentialcomposition::sequentialcomposition(atermpp::aterm_appl term)
      : process(term)
    {
      assert(fdr::detail::check_term_SequentialComposition(m_term));
    }

    /// \brief Constructor.
    sequentialcomposition::sequentialcomposition(const process& left, const process& right)
      : process(fdr::detail::gsMakeSequentialComposition(left, right))
    {}

    process sequentialcomposition::left() const
    {
      return atermpp::arg1(*this);
    }

    process sequentialcomposition::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    interrupt::interrupt()
      : process(fdr::detail::constructInterrupt())
    {}

    /// \brief Constructor.
    /// \param term A term
    interrupt::interrupt(atermpp::aterm_appl term)
      : process(term)
    {
      assert(fdr::detail::check_term_Interrupt(m_term));
    }

    /// \brief Constructor.
    interrupt::interrupt(const process& left, const process& right)
      : process(fdr::detail::gsMakeInterrupt(left, right))
    {}

    process interrupt::left() const
    {
      return atermpp::arg1(*this);
    }

    process interrupt::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    hiding::hiding()
      : process(fdr::detail::constructHiding())
    {}

    /// \brief Constructor.
    /// \param term A term
    hiding::hiding(atermpp::aterm_appl term)
      : process(term)
    {
      assert(fdr::detail::check_term_Hiding(m_term));
    }

    /// \brief Constructor.
    hiding::hiding(const process& proc, const set_expression& set)
      : process(fdr::detail::gsMakeHiding(proc, set))
    {}

    process hiding::proc() const
    {
      return atermpp::arg1(*this);
    }

    set_expression hiding::set() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    interleave::interleave()
      : process(fdr::detail::constructInterleave())
    {}

    /// \brief Constructor.
    /// \param term A term
    interleave::interleave(atermpp::aterm_appl term)
      : process(term)
    {
      assert(fdr::detail::check_term_Interleave(m_term));
    }

    /// \brief Constructor.
    interleave::interleave(const process& left, const process& right)
      : process(fdr::detail::gsMakeInterleave(left, right))
    {}

    process interleave::left() const
    {
      return atermpp::arg1(*this);
    }

    process interleave::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    sharing::sharing()
      : process(fdr::detail::constructSharing())
    {}

    /// \brief Constructor.
    /// \param term A term
    sharing::sharing(atermpp::aterm_appl term)
      : process(term)
    {
      assert(fdr::detail::check_term_Sharing(m_term));
    }

    /// \brief Constructor.
    sharing::sharing(const process& left, const process& right, const set_expression& set)
      : process(fdr::detail::gsMakeSharing(left, right, set))
    {}

    process sharing::left() const
    {
      return atermpp::arg1(*this);
    }

    process sharing::right() const
    {
      return atermpp::arg2(*this);
    }

    set_expression sharing::set() const
    {
      return atermpp::arg3(*this);
    }

    /// \brief Default constructor.
    alphaparallel::alphaparallel()
      : process(fdr::detail::constructAlphaParallel())
    {}

    /// \brief Constructor.
    /// \param term A term
    alphaparallel::alphaparallel(atermpp::aterm_appl term)
      : process(term)
    {
      assert(fdr::detail::check_term_AlphaParallel(m_term));
    }

    /// \brief Constructor.
    alphaparallel::alphaparallel(const process& left, const process& right, const set_expression& left_set, const set_expression& right_set)
      : process(fdr::detail::gsMakeAlphaParallel(left, right, left_set, right_set))
    {}

    process alphaparallel::left() const
    {
      return atermpp::arg1(*this);
    }

    process alphaparallel::right() const
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
      : process(fdr::detail::constructRepExternalChoice())
    {}

    /// \brief Constructor.
    /// \param term A term
    repexternalchoice::repexternalchoice(atermpp::aterm_appl term)
      : process(term)
    {
      assert(fdr::detail::check_term_RepExternalChoice(m_term));
    }

    /// \brief Constructor.
    repexternalchoice::repexternalchoice(const setgen& gen, const process& proc)
      : process(fdr::detail::gsMakeRepExternalChoice(gen, proc))
    {}

    setgen repexternalchoice::gen() const
    {
      return atermpp::arg1(*this);
    }

    process repexternalchoice::proc() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    repinternalchoice::repinternalchoice()
      : process(fdr::detail::constructRepInternalChoice())
    {}

    /// \brief Constructor.
    /// \param term A term
    repinternalchoice::repinternalchoice(atermpp::aterm_appl term)
      : process(term)
    {
      assert(fdr::detail::check_term_RepInternalChoice(m_term));
    }

    /// \brief Constructor.
    repinternalchoice::repinternalchoice(const setgen& gen, const process& proc)
      : process(fdr::detail::gsMakeRepInternalChoice(gen, proc))
    {}

    setgen repinternalchoice::gen() const
    {
      return atermpp::arg1(*this);
    }

    process repinternalchoice::proc() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    repsequentialcomposition::repsequentialcomposition()
      : process(fdr::detail::constructRepSequentialComposition())
    {}

    /// \brief Constructor.
    /// \param term A term
    repsequentialcomposition::repsequentialcomposition(atermpp::aterm_appl term)
      : process(term)
    {
      assert(fdr::detail::check_term_RepSequentialComposition(m_term));
    }

    /// \brief Constructor.
    repsequentialcomposition::repsequentialcomposition(const seqgen& gen, const process& proc)
      : process(fdr::detail::gsMakeRepSequentialComposition(gen, proc))
    {}

    seqgen repsequentialcomposition::gen() const
    {
      return atermpp::arg1(*this);
    }

    process repsequentialcomposition::proc() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    repinterleave::repinterleave()
      : process(fdr::detail::constructRepInterleave())
    {}

    /// \brief Constructor.
    /// \param term A term
    repinterleave::repinterleave(atermpp::aterm_appl term)
      : process(term)
    {
      assert(fdr::detail::check_term_RepInterleave(m_term));
    }

    /// \brief Constructor.
    repinterleave::repinterleave(const setgen& gen, const process& proc)
      : process(fdr::detail::gsMakeRepInterleave(gen, proc))
    {}

    setgen repinterleave::gen() const
    {
      return atermpp::arg1(*this);
    }

    process repinterleave::proc() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    repsharing::repsharing()
      : process(fdr::detail::constructRepSharing())
    {}

    /// \brief Constructor.
    /// \param term A term
    repsharing::repsharing(atermpp::aterm_appl term)
      : process(term)
    {
      assert(fdr::detail::check_term_RepSharing(m_term));
    }

    /// \brief Constructor.
    repsharing::repsharing(const setgen& gen, const process& proc, const set_expression& set)
      : process(fdr::detail::gsMakeRepSharing(gen, proc, set))
    {}

    setgen repsharing::gen() const
    {
      return atermpp::arg1(*this);
    }

    process repsharing::proc() const
    {
      return atermpp::arg2(*this);
    }

    set_expression repsharing::set() const
    {
      return atermpp::arg3(*this);
    }

    /// \brief Default constructor.
    repalphaparallel::repalphaparallel()
      : process(fdr::detail::constructRepAlphaParallel())
    {}

    /// \brief Constructor.
    /// \param term A term
    repalphaparallel::repalphaparallel(atermpp::aterm_appl term)
      : process(term)
    {
      assert(fdr::detail::check_term_RepAlphaParallel(m_term));
    }

    /// \brief Constructor.
    repalphaparallel::repalphaparallel(const setgen& gen, const process& proc, const set_expression& set)
      : process(fdr::detail::gsMakeRepAlphaParallel(gen, proc, set))
    {}

    setgen repalphaparallel::gen() const
    {
      return atermpp::arg1(*this);
    }

    process repalphaparallel::proc() const
    {
      return atermpp::arg2(*this);
    }

    set_expression repalphaparallel::set() const
    {
      return atermpp::arg3(*this);
    }

    /// \brief Default constructor.
    untimedtimeout::untimedtimeout()
      : process(fdr::detail::constructUntimedTimeOut())
    {}

    /// \brief Constructor.
    /// \param term A term
    untimedtimeout::untimedtimeout(atermpp::aterm_appl term)
      : process(term)
    {
      assert(fdr::detail::check_term_UntimedTimeOut(m_term));
    }

    /// \brief Constructor.
    untimedtimeout::untimedtimeout(const process& left, const process& right)
      : process(fdr::detail::gsMakeUntimedTimeOut(left, right))
    {}

    process untimedtimeout::left() const
    {
      return atermpp::arg1(*this);
    }

    process untimedtimeout::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    boolguard::boolguard()
      : process(fdr::detail::constructBoolGuard())
    {}

    /// \brief Constructor.
    /// \param term A term
    boolguard::boolguard(atermpp::aterm_appl term)
      : process(term)
    {
      assert(fdr::detail::check_term_BoolGuard(m_term));
    }

    /// \brief Constructor.
    boolguard::boolguard(const boolean_expression& guard, const process& proc)
      : process(fdr::detail::gsMakeBoolGuard(guard, proc))
    {}

    boolean_expression boolguard::guard() const
    {
      return atermpp::arg1(*this);
    }

    process boolguard::proc() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    linkedparallel::linkedparallel()
      : process(fdr::detail::constructLinkedParallel())
    {}

    /// \brief Constructor.
    /// \param term A term
    linkedparallel::linkedparallel(atermpp::aterm_appl term)
      : process(term)
    {
      assert(fdr::detail::check_term_LinkedParallel(m_term));
    }

    /// \brief Constructor.
    linkedparallel::linkedparallel(const process& left, const process& right, const linkpar& linked)
      : process(fdr::detail::gsMakeLinkedParallel(left, right, linked))
    {}

    process linkedparallel::left() const
    {
      return atermpp::arg1(*this);
    }

    process linkedparallel::right() const
    {
      return atermpp::arg2(*this);
    }

    linkpar linkedparallel::linked() const
    {
      return atermpp::arg3(*this);
    }

    /// \brief Default constructor.
    replinkedparallel::replinkedparallel()
      : process(fdr::detail::constructRepLinkedParallel())
    {}

    /// \brief Constructor.
    /// \param term A term
    replinkedparallel::replinkedparallel(atermpp::aterm_appl term)
      : process(term)
    {
      assert(fdr::detail::check_term_RepLinkedParallel(m_term));
    }

    /// \brief Constructor.
    replinkedparallel::replinkedparallel(const seqgen& gen, const process& proc, const linkpar& linked)
      : process(fdr::detail::gsMakeRepLinkedParallel(gen, proc, linked))
    {}

    seqgen replinkedparallel::gen() const
    {
      return atermpp::arg1(*this);
    }

    process replinkedparallel::proc() const
    {
      return atermpp::arg2(*this);
    }

    linkpar replinkedparallel::linked() const
    {
      return atermpp::arg3(*this);
    }
//--- end generated process expression class definitions ---//

} // namespace fdr

} // namespace mcrl2
