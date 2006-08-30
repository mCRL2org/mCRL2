// ---------------------------------
// utils.h
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


#ifndef UTILS_H
#define UTILS_H


#include <sstream>
#include <string>
#include <cstdlib>
#include <cmath>
using namespace std;


const double PI = 3.14159265;


class Utils
{
public:
	static string doubleToString( double d );
	static string intToString( int i );
	static double stringToDouble( string s );
	static int stringToInt( string s );
	static double radiansToDegrees( double r );
	static double degreesToRadians( double d );
	static double euclideanDistance( double x1, 
									 double y1,
		                             double x2, 
									 double y2 );

};


#endif // UTILS_H
