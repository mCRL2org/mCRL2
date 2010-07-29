from parser_template import *

USER_DRIVER_HEADER = 'driver.h'
USER_PARSER_HEADER = 'parser.h'
USER_SCANNER_HEADER = 'scanner.h'
USER_SEMANTIC_ACTIONS_HEADER = 'semantic_actions.h'
USER_NAMESPACE = 'example'
USER_PARSER_CLASSNAME = 'Parser'
USER_DRIVER_CLASSNAME = 'Driver'
USER_SCANNER_CLASSNAME_PREFIX = 'Example'
USER_SCANNER_CLASSNAME = USER_SCANNER_CLASSNAME_PREFIX + 'FlexLexer'

USER_TOKEN_DEFINITION = '''
%union {                       
    int  			integerVal;      
    double 			doubleVal;     
    std::string*		stringVal; 
    class CalcNode*		calcnode;
}

%token			END	     0	"end of file"
%token			EOL		"end of line"
%token <integerVal> 	INTEGER		"integer"
%token <doubleVal> 	DOUBLE		"double"
%token <stringVal> 	STRING		"string"

%type <calcnode>	constant variable
%type <calcnode>	atomexpr powexpr unaryexpr mulexpr addexpr expr

%destructor { delete $$; } STRING
%destructor { delete $$; } constant variable
%destructor { delete $$; } atomexpr powexpr unaryexpr mulexpr addexpr expr
'''

USER_GRAMMAR_DEFINITION = '''
constant : INTEGER
           {
	       $$ = new CNConstant($1);
	   }
         | DOUBLE
           {
	       $$ = new CNConstant($1);
	   }

variable : STRING
           {
	       if (!driver.calc.existsVariable(*$1)) {
		   error(yyloc, std::string("Unknown variable \\"") + *$1 + "\\"");
		   delete $1;
		   YYERROR;
	       }
	       else {
		   $$ = new CNConstant( driver.calc.getVariable(*$1) );
		   delete $1;
	       }
	   }

atomexpr : constant
           {
	       $$ = $1;
	   }
         | variable
           {
	       $$ = $1;
	   }
         | '(' expr ')'
           {
	       $$ = $2;
	   }

powexpr	: atomexpr
          {
	      $$ = $1;
	  }
        | atomexpr '^' powexpr
          {
	      $$ = new CNPower($1, $3);
	  }

unaryexpr : powexpr
            {
		$$ = $1;
	    }
          | '+' powexpr
            {
		$$ = $2;
	    }
          | '-' powexpr
            {
		$$ = new CNNegate($2);
	    }

mulexpr : unaryexpr
          {
	      $$ = $1;
	  }
        | mulexpr '*' unaryexpr
          {
	      $$ = new CNMultiply($1, $3);
	  }
        | mulexpr '/' unaryexpr
          {
	      $$ = new CNDivide($1, $3);
	  }
        | mulexpr '%' unaryexpr
          {
	      $$ = new CNModulo($1, $3);
	  }

addexpr : mulexpr
          {
	      $$ = $1;
	  }
        | addexpr '+' mulexpr
          {
	      $$ = new CNAdd($1, $3);
	  }
        | addexpr '-' mulexpr
          {
	      $$ = new CNSubtract($1, $3);
	  }

expr	: addexpr
          {
	      $$ = $1;
	  }

assignment : STRING '=' expr
             {
		 driver.calc.variables[*$1] = $3->evaluate();
		 std::cout << "Setting variable " << *$1
			   << " = " << driver.calc.variables[*$1] << "\\\\n";
		 delete $1;
		 delete $3;
	     }

start	: /* empty */
        | start ';'
        | start EOL
	| start assignment ';'
	| start assignment EOL
	| start assignment END
        | start expr ';'
          {
	      driver.calc.expressions.push_back($2);
	  }
        | start expr EOL
          {
	      driver.calc.expressions.push_back($2);
	  }
        | start expr END
          {
	      driver.calc.expressions.push_back($2);
	  }
'''

USER_SCANNER_DEFINITION = '''
[0-9]+ {
    yylval->integerVal = atoi(yytext);
    return token::INTEGER;
}

[0-9]+"."[0-9]* {
    yylval->doubleVal = atof(yytext);
    return token::DOUBLE;
}

[A-Za-z][A-Za-z0-9_,.-]* {
    yylval->stringVal = new std::string(yytext, yyleng);
    return token::STRING;
}

 /* gobble up white-spaces */
[ \\\\t\\\\r]+ {
    yylloc->step();
}

 /* gobble up end-of-lines */
\\\\n {
    yylloc->lines(yyleng); yylloc->step();
    return token::EOL;
}

 /* pass all other characters up to bison */
. {
    return static_cast<token_type>(*yytext);
}
'''

make_parser('parser.yy',
            'scanner.ll',
            USER_DRIVER_HEADER           ,
            USER_PARSER_HEADER           ,
            USER_SCANNER_HEADER          ,
            USER_SEMANTIC_ACTIONS_HEADER ,
            USER_NAMESPACE               ,
            USER_DRIVER_CLASSNAME        ,
            USER_PARSER_CLASSNAME        ,
            USER_SCANNER_CLASSNAME_PREFIX,
            USER_SCANNER_CLASSNAME       ,
            USER_TOKEN_DEFINITION        ,
            USER_GRAMMAR_DEFINITION      ,
            USER_SCANNER_DEFINITION
           )
