#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
//#include <regex> WHY YOU NO WORK!
#include <vector>
#include "ObjImporter.h"
#include "Vector3.h"
#include "QuadFace.h"

ObjImporter::ObjImporter(){

}

void ObjImporter::import(string file){
	vertices.resize(0);
	texCoords.resize(0);
	vertNormals.resize(0);
	quadFace.resize(0);
	string loadfilename = file;
	ifstream l;
	l.open(loadfilename.c_str());
	if (!l){
		printf("Could not find the file!\n");
	}
	else if (l.is_open()) {
		while (!l.eof()) {
			string out;
			string tmp;
			getline(l,out);
			int counter = 0;
			Vector3 *tmpVec = new Vector3();
			if (out.size() > 0){ //Not an empty line
				if (out.at(0) == 'v'){ //Starts with a v
					if (out.at(1) == ' '){ //Vertex
						for (size_t i = 2; i < out.size(); i++){ //Scan the line
							if (out.at(i) == ' ' || i == out.size()-1){ //Finishes parsing the floating point number
								if (counter == 0){
									//cout << tmp << endl;
									tmpVec->x = atof(tmp.c_str());
									tmp.resize(0);
									counter++;
								}
								else if (counter == 1){
									//cout << tmp << endl;
									tmpVec->y = atof(tmp.c_str());
									tmp.resize(0);
									counter++;
								}
								else{
									//cout << tmp << endl;
									tmp.push_back(out.at(i));
									tmpVec->z = atof(tmp.c_str());
									vertices.push_back(tmpVec);
									tmp.resize(0);
								}
							}
							else{
								tmp.push_back(out.at(i));
							}
						}
						//cout << endl;
					}
					else if (out.at(1) == 't'){ //Texture Coord (UV)
						for (size_t i = 3; i < out.size(); i++){ //Scan the line
							if (out.at(i) == ' ' || i == out.size()-1){ //Finishes parsing the floating point number
								if (counter == 0){
									//cout << tmp << endl;
									tmpVec->x = atof(tmp.c_str());
									tmp.resize(0);
									counter++;
								}
								else{
									//cout << tmp << endl;									
									tmp.push_back(out.at(i));
									tmpVec->y = atof(tmp.c_str());
									texCoords.push_back(tmpVec);
									tmp.resize(0);
								}
							}
							else{
								tmp.push_back(out.at(i));
							}
						}
						//cout << endl;
					}
					else if (out.at(1) == 'n'){ //Vertex Normal
						for (size_t i = 3; i < out.size(); i++){ //Scan the line
							if (out.at(i) == ' ' || i == out.size()-1){ //Finishes parsing the floating point number
								if (counter == 0){
									//cout << tmp << endl;
									tmpVec->x = atof(tmp.c_str());
									tmp.resize(0);
									counter++;
								}
								else if (counter == 1){
									//cout << tmp << endl;
									tmpVec->y = atof(tmp.c_str());
									tmp.resize(0);
									counter++;
								}
								else{
									//cout << tmp << endl;
									tmp.push_back(out.at(i));
									tmpVec->z = atof(tmp.c_str());
									vertNormals.push_back(tmpVec);
									tmp.resize(0);
								}
							}
							else{
								tmp.push_back(out.at(i));
							}
						}
						//cout << endl;
					}
				}
				else if(out.at(0) == 'f'){ //Face Data
					int fcounter = 0;
					QuadFace* tmpQF = new QuadFace();
					Vector3 *tmpVec2 = new Vector3();
					Vector3 *tmpVec3 = new Vector3();
					Vector3 *tmpVec4 = new Vector3();
					for (size_t i = 2; i < out.size(); i++){ //Scan the line
						if (out.at(i) == '/'){ //Scans a /
							if (fcounter == 0){
								if (counter == 0){
									//cout << tmp << '/';
									tmpVec->x = atoi(tmp.c_str()); // the integer number parsed
									tmp.resize(0);
									counter++;
								}
								else if (counter == 1){
									//cout << tmp << '/';
									tmpVec->y = atoi(tmp.c_str());
									tmp.resize(0);
									counter++;
								}
							}
							else if (fcounter == 1){
								if (counter == 0){
									//cout << tmp << '/';
									tmpVec2->x = atoi(tmp.c_str()); // the integer number parsed
									tmp.resize(0);
									counter++;
								}
								else if (counter == 1){
									//cout << tmp << '/';
									tmpVec2->y = atoi(tmp.c_str());
									tmp.resize(0);
									counter++;
								}
							}
							else if (fcounter == 2){
								if (counter == 0){
									//cout << tmp << '/';
									tmpVec3->x = atoi(tmp.c_str()); // the integer number parsed
									tmp.resize(0);
									counter++;
								}
								else if (counter == 1){
									//cout << tmp << '/';
									tmpVec3->y = atoi(tmp.c_str());
									tmp.resize(0);
									counter++;
								}
							}
							else if (fcounter == 3){
								if (counter == 0){
									//cout << tmp << '/';
									tmpVec4->x = atoi(tmp.c_str()); // the integer number parsed
									tmp.resize(0);
									counter++;
								}
								else if (counter == 1){
									//cout << tmp << '/';
									tmpVec4->y = atoi(tmp.c_str());
									tmp.resize(0);
									counter++;
								}
							}
						}
						else if (out.at(i) == ' ' || i == out.size()-1){ //end of section
							//cout << tmp << endl;
							if (i == out.size() - 1){
								tmp.push_back(out.at(i));
								if (fcounter == 2){
									tmpVec3->z = atoi(tmp.c_str());
									tmp.resize(0);
									counter = 0;
									tmpQF->setP2(tmpVec3);
									quadFace.push_back(tmpQF);
								}
								else if (fcounter == 3){
									tmpVec4->z = atoi(tmp.c_str());
									tmp.resize(0);
									counter = 0;
									tmpQF->setP3(tmpVec4);
									quadFace.push_back(tmpQF);
								}
							}
							else if (fcounter == 0){
								tmpVec->z = atoi(tmp.c_str());
								tmp.resize(0);
								counter = 0;
								tmpQF->setP0(tmpVec);
							}
							else if (fcounter == 1){
								tmpVec2->z = atoi(tmp.c_str());
								tmp.resize(0);
								counter = 0;
								tmpQF->setP1(tmpVec2);
							}
							else if (fcounter == 2){
								tmpVec3->z = atoi(tmp.c_str());
								tmp.resize(0);
								counter = 0;
								tmpQF->setP2(tmpVec3);
							}
							fcounter++;
						}
						else{
							tmp.push_back(out.at(i));
						}
					}
					//cout << endl;
				}
			}		
		}
	}
	l.close(); //End of Obj Parsing

	//Parse for texture
	loadfilename.resize(loadfilename.size()-3); //get rid of .obj
	loadfilename.push_back('m');
	loadfilename.push_back('t');
	loadfilename.push_back('l');

	cout << loadfilename << endl;

	l.open(loadfilename.c_str());
	if (l.is_open()){
		while(!l.eof()){
			string out;
			string tmp;
			getline(l,out);
			int counter = 0;
			Vector3 *tmpVec = new Vector3();
			if (out.size() > 0){
				if (out.at(0) == 'K'){
					if (out.at(1) == 'd'){
						for (size_t i = 3; i < out.size(); i++){
							if (out.at(i) == ' ' || i == out.size()-1){ //Finishes parsing the floating point number
								if (counter == 0){
									//cout << tmp << endl;
									tmpVec->x = atof(tmp.c_str());
									tmp.resize(0);
									counter++;
								}
								else if (counter == 1){
									//cout << tmp << endl;
									tmpVec->y = atof(tmp.c_str());
									tmp.resize(0);
									counter++;
								}
								else{
									//cout << tmp << endl;
									tmp.push_back(out.at(i));
									tmpVec->z = atof(tmp.c_str());
									//vertNormals.push_back(tmpVec);
									diffuse = tmpVec;
									tmp.resize(0);
								}
							}
							else{
								tmp.push_back(out.at(i));
							}	
						}
					}
					else if (out.at(1) == 'a'){
						for (size_t i = 3; i < out.size(); i++){
							if (out.at(i) == ' ' || i == out.size()-1){ //Finishes parsing the floating point number
								if (counter == 0){
									//cout << tmp << endl;
									tmpVec->x = atof(tmp.c_str());
									tmp.resize(0);
									counter++;
								}
								else if (counter == 1){
									//cout << tmp << endl;
									tmpVec->y = atof(tmp.c_str());
									tmp.resize(0);
									counter++;
								}
								else{
									//cout << tmp << endl;
									tmp.push_back(out.at(i));
									tmpVec->z = atof(tmp.c_str());
									//vertNormals.push_back(tmpVec);
									ambient = tmpVec;
									tmp.resize(0);
								}
							}
							else{
								tmp.push_back(out.at(i));
							}	
						}
					}
				}
				else if (out.at(0) == 'm'){
					if (out.at(1) == 'a' && out.at(2) == 'p'){
						for (size_t i = 7; i < out.size(); i++){
							cout << out.at(i);
							tmp.push_back(out.at(i));
						}
						tex = tmp;
						tmp.resize(0);
					}
				}	
			}
			
		}
	}
}

