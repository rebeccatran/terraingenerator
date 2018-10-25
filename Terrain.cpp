#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "basicLibrary.h"
#include "math.h"
#include <time.h>
#include <stdlib.h>	

#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/freeglut.h>
#endif
#include <stdio.h>

//global variables
int sizeX = 300;	//declaring size of the map	for x and y
int sizeZ = 300;	

float heightValues[1000][1000];	//making a 2D array for the height map values
vec3D normVectors[1000][1000];	//2D array for the normal vectors 
vec3D face1[1000][1000];	//for storing face normals for one set of triangles
vec3D face2[1000][1000];	//storing face normals for the second set of triangles 

int iterate = 1000;	//number of iterations

float minH;
float maxH;

bool solid = true;	//boolean value for solid, necessary when switching between solid and wire
bool wire = false;	//boolean value for wire, for wireframe

bool triangle = true;	//boolean value for drawing triangles or quads
bool circle = true;	//boolean value for drawing with circle algorithm or fault algorithm

bool colour = true;
bool light = true;	//boolean value for lighting, on or off
bool lightSwitch = true;	//boolean value for light0 or light1 
bool shading = true;	//bool value for shading to be flat or smooth (gouraud)
bool mapsize = true;	//changing the mapsize from 50x50 to 300x300

int yAxisRotCounter = 0;	//y and x axis counter for rotation of terrain
int xAxisRotCounter = 0;

float eye[] = {50, 100, 200};	//eye location, where camera is at
float light_pos[] = {0,100,0,1};	//light location, where light0 and light1 is at 
float light_pos1[] = {0,100,100,1};	

float amb0[4]  = {0.5, 0.1, 0.9, 1}; //setting the ambient, diffuse and specular values for light0
float diff0[4] = {0.5, 0.1, 0.9, 1}; 
float spec0[4] = {0.5, 0.1, 0.9, 1};

float amb1[4]  = {0.1, 0.81, 0, 1}; //setting the ambient, diffuse and specular values for light1
float diff1[4] = {0.1, 0.81, 0, 1}; 
float spec1[4] = {0.1, 0.81, 0, 1};

float m_amb[] = {0.33, 0.22, 0.03, 1.0}; //setting the material for the ambient, diffuse and specular values
float m_diff[] = {0.78, 0.57, 0.11, 1.0}; 
float m_spec[] = {0.99, 0.91, 0.81, 1.0}; 
float shiny = 27.8; 

float angle = 0.0f;

//this works for quads because a quadrilateral is basically two triangles put together, just finding the normal vector of two triangles that make up a quad
void normalVec(){	
	for(int i = 0; i < sizeX; i++){
		for(int j = 0; j<sizeZ; j++){
			point3D v1 = point3D(i, heightValues[i][j], j);			//declaring the points of each of the vertices
			point3D v2 = point3D(i, heightValues[i][j+1], j+1);
			point3D v3 = point3D(i+1, heightValues[i+1][j+1], j+1);
		
			vec3D U;
			U = U.directionVec(v2,v1);	//finding the direction vector of vertices v1-v2 and storing it in U
			vec3D V;
			V = V.directionVec(v3,v1);	//finding direction vector of vertices v1-v3, storing into V

			vec3D crossUV;
			crossUV = crossUV.crossProduct(U,V);	//find the cross product of the two vectors U and V 

			crossUV = crossUV.normalize();	//normalize the cross product vector 
			face1[i][j] = crossUV;	//store it in face1
		}
	}

	for(int i = 1; i < sizeX+1; i++){
		for(int j = 1; j<sizeZ+1; j++){
			point3D a1 = point3D(i, heightValues[i][j], j);			//doing the same as above, but for the other triangles
			point3D a2 = point3D(i, heightValues[i][j-1], j-1);
			point3D a3 = point3D(i-1, heightValues[i-1][j-1], j-1);
	
			vec3D U;
			U = U.directionVec(a2,a1);
			vec3D V;
			V = V.directionVec(a3,a1);

			vec3D crossUV;
			crossUV = crossUV.crossProduct(U,V);

			crossUV = crossUV.normalize();
			face2[i][j] = crossUV;
		}
	}

	vec3D vectorSum;
	for(int i = 0; i<sizeX; i++){
		for(int j = 0; j<sizeZ; j++){
			if(i>0){	//as long as i is greater than 0 (this takes care of the edge cases and won't try to add vectors that are past the edges)
				if(j>0){	//as long as j is greater than 0
					vectorSum = vectorSum.addVec(face2[i-1][j-1]);	//adding the vector from face2 and face1 at i-1,j-2 (triangles at top left)
					vectorSum = vectorSum.addVec(face1[i-1][j-1]);
				}
				vectorSum = vectorSum.addVec(face2[i-1][j]);	//only i has to be greater than 0, adds vector from face2 at i-1,j (triangle at bottom left)
			}
			if(j>0){	//only j has to be greater than 0
				vectorSum = vectorSum.addVec(face1[i][j-1]);	//adds vector from face1 at i,j-1 (triangle at top right)
			}
			vectorSum = vectorSum.addVec(face2[i][j]);	//takes care of the "middle vertex" in which face1 and face2 are at the bottom right
			vectorSum = vectorSum.addVec(face1[i][j]);

			vectorSum = vectorSum.normalize();	//normalize all of the added vectors 
			normVectors[i][j] = vectorSum;	//add the normalized vector to the normal vectors array
		}
	}

}

