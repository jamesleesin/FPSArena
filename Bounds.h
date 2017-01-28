#ifndef __BOUNDS_H__
#define __BOUNDS_H__

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <GLUT/gl.h>
#else
#include <GL/glut.h>
#include <GL/gl.h>
#endif

#include "Vector3.h"

class Bounds{
	public:
		//Constructor
		Bounds();
		Bounds(Vector3* b, int type);
		void draw();
		Vector3* b;
		int boundType;
};
#endif