vector<Vector3*> ObjImporter::getVertices(){
	/*for (size_t i = 0; i < vertices.size(); i++){
		cout << vertices.at(i)->x << ' ';
		cout << vertices.at(i)->y << ' ';
		cout << vertices.at(i)->z << endl;
	}*/
	return vertices;
}

vector<Vector3*> ObjImporter::getVertexNormals(){
	/*for (size_t i = 0; i < vertNormals.size(); i++){
		cout << vertNormals.at(i)->x << ' ';
		cout << vertNormals.at(i)->y << ' ';
		cout << vertNormals.at(i)->z << endl;
	}*/
	return vertNormals;
}

vector<Vector3*> ObjImporter::getVertexTextures(){
	/*for (size_t i = 0; i < texCoords.size(); i++){
		cout << texCoords.at(i)->x << ' ';
		cout << texCoords.at(i)->y << endl;
	}*/
	return texCoords;
}

vector<QuadFace*> ObjImporter::getFaces(){
	/*for (size_t i = 0; i < quadFace.size(); i++){
		cout << quadFace.at(i)->p0->x << '/' << quadFace.at(i)->p0->y << '/' << quadFace.at(i)->p0->z << ' ';
		cout << quadFace.at(i)->p1->x << '/' << quadFace.at(i)->p1->y << '/' << quadFace.at(i)->p1->z << ' ';
		cout << quadFace.at(i)->p2->x << '/' << quadFace.at(i)->p2->y << '/' << quadFace.at(i)->p2->z << ' ';
		if (quadFace.at(i)->initializedPoints == 3){
			cout << endl;
		}
		else{
			cout << quadFace.at(i)->p3->x << '/' << quadFace.at(i)->p3->y << '/' << quadFace.at(i)->p3->z << ' ' << endl;
		}
		
	}*/
	return quadFace;
}

Vector3* ObjImporter::getDiffuse(){
	return diffuse;
}

Vector3* ObjImporter::getAmbient(){
	return diffuse;
}

int ObjImporter::loadTexture(){

}