//circle algorithm
 void circleAlgorithm(int iterations){
 	for(int a = 0; a < iterations; a++){	//going through all the iterations
	 	int ranX = rand() % sizeX;	//generating random x center point
	 	int ranZ = rand() % sizeZ;	//generating random z center point
		int ranRadius = (rand()%20); //generating random radius point between 0-19
		float disp = (rand()%10);	//generating random disp point between 0-9	

		for(int i=0; i < sizeX; i++){	//iterating through the vertices for x
			for(int j=0; j < sizeZ; j++){	//iterating through the vertices for z 
				int dx = i - ranX;	//finding new x and z values
				int dz = j - ranZ;

				float dist = sqrtf((dx*dx) + (dz*dz));	//distance formula
				float pd = (dist*2)/ranRadius;	

				if (fabs(pd) <= 1.0){
					heightValues[i][j] += (disp/2.0) + (cos(pd*3.14)*(disp/2.0));	//calculating the heightvalues
				}
			}
		}
 	}

 //	float minH = 0.0;
 //	float maxH = 0.0;
 	for (int i = 0; i < sizeX; i++){
 		for (int j = 0; j < sizeZ; j++){
 			if(heightValues[i][j] < minH){
 				minH = heightValues[i][j];
 			} else if(heightValues[i][j] > maxH){
 				maxH = heightValues[i][j];
 			}
 		}
 	}
 	// glutPostRedisplay();
 	normalVec();

}

void faultAlgorithm(int iterations){
//	float minH = 0.0;	//declaring min and max height variables
//	float maxH = 0.0;

	srand(time(NULL));
	for(int n = 0; n < iterations; n++){	//going through all the iterations
		float v = rand()%20;	//finding a random number 
		float a = sin(v);	//taking the sin, cos of random number generated
		float b = cos(v);
		float d = sqrt((sizeX*sizeX) + (sizeZ*sizeZ));	//finding the distance
		float c = (float)rand()/RAND_MAX * d - d/2.0f;	//calculations using rand and rand_max, and the distance 

		for(int i = 0; i < sizeX; i++){	//going through all the vertices
			for(int j = 0; j < sizeZ; j++){
				if(a*i + b*j - c > 0){	//calculations with the numbers found previously 
					heightValues[i][j] += 1;	//incrementing height
					if(heightValues[i][j] > maxH){	//changing the max height value
						maxH = heightValues[i][j];
					}
				} else {
					heightValues[i][j] -= 1;	//decrementing height
					if(heightValues[i][j] < minH){	//changing the min height value
						minH = heightValues[i][j];
					}
				}
			}
		} 
	}
	// glutPostRedisplay();
	normalVec();
}

