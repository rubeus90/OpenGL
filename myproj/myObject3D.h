#include <math.h>
#include <GL/glew.h>
#include <vector>
#include <string>
#include <fstream>
#include "vector3d.h"
#include "myTexture.h"

#define PI 3.14159265

using namespace std;

class myObject3D
{
public:
	GLuint buffers[6];

	std::vector<GLfloat> vertices;
	std::vector<GLuint> indices; 
	std::vector<GLfloat> normals;
	std::vector<GLfloat> textures;
	std::vector<GLfloat> tangents;

	myTexture texture;
	myTexture bump;
	glm::mat4 model_matrix;
	
	myObject3D() {
		model_matrix = glm::mat4(1.0f);
	}

	void clear() {
	}
 
	void readMesh(char *filename)
	{
		clear();
		string s, t;
		float v1, v2, v3;
		int f1, f2, f3;

		ifstream fin(filename);

		while (getline(fin, s))
		{
			stringstream myline(s);
			myline >> t;
			if (t == "v")
			{
				myline >> v1;
				myline >> v2;
				myline >> v3;

				vertices.push_back(v1);
				vertices.push_back(v2);
				vertices.push_back(v3);
			}
			else if (t == "f")
			{
				myline >> f1;
				myline >> f2;
				myline >> f3;

				indices.push_back(f1-1);
				indices.push_back(f2-1);
				indices.push_back(f3-1);
			}
		}
	}

	void computeNormal(int v1, int v2, int v3, float & x, float & y, float & z) //v1,v2,v3 : indice in vertices[]    x,y,z : normal vector
	{
		myVector3D vector1(vertices[v1*3], vertices[v1*3+1], vertices[v1*3+2]);
		myVector3D vector2(vertices[v2*3], vertices[v2*3+1], vertices[v2*3+2]);
		myVector3D vector3(vertices[v3*3], vertices[v3*3+1], vertices[v3*3+2]);
		myVector3D n;
		n.crossproduct(vector2 + (-vector1), vector3+(-vector2));
		n.normalize();

		x = n.dX;
		y = n.dY;
		z = n.dZ;
	}

	void computeNormals( )
	{
		int i;
		float x, y, z;

		for (i = 0; i < vertices.size(); i++){
			normals.push_back(0);
		}
		//normals.resize(vertices.size());

		for (i = 0; i < indices.size(); i+=3){
			computeNormal(indices[i], indices[i + 1], indices[i + 2], x, y, z);

			normals[indices[i]*3] += x;
			normals[indices[i]*3+1] += y;
			normals[indices[i]*3+2] += z;

			normals[indices[i+1]*3] += x;
			normals[indices[i+1]*3 + 1] += y;
			normals[indices[i+1]*3 + 2] += z;

			normals[indices[i+2]*3] += x;
			normals[indices[i+2]*3 + 1] += y;
			normals[indices[i+2]*3 + 2] += z;
		}

		for (i = 0; i < normals.size(); i += 3){
			myVector3D vector(normals[i], normals[i + 1], normals[i + 2]);
			vector.normalize();

			normals[i] = vector.dX;
			normals[i + 1] = vector.dY;
			normals[i + 2] = vector.dZ;
		}
	}

	void computeCylinderTextureCoordinates(){
		int n = vertices.size()/3;
		textures.resize(2 * n);
		GLfloat x, y, z;
		for (int i = 0; i<n; i++)
		{
			x = vertices[3 * i]; y = vertices[3 * i + 1]; z = vertices[3 * i + 2];

			textures[2 * i] = z+0.5;

			if (y >= 0.0f)     textures[2 * i + 1] = atan2(y, x) / (PI);
			else if (y<0.0f)  textures[2 * i + 1] = (-atan2(y, x)) / (PI);
			//this has problems at the seam, when 1->0 and so interpoltion results in the whole image squeezed between the two border vertices.
			//if ( y>=0.0f )     textures[2*i+1] = atan2(  y,  x ) / (2*PI) ;
			//else if ( y<0.0f )  textures[2*i+1] = (2*PI + atan2(  y,  x )) / (2*PI) ;
		}
	}

	void computeSphereTextureCoordinates(){
		int n = vertices.size() / 3;
		textures.resize(2 * n);
		GLfloat x, y, z;
		for (int i = 0; i<n; i++)
		{
			x = vertices[3 * i]; y = vertices[3 * i + 1]; z = vertices[3 * i + 2];

			textures[2 * i +1 ] = atan2(sqrt(x*x + y*y),z) / (PI);
			//textures[2 * i] = acos(z / sqrt(x*x + y*y + z*z)) / (PI);
			if ( y>=0.0f )     textures[2*i] = atan2(  y,  x ) / (2*PI) ;
			else if ( y<0.0f )  textures[2*i] = (2*PI + atan2(  y,  x )) / (2*PI) ;
		}
	}

	void computePlaneTextureCoordinates(){
		int n = vertices.size() / 3;
		textures.resize(2 * n);
		GLfloat x, y, xmax=vertices[0], ymax=vertices[1];
		for (int i = 0; i<n; i++)
		{
			if (vertices[3*i] > xmax) xmax = vertices[3*i];
			if (vertices[3 * i + 1] > ymax) ymax = vertices[3 * i + 1];
		}

		for (int i = 0; i<n; i++)
		{
			x = vertices[3 * i]; y = vertices[3 * i + 1];

			if(y>0.0f) textures[2 * i + 1] = y / ymax;
			if(x>0.0f) textures[2 * i] = x / xmax;
		}
	}

