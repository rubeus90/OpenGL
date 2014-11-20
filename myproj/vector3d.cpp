#include "stdafx.h"
#include "vector3d.h"
#include "point3d.h"
#include <iostream>
 
myVector3D::myVector3D() {}

myVector3D::myVector3D(double dx, double dy, double dz)
{
    dX = dx;
    dY = dy;
    dZ = dz;
}

double myVector3D::operator*(myVector3D & v1)
{
    return (v1.dX*dX + v1.dY*dY + v1.dZ*dZ);
}

myVector3D myVector3D::operator+(myVector3D & v1)
{
	return myVector3D(dX+v1.dX, dY+v1.dY, dZ+v1.dZ);
}

myVector3D myVector3D::operator-()
{
	return myVector3D(-dX, -dY, -dZ);
}

myVector3D myVector3D::operator*(double s)
{
	return myVector3D(dX*s, dY*s, dZ*s);
}

void myVector3D::crossproduct(myVector3D & v1, myVector3D & v2)
{
	dX = v1.dY * v2.dZ - v1.dZ * v2.dY;
	dY = v1.dZ * v2.dX - v1.dX * v2.dZ;
	dZ = v1.dX * v2.dY - v1.dY * v2.dX;
}

myVector3D myVector3D::crossproduct(myVector3D & v1)
{
	myVector3D result;
	result.crossproduct(*this, v1);
	return result;
}

double myVector3D::length( )
{
    return sqrt( dX*dX + dY*dY + dZ*dZ ); 
}

void myVector3D::normalize( )
{
    double l = length();
    dX = dX/l;
    dY = dY/l;
    dZ = dZ/l;
}

void myVector3D::rotate(myVector3D & lp, double theta)
{
	//rotate vector *this around the line defined by lp through the origin by theta degrees.
	const double cos_theta = cos(theta);
	const double dot = *this*lp;
	myVector3D cross = crossproduct(lp);
	dX *= cos_theta; dY *= cos_theta; dZ *= cos_theta; 
	dX += lp.dX * dot * (1.0-cos_theta); dY += lp.dY * dot * (1.0-cos_theta); dZ += lp.dZ * dot * (1.0-cos_theta);
	dX -= cross.dX*sin(theta); dY -= cross.dY*sin(theta); dZ -= cross.dZ*sin(theta);
}

void myVector3D::print(char *s)
{
	  std::cout << s << dX << ", " << dY << ", " << dZ << "\n";
}