void drawMapTriangles(){	//drawing the triangles for the mesh
	vec3D n, n1, n2;	//declaring variables to use with normal vectors

	glPushMatrix();	//using a pop matrix so that the rotates and translate are only done to this section
		glRotatef(10 * xAxisRotCounter,1, 0, 0);	//rotating the terrain around the x axis 
		glRotatef(10 * yAxisRotCounter, 0, 1, 0);	//rotating the terrain around the y axis

		glTranslatef(-sizeX / 2, 0, -sizeZ / 2);	//translating the triangles to be in the centre of the terrain

		if (light == false){	//turning the light on and off, if false
			glDisable(GL_LIGHTING);	//turn light off
		}
		if(light == true){	//if it true
			glEnable(GL_LIGHTING);	//turn light on 
			glColorMaterial(GL_FRONT, GL_AMBIENT);	//changing ambient colour of material -- necessary for wireframe
			glEnable(GL_COLOR_MATERIAL);	//enable the material for the terrain
		}

		// if(colour == true){
		// 	glEnable(GL_COLOR_MATERIAL);
		// } else if(colour == false){
		// 	float rheight = 
		// }

		glBegin(GL_TRIANGLES);	//drawing the triangles
			for(int i = 0; i<sizeX; i++){	//iterating through the vertices of x and z
				for(int j = 0; j<sizeZ; j++){
					n = normVectors[i][j];	//accessing the normal vectors array at i,j, i,j+1, and i+1,j+1 and setting them to variables
					n1 = normVectors[i][j+1];
					n2 = normVectors[i+1][j+1];
					glNormal3f(n.x, n.y, n.z);	//normal for the vertex at i,j
					glVertex3f(i, heightValues[i][j], j);	//drawing vertex at i,j (vertices are drawn in counter-clockwise direction)
					glNormal3f(n1.x, n1.y, n1.z);	//normal for the vertex at i,j+1
					glVertex3f(i, heightValues[i][j+1], j+1);	//drawing vertex at i,j+1 
					glNormal3f(n2.x, n2.y, n2.z);	//normal for vertex at i+1,j+1
					glVertex3f(i+1, heightValues[i+1][j+1], j+1);	//drawing vertex at i+1,j+1
				}
			}
		glEnd();

		glBegin(GL_TRIANGLES);	//drawing the other half of the triangles for the mesh, same as above but drawn in the opposite
			for(int i = 1; i<sizeX+1; i++){	
				for(int j = 1; j<sizeZ+1; j++){
					n = normVectors[i][j];
					n1 = normVectors[i][j-1];
					n2 = normVectors[i-1][j-1];
					glNormal3f(n.x, n.y, n.z);
					glVertex3f(i, heightValues[i][j], j);
					glNormal3f(n1.x, n1.y, n1.z);
					glVertex3f(i, heightValues[i][j-1], j-1);
					glNormal3f(n2.x, n2.y, n2.z);
					glVertex3f(i-1, heightValues[i-1][j-1], j-1);
				}
			}
		glEnd();

	glPopMatrix();	//popping the matrix 
}

void drawMapQuads(){	//function to draw the quads
	vec3D n, n1, n2, n3;

	glPushMatrix();
		glRotatef(10 * xAxisRotCounter,1, 0, 0);	//same as drawMapTriangles for this section, rotating and translating, turning light on/off
		glRotatef(10 * yAxisRotCounter, 0, 1, 0);

		glTranslatef(-sizeX / 2, 0, -sizeZ / 2);

		if (light == false){
			glDisable(GL_LIGHTING);
		}
		if(light == true){
			glEnable(GL_LIGHTING);
			glColorMaterial(GL_FRONT, GL_AMBIENT);	
			glEnable(GL_COLOR_MATERIAL);
		}

		glBegin(GL_QUADS);	//drawing the quadrilaterals 
			for(int i = 0; i<sizeX; i++){	//iterating through vertices  
				for(int j = 0; j<sizeZ; j++){
					n = normVectors[i][j];	//accessing normal vectors array at i,j, i,j+1, i+1,j+1 and i+1,j
					n1 = normVectors[i][j+1];
					n2 = normVectors[i+1][j+1];
					n3 = normVectors[i+1][j];
					glNormal3f(n.x, n.y, n.z);
					glVertex3f(i, heightValues[i][j], j);	//same idea as triangles, but added another vertex for the quadrilateral
					glNormal3f(n1.x, n1.y, n1.z);
					glVertex3f(i, heightValues[i][j+1], j+1);
					glNormal3f(n2.x, n2.y, n2.z);
					glVertex3f(i+1, heightValues[i+1][j+1], j+1);
					glNormal3f(n3.x, n3.y, n3.z);
					glVertex3f(i+1, heightValues[i+1][j], j);
				}
			}
		glEnd();

	glPopMatrix();
}

