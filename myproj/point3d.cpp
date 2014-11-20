#include "stdafx.h"
#include "point3d.h"
#include "vector3d.h"
#include <iostream>

myPoint3D::myPoint3D() {}

myPoint3D::myPoint3D(double x, double y, double z)
{
    X = x;
    Y = y;
    Z = z; 
}

myPoint3D myPoint3D::operator+(myVector3D & v1)
{
	return myPoint3D(X+v1.dX, Y+v1.dY, Z+v1.dZ);
}

myPoint3D & myPoint3D::operator+=(myVector3D & v1)
{
	X += v1.dX;
	Y += v1.dY;
	Z += v1.dZ;
	return *this;
}

double myPoint3D::dist(myPoint3D p1)
{
    return sqrt(  (p1.X-X)*(p1.X-X) + (p1.Y-Y)*(p1.Y-Y) + (p1.Z-Z)*(p1.Z-Z)  ); 
}

void myPoint3D::rotate(myVector3D & lp, double theta)
{
	myVector3D tmp(X, Y, Z);
	tmp.rotate(lp, theta);
	X = tmp.dX; Y = tmp.dY; Z = tmp.dZ;
}

void myPoint3D::print(char *s)
{
	  std::cout << s << X << ", " << Y << ", " << Z << "\n";
}

