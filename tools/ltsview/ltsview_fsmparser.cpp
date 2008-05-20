/* A Bison parser, made by GNU Bison 2.3.  */
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING).

/* Skeleton implementation for Bison LALR(1) parsers in C++

   Copyright (C) 2002, 2003, 2004, 2005, 2006 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

// Take the name prefix into account.
#define yylex   ltsviewlex

#include "ltsview_fsmparser.hpp"

/* User implementation prologue.  */
#line 56 "ltsview_fsmparser.yy"

#include "fileloader.h"
#include "ltsview_fsmlexer.h"

/* this "connects" the bison parser in the fileloader to the flex
 * scanner class object. it defines the yylex() function call to pull
 * the next token from the current lexer object of the fileloader
 * context.  */
#undef yylex
#define yylex fileloader.lexer->lex



/* Line 317 of lalr1.cc.  */
#line 57 "ltsview_fsmparser.cpp"

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* FIXME: INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#define YYUSE(e) ((void) (e))

/* A pseudo ostream that takes yydebug_ into account.  */
# define YYCDEBUG							\
  for (bool yydebugcond_ = yydebug_; yydebugcond_; yydebugcond_ = false)	\
    (*yycdebug_)

/* Enable debugging if requested.  */
#if YYDEBUG

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)	\
do {							\
  if (yydebug_)						\
    {							\
      *yycdebug_ << Title << ' ';			\
      yy_symbol_print_ ((Type), (Value), (Location));	\
      *yycdebug_ << std::endl;				\
    }							\
} while (false)

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug_)				\
    yy_reduce_print_ (Rule);		\
} while (false)

# define YY_STACK_PRINT()		\
do {					\
  if (yydebug_)				\
    yystack_print_ ();			\
} while (false)

#else /* !YYDEBUG */

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_REDUCE_PRINT(Rule)
# define YY_STACK_PRINT()

#endif /* !YYDEBUG */

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab

namespace ltsview
{
#if YYERROR_VERBOSE

  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
  LTSViewFSMParser::yytnamerr_ (const char *yystr)
  {
    if (*yystr == '"')
      {
        std::string yyr = "";
        char const *yyp = yystr;

        for (;;)
          switch (*++yyp)
            {
            case '\'':
            case ',':
              goto do_not_strip_quotes;

            case '\\':
              if (*++yyp != '\\')
                goto do_not_strip_quotes;
              /* Fall through.  */
            default:
              yyr += *yyp;
              break;

            case '"':
              return yyr;
            }
      do_not_strip_quotes: ;
      }

    return yystr;
  }

#endif

  /// Build a parser object.
  LTSViewFSMParser::LTSViewFSMParser (class FileLoader& fileloader_yyarg)
    : yydebug_ (false),
      yycdebug_ (&std::cerr),
      fileloader (fileloader_yyarg)
  {
  }

  LTSViewFSMParser::~LTSViewFSMParser ()
  {
  }

#if YYDEBUG
  /*--------------------------------.
  | Print this symbol on YYOUTPUT.  |
  `--------------------------------*/

  inline void
  LTSViewFSMParser::yy_symbol_value_print_ (int yytype,
			   const semantic_type* yyvaluep, const location_type* yylocationp)
  {
    YYUSE (yylocationp);
    YYUSE (yyvaluep);
    switch (yytype)
      {
         default:
	  break;
      }
  }


  void
  LTSViewFSMParser::yy_symbol_print_ (int yytype,
			   const semantic_type* yyvaluep, const location_type* yylocationp)
  {
    *yycdebug_ << (yytype < yyntokens_ ? "token" : "nterm")
	       << ' ' << yytname_[yytype] << " ("
	       << *yylocationp << ": ";
    yy_symbol_value_print_ (yytype, yyvaluep, yylocationp);
    *yycdebug_ << ')';
  }
#endif /* ! YYDEBUG */

