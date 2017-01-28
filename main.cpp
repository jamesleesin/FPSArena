#ifdef __APPLE__
#include <GLUT/glut.h>
#include <GLUT/gl.h>
#else
#include <GL/glut.h>
#include <GL/gl.h>
#endif

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <time.h>
#include <string>

#include "Vector3.h"
#include "QuadFace.h"
#include "GameObject.h"
#include "ObjImporter.h"

using namespace std;

////////////////////////////////////////////////////VARIABLES///////////////////////////////////////////////////////
int timeHour = 12; //peak time

ObjImporter *objImporter;
bool update = false;
bool debug = true;
static float gravity = 0.981f;
int winX = 800;
int winY = 600;
//////////////////////////PLAYER//////////////////////////////////
GameObject* player = new GameObject(1,10,1);
int w = 0;
int s = 0;
int a = 0;
int d = 0;
int aup = 0;
int aleft = 0;
int aright = 0;
int adown = 0;
// angle of rotation for the camera direction
float angle_x=0.0;
float angle_y=0;
float room = 200.0f;

// for mouse
int deltaX;
int deltaY;

// player stats
int maxHealth = 100;
int health = 100;
int maxExperience = 1000;
int experience = 0;
int score = 0;
float fogColor[4] = {0.3, 0.3, 0.3, 0.7};
float fogNear = 150;
float fogFar = 300;

int waveNumber = 0;
bool gameOver = false;
bool playerDead = false;
float deathAlpha = 0.0;

// game variables
vector<GameObject> enemies;
vector<GameObject> environment;
GLfloat mat_diffuse[] = {0.5,0.5,0.5,1.0};
GLfloat tmp_diffuse[] = {0.3,0.3,0.3,1.0};
GLfloat light_position[] = { -40, 30, 0, 0.0 };
GLfloat light_diffuse[] = {1,1,1,1};
GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
GLfloat light_diffuseTarget[] = {0.917,0.917,0.917};
vector<GameObject> enemies_template;
vector<GameObject> environment_template;
vector< vector<float> > attackList;
double start[] ={0,0,0}, end[]={1,1,1};

// textures
GLuint textures[5];
int width, height, maxImg;
GLubyte* groundTexture;
GLubyte* batTexture;
GLubyte* snailTexture;
GLubyte* rockTexture;
GLubyte* treeTexture;



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* LoadPPM -- loads the specified ppm file, and returns the image data as a GLubyte 
 *  (unsigned byte) array. Also returns the width and height of the image, and the
 *  maximum colour value by way of arguments
 */
GLubyte* LoadPPM(char const* file, int* width, int* height, int* maxImg)
{
	GLubyte* img;
	FILE *fd;
	int n, m;
	int  k, nm;
	char c;
	int i;
	char b[100];
	float s;
	int red, green, blue;
	
	/* first open file and check if it's an ASCII PPM (indicated by P3 at the start) */
	fd = fopen(file, "r");
	fscanf(fd,"%[^\n] ",b);
	if(b[0]!='P'|| b[1] != '3')
	{
		printf("%s is not a PPM file!\n",file); 
		exit(0);
	}
	fscanf(fd, "%c",&c);

	/* next, skip past the comments - any line starting with #*/
	while(c == '#') 
	{
		fscanf(fd, "%[^\n] ", b);
		fscanf(fd, "%c",&c);
	}
	ungetc(c,fd); 

	/* now get the dimensions and max colour value from the image */
	fscanf(fd, "%d %d %d", &n, &m, &k);

	//printf("%d rows  %d columns  max value= %d\n",n,m,k);

	/* calculate number of pixels and allocate storage for this */
	nm = n*m;
	img = (GLubyte*)malloc(3*sizeof(GLuint)*nm);
	s=255.0/k;
	//s = 1023.0/k;

	/* for every pixel, grab the read green and blue values, storing them in the image data array */
	for(i=0;i<nm;i++) 
	{
		fscanf(fd,"%d %d %d",&red, &green, &blue );
		img[3*nm-3*i-3]=red*s;
		img[3*nm-3*i-2]=green*s;
		img[3*nm-3*i-1]=blue*s;
	}
	/* finally, set the "return parameters" (width, height, max) and return the image array */
	*width = n;
	*height = m;
	*maxImg = k;
	
	return img;
}

// removes an enemy from the vector, and give player appropriate reward
void killEnemy(int enemy){
	if (enemies.size() > 0){
		enemies.erase(enemies.begin()+enemy);
		score += 50;
		experience += 50;
		if (experience >= maxExperience){
			experience = 0;
			maxExperience += 1000;
		}
	}
}

