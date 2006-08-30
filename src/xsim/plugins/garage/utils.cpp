// ---------------------------------
// utils.cpp
// (c) 2005
// A.J. Pretorius 
// 
// Dept of Math and Computer Science 
// Technische Universitiet Eindhoven
// a.j.pretorius@tue.nl
// ---------------------------------
//
// This class provides often used
//		utility functions. These
//		include conversion functions
//		and some math functions. The
//		functions are static, so no
//		objects need to be created
//		to invoke them on.
//
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


#include "utils.h"




// -------------------------------------
string Utils::doubleToString( double d )
// -------------------------------------
{
	ostringstream oss;
	string result;

	oss << d;
	result = oss.str();
	
	return result;
}




// -------------------------------
string Utils::intToString( int i )
// -------------------------------
{
	ostringstream oss;
	string result;

	oss << i;
	result = oss.str();
	
	return result;
}




// -------------------------------------
double Utils::stringToDouble( string s )
// -------------------------------------
{
	double result;
	result = atof( s.c_str() );
	
	return result;
}




// -------------------------------
int Utils::stringToInt( string s )
// -------------------------------
{
	int result;
	result = atoi( s.c_str() );

	return result;
}




// ---------------------------------------
double Utils::radiansToDegrees( double r )
// ---------------------------------------
{
	double degrees = 0;
	degrees = r * (180 / PI);
	return degrees;
}




// ---------------------------------------
double Utils::degreesToRadians( double d )
// ---------------------------------------
{
	double radians = 0;
	radians = d * (PI / 180);
	return radians;
}




// ----------------------------------------------------
static double euclideanDistance( double x1, double y1,
		                         double x2, double y2 )
// ----------------------------------------------------
{
	double result;
	result = pow( x1 - x2, 2 ) + pow( y1 - y2, 2 );
	result = sqrt( result );
	return result;
}
