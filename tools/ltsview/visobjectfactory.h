// Author(s): Bas Ploeger and Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file visobjectfactory.h
/// \brief Header file for VisObjectFactory class

#ifndef VISOBJECTFACTORY_H
#define VISOBJECTFACTORY_H
#include <vector>
#include "utils.h"

class PrimitiveFactory; /* forward declaration */
class VisObject; /* forward declaration */

class VisObjectFactory {
	public:
		VisObjectFactory();
		~VisObjectFactory();
		void clear();
		void drawObjects(PrimitiveFactory *pf,unsigned char alpha,
                                 bool texture);
		int  makeObject(int primitive, std::vector<int> &ids);
		void sortObjects(Utils::Point3D viewpoint);
		void updateObjectColor(int obj,Utils::RGB_Color color);
                void updateObjectTexture(int obj,
                                    std::vector<Utils::RGB_Color> &texColours);
		void updateObjectMatrix(int obj);
	private:
		std::vector<VisObject*> objects;
		std::vector<VisObject*> objects_sorted;
};
#endif