//function which preforms intersection test
bool Intersect(){
	int x = winX/2;
	int y = winY/2;
	for (size_t i = 0; i < enemies.size(); i++){
		GameObject *g = &enemies.at(i);

		glPushMatrix();
		glTranslatef(g->position->x, g->position->y, g->position->z);
		
		//allocate matricies memory
		double matModelView[16], matProjection[16]; 
		int viewport[4]; 

		//grab the matricies
		glGetDoublev(GL_MODELVIEW_MATRIX, matModelView); 
		glGetDoublev(GL_PROJECTION_MATRIX, matProjection); 
		glGetIntegerv(GL_VIEWPORT, viewport); 

		//unproject the values
		double winX = (double)x; 
		double winY = viewport[3] - (double)y; 

		// get point on the 'near' plane (third param is set to 0.0)
		gluUnProject(winX, winY, 0.0, matModelView, matProjection, 
	         viewport, &start[0], &start[1], &start[2]); 

		// get point on the 'far' plane (third param is set to 1.0)
		gluUnProject(winX, winY, 1.0, matModelView, matProjection, 
	         viewport, &end[0], &end[1], &end[2]); 

		// check for intersection against spherical body of enemy
		double A, B, C;

		double R0x, R0y, R0z;
		double Rdx, Rdy, Rdz;

		R0x = start[0];
		R0y = start[1];
		R0z = start[2];

		Rdx = end[0] - start[0];
		Rdy = end[1] - start[1];
		Rdz = end[2] - start[2];

		//magnitude!
		double M = sqrt(Rdx*Rdx + Rdy*Rdy + Rdz* Rdz);

		//unit vector!
		Rdx /= M;
		Rdy /= M;
		Rdz /= M;

		//A = Rd dot Rd
		A = Rdx*Rdx + Rdy*Rdy + Rdz*Rdz;
		double Btempx, Btempy, Btempz;
		int changeInY;
		
		// adjust for different heights of enemies
		if (g->returnType() == 3){
			changeInY = 2;
		}
		else{
			changeInY = 1;
		}

		Btempx = R0x;
		Btempz =  R0z;
		Btempy =  R0y-changeInY;
		B = Btempx * Rdx + Btempy * Rdy + Btempz *Rdz;
		B *= 2.0;
		C = R0x*R0x + (R0y-changeInY)*(R0y-changeInY) + R0z* R0z - (g->bounds->b->x/2 * g->bounds->b->x/2);

		double sq = B*B  - 4*A*C;

		double t0 = 0, t1 = 0;

		if(sq < 0){
		}else{
			// if intersect then kill enemy
			t0 = ((-1) * B + sqrt(sq))/(2*A);
			t1 = ((-1) * B - sqrt(sq))/(2*A);
			killEnemy(i);
			return true;
		}
		glPopMatrix();
	}
	return false; //else returns false
}

