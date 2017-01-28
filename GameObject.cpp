#include "GameObject.h"

#include <GL/glut.h>
#include <GL/gl.h>
#include <stdio.h>
#include <math.h>
using namespace std;
GameObject::GameObject(){
	position = new Vector3();
	rotation = new Vector3();
	bounds = new Bounds();
	scale = new Vector3();
	isActive = false;
	direction = new Vector3();
}

GameObject::GameObject(float x, float y, float z){
	position = new Vector3(x,y,z);
	rotation = new Vector3(0,0,0);
	velocity = new Vector3(0,0,0);
	direction = new Vector3(0,0,0);
	acceleration = new Vector3(0,0,0);
	bounds = new Bounds();
	bounce = 0;
	scale = new Vector3(1,1,1);
	isActive = false;
	isGrounded = false;
	isKinematic = false;
	diffuse = new Vector3(0.7,0.7,0.7);
	ambient = new Vector3(0.7,0.7,0.7);
	breathe = 0;
}

GameObject::GameObject(GameObject* g){
	position = new Vector3(0,0,0);
	rotation = new Vector3(0,0,0);
	velocity = new Vector3(0,0,0);
	direction = new Vector3(0,0,0);
	acceleration = new Vector3(0,0,0);
	bounds = new Bounds();
	bounce = 0;
	scale = new Vector3(1,1,1);
	isActive = true;
	isGrounded = false;
	isKinematic = false;

	diffuse = new Vector3(g->diffuse->x,g->diffuse->y,g->diffuse->z);
	ambient = new Vector3(g->ambient->x,g->ambient->y,g->diffuse->z);

	vertexList = g->vertexList;
	normalList = g->normalList;
	faceList = g->faceList;
	texList = g->texList;

	breathe = rand()%100;
}

void GameObject::draw(){
	glPushMatrix();
	glTranslatef(position->x, position->y, position->z);
	glRotatef(rotation->y,0,1,0);
	glScalef(scale->x,scale->y,scale->z);
	if (isActive){
		glBegin(GL_TRIANGLES);
			for (size_t i = 0; i < faceList.size(); i++){
				if (faceList.at(i)->initializedPoints == 4){
					//quad to tri
						glNormal3f(normalList.at(faceList.at(i)->p0->z-1)->x, normalList.at(faceList.at(i)->p0->z-1)->y, normalList.at(faceList.at(i)->p0->z-1)->z);
						glTexCoord2f(texList.at(faceList.at(i)->p0->y-1)->x, texList.at(faceList.at(i)->p0->y-1)->y);
						glVertex3f(vertexList.at(faceList.at(i)->p0->x-1)->x, vertexList.at(faceList.at(i)->p0->x-1)->y, vertexList.at(faceList.at(i)->p0->x-1)->z);

						glNormal3f(normalList.at(faceList.at(i)->p1->z-1)->x, normalList.at(faceList.at(i)->p1->z-1)->y, normalList.at(faceList.at(i)->p1->z-1)->z);
						glTexCoord2f(texList.at(faceList.at(i)->p1->y-1)->x, texList.at(faceList.at(i)->p1->y-1)->y);
						glVertex3f(vertexList.at(faceList.at(i)->p1->x-1)->x, vertexList.at(faceList.at(i)->p1->x-1)->y, vertexList.at(faceList.at(i)->p1->x-1)->z);

						glNormal3f(normalList.at(faceList.at(i)->p2->z-1)->x, normalList.at(faceList.at(i)->p2->z-1)->y, normalList.at(faceList.at(i)->p2->z-1)->z);
						glTexCoord2f(texList.at(faceList.at(i)->p2->y-1)->x, texList.at(faceList.at(i)->p2->y-1)->y);
						glVertex3f(vertexList.at(faceList.at(i)->p2->x-1)->x, vertexList.at(faceList.at(i)->p2->x-1)->y, vertexList.at(faceList.at(i)->p2->x-1)->z);

						glNormal3f(normalList.at(faceList.at(i)->p2->z-1)->x, normalList.at(faceList.at(i)->p2->z-1)->y, normalList.at(faceList.at(i)->p2->z-1)->z);
						glTexCoord2f(texList.at(faceList.at(i)->p2->y-1)->x, texList.at(faceList.at(i)->p2->y-1)->y);
						glVertex3f(vertexList.at(faceList.at(i)->p2->x-1)->x, vertexList.at(faceList.at(i)->p2->x-1)->y, vertexList.at(faceList.at(i)->p2->x-1)->z);

						glNormal3f(normalList.at(faceList.at(i)->p3->z-1)->x, normalList.at(faceList.at(i)->p3->z-1)->y, normalList.at(faceList.at(i)->p3->z-1)->z);
						glTexCoord2f(texList.at(faceList.at(i)->p3->y-1)->x, texList.at(faceList.at(i)->p3->y-1)->y);
						glVertex3f(vertexList.at(faceList.at(i)->p3->x-1)->x, vertexList.at(faceList.at(i)->p3->x-1)->y, vertexList.at(faceList.at(i)->p3->x-1)->z);

						glNormal3f(normalList.at(faceList.at(i)->p0->z-1)->x, normalList.at(faceList.at(i)->p0->z-1)->y, normalList.at(faceList.at(i)->p0->z-1)->z);
						glTexCoord2f(texList.at(faceList.at(i)->p0->y-1)->x, texList.at(faceList.at(i)->p0->y-1)->y);
						glVertex3f(vertexList.at(faceList.at(i)->p0->x-1)->x, vertexList.at(faceList.at(i)->p0->x-1)->y, vertexList.at(faceList.at(i)->p0->x-1)->z);
				}
				else{
						glNormal3f(normalList.at(faceList.at(i)->p0->z-1)->x, normalList.at(faceList.at(i)->p0->z-1)->y, normalList.at(faceList.at(i)->p0->z-1)->z);
						glTexCoord2f(texList.at(faceList.at(i)->p0->y-1)->x, texList.at(faceList.at(i)->p0->y-1)->y);
						glVertex3f(vertexList.at(faceList.at(i)->p0->x-1)->x, vertexList.at(faceList.at(i)->p0->x-1)->y, vertexList.at(faceList.at(i)->p0->x-1)->z);

						glNormal3f(normalList.at(faceList.at(i)->p1->z-1)->x, normalList.at(faceList.at(i)->p1->z-1)->y, normalList.at(faceList.at(i)->p1->z-1)->z);
						glTexCoord2f(texList.at(faceList.at(i)->p1->y-1)->x, texList.at(faceList.at(i)->p1->y-1)->y);
						glVertex3f(vertexList.at(faceList.at(i)->p1->x-1)->x, vertexList.at(faceList.at(i)->p1->x-1)->y, vertexList.at(faceList.at(i)->p1->x-1)->z);

						glNormal3f(normalList.at(faceList.at(i)->p2->z-1)->x, normalList.at(faceList.at(i)->p2->z-1)->y, normalList.at(faceList.at(i)->p2->z)->z);
						glTexCoord2f(texList.at(faceList.at(i)->p2->y-1)->x, texList.at(faceList.at(i)->p2->y-1)->y);
						glVertex3f(vertexList.at(faceList.at(i)->p2->x-1)->x, vertexList.at(faceList.at(i)->p2->x-1)->y, vertexList.at(faceList.at(i)->p2->x-1)->z);
				}
			}
		glEnd();
	}
	glPopMatrix();
}

