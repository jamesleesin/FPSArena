#ifndef __QUADFACE_H__
#define __QUADFACE_H__
#include "Vector3.h"
class QuadFace{
	public:
		QuadFace();
		void setP0(Vector3* v);
		void setP1(Vector3* v);
		void setP2(Vector3* v);
		void setP3(Vector3* v);
		Vector3* p0;
		Vector3* p1;
		Vector3* p2;
		Vector3* p3;

		int initializedPoints;
};
#endif