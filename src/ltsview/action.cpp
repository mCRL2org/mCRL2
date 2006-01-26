#include "action.h"

Action::Action( string actname, vector< string > actparams )
{
  AFun act = ATmakeAFun( "Action", 2, ATfalse );
  ATermList paramsAT = ATempty;
  for ( int i = actparams.size()-1 ; i >= 0 ; --i )
  {
    paramsAT = ATinsert( paramsAT, ATmake( "<str>", actparams[i].c_str() ) );
  }
  action = ATmakeAppl2( act, ATmake( "<str>", actname.c_str() ), (ATerm)paramsAT );
  ATprotect( (ATerm*) &action );
}

Action::~Action()
{
  ATunprotect( (ATerm*) &action );
}

string Action::ToString() const
{
  return "";
}