void GameObject::addForce(Vector3* f){
	acceleration->x += f->x;
	acceleration->y += f->y;
	acceleration->z += f->z;
}

void GameObject::setVertexList(vector<Vector3*> vec){
	vertexList = vec;
}

void GameObject::setNormalList(vector<Vector3*> vec){
	normalList = vec;
}

void GameObject::setFaceList(vector<QuadFace*> vec){
	faceList = vec;
}

void GameObject::setTextureList(vector<Vector3*> vec){
	texList = vec;
}

void GameObject::setDiffuse(Vector3* vec){
	diffuse = vec;
}

void GameObject::setAmbient(Vector3* vec){
	ambient = vec;
}

void GameObject::setTexture(int id){
	textureIndex = id;
}

int GameObject::returnType(){
	return type;
}

void GameObject::setType(int t){
	type = t;
}

Vector3* GameObject::returnDirection(){
	return direction;
}

void GameObject::setDirection(Vector3* d){
	this->direction->x = d->x;
	this->direction->y = d->y;
	this->direction->z = d->z;
}

void GameObject::update(){
	float vx = velocity->x + acceleration->x * 0.016;
	float vy;
	if (isKinematic){
		vy= velocity->y + (acceleration->y - 9.81/2) * 0.016;
	}
	else{
		vy= velocity->y + acceleration->y * 0.016;
	}
	float vz = velocity->z + acceleration->z * 0.016;



	velocity->x = vx;
	velocity->y = vy;
	velocity->z = vz;

	position->x += velocity->x;
	position->y += velocity->y;
	position->z += velocity->z;	

	//velocity damping
	velocity->x /= 1.1;
	velocity->y /= 1.1;
	velocity->z /= 1.1;

	if (position->y < bounds->b->x){
		position->y = bounds->b->x;
		velocity->y = -velocity->y * bounce;
	}

	if (type == 3){
		breathe+=0.02;
		position->y += sin(breathe) * 0.1;
	}
}