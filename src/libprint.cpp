#include <string>
#include <ostream>
#include <sstream>
#include "gsfunc.h"
#include "gslowlevel.h"

#define GS_PRINT_STREAM 1
#include "libprint_common.h"

using namespace std;

void DataExpressionToStream(ostream &Stream, ATermAppl Expr)
{
	PrintPartStream(Stream,Expr,false,0);
}

string DataExpressionToString(ATermAppl Expr)
{
	stringstream ss;

	DataExpressionToStream(ss,Expr);

	return ss.str();
}
