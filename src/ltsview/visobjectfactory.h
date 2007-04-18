#ifndef VISOBJECTFACTORY_H
#define VISOBJECTFACTORY_H
#include <vector>
#include "primitivefactory.h"
#include "utils.h"

class VisObject; /* forward declaration */

class VisObjectFactory {
	public:
		VisObjectFactory();
		~VisObjectFactory();
		void clear();
		void drawObjects(PrimitiveFactory *pf,unsigned char alpha);
		int  makeObject(int primitive);
		void sortObjects(Utils::Point3D viewpoint);
		void updateObjectColor(int obj,Utils::RGB_Color color);
		void updateObjectMatrix(int obj);
	private:
		std::vector<VisObject*> objects;
		std::vector<VisObject*> objects_sorted;
};
#endif
