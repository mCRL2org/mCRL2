#include "statevectordescription.h"

StateVectorDescription::StateVectorDescription( vector< string > paramNames, vector< string > paramTypes )
// pre: paramNames.size() == paramTypes.size()
{
  AFun paramId = ATmakeAFun( "ParamId", 2, ATfalse );
  svDescription = ATempty;
  for ( int i = paramNames.size()-1 ; i >= 0 ; --i )
  {
    ATinsert( svDescription, 
	(ATerm) ATmakeAppl2( paramId, 
	  ATmake( "<str>", paramNames[i].c_str() ), 
	  ATmake( "<str>", paramTypes[i].c_str() ) ) );
  }
  ATprotect( (ATerm*) &svDescription );
}

StateVectorDescription::~StateVectorDescription()
{
  ATunprotect( (ATerm*) &svDescription );
}