void reset(){	//function to make a new terrain
	glPushMatrix();
		glRotatef(10 * xAxisRotCounter,1, 0, 0);	//rotating x and y axis
		glRotatef(10 * yAxisRotCounter, 0, 1, 0);

		glTranslatef(-sizeX / 2, 0, -sizeZ / 2);	//translating terrain to be in the middle

		for(int i = 0; i<sizeX; i++){	//nested for loop to iterate through the height map values, iteration through x values
			for(int j = 0; j<sizeZ; j++){	//iteration through the z values
				heightValues[i][j] = 0;	//reset the values within the height map to 0
			}
		}

	glPopMatrix();
}

void toggle(){	//function to switch between wireframe and solid or both
	if (solid == true && wire == false){	//if solid is on and wire is not 
		solid = false;	//switch from solid
		wire = true;	//switch to wireframe
	} else if(solid == false && wire == true){	//if solid is off and wire is on
		solid = true;	//switch to solid
		wire = true;	//keep wireframe
	} else {	//if they are both on
		solid = true;	//reset back to just solid
		wire = false;	//turn wireframe off 
	}
	// glutPostRedisplay();	//refresh the window
}

//keyboard instructions printed out to the console
void instructions(){
	printf("The default is on circle algorithm, drawn with triangles, smooth shading, light on, light0 movement and on a 300x300 mapsize\n");
	printf("Arrow Keys ------------------------- Move terrain about x and y axis\n");
	printf("'w'--------------------------------- Switch modes between solid and wireframe or both\n");
    printf("'t' -------------------------------- Draw terrain using triangles\n");
    printf("'y' -------------------------------- Draw terrain using quadrilaterals\n");
	printf("'c' -------------------------------- Draw terrain using circles algorithm\n");
	printf("'f' -------------------------------- Draw terrain using fault algorithm\n");
	printf("'r' -------------------------------- Render a new terrain\n");
	printf("'l' -------------------------------- Turn lighting on and off\n");
	printf("'i' -------------------------------- Switch between moving light0 or light1 around\n");
	printf("'h','k','u','j' -------------------- Move whichever light you are on (based on 'i') about x and y axis\n");
	printf("'s' -------------------------------- Switch modes between flat shading and smooth shading\n");
	printf("'m' -------------------------------- Switch the terrain size from 300x300 to 50x50\n 	");
	printf("'q'/esc ---------------------------- Close/exit\n");
}

 void keyboard(unsigned char key, int x, int y){
 	switch (key){
 		case 'q':	//if q is pressed
 		case 27:
 			exit (0);	//exit the window
 		break;

 		case 'w':	//if w is pressed
 		case 'W':
 			toggle();	//toggle function is called to switch between wire and solid, or both
 		break;

 		case 't':	//if t is pressed
 		case 'T':
 			if (triangle == false){	//the toggle should be on rectangles if false
 				drawMapTriangles();	//change to triangles 
 				triangle = true;	//make it true for triangles
 			}
 		break;

 		case 'y':	//if y is pressed
 		case 'Y':
 			if (triangle == true){	//true indicates that triangles are drawn
 				drawMapQuads();	//therefore change it to quads and draw quads with quad func
 				triangle = false;	//change bool to false to make it represent quads being drawn
 			}
 		break;

 		case 'r':	//if r is pressed
 			reset();	//reset function is called
 			
 			if (circle == true){	//if the terrain is currently on circles algorithm
 				circleAlgorithm(iterate);	//will render a new terrain for circles
 			} else {	//if it is on fault algorithm
 				faultAlgorithm(iterate);	//will render a new terrain for fault 
 			}
 		break;

 		case 'c':	//switching to circles, from fault 
 			if (circle == false){	//if the terrain is currently on fault
 				reset();	//reset the heightmap
 				circleAlgorithm(iterate);	//draw new heightmap with circle algorithm
 				circle = true;	//set the boolean to true, which shows that it is currently on circles
 			}
 		break;
			
 		case 'f':
 			if (circle == true){	//if the terrain is currently on circles
 				reset();	//reset the heightmap
 				faultAlgorithm(iterate);	//draw new heightmap with fault algorithm
 				circle = false;	//set boolean to false, representing that terrain is currently drawn with fault
 			}
 		break;

 		case 'l':
 			if(light == false){	//if the light is off
 				light = true;	//set light to be true, to be turned on 
 			} else {
 				light = false;	//if it is on, set the light to be false and turned off 
 			}
 		break;

 		case 's':
 			if (shading == true){	//shading at the current state should be smooth
 				glShadeModel(GL_FLAT);	//draw terrain with flat shading
 				shading = false;	//set bool to be false, to switch to flat
 			} else {
 				glShadeModel(GL_SMOOTH);	//draw terrain with smooth/gouraud shading
 				shading = true;	//bool set to true, to go back to smooth shading
  			}
 		break;

 		case 'm':
 			if (mapsize == true){	//boolean to switch between 50x50 and 300x300, default being 300x300
 				sizeX = 50;	//set the map size to be 50 for x and y
 				sizeZ = 50;
 				iterate = 200;	//make the number of iterations smaller for the smaller map size
 				mapsize = false;	//bool set to false, to switch back to 300x300 if user chooses
 			} else {
 				sizeX = 300;	//set map size to be 300 for x and y
 				sizeZ = 300;
 				iterate = 1000;	//increase the number of iterations back to the original amount, with larger map size
 				mapsize = true;	//boolean true representing mapsize is currently 300x300
 			}
 		break;

 		case 'i':	//this is for switching between light0 and light1 with moving the lighting
	 		if(lightSwitch == true){	//if lightswitch is true, currently on light0
	 			lightSwitch = false;	//switch from light0 to light1
	 		} else {					//if lightswitch is false, currently on light1
	 			lightSwitch = true;		//switch from light1 to light0
	 		}
	 	break;

 		case 'h':	//left
	 		if(lightSwitch == true){	//if lightswitch is true, moving light0
	 			light_pos[0]-=100;	//decrementing the light0 x value
	 		} else {				//lightswitch is false, moving light1
	 			light_pos1[0]-=100;	 //decrementing the light1 x value
	 		}
 		break;

 		case 'k':	//right
	 		if(lightSwitch == true){	//light switch is true, moving light0
	 			light_pos[0]+=10;	//incrementing light0 x value
	 		} else {
	 			light_pos1[0]+=10;	 	//moving light1, incrementing x value
	 		}

 		break;

 		case 'u':	//up
	 		if(lightSwitch == true){	
	 			printf("I'm moving!");
	 			light_pos[1]+=10;	//moving light0, inc y value
	 		} else {		
	 			light_pos1[1]+=10; 	//moving light1, inc y value	
	 		}
 		break; 		

 		case 'j':	//down
	 		if(lightSwitch == true){
	 			light_pos[1]-=10;	//moving light0, dec y value
	 		} else {
	 			light_pos1[1]-=10;		//moving light1, dec y value
	 		}
 		break;
 	}
 	glutPostRedisplay();
 }

 void special(int key, int x, int y){	// arrow key presses move the camera
    switch(key){
    	case GLUT_KEY_LEFT: 
    		yAxisRotCounter--; //decrement the counter for the y axis
    	break; 

    	case GLUT_KEY_RIGHT: 
    		yAxisRotCounter++;	//increment the counter for y axis
    	break;

    	case GLUT_KEY_UP: 
    		xAxisRotCounter++;	//increment the counter for x axis
    	break;

    	case GLUT_KEY_DOWN: 
    		xAxisRotCounter--;	//decrement the counter for x axis
    	break;
    }
    glutPostRedisplay();
}

