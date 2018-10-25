#include <math.h>
#include "basicLibrary.h"

point3D::point3D(){
	
}

point3D::point3D(float xIn, float yIn, float zIn){
	x = xIn;	//setting the x, y, z values
	y = yIn;
	z = zIn;
}

float point3D::distance(point3D p2){
	float dist = sqrt(pow(p2.x-x,2) + pow(p2.y-y, 2) + pow(p2.z-z,2));	//finding the distance
	return dist;
}


vec3D::vec3D(){

}

vec3D::vec3D(float xIn, float yIn, float zIn){	//setting the vector x,y,z
	x = xIn;
	y = yIn;
	z = zIn;
	length();
}

float vec3D::length(){
	float L = sqrt((pow(x,2) + pow(y,2) + pow(z,2)));	//finding the length 
	mag = L;
	return L;
}

vec3D vec3D::normalize(){	//normalize the vector
	vec3D normVec(x,y,z);
	normVec.x = x/length();
	normVec.y = y/length();
	normVec.z = z/length();

	if (normVec.length() == 1){
		return normVec;
	}
	return normVec;
}

vec3D vec3D::crossProduct(vec3D v1, vec3D v2){	//finding the cross product of two vectors 
	vec3D vector3D = vec3D();
	vector3D.x = (v1.y*v2.z) - (v1.z*v2.y);
	vector3D.y = (v1.z*v2.x) - (v1.x*v2.z);
	vector3D.z = (v1.x*v2.y) - (v1.y*v2.x);
	return vector3D;
}

vec3D vec3D::directionVec(point3D p1, point3D p2){	//finding the direction vector of two points 
	vec3D direction = vec3D();
	direction.x = p2.x - p1.x;
	direction.y = p2.y - p1.y;
	direction.z = p2.z - p1.z;
	return direction;
}

vec3D vec3D::addVec(vec3D v2){	//adding two vectors together 
	vec3D add = vec3D();
	add.x = x + v2.x;
	add.y = y + v2.y;
	add.z = z + v2.z;
	return add;
}