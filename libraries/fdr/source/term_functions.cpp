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
    true_::true_()
      : boolean_expression(fdr::detail::constructtrue())
    {}

    /// \brief Constructor.
    /// \param term A term
    INLINEtrue_::true_(atermpp::aterm_appl term)
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
    INLINEfalse_::false_(atermpp::aterm_appl term)
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
    INLINEand_::and_(atermpp::aterm_appl term)
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
    INLINEor_::or_(atermpp::aterm_appl term)
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
    INLINEnot_::not_(atermpp::aterm_appl term)
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
    INLINEnull::null(atermpp::aterm_appl term)
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
    INLINEelem::elem(atermpp::aterm_appl term)
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
    INLINEmember::member(atermpp::aterm_appl term)
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
    INLINEempty::empty(atermpp::aterm_appl term)
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
    INLINEequal::equal(atermpp::aterm_appl term)
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
    INLINEnotequal::notequal(atermpp::aterm_appl term)
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
    INLINEless::less(atermpp::aterm_appl term)
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
    INLINElessorequal::lessorequal(atermpp::aterm_appl term)
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
    INLINEgreater::greater(atermpp::aterm_appl term)
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
    INLINEgreaterorequal::greaterorequal(atermpp::aterm_appl term)
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
    INLINEconditional::conditional(atermpp::aterm_appl term)
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
    INLINEidentifier::identifier(atermpp::aterm_appl term)
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
    INLINElambdaappl::lambdaappl(atermpp::aterm_appl term)
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
    INLINElocaldef::localdef(atermpp::aterm_appl term)
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
    INLINEbracketed::bracketed(atermpp::aterm_appl term)
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
    INLINEpattern::pattern(atermpp::aterm_appl term)
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
    INLINEdot::dot(atermpp::aterm_appl term)
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
    INLINEnumb::numb(atermpp::aterm_appl term)
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
    INLINEbool_::bool_(atermpp::aterm_appl term)
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
    set::set()
      : expression(fdr::detail::constructSet())
    {}

    /// \brief Constructor.
    /// \param term A term
    INLINEset::set(atermpp::aterm_appl term)
      : expression(term)
    {
      assert(fdr::detail::check_term_Set(m_term));
    }

    /// \brief Constructor.
    set::set(const set_expression& operand)
      : expression(fdr::detail::gsMakeSet(operand))
    {}

    set_expression set::operand() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    seq::seq()
      : expression(fdr::detail::constructSeq())
    {}

    /// \brief Constructor.
    /// \param term A term
    INLINEseq::seq(atermpp::aterm_appl term)
      : expression(term)
    {
      assert(fdr::detail::check_term_Seq(m_term));
    }

    /// \brief Constructor.
    seq::seq(const seq_expression& operand)
      : expression(fdr::detail::gsMakeSeq(operand))
    {}

    seq_expression seq::operand() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    tuple::tuple()
      : expression(fdr::detail::constructTuple())
    {}

    /// \brief Constructor.
    /// \param term A term
    INLINEtuple::tuple(atermpp::aterm_appl term)
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
    INLINEdotted::dotted(atermpp::aterm_appl term)
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
    INLINElambda::lambda(atermpp::aterm_appl term)
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
    INLINElambdaexpr::lambdaexpr(atermpp::aterm_appl term)
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
    INLINEnumber::number(atermpp::aterm_appl term)
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
    INLINEcard::card(atermpp::aterm_appl term)
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
    INLINElength::length(atermpp::aterm_appl term)
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
    INLINEplus::plus(atermpp::aterm_appl term)
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
    INLINEminus::minus(atermpp::aterm_appl term)
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
    INLINEtimes::times(atermpp::aterm_appl term)
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
    INLINEdiv::div(atermpp::aterm_appl term)
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
    INLINEmod::mod(atermpp::aterm_appl term)
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
    INLINEmin::min(atermpp::aterm_appl term)
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
    INLINEcat::cat(atermpp::aterm_appl term)
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
    INLINEconcat::concat(atermpp::aterm_appl term)
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
    INLINEhead::head(atermpp::aterm_appl term)
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
    INLINEtail::tail(atermpp::aterm_appl term)
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
    INLINEchanset::chanset(atermpp::aterm_appl term)
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
    union::union()
      : set_expression(fdr::detail::constructunion())
    {}

    /// \brief Constructor.
    /// \param term A term
    INLINEunion::union(atermpp::aterm_appl term)
      : set_expression(term)
    {
      assert(fdr::detail::check_term_union(m_term));
    }

    /// \brief Constructor.
    union::union(const set_expression& left, const set_expression& right)
      : set_expression(fdr::detail::gsMakeunion(left, right))
    {}

    set_expression union::left() const
    {
      return atermpp::arg1(*this);
    }

    set_expression union::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    inter::inter()
      : set_expression(fdr::detail::constructinter())
    {}

    /// \brief Constructor.
    /// \param term A term
    INLINEinter::inter(atermpp::aterm_appl term)
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
    INLINEdiff::diff(atermpp::aterm_appl term)
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
    union::union()
      : set_expression(fdr::detail::constructUnion())
    {}

    /// \brief Constructor.
    /// \param term A term
    INLINEunion::union(atermpp::aterm_appl term)
      : set_expression(term)
    {
      assert(fdr::detail::check_term_Union(m_term));
    }

    /// \brief Constructor.
    union::union(const set_expression& set)
      : set_expression(fdr::detail::gsMakeUnion(set))
    {}

    set_expression union::set() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    inter::inter()
      : set_expression(fdr::detail::constructInter())
    {}

    /// \brief Constructor.
    /// \param term A term
    INLINEinter::inter(atermpp::aterm_appl term)
      : set_expression(term)
    {
      assert(fdr::detail::check_term_Inter(m_term));
    }

    /// \brief Constructor.
    inter::inter(const set_expression& set)
      : set_expression(fdr::detail::gsMakeInter(set))
    {}

    set_expression inter::set() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    set::set()
      : set_expression(fdr::detail::constructset())
    {}

    /// \brief Constructor.
    /// \param term A term
    INLINEset::set(atermpp::aterm_appl term)
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
    set::set()
      : set_expression(fdr::detail::constructSet())
    {}

    /// \brief Constructor.
    /// \param term A term
    INLINEset::set(atermpp::aterm_appl term)
      : set_expression(term)
    {
      assert(fdr::detail::check_term_Set(m_term));
    }

    /// \brief Constructor.
    set::set(const set_expression& set)
      : set_expression(fdr::detail::gsMakeSet(set))
    {}

    set_expression set::set() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    seq::seq()
      : set_expression(fdr::detail::constructSeq())
    {}

    /// \brief Constructor.
    /// \param term A term
    INLINEseq::seq(atermpp::aterm_appl term)
      : set_expression(term)
    {
      assert(fdr::detail::check_term_Seq(m_term));
    }

    /// \brief Constructor.
    seq::seq(const seq_expression& seq)
      : set_expression(fdr::detail::gsMakeSeq(seq))
    {}

    seq_expression seq::seq() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    extensions::extensions()
      : set_expression(fdr::detail::constructextensions())
    {}

    /// \brief Constructor.
    /// \param term A term
    INLINEextensions::extensions(atermpp::aterm_appl term)
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
    INLINEproductions::productions(atermpp::aterm_appl term)
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
    INLINEexprs::exprs(atermpp::aterm_appl term)
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

} // namespace fdr

} // namespace mcrl2