  void
  LTSViewFSMParser::yydestruct_ (const char* yymsg,
			   int yytype, semantic_type* yyvaluep, location_type* yylocationp)
  {
    YYUSE (yylocationp);
    YYUSE (yymsg);
    YYUSE (yyvaluep);

    YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

    switch (yytype)
      {
        case 8: /* "\"string\"" */
#line 53 "ltsview_fsmparser.yy"
	{ delete (yyvaluep->stringVal); };
#line 217 "ltsview_fsmparser.cpp"
	break;
      case 9: /* "\"quoted string\"" */
#line 53 "ltsview_fsmparser.yy"
	{ delete (yyvaluep->stringVal); };
#line 222 "ltsview_fsmparser.cpp"
	break;
      case 23: /* "type_name" */
#line 54 "ltsview_fsmparser.yy"
	{ delete (yyvaluep->stringVal); };
#line 227 "ltsview_fsmparser.cpp"
	break;
      case 24: /* "type_name1" */
#line 54 "ltsview_fsmparser.yy"
	{ delete (yyvaluep->stringVal); };
#line 232 "ltsview_fsmparser.cpp"
	break;
      case 33: /* "action" */
#line 54 "ltsview_fsmparser.yy"
	{ delete (yyvaluep->stringVal); };
#line 237 "ltsview_fsmparser.cpp"
	break;

	default:
	  break;
      }
  }

  void
  LTSViewFSMParser::yypop_ (unsigned int n)
  {
    yystate_stack_.pop (n);
    yysemantic_stack_.pop (n);
    yylocation_stack_.pop (n);
  }

  std::ostream&
  LTSViewFSMParser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  LTSViewFSMParser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  LTSViewFSMParser::debug_level_type
  LTSViewFSMParser::debug_level () const
  {
    return yydebug_;
  }

