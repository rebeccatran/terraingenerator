//Header file
#ifndef INCLUDED_BLIB_H
#define INCLUDED_BLIB_H

class point3D;
class vec3D;
class colour;

class point3D{	//class of points in 3D
public:
	point3D();
	point3D(float xIn, float yIn, float zIn);	//takes in the points for x, y, z

	float x,y,z;
	float distance (point3D p2);	//distance formula

private:

};

class vec3D{	//class of vectors in 3D
public:
	vec3D();
	vec3D(float xIn, float yIn, float zIn);	//takes in the x, y, z 

	float x, y, z, mag;

	float length();	//finding the length of the vector
	vec3D normalize();	//normalizing vectors
	vec3D crossProduct(vec3D v1, vec3D v2);	//finding the cross product 
	vec3D directionVec (point3D p1, point3D p2);	//finding the direction of two vectors
	vec3D addVec(vec3D v2);	//adding vectors together	

private:

};


class colour{
public:
	colour();
	colour(float rIn, float gIn, float bIn);

	float r, g, b;

private:

};

#endif