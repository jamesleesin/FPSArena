#include "QuadFace.h"
#include "Vector3.h"

QuadFace::QuadFace(){
	initializedPoints = 0;
}

void QuadFace::setP0(Vector3* v){
	p0 = v;
	initializedPoints = 1;
}

void QuadFace::setP1(Vector3* v){
	p1 = v;
	initializedPoints = 2;
}

void QuadFace::setP2(Vector3* v){
	p2 = v;
	initializedPoints = 3;
}

void QuadFace::setP3(Vector3* v){
	p3 = v;
	initializedPoints = 4;
}