  void
  LTSViewFSMParser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }


  int
  LTSViewFSMParser::parse ()
  {
    /// Look-ahead and look-ahead in internal form.
    int yychar = yyempty_;
    int yytoken = 0;

    /* State.  */
    int yyn;
    int yylen = 0;
    int yystate = 0;

    /* Error handling.  */
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// Semantic value of the look-ahead.
    semantic_type yylval;
    /// Location of the look-ahead.
    location_type yylloc;
    /// The locations where the error started and ended.
    location yyerror_range[2];

    /// $$.
    semantic_type yyval;
    /// @$.
    location_type yyloc;

    int yyresult;

    YYCDEBUG << "Starting parse" << std::endl;


    /* User initialization code.  */
    #line 26 "ltsview_fsmparser.yy"
{
    // initialize the initial location object
    yylloc.begin.filename = yylloc.end.filename = &fileloader.filename;
}
  /* Line 547 of yacc.c.  */
#line 319 "ltsview_fsmparser.cpp"
    /* Initialize the stacks.  The initial state will be pushed in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystate_stack_ = state_stack_type (0);
    yysemantic_stack_ = semantic_stack_type (0);
    yylocation_stack_ = location_stack_type (0);
    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yylloc);

    /* New state.  */
  yynewstate:
    yystate_stack_.push (yystate);
    YYCDEBUG << "Entering state " << yystate << std::endl;
    goto yybackup;

    /* Backup.  */
  yybackup:

    /* Try to take a decision without look-ahead.  */
    yyn = yypact_[yystate];
    if (yyn == yypact_ninf_)
      goto yydefault;

    /* Read a look-ahead token.  */
    if (yychar == yyempty_)
      {
	YYCDEBUG << "Reading a token: ";
	yychar = yylex (&yylval, &yylloc);
      }


    /* Convert token to internal form.  */
    if (yychar <= yyeof_)
      {
	yychar = yytoken = yyeof_;
	YYCDEBUG << "Now at end of input." << std::endl;
      }
    else
      {
	yytoken = yytranslate_ (yychar);
	YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
      }

    /* If the proper action on seeing token YYTOKEN is to reduce or to
       detect an error, take that action.  */
    yyn += yytoken;
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yytoken)
      goto yydefault;

    /* Reduce or error.  */
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
	if (yyn == 0 || yyn == yytable_ninf_)
	goto yyerrlab;
	yyn = -yyn;
	goto yyreduce;
      }

    /* Accept?  */
    if (yyn == yyfinal_)
      goto yyacceptlab;

    /* Shift the look-ahead token.  */
    YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

    /* Discard the token being shifted unless it is eof.  */
    if (yychar != yyeof_)
      yychar = yyempty_;

    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yylloc);

    /* Count tokens shifted since error; after three, turn off error
       status.  */
    if (yyerrstatus_)
      --yyerrstatus_;

    yystate = yyn;
    goto yynewstate;

  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[yystate];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;

  /*-----------------------------.
  | yyreduce -- Do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    /* If YYLEN is nonzero, implement the default value of the action:
       `$$ = $1'.  Otherwise, use the top of the stack.

       Otherwise, the following line sets YYVAL to garbage.
       This behavior is undocumented and Bison
       users should not rely upon it.  */
    if (yylen)
      yyval = yysemantic_stack_[yylen - 1];
    else
      yyval = yysemantic_stack_[0];

    {
      slice<location_type, location_stack_type> slice (yylocation_stack_, yylen);
      YYLLOC_DEFAULT (yyloc, slice, yylen);
    }
    YY_REDUCE_PRINT (yyn);
    switch (yyn)
      {
	  case 2:
#line 72 "ltsview_fsmparser.yy"
    { fileloader.num_pars = 0;
      fileloader.ignore_par.clear();
    ;}
    break;

  case 3:
#line 77 "ltsview_fsmparser.yy"
    { fileloader.state_id = 0; ;}
    break;

  case 6:
#line 88 "ltsview_fsmparser.yy"
    { fileloader.par_values.clear(); ;}
    break;

  case 7:
#line 90 "ltsview_fsmparser.yy"
    { if (!fileloader.ignore_par[fileloader.num_pars]) {
        fileloader.lts->addParameter(fileloader.par_name,
            fileloader.par_type,fileloader.par_values);
      }
      ++fileloader.num_pars;
    ;}
    break;

  case 9:
#line 100 "ltsview_fsmparser.yy"
    { fileloader.par_name = *(yysemantic_stack_[(1) - (1)].stringVal) ;}
    break;

  case 11:
#line 106 "ltsview_fsmparser.yy"
    { fileloader.ignore_par.push_back((yysemantic_stack_[(3) - (2)].integerVal) == 0);
      fileloader.par_values.reserve((yysemantic_stack_[(3) - (2)].integerVal));
    ;}
    break;

  case 12:
#line 113 "ltsview_fsmparser.yy"
    { fileloader.par_type = *(yysemantic_stack_[(1) - (1)].stringVal); ;}
    break;

  case 14:
#line 119 "ltsview_fsmparser.yy"
    { (yyval.stringVal) = new std::string("") ;}
    break;

  case 15:
#line 122 "ltsview_fsmparser.yy"
    { (yyval.stringVal) = (yysemantic_stack_[(1) - (1)].stringVal) ;}
    break;

  case 16:
#line 125 "ltsview_fsmparser.yy"
    {
      (yyval.stringVal) = new std::string(*(yysemantic_stack_[(3) - (1)].stringVal) + "->" + *(yysemantic_stack_[(3) - (3)].stringVal));
    ;}
    break;

  case 17:
#line 132 "ltsview_fsmparser.yy"
    { (yyval.stringVal) = (yysemantic_stack_[(1) - (1)].stringVal) ;}
    break;

  case 18:
#line 135 "ltsview_fsmparser.yy"
    {
      (yyval.stringVal) = new std::string("(" + *(yysemantic_stack_[(3) - (2)].stringVal) + ")");
    ;}
    break;

  case 21:
#line 149 "ltsview_fsmparser.yy"
    { fileloader.par_values.push_back(*(yysemantic_stack_[(1) - (1)].stringVal)); ;}
    break;

  case 23:
#line 158 "ltsview_fsmparser.yy"
    { 
      fileloader.par_index = 0; 
      fileloader.state_vector.clear();
    ;}
    break;

  case 24:
#line 163 "ltsview_fsmparser.yy"
    { 
      fileloader.lts->addState(fileloader.state_id,fileloader.state_vector);
      ++fileloader.state_id;
    ;}
    break;

  case 27:
#line 174 "ltsview_fsmparser.yy"
    { 
      if (fileloader.par_index >= fileloader.ignore_par.size())
      {
        fileloader.error("too many state parameter values");
      }
      if (!fileloader.ignore_par[fileloader.par_index])
      {
        if (fileloader.state_vector.size() >= fileloader.lts->getNumParameters())
        {
          fileloader.error("too many state parameter values");
        }
        if ((yysemantic_stack_[(2) - (2)].integerVal) < 0 || (yysemantic_stack_[(2) - (2)].integerVal) >= fileloader.lts->getNumParameterValues(
                fileloader.state_vector.size()))
        {
          fileloader.error("state parameter value out of bounds");
        }
        fileloader.state_vector.push_back((yysemantic_stack_[(2) - (2)].integerVal));
      }
      ++fileloader.par_index;
    ;}
    break;

  case 30:
#line 207 "ltsview_fsmparser.yy"
    {
      std::map< std::string,int >::iterator p = fileloader.labels.find(*(yysemantic_stack_[(3) - (3)].stringVal));
      int l;
      if (p == fileloader.labels.end())
      {
        l = fileloader.lts->addLabel(*(yysemantic_stack_[(3) - (3)].stringVal));
        fileloader.labels[*(yysemantic_stack_[(3) - (3)].stringVal)] = l;
      }
      else
      {
        l = p->second;
      }
      // State ids in the FSM file are 1-based, but in our administration
      // they are 0-based!
      fileloader.lts->addTransition((yysemantic_stack_[(3) - (1)].integerVal)-1,(yysemantic_stack_[(3) - (2)].integerVal)-1,l);
    ;}
    break;

  case 31:
#line 227 "ltsview_fsmparser.yy"
    { (yyval.stringVal) = new std::string("") ;}
    break;

  case 32:
#line 230 "ltsview_fsmparser.yy"
    { (yyval.stringVal) = (yysemantic_stack_[(1) - (1)].stringVal) ;}
    break;


    /* Line 675 of lalr1.cc.  */
#line 584 "ltsview_fsmparser.cpp"
	default: break;
      }
    YY_SYMBOL_PRINT ("-> $$ =", yyr1_[yyn], &yyval, &yyloc);

    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();

    yysemantic_stack_.push (yyval);
    yylocation_stack_.push (yyloc);

    /* Shift the result of the reduction.  */
    yyn = yyr1_[yyn];
    yystate = yypgoto_[yyn - yyntokens_] + yystate_stack_[0];
    if (0 <= yystate && yystate <= yylast_
	&& yycheck_[yystate] == yystate_stack_[0])
      yystate = yytable_[yystate];
    else
      yystate = yydefgoto_[yyn - yyntokens_];
    goto yynewstate;

  /*------------------------------------.
  | yyerrlab -- here on detecting error |
  `------------------------------------*/
  yyerrlab:
    /* If not already recovering from an error, report this error.  */
    if (!yyerrstatus_)
      {
	++yynerrs_;
	error (yylloc, yysyntax_error_ (yystate));
      }

    yyerror_range[0] = yylloc;
    if (yyerrstatus_ == 3)
      {
	/* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

	if (yychar <= yyeof_)
	  {
	  /* Return failure if at end of input.  */
	  if (yychar == yyeof_)
	    YYABORT;
	  }
	else
	  {
	    yydestruct_ ("Error: discarding", yytoken, &yylval, &yylloc);
	    yychar = yyempty_;
	  }
      }

    /* Else will try to reuse look-ahead token after shifting the error
       token.  */
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:

    /* Pacify compilers like GCC when the user code never invokes
       YYERROR and the label yyerrorlab therefore never appears in user
       code.  */
    if (false)
      goto yyerrorlab;

    yyerror_range[0] = yylocation_stack_[yylen - 1];
    /* Do not reclaim the symbols of the rule which action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    yystate = yystate_stack_[0];
    goto yyerrlab1;

  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;	/* Each real token shifted decrements this.  */

    for (;;)
      {
	yyn = yypact_[yystate];
	if (yyn != yypact_ninf_)
	{
	  yyn += yyterror_;
	  if (0 <= yyn && yyn <= yylast_ && yycheck_[yyn] == yyterror_)
	    {
	      yyn = yytable_[yyn];
	      if (0 < yyn)
		break;
	    }
	}

	/* Pop the current state because it cannot handle the error token.  */
	if (yystate_stack_.height () == 1)
	YYABORT;

	yyerror_range[0] = yylocation_stack_[0];
	yydestruct_ ("Error: popping",
		     yystos_[yystate],
		     &yysemantic_stack_[0], &yylocation_stack_[0]);
	yypop_ ();
	yystate = yystate_stack_[0];
	YY_STACK_PRINT ();
      }

    if (yyn == yyfinal_)
      goto yyacceptlab;

    yyerror_range[1] = yylloc;
    // Using YYLLOC is tempting, but would change the location of
    // the look-ahead.  YYLOC is available though.
    YYLLOC_DEFAULT (yyloc, (yyerror_range - 1), 2);
    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yyloc);

    /* Shift the error token.  */
    YY_SYMBOL_PRINT ("Shifting", yystos_[yyn],
		   &yysemantic_stack_[0], &yylocation_stack_[0]);

    yystate = yyn;
    goto yynewstate;

    /* Accept.  */
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;

    /* Abort.  */
  yyabortlab:
    yyresult = 1;
    goto yyreturn;

  yyreturn:
    if (yychar != yyeof_ && yychar != yyempty_)
      yydestruct_ ("Cleanup: discarding lookahead", yytoken, &yylval, &yylloc);

    /* Do not reclaim the symbols of the rule which action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    while (yystate_stack_.height () != 1)
      {
	yydestruct_ ("Cleanup: popping",
		   yystos_[yystate_stack_[0]],
		   &yysemantic_stack_[0],
		   &yylocation_stack_[0]);
	yypop_ ();
      }

    return yyresult;
  }

  // Generate an error message.
  std::string
  LTSViewFSMParser::yysyntax_error_ (int yystate)
  {
    std::string res;
    YYUSE (yystate);
#if YYERROR_VERBOSE
    int yyn = yypact_[yystate];
    if (yypact_ninf_ < yyn && yyn <= yylast_)
      {
	/* Start YYX at -YYN if negative to avoid negative indexes in
	   YYCHECK.  */
	int yyxbegin = yyn < 0 ? -yyn : 0;

	/* Stay within bounds of both yycheck and yytname.  */
	int yychecklim = yylast_ - yyn + 1;
	int yyxend = yychecklim < yyntokens_ ? yychecklim : yyntokens_;
	int count = 0;
	for (int x = yyxbegin; x < yyxend; ++x)
	  if (yycheck_[x + yyn] == x && x != yyterror_)
	    ++count;

	// FIXME: This method of building the message is not compatible
	// with internationalization.  It should work like yacc.c does it.
	// That is, first build a string that looks like this:
	// "syntax error, unexpected %s or %s or %s"
	// Then, invoke YY_ on this string.
	// Finally, use the string as a format to output
	// yytname_[tok], etc.
	// Until this gets fixed, this message appears in English only.
	res = "syntax error, unexpected ";
	res += yytnamerr_ (yytname_[tok]);
	if (count < 5)
	  {
	    count = 0;
	    for (int x = yyxbegin; x < yyxend; ++x)
	      if (yycheck_[x + yyn] == x && x != yyterror_)
		{
		  res += (!count++) ? ", expecting " : " or ";
		  res += yytnamerr_ (yytname_[x]);
		}
	  }
      }
    else
