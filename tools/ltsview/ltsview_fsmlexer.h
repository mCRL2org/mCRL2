#ifndef FSMLEXER_H
#define FSMLEXER_H

// Flex expects the signature of yylex to be defined in the macro YY_DECL, and
// the C++ parser expects it to be declared. We can factor both as follows.

#ifndef YY_DECL

#define YY_DECL                                    \
  ltsview::LTSViewFSMParser::token_type                   \
  ltsview::LTSViewFSMLexer::lex(                          \
        ltsview::LTSViewFSMParser::semantic_type* yylval, \
        ltsview::LTSViewFSMParser::location_type* yylloc  \
  )
#endif

#ifndef __FLEX_LEXER_H
#define yyFlexLexer LTSViewFSMFlexLexer
#include "FlexLexer.h"
#undef yyFlexLexer
#endif

#include "ltsview_fsmparser.hpp"

namespace ltsview {

class LTSViewFSMLexer : public LTSViewFSMFlexLexer
{
public:
    LTSViewFSMLexer(std::istream* arg_yyin = 0,
            std::ostream* arg_yyout = 0);

    virtual ~LTSViewFSMLexer();

    virtual LTSViewFSMParser::token_type lex(
        LTSViewFSMParser::semantic_type* yylval,
        LTSViewFSMParser::location_type* yylloc
        );
};

}

#endif // FSMLEXER_H
