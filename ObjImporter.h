#ifndef __OBJIMPORTER_H__
#define __OBJIMPORTER_H__

#include <vector>
#include <string>
#include "Vector3.h"
#include "QuadFace.h"
#include <GL/gl.h>
using namespace std;

class ObjImporter{
	public:
		ObjImporter();
		vector<Vector3*> getVertices();
		vector<Vector3*> getVertexNormals();
		vector<Vector3*> getVertexTextures();
		vector<QuadFace*> getFaces();
		Vector3* getDiffuse();
		Vector3* getAmbient();
		int loadTexture();
		void import(string file);

		vector<GLuint*> textures;

		vector<Vector3*> vertices;
		vector<Vector3*> texCoords;
		vector<Vector3*> vertNormals;
		vector<QuadFace*> quadFace;
		Vector3* diffuse;
		Vector3* ambient;
		string tex;
};
#endif