void init(void){
	glClearColor(0, 0, 0, 0);	//clearing all colours
	glColor3f(1, 1, 1);	//changing colour

	glEnable(GL_LIGHTING); 	//enabling lighting
	glEnable(GL_LIGHT0);	//turning on light0
	glEnable(GL_LIGHT1);	//turning on light1

	glLightfv(GL_LIGHT0, GL_AMBIENT, amb0); //setting the amb, diff, and spec for light0
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diff0); 
	glLightfv(GL_LIGHT0, GL_SPECULAR, spec0);

	glLightfv(GL_LIGHT1, GL_AMBIENT, amb1); 	//setting the amb, diff, and spec for light1
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diff1); 
	glLightfv(GL_LIGHT1, GL_SPECULAR, spec1);

    glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    gluPerspective(45, 1, 1, 2000);		//setting up the perspective
    glShadeModel(GL_SMOOTH);
 }

/* display function - GLUT display callback function
 *		clears the screen, sets the camera position, draws the ground plane and movable box
 */
 void display(void){
 	float origin[3] = {0,0,0};
 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /************************************************************************
     
                                    CAMERA SET UP
     
     ************************************************************************/

    glMatrixMode(GL_MODELVIEW); 
    glLoadIdentity();
    gluLookAt(eye[0], eye[1], eye[2], 0, 0, 0, 0, 1, 0); 	//position the eye/camera

	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);	//putting light0 and light1 into position
	glLightfv(GL_LIGHT1, GL_POSITION, light_pos1);

	// making front and back to have the same material -- if you want front only GL_FRONT
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,  m_amb); //putting material onto the terrain
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  m_diff); 
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  m_spec); 
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS,  shiny);

    if (triangle == true){	//if the terrain is on triangle
    	if(solid == true){	//shading is solid
    		glColor3f(0.2,0.2,0.8);	//change the colour 
    		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	//draw shading to be filled (solid)
    		drawMapTriangles();	//run to draw map with triangles
    	}
    	if(wire == true){	//if wire is on
    		glColor3f(0.2,0.8,0.8);	//change colour of wire 
    		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	//draw terrain using lines/wireframe
    		drawMapTriangles();	//draw map with triangles
    	}
    } else {	//if terrain is on quadrilaterals 
    	if(solid == true){	//shading is solid
    		glColor3f(0.2,0.2,0.8);	//change colour
    		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	//draw shading to be solid
    		drawMapQuads();		//draw map with quadrilaterals 
    	}
    	if(wire == true){	//terrain is on quads and wire is on
    		glColor3f(0.2,0.8,0.8);	//change colour of wire 
    		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	//draw terrain using wireframe
    		drawMapQuads();	//draw map with quadrilaterals 
    	}
    }
	
	glutSwapBuffers();
	
}

/* main function - program entry point */
int main(int argc, char** argv)
{
	srand(time(NULL));
	glutInit(&argc, argv);		//starts up GLUT
	
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	
	glutInitWindowSize(800, 800);	//window size of 800x800
	glutInitWindowPosition(100, 100);	//placement on screen

	glutCreateWindow("Terrain Modeling");	//creates the window

	glutDisplayFunc(display);	//registers "display" as the display callback function
	glutKeyboardFunc(keyboard);	//running keyboard functions
	glutSpecialFunc(special);	//running special functions

	glEnable(GL_DEPTH_TEST);	//enables depth
	glCullFace(GL_BACK);	//not draw the back face
	glEnable(GL_CULL_FACE);

	init();

	circleAlgorithm(iterate);	//calling the circle algorithm to begin with
	// normalVec();
	instructions();	//call instructions functinon to display 

	glutMainLoop();				//starts the event loop

	return(0);					//return may not be necessary on all compilers
}