// initialize the game
void init(){

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
		
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	// set up player bounds
	player->rotation->z = -1;
	player->bounds = new Bounds(new Vector3(1,5,1),0);
	
	// set up the textures
	glEnable(GL_TEXTURE_2D);
	glGenTextures(5, textures);
	
	// load textures
	groundTexture = LoadPPM("data/grass.ppm", &width, &height, &maxImg);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	gluBuild2DMipmaps( GL_TEXTURE_2D, 4, width,   height, GL_RGB, GL_UNSIGNED_BYTE, groundTexture );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	rockTexture = LoadPPM("data/rock.ppm", &width, &height, &maxImg);
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	gluBuild2DMipmaps( GL_TEXTURE_2D, 4, width,   height, GL_RGB, GL_UNSIGNED_BYTE, rockTexture );
	
	treeTexture = LoadPPM("data/Tree.ppm", &width, &height, &maxImg);
	glBindTexture(GL_TEXTURE_2D, textures[2]);
	gluBuild2DMipmaps( GL_TEXTURE_2D, 4, width,   height, GL_RGB, GL_UNSIGNED_BYTE, treeTexture );
		
	batTexture = LoadPPM("data/Bat.ppm", &width, &height, &maxImg);
	glBindTexture(GL_TEXTURE_2D, textures[3]);
	gluBuild2DMipmaps( GL_TEXTURE_2D, 4, width,   height, GL_RGB, GL_UNSIGNED_BYTE, batTexture );
	
	snailTexture = LoadPPM("data/Snail.ppm", &width, &height, &maxImg);
	glBindTexture(GL_TEXTURE_2D, textures[4]);
	gluBuild2DMipmaps( GL_TEXTURE_2D, 4, width,   height, GL_RGB, GL_UNSIGNED_BYTE, snailTexture );
		
	// load the objects using the obj importer
	objImporter = new ObjImporter();

	GameObject* g = new GameObject();
	objImporter->import("data/Bat.obj");
	g->setVertexList(objImporter->getVertices());
	g->setFaceList(objImporter->getFaces());
	g->setNormalList(objImporter->getVertexNormals());
	g->setTextureList(objImporter->getVertexTextures());
	g->setDiffuse(objImporter->getDiffuse());
	g->setAmbient(objImporter->getAmbient());

	g->isActive = true;
	enemies_template.push_back(g);

	GameObject* g2 = new GameObject();
	objImporter->import("data/Snail.obj");
	g2->setVertexList(objImporter->getVertices());
	g2->setFaceList(objImporter->getFaces());
	g2->setNormalList(objImporter->getVertexNormals());
	g2->setTextureList(objImporter->getVertexTextures());
	g2->setDiffuse(objImporter->getDiffuse());
	g2->setAmbient(objImporter->getAmbient());

	g2->isActive = true;
	enemies_template.push_back(g2);

	GameObject* g3 = new GameObject();
	objImporter->import("data/Rock.obj");
	g3->setVertexList(objImporter->getVertices());
	g3->setFaceList(objImporter->getFaces());
	g3->setNormalList(objImporter->getVertexNormals());
	g3->setTextureList(objImporter->getVertexTextures());
	g3->setDiffuse(objImporter->getDiffuse());
	g3->setAmbient(objImporter->getAmbient());

	g3->isActive = true;
	environment_template.push_back(g3);
	
	GameObject* g4 = new GameObject();
	objImporter->import("data/Tree.obj");
	g4->setVertexList(objImporter->getVertices());
	g4->setFaceList(objImporter->getFaces());
	g4->setNormalList(objImporter->getVertexNormals());
	g4->setTextureList(objImporter->getVertexTextures());
	g4->setDiffuse(objImporter->getDiffuse());
	g4->setAmbient(objImporter->getAmbient());

	g4->isActive = true;
	environment_template.push_back(g4);

	// enable lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}

// spawn x enemies where x = num, and of a certain type 
// 3 = bat, 4 = snail
void spawnEnemies(int num, int type){
	for (int eNum= 0; eNum < num; eNum++){
		// randomize spawn location, but don't let it spawn too close to player
		int tx = rand()% (int)(room*2-40) - room+20;
		int tz = rand()% (int)(room*2-40) - room+20;
		while(tx > player->position->x -20 && tx < player->position->x + 20)
			tx = rand()% (int)(room*2-40) - room+20;
		while(tz > player->position->z -20 && tz < player->position->z + 20)
			tz = rand()% (int)(room*2-40) - room+20;
		
		// randomize a starting direction for the enemy
		float dx, dz;
		if (tx < player->position->x)
			dx = rand()% 100 / 100.0;
		else
			dx = -1*(rand()% 100 / 100.0);
		
		if (tz < player->position->z)
			dz = rand()% 100 / 100.0;
		else
			dz = -1*(rand()% 100 / 100.0);
		
		// add the enemy to the enemies vector
		if (type == 3){
			enemies.push_back(new GameObject(enemies_template.at(0)));
			enemies.at(enemies.size()-1).setType(3);
		}
		else if (type == 4){
			enemies.push_back(new GameObject(enemies_template.at(1)));
			enemies.at(enemies.size()-1).setType(4);
		}
		
		// adjust the height values
		enemies.at(enemies.size()-1).position->x = tx;
		if (enemies.at(enemies.size()-1).type == 3){
			enemies.at(enemies.size()-1).position->y = 5;
		}
		else if (enemies.at(enemies.size()-1).type == 4){
			enemies.at(enemies.size()-1).position->y = 0;
		}
		enemies.at(enemies.size()-1).position->z = tz;
		enemies.at(enemies.size()-1).setDirection(new Vector3(dx, 0, dz));
	}
}

// push an attack onto the array list
void enemyAttack(int i){
	vector<float> att;
	att.push_back(enemies.at(i).position->x);
	att.push_back(enemies.at(i).position->y + 1);
	att.push_back(enemies.at(i).position->z);
	float xVec = player->position->x - enemies.at(i).position->x;
	float zVec = player->position->z - enemies.at(i).position->z;
	// normalize
	float vLen = sqrt(xVec*xVec + zVec*zVec);
	att.push_back(xVec/vLen);
	att.push_back(0.0);
	att.push_back(zVec/vLen);
	// bats do 1 damage, snails do 2
	if (enemies[i].returnType() == 3){
		att.push_back(1.0);
	}
	else if (enemies[i].returnType() == 4){
		att.push_back(2.0);
	}
	attackList.push_back(att);
}

// start the game
void startGame(){
	// add trees and rocks
	for (int i = 0; i < 100; i++){
		// randomize location of the trees and rocks, but
		// dont let them spawn near the player
		int rx = rand()% (int)room*2 - room;
		int rz = rand()% (int)room*2 - room;
		while(rx > player->position->x -20 && rx < player->position->x + 20)
			rx = rand()% (int)(room*2-40) - room+20;
		while(rz > player->position->z -20 && rz < player->position->z + 20)
			rz = rand()% (int)(room*2-40) - room+20;
		
		// scale the trees and rocks randomly
		float scale;
		if (i < 50){
			scale = (float)rand()/(float)(RAND_MAX) * 1.8 + 1; 	
			environment.push_back(new GameObject(environment_template.at(0)));
			environment.at(environment.size()-1).setType(1);
		}
		else if (i >= 50){
			scale = (float)rand()/(float)(RAND_MAX) * 2 + 1; 	
			environment.push_back(new GameObject(environment_template.at(1)));
			environment.at(environment.size()-1).setType(2);
		}
		
		environment.at(environment.size()-1).position->x = rx;
		environment.at(environment.size()-1).position->y = 0;
		environment.at(environment.size()-1).position->z = rz;
		environment.at(environment.size()-1).scale->x = scale;
		environment.at(environment.size()-1).scale->y = scale;
		environment.at(environment.size()-1).scale->z = scale;
	}
}

// update positions of the enemy attacks
void moveEnemyAttacks(){
	bool deleteAttack;
	for (int i = 0; i < attackList.size(); i++){
		deleteAttack = false;
		attackList[i][0] += attackList[i][3];
		attackList[i][1] += attackList[i][4];
		attackList[i][2] += attackList[i][5];

		// remove it if it collides with a tree or rock
		for (int e = 0; e < environment.size(); e++){
			// rock 
			if (environment.at(e).returnType() == 1){
				if (attackList[i][0] > environment.at(e).position->x - environment.at(e).scale->x - 2 && attackList[i][0] < environment.at(e).position->x +environment.at(e).scale->x + 2 && attackList[i][2] > environment.at(e).position->z - environment.at(e).scale->z - 2 && attackList[i][2] < environment.at(e).position->z + environment.at(e).scale->z + 2 && attackList[i][1] < 5 + environment.at(e).scale->y){
					deleteAttack = true;
				}
			}
			// trees
			if (environment.at(e).returnType() == 2){
				if (attackList[i][0] > environment.at(e).position->x - environment.at(e).scale->x - 1.5 && attackList[i][0] < environment.at(e).position->x + environment.at(e).scale->x + 1.5 && 
				attackList[i][2] > environment.at(e).position->z - environment.at(e).scale->z -1.5 && attackList[i][2] < environment.at(e).position->z + environment.at(e).scale->z + 1.5 && attackList[i][1] < 15 + environment.at(e).scale->y){
					deleteAttack = true;
				}
			}
		}
		
		// also do attack with player collision here
		if (attackList[i][0] > player->position->x - 2 && attackList[i][0] < player->position->x + 2 && attackList[i][2] > player->position->z -2 && attackList[i][2] < player->position->z + 2 && attackList[i][1] < 20){
			// remove the attack and lower health
			health -= attackList[i][6];
			deleteAttack = true;
		}
		
		// remove it if it exits the playing area
		if (attackList[i][0] > room || attackList[i][0] < -room || attackList[i][2] > room || attackList[i][2] < -room){
			deleteAttack = true;
		}
		
		if (deleteAttack)
			attackList.erase(attackList.begin() + i);
	}
}

// draw the enemy attacks using spheres
void renderEnemyAttacks(){
	glDisable(GL_TEXTURE_2D);
	for (int i = 0; i < attackList.size(); i++){
		glPushMatrix();
		glTranslatef(attackList[i][0], attackList[i][1], attackList[i][2]);
		glutSolidSphere(0.3, 5, 5);
		glPopMatrix();
	}
	glEnable(GL_TEXTURE_2D);
}

// the AI for the enemies
void enemyAI(){
	float x = player->position->x;
	float y = player->position->y;
	float z = player->position->z;
	
	// for each enemy...
	for (int i = 0; i < enemies.size(); i ++){
		// move the enemy based on current direction
		if (enemies[i].returnType() == 3){	
			enemies[i].position->x += enemies[i].direction->x /2;
			enemies[i].position->z += enemies[i].direction->z /2;	
		}
		else if (enemies[i].returnType() == 4){		
			enemies[i].position->x += enemies[i].direction->x /4;
			enemies[i].position->z += enemies[i].direction->z /4;	
		}
		
		// adjust the direction based on where the player is, with a randomized value
		// in the algorithm so they move differently from other enemies
		float adjustmentFactor = rand()%400 + 600;
		
		if (enemies[i].position->x <= player->position->x)
			enemies[i].direction->x += (player->position->x - enemies[i].position->x) /adjustmentFactor;
		else if (enemies[i].position->x > player->position->x)
			enemies[i].direction->x -= (enemies[i].position->x - player->position->x) /adjustmentFactor;
		
		if (enemies[i].position->z <= player->position->z)
			enemies[i].direction->z += (player->position->z - enemies[i].position->z) /adjustmentFactor;
		else if (enemies[i].position->z > player->position->z)
			enemies[i].direction->z -= (enemies[i].position->z - player->position->z) /adjustmentFactor;

		float dx = player->position->x - enemies[i].position->x;
		float dz = player->position->z - enemies[i].position->z;
		float m = sqrt(dx*dx + dz*dz);
		dx /= m;
		dz /= m;
		float roty;
	
		roty = (float) atan2(dx, dz) * 180 / 3.14;
		enemies[i].rotation->y = roty;

		// normalize the vector
		float len = sqrt(enemies[i].direction->x *enemies[i].direction->x + enemies[i].direction->z*enemies[i].direction->z); 
		enemies[i].direction->x /= len;
		enemies[i].direction->z /= len;
		
		// bounce off the walls
		if (enemies[i].position->x <= -room + 10){
			enemies[i].direction->x *= -1;
		}
		else if (enemies[i].position->x >= room-10 ){
			enemies[i].direction->x *= -1;
		}
		if (enemies[i].position->z <= -room +10 ){
			enemies[i].direction->z *= -1;
		}
		else if (enemies[i].position->z >= room-10){
			enemies[i].direction->z *= -1;	
		}
		
		// decide whether to attack or not
		int aRand = rand()%200;
		// bats attack faster than snails
		if (enemies[i].returnType() == 3){
			if (aRand < 2){
				enemyAttack(i);
			}
		}
		else if (enemies[i].returnType() == 4){
			if (aRand == 0){
				enemyAttack(i);
			}
		}
	}

	// ground the enemies
	for (size_t i = 0; i < enemies.size(); i++){
		enemies.at(i).update();
		if (enemies.at(i).isGrounded){
			Vector3* v = new Vector3(0,200,0);
			enemies.at(i).addForce(v);
			delete v;
		}
	}
}

//Camera Control
// move player based on what angle camera is at
void movementListener(){

	float x = 0;
	float y = 0;
	float z = 0;

	// set it so that the angle is always between -2pi rad and 2pi rad
	if (angle_x > 6.28319 || angle_x < -6.28319){
  		angle_x = 0;
 	}
  
	float fraction;
	fraction = 4;
	 
	if ((angle_x >= 0 && angle_x <= 3.14159/2) || (angle_x > 3.14159 && angle_x <= 3.14159 + 3.14159/2) || (angle_x < 0 && angle_x >= -3.14159/2) || (angle_x < -3.14159 && angle_x >= -3.14159 - 3.14159/2)){
		if (w){
	   		z -= fraction * cos(angle_x + 3.14/2);
	   		x += fraction*sin(angle_x + 3.14/2);
		}
	  	if (s){
	   		z += fraction * cos(angle_x + 3.14/2);
	   		x -= fraction*sin(angle_x + 3.14/2);
	  	}
	  	if (a){
	   		x -= fraction * cos(angle_x+ 3.14/2);
	   		z -= fraction * sin(angle_x + 3.14/2);
	  	}
	  	if (d){
	   		x += fraction * cos(angle_x + 3.14/2);
	   		z += fraction * sin(angle_x + 3.14/2);
	  	}
	}
	else if ((angle_x > 3.14159/2 && angle_x <= 3.14159)|| (angle_x > 3.14159 + 3.14159/2 && angle_x < 6.28319)||(angle_x < -3.14159/2 && angle_x > -3.14159) || (angle_x < -3.14159 - 3.14159/2 && angle_x > -6.28319)) {
		if (w){
	   		z -= fraction * cos(angle_x + 3.14/2);
	   		x += fraction*sin(angle_x + 3.14/2);
	  	}
	  	if (s){
	   		z+= fraction * cos(angle_x + 3.14/2);
	   		x -= fraction*sin(angle_x + 3.14/2);
	  	}
	  	if (a){
	   		z -= fraction * cos(angle_x);
	   		x += fraction * sin(angle_x);
	  	}
	  	if (d){
	   		z += fraction * cos(angle_x);
	   		x -= fraction * sin(angle_x);
	  	}
	}

	player->acceleration->x = x;
	player->acceleration->y = y;
	player->acceleration->z = z;
	
	// make sure player stays within room bounds
	if (player->position->x - 5 < -room)
		player->position->x = -room+5;
	else if (player->position->x + 5 > room)
		player->position->x = room-5;
	if (player->position->z - 5 < -room)
		player->position->z = -room+5;
	else if (player->position->z + 5 > room)
		player->position->z = room - 5;

}

// rotate the screen based on how the mouse moved
void rotateScreen(){
	angle_x -= (float)(deltaX * 0.001);
	angle_y += (float)(deltaY * 0.001);
	 
	float lx = sin(angle_x + (3.14)/2) * 0.001;
	float lz = -cos(angle_x+ (3.14)/2) * 0.001;
	float ly = angle_y * 0.001;
	if (deltaX != 0 || deltaY != 0)
		glutWarpPointer(winX/2, winY/2);
	 
	player->rotation->x = lx;
	player->rotation->y = ly;
	player->rotation->z = lz;

}

//Drawing the room
void renderScene(void) {
	// Draw ground
	glColor3f(0.9f, 0.9f, 0.9f);
	glTexImage2D(GL_TEXTURE_2D, 9,GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, groundTexture);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	
	glBegin(GL_QUADS);	
		glNormal3f(0,1,0);

		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
		glTexCoord2f(0, 0);
		glVertex3f(-room, 0.0f, -room);
		glTexCoord2f(100, 0);
		glVertex3f(-room, 0.0f,  room);
		glTexCoord2f(100, 100);
		glVertex3f( room, 0.0f,  room);
		glTexCoord2f(0,100);
		glVertex3f( room, 0.0f, -room);
	glEnd();
	
	// draw walls 
	glTexImage2D(GL_TEXTURE_2D, 9,GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, rockTexture);
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	glBegin(GL_QUADS);
		
		tmp_diffuse[0] = mat_diffuse[0] - 0.1;
		tmp_diffuse[1] = mat_diffuse[1] - 0.1;
		tmp_diffuse[2] = mat_diffuse[2] - 0.1;

		glMaterialfv(GL_FRONT, GL_DIFFUSE, tmp_diffuse);
		
		glTexCoord2f(0, 0);
		glVertex3f(-room, 0, -room);
		glTexCoord2f(0, 20);
		glVertex3f(-room, room*2, -room);
		glTexCoord2f(20, 20);
		glVertex3f(-room, room*2, room);
		glTexCoord2f(20, 0);
		glVertex3f(-room, 0, room);
		
		tmp_diffuse[0] -= 0.1;
		tmp_diffuse[1] -= 0.1;
		tmp_diffuse[2] -= 0.1;

		glMaterialfv(GL_FRONT, GL_DIFFUSE, tmp_diffuse);
		
		glTexCoord2f(0, 0);
		glVertex3f(-room, 0, room);
		glTexCoord2f(0, 20);
		glVertex3f(-room, room*2, room);
		glTexCoord2f(20, 20);
		glVertex3f(room, room*2, room);
		glTexCoord2f(20, 0);
		glVertex3f(room, 0, room);
		
		tmp_diffuse[0] += 0.1;
		tmp_diffuse[1] += 0.1;
		tmp_diffuse[2] += 0.1;

		glMaterialfv(GL_FRONT, GL_DIFFUSE, tmp_diffuse);
		
		glTexCoord2f(0, 0);
		glVertex3f(room, 0, room);
		glTexCoord2f(0, 20);
		glVertex3f(room, room*2, room);
		glTexCoord2f(20, 20);
		glVertex3f(room, room*2, -room);
		glTexCoord2f(20, 0);
		glVertex3f(room, 0, -room);
		
		tmp_diffuse[0] += 0.2;
		tmp_diffuse[1] += 0.2;
		tmp_diffuse[2] += 0.2;

		glMaterialfv(GL_FRONT, GL_DIFFUSE, tmp_diffuse);
		
		glTexCoord2f(0, 0);
		glVertex3f(room, 0, -room);
		glTexCoord2f(0, 20);
		glVertex3f(room, room*2, -room);
		glTexCoord2f(20, 20);
		glVertex3f(-room, room*2, -room);
		glTexCoord2f(20, 0);
		glVertex3f(-room, 0, -room);	
	glEnd();
	
	for (int i = 0; i < environment.size(); i++){
		// bind tree texture (only need to bind tree since rock takes the texture of the wall)
		if (environment.at(i).returnType() == 2){
			glTexImage2D(GL_TEXTURE_2D, 9,GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, treeTexture);
			glBindTexture(GL_TEXTURE_2D, textures[2]);
		}
		environment.at(i).draw();
	}
}

// draw the enemies
void renderEnemies(){
	for (size_t i = 0; i < enemies.size(); i++){
		// bind bat texture
		if (enemies.at(i).returnType() == 3){
			glTexImage2D(GL_TEXTURE_2D, 9,GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, batTexture);
			glBindTexture(GL_TEXTURE_2D, textures[3]);
		}
		// bind snail texture
		else if (enemies.at(i).returnType() == 4){
			glTexImage2D(GL_TEXTURE_2D, 9,GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, snailTexture);
			glBindTexture(GL_TEXTURE_2D, textures[4]);
		}
		enemies.at(i).draw();
	}
}

// check for collision between objects
void checkCollision(){
	// when 2 enemies get too close, adjust their directions so they move away from each other
	if (enemies.size() > 1)
		for (size_t i = 0; i < enemies.size(); i++){
			for (size_t j = i+1; j < enemies.size(); j++){
				if (j != i){
					Vector3* a = enemies.at(i).position;
					Vector3* b = enemies.at(j).position;
					float dist = sqrt((a->x - b->x)*(a->x - b->x) + (a->y - b->y)*(a->y - b->y) + (a->y - b->y)*(a->y - b->y));
					a = enemies.at(i).bounds->b;
					b = enemies.at(j).bounds->b;
					float minDist = a->x + b->x; + 20; //radius + distance where they start slowing down
					if (minDist > dist){ //two objects are too close 
						if(enemies[i].direction->x <= 0)
							enemies[i].direction->x += 0.002;
						else if (enemies[i].direction->x > 0)
							enemies[i].direction->x -= 0.002;
						if(enemies[i].direction->z <= 0)
							enemies[i].direction->z += 0.002;
						else if (enemies[i].direction->z > 0)
							enemies[i].direction->z -= 0.002;
					}
				}
			}
		}
	
	// don't let the player walk into trees or rocks 
	Vector3* pp = player->position;
	Vector3* pb = player->bounds->b;
	for (size_t p = 0; p < environment.size(); p++){
		Vector3* b = environment.at(p).position;
		float dist = sqrt((pp->x - b->x)*(pp->x - b->x) + (pp->z - b->z)*(pp->z - b->z));
		Vector3* bb = environment.at(p).bounds->b;
		float minDist = pb->x + bb->x + environment.at(p).scale->x;
		if (minDist > dist){ //Too close

			player->position->x = pp->x - player->velocity->x *1.1 ;
			player->position->z = pp->z - player->velocity->z*1.1 ;

			//direction from player to that object
			float xdir = b->x - pp->x;
			float zdir = b->z - pp->z;
			float m = sqrt((xdir*xdir) + (zdir*zdir));

			//unit vector of normal
			xdir /= m;
			zdir /= m;
			player->velocity->x = -player->velocity->x * 0.5;
			player->velocity->z = -player->velocity->z * 0.5;
		}
	}
}

// draws the HUD for the player
void drawOverlay(void){
	// Use the Projection Matrix
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0, 800, 600, 0.0, -1.0, 10.0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glDisable(GL_CULL_FACE);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glDisable(GL_DEPTH_TEST);
	
	// player is dead if health <= 0
	if(health <= 0){
		health = 0;
		playerDead = true;
	}

	// add a shade of red to the screen based on how low player hp is
	if (!playerDead){
		if (health < 50)
			deathAlpha = 0.002*(50-health);
	}
	//if game over, slowly fill the screen with red
	else if (playerDead){
		if (deathAlpha<0.6)
			deathAlpha += 0.01;
		else{
			gameOver = true;
		}
	}
	
	glColor4f(1.0, 0, 0, deathAlpha);
	glBegin(GL_POLYGON);
	glVertex2f(0, 0);
	glVertex2f(0, winY);
	glVertex2f(winX, winY);
	glVertex2f(winX, 0);
	glEnd();
	
	// draw basic background squares for HUD
	glColor3f(0.7, 0.7, 0.7);
	glBegin(GL_POLYGON);
	glVertex2f(20, 20);
	glVertex2f(20, 80);
	glVertex2f(230, 80);
	glVertex2f(230, 20);
	glEnd();
	
	glBegin(GL_POLYGON);
	glVertex2f(780, 20);
	glVertex2f(780, 70);
	glVertex2f(730, 70);
	glVertex2f(730, 20);
	glEnd();
	
	glColor3f(1, 0, 0);

	glLineWidth(1);
	// draw targeting reticle
	glBegin(GL_LINE_LOOP);
    for(int i = 0; i <10; i++)
    {
        float theta = 2.0f * 3.1415926f * float(i) / float(10);//get the current angle
        float x = 20 * cosf(theta);//calculate the x component
        float y = 20 * sinf(theta);//calculate the y component
        glVertex2f(x + 400, y + 300);//output vertex
    }
    glEnd();

	glBegin(GL_LINES);
	glVertex2f(400, 270);
	glVertex2f(400, 330);
	glEnd();
	
	glBegin(GL_LINES);
	glVertex2f(370, 300);
	glVertex2f(430, 300);
	glEnd();
	
	glColor3f(0, 1, 0);
	float drawnHp = health*2 + 25;
	glLineWidth(10);
	glBegin(GL_LINES);
	glVertex2f(25, 40);
	glVertex2f(drawnHp, 40);
	glEnd();
	
	glColor3f(1, 1, 0);
	float drawnExp = 200*((float)experience/maxExperience) + 25;
	glBegin(GL_LINES);
	glVertex2f(25, 60);
	glVertex2f(drawnExp, 60);
	glEnd();
	
	glLineWidth(3);
	
	// draw a sun or a moon depending on time hour
	// daytime, draw a sun
	if (timeHour  > 5 && timeHour < 19)
		glColor3f(1, 1, 0);
	// nighttime, draw a moon
	else 
		glColor3f(0.9, 0.9, 0.9);
	
	glBegin(GL_POLYGON);
	glVertex2f(755, 30);
	glVertex2f(750, 32.5);
	glVertex2f(745, 35);
	glVertex2f(742.5, 40);
	glVertex2f(740, 45);
	glVertex2f(742.5, 50);
	glVertex2f(745, 55);
	glVertex2f(750, 57.5);
	glVertex2f(755, 60);
	glVertex2f(760, 57.5);
	glVertex2f(765, 55);
	glVertex2f(767.5, 50);
	glVertex2f(770, 45);
	glVertex2f(767.5, 40);
	glVertex2f(765, 35);
	glVertex2f(760, 32.5);
	glEnd();
	
	// show score at bottom of screen
	glColor3f(1, 1, 0);
	
	// show wave number
	char waveDisplay1 [5];
	strcpy(waveDisplay1, "Wave ");
	char waveDisplay2 [3];
	itoa(waveNumber, waveDisplay2, 10);
	glPushMatrix();
	glTranslatef(30, winY-30, 0);
	glScalef(0.15, -0.15, 1);
	for (int i = 0; i < strlen(waveDisplay1); i++)
		glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, waveDisplay1[i]);
	for (int i = 0; i < strlen(waveDisplay2); i++)
		glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, waveDisplay2[i]);
	glPopMatrix();
	
	// show score 
	glPushMatrix();
	glTranslatef(winX - 100, winY - 30, 0);
	glScalef(0.15, -0.15, 1);
	char scoreString [5];
	itoa(score, scoreString, 10);
	for (int i = 0; i < strlen(scoreString); i++)
		glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, scoreString[i]);
	glPopMatrix();
	
	// Get Back to the Modelview
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