#endif
      res = YY_("syntax error");
    return res;
  }


  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
  const signed char LTSViewFSMParser::yypact_ninf_ = -16;
  const signed char
  LTSViewFSMParser::yypact_[] =
  {
       -16,     1,   -16,   -16,     2,    -1,     0,   -16,   -16,   -16,
     -16,     4,     7,     3,     5,    -5,   -16,     8,   -16,     6,
      -5,   -16,   -16,     9,   -16,   -16,    10,   -16,    -2,    -5,
     -16,    11,   -16,    14,   -16,   -16,    13,    15,    16,   -16,
     -16,   -16,    17,   -16,   -16,   -16
  };

  /* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
     doesn't specify something else to do.  Zero means the default is an
     error.  */
  const unsigned char
  LTSViewFSMParser::yydefact_[] =
  {
         2,     0,     5,     1,     6,     0,     0,     3,     9,     7,
      22,     0,     0,    23,     0,    14,     8,     0,    26,     0,
      14,    17,    10,    12,    15,    28,    24,    11,     0,     0,
      19,     4,    27,     0,    18,    16,    13,     0,     0,    25,
      21,    20,    31,    29,    32,    30
  };

  /* YYPGOTO[NTERM-NUM].  */
  const signed char
  LTSViewFSMParser::yypgoto_[] =
  {
       -16,   -16,   -16,   -16,   -16,   -16,   -16,   -16,   -16,   -16,
     -16,   -16,    -7,   -15,   -16,   -16,   -16,   -16,   -16,   -16,
     -16,   -16,   -16
  };

  /* YYDEFGOTO[NTERM-NUM].  */
  const signed char
  LTSViewFSMParser::yydefgoto_[] =
  {
        -1,     1,     2,    10,     4,     6,    12,     9,    11,    15,
      22,    30,    23,    24,    36,    41,    13,    18,    33,    26,
      31,    38,    45
  };

  /* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule which
     number is the opposite.  If zero, do what YYDEFACT says.  */
  const signed char LTSViewFSMParser::yytable_ninf_ = -1;
  const unsigned char
  LTSViewFSMParser::yytable_[] =
  {
        20,     3,     7,    21,    34,    29,     5,    17,     8,    14,
      16,    25,    27,    28,    35,    19,    29,    39,     0,    43,
      32,    37,    40,     0,     0,    42,    44
  };

  /* YYCHECK.  */
  const signed char
  LTSViewFSMParser::yycheck_[] =
  {
         5,     0,     3,     8,     6,     7,     4,     4,     8,     5,
       3,     3,     6,    20,    29,    10,     7,     3,    -1,     3,
      10,    10,     9,    -1,    -1,    10,     9
  };

  /* STOS_[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
  const unsigned char
  LTSViewFSMParser::yystos_[] =
  {
         0,    12,    13,     0,    15,     4,    16,     3,     8,    18,
      14,    19,    17,    27,     5,    20,     3,     4,    28,    10,
       5,     8,    21,    23,    24,     3,    30,     6,    23,     7,
      22,    31,    10,    29,     6,    24,    25,    10,    32,     3,
       9,    26,    10,     3,     9,    33
  };

#if YYDEBUG
  /* TOKEN_NUMBER_[YYLEX-NUM] -- Internal symbol number corresponding
     to YYLEX-NUM.  */
  const unsigned short int
  LTSViewFSMParser::yytoken_number_[] =
  {
         0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265
  };
