// Copyright (c) 2009-2011 University of Twente
// Copyright (c) 2009-2011 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2011 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2011 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <string>
#include <algorithm>

#include "LiftingStrategy.h"
#include "LinearLiftingStrategy.h"
#include "PredecessorLiftingStrategy.h"
#include "FocusListLiftingStrategy.h"
#include "MaxMeasureLiftingStrategy.h"
#include "OldMaxMeasureLiftingStrategy.h"
#include "LinPredLiftingStrategy.h"

#include <stdlib.h>

#include "compatibility.h"
#define strcasecmp compat_strcasecmp

/// \brief Convert string to lowercase.
/// \param in An input string
/// \return The text in \a in in lowercase
static inline
std::string tolower(std::string s)
{
  // dirty trick in the function to disambiguate between two tolower functions!
  std::transform(s.begin(), s.end(), s.begin(), (int(*)(int)) tolower);
  return s;
}

/// \brief Check whether s contains a non-whitespace character.
static inline
bool nonempty(std::string s)
{
  s.erase(std::remove_if(s.begin(), s.end(), std::isspace), s.end());
  return !s.empty();
}

LiftingStrategyFactory::~LiftingStrategyFactory()
{
}

LiftingStrategyFactory *
    LiftingStrategyFactory::create(const std::string &description)
{
    if (description.empty()) return NULL;

    // Split into parts, separated by semicolon characters
    std::vector<std::string> parts;
    std::string::size_type i, j;
    for (i = 0; (j = description.find(':', i)) != std::string::npos; i = j + 1)
    {
        parts.push_back(std::string(description, i, j - i));
    }
    parts.push_back(std::string(description, i, j));

  std::string case_ = tolower(parts[0]);
  if(case_ == "linear" || case_ == "lin")
  {
		bool backward  = parts.size() > 1 && nonempty(parts[1]);
    bool alternate = parts.size() > 2 && nonempty(parts[2]);
    return new LinearLiftingStrategyFactory(backward, alternate);
  }
  else if(case_ == "predecessor" || case_ == "pred")
  {
		bool backward = parts.size() > 1 && nonempty(parts[1]);
    bool stack    = parts.size() > 2 && nonempty(parts[2]);
    return new PredecessorLiftingStrategyFactory(backward, stack);
  }
  else if(case_ == "focuslist" || case_ == "focus")
  {
		bool backward     = parts.size() > 1 && nonempty(parts[1]);
    bool alternate    = parts.size() > 2 && nonempty(parts[2]);
    double max_size   = (parts.size() > 3 ? atof(parts[3].c_str()) : 0);
    double lift_ratio = (parts.size() > 4 ? atof(parts[4].c_str()) : 0);
    return new FocusListLiftingStrategyFactory(
          backward, alternate, max_size, lift_ratio );
  }
  else if(case_ == "maxmeasure")
  {
		return new MaxMeasureLiftingStrategyFactory();
  }
  else if(case_ == "oldmaxmeasure")
  {
		return new OldMaxMeasureLiftingStrategyFactory();
  }
  else if(case_ == "linpred")
  {
    return new LinPredLiftingStrategyFactory();
  }
  else
  {
	  return NULL;
	}
}