// when game is over display a black screen displaying the score
void displayEndScore(){
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0, 800, 600, 0.0, -1.0, 10.0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glDisable(GL_DEPTH_TEST);
	
	// display "Game Over" as well as player end score
	glColor3f(1, 0, 0);
	glLoadIdentity();
	char gameOverString [9];
	strcpy(gameOverString, "Game Over");
	
	glPushMatrix();
	glTranslatef(winX/2-100, 300, 0);
	glScalef(0.2, -0.2, 0);
	for (int i = 0; i < strlen(gameOverString); i++)
		glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, gameOverString[i]);
	
	char scoreLabelString [7];
	strcpy(scoreLabelString, "Score: ");
	glTranslatef(-1100, -200, 0);
	for (int i = 0; i < strlen(scoreLabelString); i++)
		glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, scoreLabelString[i]);
	
	char scoreString [5];
	itoa(score, scoreString, 10);
	
	glTranslatef(80, 0, 0);
	for (int i = 0; i < strlen(scoreString); i++)
		glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, scoreString[i]);
	
	glPopMatrix();
}

// runs the calculations of the game "behind the scenes" at a rate 
// of 40 times per second to keep the gamespeed similar across various 
// computers
void runGameCalculations(int n){
	enemyAI();
	checkCollision();
	moveEnemyAttacks();
	rotateScreen();
	movementListener();
	player->update();
	
	float speed = 0.001;
	
	if (timeHour > 23){
		timeHour = 0;
		light_diffuseTarget[0] = light_diffuseTarget[0] + 0.14;
	}
	if (timeHour > 12){ // adjust brightness and fog
		if (light_diffuse[0] > light_diffuseTarget[0]){
			light_diffuse[0] = light_diffuse[0] - speed;
			light_diffuse[1] = light_diffuse[1] - speed;
			light_diffuse[2] = light_diffuse[2] - speed;
			fogNear -= 0.18;
			fogFar -= 0.18;
		}
		else{
			timeHour++;
			light_diffuseTarget[0] = light_diffuseTarget[0] - 0.070;
			// but make fog color lighter (moonlight)
			fogColor[0] = 0.3 + timeHour/80;
			fogColor[1] = 0.3 + timeHour/80;
			fogColor[2] = 0.3 + timeHour/80;
		}
		
		glutTimerFunc(25,runGameCalculations,0);
	}
	else if (timeHour < 12){ //adjust brightness and fog
		if (light_diffuse[0] < light_diffuseTarget[0]){
			light_diffuse[0] = light_diffuse[0] + speed;
			light_diffuse[1] = light_diffuse[1] + speed;
			light_diffuse[2] = light_diffuse[2] + speed;
			fogNear += 0.18;
			fogFar += 0.18;
		}
		else{
			timeHour++;

			light_diffuseTarget[0] = light_diffuseTarget[0] + 0.070;
			fogColor[0] = 0.3;
			fogColor[1] = 0.3;
			fogColor[2] = 0.3;
		}

		glutTimerFunc(25,runGameCalculations,0);
	}
	else{
		if (n > 10){
			timeHour++;
			light_diffuseTarget[0] = light_diffuseTarget[0] - 0.140;
			glutTimerFunc(25,runGameCalculations,0);
		}
		else{
			glutTimerFunc(25,runGameCalculations, n+1);
		}
	}
}