#endif

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
  const unsigned char
  LTSViewFSMParser::yyr1_[] =
  {
         0,    11,    13,    14,    12,    15,    16,    17,    15,    19,
      18,    20,    22,    21,    23,    23,    23,    24,    24,    25,
      25,    26,    27,    28,    29,    27,    30,    30,    31,    31,
      32,    33,    33
  };

  /* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
  const unsigned char
  LTSViewFSMParser::yyr2_[] =
  {
         0,     2,     0,     0,     9,     0,     0,     0,     5,     0,
       4,     3,     0,     3,     0,     1,     3,     1,     3,     0,
       2,     1,     0,     0,     0,     5,     0,     2,     0,     3,
       3,     0,     1
  };

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
  /* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
     First, the terminals, then, starting at \a yyntokens_, nonterminals.  */
  const char*
  const LTSViewFSMParser::yytname_[] =
  {
    "\"end of file\"", "error", "$undefined", "\"end of line\"",
  "\"section separator\"", "\"opening parenthesis\"",
  "\"closing parenthesis\"", "\"arrow symbol\"", "\"string\"",
  "\"quoted string\"", "\"number\"", "$accept", "fsm_file", "@1", "@2",
  "params", "@3", "@4", "param", "@5", "cardinality", "type_def", "@6",
  "type_name", "type_name1", "type_values", "type_value", "states", "@7",
  "@8", "state", "transitions", "transition", "action", 0
  };
