// Copyright (c) 2009-2013 University of Twente
// Copyright (c) 2009-2013 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2013 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2013 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "Decimal.h"
#include <vector>

Decimal operator+(const Decimal &s, const Decimal &t)
{
    size_t i = s.size(), j = t.size();
    std::vector<char> sum;
    sum.reserve(std::max(i, j) + 1);
    unsigned carry = 0;
    while (i || j)
    {
        carry += (i > 0 ? s[--i] - '0' : 0);
        carry += (j > 0 ? t[--j] - '0' : 0);
        sum.push_back(char(carry%10 + '0'));
        carry /= 10;
    }
    if (carry) sum.push_back(char(carry + '0'));
    return Decimal(std::string(sum.rbegin(), sum.rend()));
}

Decimal operator*(const Decimal &s, const Decimal &t)
{
    std::vector<char> product(s.size() + t.size(), '0');
    for (size_t i = 0; i < s.size(); ++i)
    {
        for (size_t j = 0; j < t.size(); ++j)
        {
            unsigned carry = (unsigned)(s[i] - '0')*(t[j] - '0');
            for ( size_t k = (s.size() - 1 - i) + (t.size() - 1 - j);
                  carry > 0; ++k )
            {
                carry += (product[k] - '0');
                product[k] = char(carry%10 + '0');
                carry /= 10;
            }
        }
    }
    while (product.size() > 1 && product.back() == '0') product.pop_back();
    return std::string(product.rbegin(), product.rend());
}
