// Copyright (c) 2007, 2009 University of Twente
// Copyright (c) 2007, 2009 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "LiftingStrategy.h"
#include "LinearLiftingStrategy.h"
#include "PredecessorLiftingStrategy.h"
#include "FocusListLiftingStrategy.h"
#include "MaxMeasureLiftingStrategy.h"
#include "OldMaxMeasureLiftingStrategy.h"

#include <stdlib.h>

#include "compatibility.h"
#define strcasecmp compat_strcasecmp

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

    if (strcasecmp(parts[0].c_str(), "linear") == 0)
    {
        int backward = (parts.size() > 1 ? atoi(parts[1].c_str()) : 0);
        return new LinearLiftingStrategyFactory(backward);
    }
    else
    if ( strcasecmp(parts[0].c_str(), "predecessor") == 0 ||
         strcasecmp(parts[0].c_str(), "pred") == 0 )
    {
        int backward = (parts.size() > 1 ? atoi(parts[1].c_str()) : 0);
        int stack    = (parts.size() > 2 ? atoi(parts[2].c_str()) : 0);
        return new PredecessorLiftingStrategyFactory(backward, stack);
    }
    else
    if ( strcasecmp(parts[0].c_str(), "focuslist") == 0 ||
         strcasecmp(parts[0].c_str(), "focus") == 0 )
    {
        int backward = (parts.size() > 1 ? atoi(parts[1].c_str()) : 0);
        double ratio = (parts.size() > 2 ? atof(parts[2].c_str()) : 0);
        return new FocusListLiftingStrategyFactory(backward, ratio);
    }
    else
    if (strcasecmp(parts[0].c_str(), "maxmeasure") == 0)
    {
        return new MaxMeasureLiftingStrategyFactory();
    }
    else
    if (strcasecmp(parts[0].c_str(), "oldmaxmeasure") == 0)
    {
        return new OldMaxMeasureLiftingStrategyFactory();
    }
    else
    {
        // No suitable strategy found
        return NULL;
    }
}
