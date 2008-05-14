//  Author(s): A.J. (Hannes) Pretorius
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//  LICENSE_1_0.txt)
//
/// \file ./utils.h

// --- utils.h ------------------------------------------------------
// (c) 2007  -  A.J. Pretorius  -  Eindhoven University of Technology
// ---------------------------  *  ----------------------------------

#ifndef UTILS_H
#define UTILS_H

#include <cstddef>
#include <cstdlib>
#include <cmath>
#include <sstream>
#include <string>
#include <map>
#include <set>
#include <vector>
using namespace std;

const double PI = 3.14159265;
const double E  = 2.71828183;

class Utils
{
public:
    // -- string conversion functions -------------------------------
    static string dblToStr( const double &d );
    static string intToStr( const int &i );
    static double strToDbl( const string &s );
    static int strToInt( const string &s );
    
    // -- trig functions --------------------------------------------
    static double radToDegr( const double &r );
    static double degrToRad( const double &d );

    static double calcAngleDg( 
        const double &x, 
        const double &y );
    static double calcAngleRd( 
        const double &x, 
        const double &y );

    // -- math functions --------------------------------------------
    static int rndToInt( const double &f );
    static double rndToNearestMult( 
        const double &value,
        const double &factor );
    
    static double dist( 
        const double &x1, const double &y1,
        const double &x2, const double &y2 );
    static double abs( const double &val );
	static double maxx(
        const double &d0, 
        const double &d1 );
    static double minn(
        const double &d0,
        const double &d1 );
    
    static double perc(
        const int &numr,
        const int &denm );
    static double perc(
        const double &numr,
        const double &denm );

    static double fishEye( 
        const double &distortion,
        const double &value );

    // -- statistics functions --------------------------------------
    static double mean( const vector< double > vals );
    static double variance( const vector< double > vals );
    static double stdDev( const vector< double > vals );

    // -- classification (binning ) ---------------------------------
    static void classEqualIntervals(
        const size_t &numClasses,
        const vector< double > &values,
        vector< string > &legendClasses,
        map< double, int > &valuesToClasses );
    static void classifyQuantiles(
        const size_t &numClasses,
        set< double > &values,
        vector< string > &legendClasses,
        map< double, int > &valuesToClasses );
    static void classifyMeanStandardDeviation(
        const size_t &numClasses,
        const vector< double > &values,
        vector< string > &legendClasses,
        map< double, int > &valuesToClasses );
    /*
    static void classifyOptimal(
        const int &numClasses,
        const vector< double > &values,
        vector< string > &legendClasses,
        map< double, int > &valuesToClasses );
    */
};

#endif

// -- end -----------------------------------------------------------