#endif

#if YYDEBUG
  /* YYRHS -- A `-1'-separated list of the rules' RHS.  */
  const LTSViewFSMParser::rhs_number_type
  LTSViewFSMParser::yyrhs_[] =
  {
        12,     0,    -1,    -1,    -1,    13,    15,     4,     3,    14,
      27,     4,     3,    31,    -1,    -1,    -1,    -1,    15,    16,
      18,    17,     3,    -1,    -1,     8,    19,    20,    21,    -1,
       5,    10,     6,    -1,    -1,    23,    22,    25,    -1,    -1,
      24,    -1,    23,     7,    24,    -1,     8,    -1,     5,    23,
       6,    -1,    -1,    25,    26,    -1,     9,    -1,    -1,    -1,
      -1,    27,    28,    30,    29,     3,    -1,    -1,    30,    10,
      -1,    -1,    31,    32,     3,    -1,    10,    10,    33,    -1,
      -1,     9,    -1
  };

  /* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
     YYRHS.  */
  const unsigned char
  LTSViewFSMParser::yyprhs_[] =
  {
         0,     0,     3,     4,     5,    15,    16,    17,    18,    24,
      25,    30,    34,    35,    39,    40,    42,    46,    48,    52,
      53,    56,    58,    59,    60,    61,    67,    68,    71,    72,
      76,    80,    81
  };

  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
  const unsigned char
  LTSViewFSMParser::yyrline_[] =
  {
         0,    72,    72,    77,    72,    84,    88,    90,    87,   100,
     100,   105,   113,   112,   119,   121,   124,   131,   134,   141,
     144,   148,   154,   158,   163,   157,   170,   173,   198,   201,
     206,   227,   229
  };

  // Print the state stack on the debug stream.
  void
  LTSViewFSMParser::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (state_stack_type::const_iterator i = yystate_stack_.begin ();
	 i != yystate_stack_.end (); ++i)
      *yycdebug_ << ' ' << *i;
    *yycdebug_ << std::endl;
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  LTSViewFSMParser::yy_reduce_print_ (int yyrule)
  {
    unsigned int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    /* Print the symbols being reduced, and their result.  */
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
	       << " (line " << yylno << "), ";
    /* The symbols being reduced.  */
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
		       yyrhs_[yyprhs_[yyrule] + yyi],
		       &(yysemantic_stack_[(yynrhs) - (yyi + 1)]),
		       &(yylocation_stack_[(yynrhs) - (yyi + 1)]));
  }
