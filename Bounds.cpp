#include "Bounds.h"
#include "Vector3.h"

#include <GL/glut.h>
Bounds::Bounds(){
	b = new Vector3(2,2,2);
	boundType = 0;
}

Bounds::Bounds(Vector3* b, int type){
	this->b = b;
	boundType = type;
}

void Bounds::draw(){
	switch(boundType){
	case 0:
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		glColor3f(0,1,0);
		glutWireSphere(b->x,10,10);
		glEnable(GL_LIGHTING);
		glEnable(GL_TEXTURE_2D);
		break;
	}
}