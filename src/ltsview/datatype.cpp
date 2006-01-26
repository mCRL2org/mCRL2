#include "datatype.h"

DataType::DataType( string name, set< string > values )
{
  AFun typeId = ATmakeAFun( "TypeId", 2, ATfalse );
  ATermList valuesAT = ATempty;
  for ( set< string >::reverse_iterator it = values.rbegin() ;
      it != values.rend() ; ++it )
  {
    valuesAT = ATinsert( valuesAT, ATmake( "<str>", (*it).c_str() ) );
  }
  dataType = ATmakeAppl2( typeId, ATmake( "<str>", name.c_str() ), (ATerm)valuesAT );
  ATprotect( (ATerm*) &dataType );
}

DataType::~DataType()
{
  ATunprotect( (ATerm*) &dataType );
}