	void createObjectBuffers()
	{
		glGenBuffers(5, buffers);

		glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * 4, &vertices.front(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * 4, &indices.front(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
		glBufferData(GL_ARRAY_BUFFER, normals.size() * 4, &normals.front(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, buffers[3]);
		glBufferData(GL_ARRAY_BUFFER, textures.size() * 4, &textures.front(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, buffers[4]);
		glBufferData(GL_ARRAY_BUFFER, tangents.size() * 4, &tangents.front(), GL_STATIC_DRAW);

	}

	void displayObject(GLuint shaderprogram, glm::mat4 viewmatrix)
	{
		//model_matrix = glm::mat4(1.0f);
		glUniformMatrix4fv(glGetUniformLocation(shaderprogram, "mymodel_matrix"), 1, GL_FALSE, &model_matrix[0][0]);

		glm::mat3 normal_matrix = glm::transpose(glm::inverse(glm::mat3(viewmatrix*model_matrix)));
		glUniformMatrix3fv(glGetUniformLocation(shaderprogram, "mynormal_matrix"), 1, GL_FALSE, &normal_matrix[0][0]);

		//Vertex
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		//Normals
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

		//Textures
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, buffers[3]);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

		//Tangents
		glEnableVertexAttribArray(3);
		glBindBuffer(GL_ARRAY_BUFFER, buffers[4]);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture.texName);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, bump.texName);

		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	}

	void drawNormals()
	{
		glBegin(GL_LINES);
		for (int i = 0; i < vertices.size(); i += 3)
		{
			glVertex3f(vertices[i], vertices[i + 1], vertices[i + 2]);
			glVertex3f(vertices[i] + normals[i] / 10.0, vertices[i + 1] + normals[i + 1] / 10.0, vertices[i + 2] + normals[i + 2] / 10.0);
		}
		glEnd();
	}

	void translate(double x, double y, double z){
		glm::mat4 tmp = glm::translate(glm::vec3(x, y, z));
		model_matrix = tmp * model_matrix;
	}

	void rotate(double axis_x, double axis_y, double axis_z, double angle){
		glm::mat4 tmp = glm::rotate((float)angle, glm::vec3(axis_x, axis_y, axis_z));
		model_matrix = tmp * model_matrix;
	}

	void scale(double x, double y, double z){
		glm::mat4 tmp = glm::scale(glm::vec3(x, y, z));
		model_matrix = tmp * model_matrix;
	}

	void computeTangent(int v0, int v1, int v2, float & x, float & y, float & z)
	{
		float du1 = textures[2 * v1] - textures[2 * v0];
		float dv1 = textures[2 * v1 + 1] - textures[2 * v0 + 1];
		float du2 = textures[2 * v2] - textures[2 * v0];
		float dv2 = textures[2 * v2 + 1] - textures[2 * v0 + 1];

		float f = 1.0f / (du1 * dv2 - du2 * dv1);
		if ((du1*dv2 - du2*dv1) == 0){
			x = y = z = 0; return;
		}

		float e1x = vertices[3 * v1] - vertices[3 * v0];
		float e1y = vertices[3 * v1 + 1] - vertices[3 * v0 + 1];
		float e1z = vertices[3 * v1 + 2] - vertices[3 * v0 + 2];

		float e2x = vertices[3 * v2] - vertices[3 * v0];
		float e2y = vertices[3 * v2 + 1] - vertices[3 * v0 + 1];
		float e2z = vertices[3 * v2 + 2] - vertices[3 * v0 + 2];

		x = f * (dv2 * e1x - dv1 * e2x);
		y = f * (dv2 * e1y - dv1 * e2y);
		z = f * (dv2 * e1z - dv1 * e2z);
	}

	void computeTangents()
	{
		int i, j, k;
		GLfloat x1, y1, z1;

		int n = vertices.size() / 3;
		int m = indices.size() / 3;

		tangents.resize(3 * n);
		int *incidences = new int[n];
		for (i = 0; i<3 * n; i++) tangents[i] = 0.0;
		for (i = 0; i<n; i++) incidences[i] = 0;

		for (j = 0; j<m; j++)
		{
			computeTangent(indices[3 * j], indices[3 * j + 1], indices[3 * j + 2], x1, y1, z1);
			tangents[3 * indices[3 * j]] += x1; tangents[3 * indices[3 * j] + 1] += y1; tangents[3 * indices[3 * j] + 2] += z1;
			tangents[3 * indices[3 * j + 1]] += x1; tangents[3 * indices[3 * j + 1] + 1] += y1; tangents[3 * indices[3 * j + 1] + 2] += z1;
			tangents[3 * indices[3 * j + 2]] += x1; tangents[3 * indices[3 * j + 2] + 1] += y1; tangents[3 * indices[3 * j + 2] + 2] += z1;
			incidences[indices[3 * j]]++; incidences[indices[3 * j + 1]]++; incidences[indices[3 * j + 2]]++;
		}
		for (i = 0; i<n; i++) {
			float l = sqrt(tangents[3 * i] * tangents[3 * i] + tangents[3 * i + 1] * tangents[3 * i + 1] + tangents[3 * i + 2] * tangents[3 * i + 2]);
			tangents[3 * i] /= l; tangents[3 * i + 1] /= l; tangents[3 * i + 2] /= l;
		}
	}
};