// the display method, displays all the visible parts of the game
void display(void){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	
	// if gmae is not over ...
	if (!gameOver){
		float x = player->position->x;
		float y = player->position->y;
		float z = player->position->z;
		float lx = player->rotation->x;
		float ly = player->rotation->y;
		float lz = player->rotation->z;

		gluLookAt(	x, y, z,
			x+lx,y+ly,z+lz,
			0,1.0,0);
		
		// light
		glLightfv(GL_LIGHT0, GL_POSITION, light_position);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
		glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
		glLightfv(GL_LIGHT0, GL_AMBIENT, light_diffuse);

		renderScene();
		renderEnemies();
		renderEnemyAttacks();

		// fog
		glFogi(GL_FOG_MODE, GL_LINEAR);
		glHint(GL_FOG_HINT, GL_NICEST);
		glFogfv(GL_FOG_COLOR, fogColor);
		glFogf(GL_FOG_START, fogNear);
		glFogf(GL_FOG_END, fogFar);
		glEnable(GL_FOG);
		
		drawOverlay();
	}
	// if game is over show end screen
	else if (gameOver){
		displayEndScore();
	}
		
	glutSwapBuffers();
	glutPostRedisplay();
}

// keyboard control
void onKeyboardUp(unsigned char key, int xx, int yy){
	switch(key){
	case 'w':
		w = 0;
		break;
	case 's':
		s = 0;
		break;
	case 'a':
		a = 0;
		break;
	case 'd':
		d = 0;
		break;
	}
}

