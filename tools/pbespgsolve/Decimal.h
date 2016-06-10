// Copyright (c) 2009-2013 University of Twente
// Copyright (c) 2009-2013 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2013 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2013 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef DECIMAL_H_INCLUDED
#define DECIMAL_H_INCLUDED

#include <string>
#include <sstream>

class Decimal
{
    std::string s;

    static std::string str(unsigned i)
    {
        std::ostringstream oss;
        oss << i;
        return oss.str();
    }

public:
    Decimal(const std::string &t) : s(t) { }
    Decimal(const unsigned i) : s(str(i)) { }
    Decimal(const Decimal &d) : s(d.s) { }
    Decimal &operator=(const Decimal &d) { s = d.s; return *this; }
    const std::string &str() const { return s; }
    const char *c_str() const { return s.c_str(); }
    size_t size() const { return s.size(); }
    char operator[](size_t i) const { return s[i]; }
};

Decimal operator+(const Decimal &s, const Decimal &t);
Decimal operator*(const Decimal &s, const Decimal &t);

#endif /* ndef DECIMAL_H_INCLUDED */