#endif // YYDEBUG

  /* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
  LTSViewFSMParser::token_number_type
  LTSViewFSMParser::yytranslate_ (int t)
  {
    static
    const token_number_type
    translate_table[] =
    {
           0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10
    };
    if ((unsigned int) t <= yyuser_token_number_max_)
      return translate_table[t];
    else
      return yyundef_token_;
  }

  const int LTSViewFSMParser::yyeof_ = 0;
  const int LTSViewFSMParser::yylast_ = 26;
  const int LTSViewFSMParser::yynnts_ = 23;
  const int LTSViewFSMParser::yyempty_ = -2;
  const int LTSViewFSMParser::yyfinal_ = 3;
  const int LTSViewFSMParser::yyterror_ = 1;
  const int LTSViewFSMParser::yyerrcode_ = 256;
  const int LTSViewFSMParser::yyntokens_ = 11;

  const unsigned int LTSViewFSMParser::yyuser_token_number_max_ = 265;
  const LTSViewFSMParser::token_number_type LTSViewFSMParser::yyundef_token_ = 2;

} // namespace ltsview

#line 233 "ltsview_fsmparser.yy"


void ltsview::LTSViewFSMParser::error(const LTSViewFSMParser::location_type &l, 
                               const std::string &m)
{
  fileloader.error(l,m);
}