// keyboard control
void onKeyboard(unsigned char key, int xx, int yy){
	switch(key){
	case 27:
		exit(0);
		break;
	case 'w':
		w = 1;
		break;
	case 's':
		s = -1;
		break;
	case 'a':
		a = -1;
		break;
	case 'd':
		d = 1;
		break;
	}
}

// keyboard control
void onSpecKeyboardUp(int key, int xx, int yy){
	switch(key){
		case GLUT_KEY_UP:
			aup = 0;
			break;
		case GLUT_KEY_DOWN:
			adown = 0;
			break;
		case GLUT_KEY_LEFT:
			aleft = 0;
			break;
		case GLUT_KEY_RIGHT:
			aright = 0;
			break;
	}
}

// keyboard control
void onSpecKeyboard(int key, int xx, int yy){
	switch(key){
		case GLUT_KEY_UP:
			aup = 1;
			break;
		case GLUT_KEY_DOWN:
			adown = 1;
			break;
		case GLUT_KEY_LEFT:
			aleft = 1;
			break;
		case GLUT_KEY_RIGHT:
			aright = 1;
			break;
	}
}

//resize function
//Based on lighthouse tutorial
void changeSize(int w, int h) {
	winX = w;
	winY = h;
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if (h == 0)
		h = 1;

	float ratio =  w * 1.0 / h;

	// Use the Projection Matrix
	glMatrixMode(GL_PROJECTION);
	// Reset Matrix
	glLoadIdentity();
	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);
	// Set the correct perspective.
	gluPerspective(45,ratio,1,1000);
	// Get Back to the Modelview
	glMatrixMode(GL_MODELVIEW);
}
void limitUpdate(int n){
	
}

