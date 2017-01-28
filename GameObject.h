#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <GLUT/gl.h>
#else
#include <GL/glut.h>
#include <GL/gl.h>
#endif

#include <GL/gl.h>
#include <vector>
#include "Vector3.h"
#include "Bounds.h"
#include "QuadFace.h"
using namespace std;

class GameObject
{
	public:
		//Constructors
		GameObject();
		GameObject(float x, float y, float z);
		GameObject(GameObject* g);

		//Functions
		void update();
		void draw();
		void addForce(Vector3* f);

		void setVertexList(vector<Vector3*> vec);
		void setNormalList(vector<Vector3*> vec);
		void setFaceList(vector<QuadFace*> vec);
		void setTextureList(vector<Vector3*> vec);
		void setDiffuse(Vector3* vec);
		void setAmbient(Vector3* vec);
		void setTexture(int tex);
		int returnType();
		void setType(int t);
		void setDirection(Vector3* d);
		Vector3* returnDirection();
		float returnX();

		//Variables
		//Obj Values
		vector<Vector3*> vertexList;
		vector<Vector3*> normalList;
		vector<QuadFace*> faceList;
		vector<Vector3*> texList;
		//Physics Values
		Vector3* acceleration;
		Vector3* velocity;

		//Rendering Transformations
		Vector3* position;
		Vector3* rotation;
		Vector3* scale;

		//Bouding Box
		Bounds* bounds;

		//Textures/Materials
		int textureIndex;
		//int boogie;
		Vector3* diffuse;
		Vector3* ambient;

		float bounce;
		bool isKinematic;
		bool isActive;
		bool isGrounded;
		int type;
		float breathe;
		Vector3* direction;
};
#endif