// spawn the next wave of enemies
void nextWave(int n){
	// if number of enemies on screen is less than 40 then
	// spawn next wave, or else wait and check again every second
	// this is in order to stop too many enemies from being
	// on the screen at once.
	if (enemies.size() <= 40){
		waveNumber++;
		
		if (waveNumber == 1){
			spawnEnemies(3, 3);
		}
		else if (waveNumber == 2){
			spawnEnemies(5, 3);
		}
		else if (waveNumber == 3){
			spawnEnemies(3, 4);
		}	
		else if (waveNumber == 4){
			spawnEnemies(5, 3);
			spawnEnemies(3, 4);
		}
		else if (waveNumber == 5){
			spawnEnemies(7, 3);
			spawnEnemies(5, 4);
		}	
		else if (waveNumber > 5){
			spawnEnemies(10, 3);
			spawnEnemies(5, 4);
		}	
		glutTimerFunc(10000, nextWave, 0);
	}
	else{
		glutTimerFunc(1000, nextWave, 0);
	}
}

// mouse motion, calculate change for screen rotation
void motion(int x, int y){
	deltaX = winX/2-x;
	deltaY = winY/2-y;
}

// on mouse click, calculate intersect
void mouse(int button, int state, int x, int y){
	if(button ==  GLUT_LEFT_BUTTON && state == GLUT_DOWN){
		Intersect();
	}
}

int main(int argc, char **argv) {
	// init GLUT and create window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	
	glutInitWindowSize(winX,winY);
	glutInitWindowPosition(50, 50);
	glutCreateWindow("Arena Game");
	srand(time(NULL));

	init();
	startGame();
	// register callbacks
	glutDisplayFunc(display);
	glutReshapeFunc(changeSize);
	glutTimerFunc(10000, nextWave, 0);
	glutTimerFunc(25,runGameCalculations,0);

	glutKeyboardFunc(onKeyboard);
	glutKeyboardUpFunc(onKeyboardUp);

	glutPassiveMotionFunc(motion);
	glutMotionFunc(motion);

	glutSpecialFunc(onSpecKeyboard);
	glutSpecialUpFunc(onSpecKeyboardUp);
	glutSetCursor(GLUT_CURSOR_NONE);
	glutMouseFunc(mouse);
	//Enable Depth test
	glEnable(GL_DEPTH_TEST);
	
	glutMainLoop();

	return 1;
}