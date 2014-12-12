#include "stdafx.h"
#include <fstream>
#include <GL/glew.h>
#include <freeglut.h>
#include <string>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
using namespace std;

#include "shaders.h"

#include "point3d.h"
#include "vector3d.h"
#include "myObject3D.h"

// width and height of the window.
int Glut_w = 600, Glut_h = 400; 

//Variables and their values for the camera setup.
myPoint3D camera_eye(0,10,15);
myVector3D camera_up(0,1,0);
myVector3D camera_forward (0,0,-1);

float fovy = 90;
float zNear = 0.2;
float zFar = 6000;

int button_pressed = 0; // 1 if a button is currently being pressed.
int GLUTmouse[2] = { 0, 0 };

GLuint vertexshader, fragmentshader, shaderprogram1; // shaders

GLuint renderStyle = 0;			GLuint renderStyle_loc;
GLuint projection_matrix_loc;
GLuint view_matrix_loc;
GLuint normal_matrix_loc;
GLuint buffers[6];

GLuint mylight_position_loc;
GLuint mylight_color_loc;
GLuint mylight_direction_loc;
GLuint mylight_type_loc;
GLuint light_type = 0;

vector<GLfloat> vertices;
vector<GLfloat> normals;
vector<GLuint> indices;

//myObject3D *obj1, *obj2, *obj3, *obj4, *obj5, *obj6, *obj7,*obj8, *obj9, *obj10, *obj11, *obj12,*obj13,*obj14;

vector<myObject3D> listObjects;

//This function is called when a mouse button is pressed.
void mouse(int button, int state, int x, int y)
{
  // Remember button state 
  button_pressed = (state == GLUT_DOWN) ? 1 : 0;

   // Remember mouse position 
  GLUTmouse[0] = x;
  GLUTmouse[1] = Glut_h - y;
}

//This function is called when the mouse is dragged.
void mousedrag(int x, int y)
{
  // Invert y coordinate
  y = Glut_h - y;

  //change in the mouse position since last time
  int dx = x - GLUTmouse[0];
  int dy = y - GLUTmouse[1];

  GLUTmouse[0] = x;
  GLUTmouse[1] = y;

  if (dx == 0 && dy == 0) return;
  if (button_pressed == 0) return;

  double vx = (double) dx / (double) Glut_w;
  double vy = (double) dy / (double) Glut_h;
  double theta = 4.0 * (fabs(vx) + fabs(vy));

  myVector3D camera_right = camera_forward.crossproduct(camera_up);
  camera_right.normalize();

  myVector3D tomovein_direction = -camera_right*vx + -camera_up*vy;

  myVector3D rotation_axis = tomovein_direction.crossproduct(camera_forward);
  rotation_axis.normalize();
  
  camera_forward.rotate(rotation_axis, theta);
  
  camera_up.rotate(rotation_axis, theta);
  camera_eye.rotate(rotation_axis, theta);
 
  camera_up.normalize();
  camera_forward.normalize();

  glutPostRedisplay();
}

void mouseWheel(int button, int dir, int x, int y)
{
	if (dir > 0)
		camera_eye += camera_forward * 0.02;
	else
		camera_eye += -camera_forward * 0.02;
	glutPostRedisplay();
}

//This function is called when a key is pressed.
void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 27:  // Escape to quit
		exit(0);
		break;
	case 's':
		renderStyle = (renderStyle + 1) % 2;
		glUniform1i(renderStyle_loc, renderStyle);
		break;
	case 'r':
		camera_eye = myPoint3D(0, 0, 2);
		camera_up = myVector3D(0, 1, 0);
		camera_forward = myVector3D(0, 0, -1);
		break;
	case 'l':
		light_type = (light_type + 1) % 3;
		glUniform1i(mylight_type_loc, light_type);
		break;
	}
	glutPostRedisplay();
}

bool isCollision(myVector3D delta){
	myPoint3D tmp = camera_eye + delta;

	if (tmp.X < -24 || tmp.X > 73 || tmp.Z < -12 || tmp.Z > 43){
		return true;
	}

	for (int i = 8; i < listObjects.size(); i++){
		if (tmp.X >= listObjects[i].xmin && tmp.X <= listObjects[i].xmax && tmp.Z >= listObjects[i].zmin && tmp.Z <= listObjects[i].zmax){
			return true;
		}
	}
	return false;
}

//This function is called when an arrow key is pressed.
void keyboard2(int key, int x, int y) {
	switch(key) {
	case GLUT_KEY_UP:
		camera_eye = isCollision(camera_forward*1.1) ? camera_eye : camera_eye + camera_forward*1.1;
		break;
	case GLUT_KEY_DOWN:
		camera_eye = isCollision(-camera_forward*1.1) ? camera_eye : camera_eye + -camera_forward*1.1;
		break;
	case GLUT_KEY_LEFT:
		camera_up.normalize();
		camera_forward.rotate(camera_up, 0.1);
		camera_forward.normalize();
		break;
	case GLUT_KEY_RIGHT:
		camera_up.normalize();
		camera_forward.rotate(camera_up, -0.1);
		camera_forward.normalize();
		break;
	}

	// -->
	glutPostRedisplay();
}

void reshape(int width, int height){
	Glut_w = width;
	Glut_h = height;
	glm::mat4 projection_matrix = glm::perspective(fovy, Glut_w/(float)Glut_h, zNear, zFar);
	glUniformMatrix4fv(projection_matrix_loc, 1, GL_FALSE, &projection_matrix[0][0]);
	glViewport(0, 0, Glut_w, Glut_h);
}

//This function is called to display objects on screen.
void display() 
{
	//For display the light
	//glUniform1i(renderStyle_loc, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, Glut_w, Glut_h);

	glm::mat4 projection_matrix = 
		glm::perspective(fovy, Glut_w/(float)Glut_h, zNear, zFar);
	glUniformMatrix4fv(projection_matrix_loc, 1, GL_FALSE, &projection_matrix[0][0]);

	glm::mat4 view_matrix = 
		glm::lookAt(glm::vec3(camera_eye.X, camera_eye.Y, camera_eye.Z), 
					glm::vec3(camera_eye.X + camera_forward.dX, camera_eye.Y + camera_forward.dY, camera_eye.Z + camera_forward.dZ), 
					glm::vec3(camera_up.dX, camera_up.dY, camera_up.dZ));
	glUniformMatrix4fv(view_matrix_loc, 1, GL_FALSE, &view_matrix[0][0]);


	glm::mat3 normal_matrix = glm::transpose(glm::inverse(glm::mat3(view_matrix)));
	glUniformMatrix3fv(normal_matrix_loc, 1, GL_FALSE, &normal_matrix[0][0]);
	
	//Light
	glm::vec4 light_position = glm::vec4(0, 5, 5, 1);
	glUniform4fv(mylight_position_loc, 1, &light_position[0]);
	
	glm::vec4 light_color = glm::vec4(1, 1, 1, 0);
	glUniform4fv(mylight_color_loc, 1, &light_color[0]);

	glm::vec3 light_direction = glm::vec3(-2, -1, -1);
	glUniform3fv(mylight_direction_loc, 1, &light_direction[0]);

	glUniform1i(mylight_type_loc, light_type);

	//glUniform1i(tex_loc, 1);

	//Draw object
	glUniform1i(renderStyle_loc, 2);
	for (int i = 0; i < listObjects.size(); i++){
		listObjects[i].displayObject(shaderprogram1, view_matrix);
	}
	/*obj1->displayObject(shaderprogram1, view_matrix);
	obj2->displayObject(shaderprogram1, view_matrix);
	obj3->displayObject(shaderprogram1, view_matrix);
	obj4->displayObject(shaderprogram1, view_matrix);
	obj5->displayObject(shaderprogram1, view_matrix);
	obj6->displayObject(shaderprogram1, view_matrix);
	obj7->displayObject(shaderprogram1, view_matrix);
	obj8->displayObject(shaderprogram1, view_matrix);
	obj9->displayObject(shaderprogram1, view_matrix);
	obj10->displayObject(shaderprogram1, view_matrix);
	obj11->displayObject(shaderprogram1, view_matrix);
	obj12->displayObject(shaderprogram1, view_matrix);
	obj13->displayObject(shaderprogram1, view_matrix);
	obj14->displayObject(shaderprogram1, view_matrix);*/
	
	//Draw the light
	glUniform1i(renderStyle_loc, 4);
	glPointSize(12.0);
	glBegin(GL_POINTS);
	glVertex3f(light_position[0], light_position[1], light_position[2]);
	glEnd();
	/*glBegin(GL_LINES);
	glVertex3f(light_position[0], light_position[1], light_position[2]);
	glVertex3f(light_position[0] + light_direction[0], light_position[1] + light_direction[2], light_position[2] + light_direction[2] );
	glEnd();*/


	//obj1 -> drawNormals();
	/*{
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);

	glDrawElements(GL_TRIANGLES, 12 * 3, GL_UNSIGNED_INT, 0);
	}*/

	glFlush();
}

//This function is called from the main to initalize everything.
void init()
{
    vertexshader = initshaders(GL_VERTEX_SHADER, "shaders/light.vert.glsl") ;
    fragmentshader = initshaders(GL_FRAGMENT_SHADER, "shaders/light.frag.glsl") ;
    shaderprogram1 = initprogram(vertexshader, fragmentshader);

	renderStyle_loc = glGetUniformLocation(shaderprogram1, "myrenderStyle") ;
	glUniform1i(renderStyle_loc, renderStyle);

	projection_matrix_loc = glGetUniformLocation(shaderprogram1, "myprojection_matrix");
	view_matrix_loc = glGetUniformLocation(shaderprogram1, "myview_matrix");
	normal_matrix_loc = glGetUniformLocation(shaderprogram1, "mynormal_matrix");

	mylight_position_loc = glGetUniformLocation(shaderprogram1, "mylight_position");
	mylight_color_loc = glGetUniformLocation(shaderprogram1, "mylight_color");
	mylight_direction_loc = glGetUniformLocation(shaderprogram1, "mylight_direction");
	mylight_type_loc = glGetUniformLocation(shaderprogram1, "mylight_type");

	// 4 murs autours
	myObject3D* obj0 = new myObject3D();
	obj0->readMesh("sol.obj");
	obj0->scale(3, 2, 1);
	obj0->computeNormals();
	obj0->computePlaneTextureCoordinates();
	obj0->computeTangents();
	obj0->createObjectBuffers();
	obj0->texture.readTexture("murs.ppm");
	obj0->bump.readTexture("br-normal.ppm");
	obj0->rotate(0, 1, 0, 90);
	obj0->translate(-25, 10, 15);
	listObjects.push_back(*obj0);

	myObject3D* obj1 = new myObject3D();
	obj1->readMesh("sol.obj");
	obj1->scale(5, 2, 1);
	obj1->computeNormals();
	obj1->computePlaneTextureCoordinates();
	obj1->computeTangents();
	obj1->createObjectBuffers();
	obj1->texture.readTexture("murs.ppm");
	obj1->bump.readTexture("br-normal.ppm");
	obj1->translate(25, 10, -15);
	listObjects.push_back(*obj1);

	myObject3D* obj2 = new myObject3D();
	obj2->readMesh("sol.obj");
	obj2->scale(5, 2, 1);
	obj2->computeNormals();
	obj2->computePlaneTextureCoordinates();
	obj2->computeTangents();
	obj2->createObjectBuffers();
	obj2->texture.readTexture("murs.ppm");
	obj2->bump.readTexture("br-normal.ppm");
	obj2->translate(25, 10, 45);
	listObjects.push_back(*obj2);

	myObject3D* obj3 = new myObject3D();
	obj3->readMesh("sol.obj");
	obj3->scale(3, 2, 1);
	obj3->computeNormals();
	obj3->computePlaneTextureCoordinates();
	obj3->computeTangents();
	obj3->createObjectBuffers();
	obj3->texture.readTexture("murs.ppm");
	obj3->bump.readTexture("br-normal.ppm");
	obj3->rotate(0, 1, 0, 90);
	obj3->translate(75, 10, 15);
	listObjects.push_back(*obj3);

	// Murs sÚparateur
	myObject3D* obj4 = new myObject3D();
	obj4->readMesh("sol.obj");
	obj4->scale(2, 2, 1);
	obj4->computeNormals();
	obj4->computePlaneTextureCoordinates();
	obj4->computeTangents();
	obj4->createObjectBuffers();
	obj4->texture.readTexture("murs.ppm");
	obj4->bump.readTexture("br-normal.ppm");
	obj4->rotate(0, 1, 0, 90);
	obj4->translate(25, 10, 5);
	listObjects.push_back(*obj4);

	// Sol Chambre
	myObject3D* obj5 = new myObject3D();
	obj5->readMesh("sol.obj");
	obj5->computeNormals();
	obj5->computePlaneTextureCoordinates();
	obj5->computeTangents();
	obj5->createObjectBuffers();
	obj5->texture.readTexture("floor.ppm");
	obj5->bump.readTexture("br-normal.ppm");
	obj5->scale(2.5, 3, 1);
	obj5->rotate(1, 0, 0, 90);
	obj5->translate(0, 0, 15); //(y,z,x)
	listObjects.push_back(*obj5);

	// Sol salon
	myObject3D* obj6 = new myObject3D();
	obj6->readMesh("sol.obj");
	obj6->scale(2.5, 3, 1);
	obj6->computeNormals();
	obj6->computePlaneTextureCoordinates();
	obj6->computeTangents();
	obj6->createObjectBuffers();
	obj6->texture.readTexture("table.ppm");
	obj6->bump.readTexture("br-normal.ppm");
	obj6->rotate(1, 0, 0, 90);
	obj6->translate(50, 0, 15); //(x,z,y)
	listObjects.push_back(*obj6);

	// TOIT
	myObject3D* obj7 = new myObject3D();
	obj7->readMesh("sol.obj");
	obj7->scale(5, 3, 1);
	obj7->computeNormals();
	obj7->computePlaneTextureCoordinates();
	obj7->computeTangents();
	obj7->createObjectBuffers();
	obj7->texture.readTexture("roof.ppm");
	obj7->bump.readTexture("wall-normal.ppm");
	obj7->translate(25, 15, -30);
	obj7->rotate(1, 0, 0, 90);
	listObjects.push_back(*obj7);

	/*				OBJECTS				*/

	//Etagere
	myObject3D* obj8 = new myObject3D();
	obj8->readMesh("etagere.obj");
	obj8->computeNormals();
	obj8->computePlaneTextureCoordinates();
	obj8->computeTangents();
	obj8->createObjectBuffers();
	obj8->texture.readTexture("etagere.ppm");
	obj8->bump.readTexture("etagereNormal.ppm");
	obj8->translate(-15, 0, 43);
	listObjects.push_back(*obj8);

	// Etagere deco
	myObject3D* obj9 = new myObject3D();
	obj9->readMesh("deco.obj");
	obj9->computeNormals();
	obj9->computePlaneTextureCoordinates();
	obj9->computeTangents();
	obj9->createObjectBuffers();
	obj9->texture.readTexture("etagere.ppm");
	obj9->bump.readTexture("etagereNormal.ppm");
	obj9->translate(79, -4, 5);
	listObjects.push_back(*obj9);

	// Commode
	myObject3D* obj10 = new myObject3D();
	obj10->readMesh("meuble.obj");
	obj10->computeNormals();
	obj10->computeSphereTextureCoordinates();
	obj10->computeTangents();
	obj10->createObjectBuffers();
	obj10->texture.readTexture("meuble.ppm");
	obj10->bump.readTexture("br-normal.ppm");
	obj10->translate(20, 0, 0);
	listObjects.push_back(*obj10);

	// Canape
	myObject3D* obj11 = new myObject3D();
	obj11->readMesh("canape.obj");
	obj11->computeNormals();
	obj11->computeSphereTextureCoordinates();
	obj11->computeTangents();
	obj11->createObjectBuffers();
	obj11->texture.readTexture("canape.ppm");
	obj11->bump.readTexture("br-normal.ppm");
	obj11->translate(32, 0, 0);
	listObjects.push_back(*obj11);

	// Bureau
	myObject3D* obj12 = new myObject3D();
	obj12->readMesh("desk.obj");
	obj12->computeNormals();
	obj12->computeSphereTextureCoordinates();
	obj12->computeTangents();
	obj12->createObjectBuffers();
	obj12->texture.readTexture("table.ppm");
	obj12->bump.readTexture("br-normal.ppm");
	obj12->translate(65, 0, 40);
	listObjects.push_back(*obj12);

	// Table
	myObject3D* obj13 = new myObject3D();
	obj13->readMesh("table.obj");
	obj13->computeNormals();
	obj13->computeSphereTextureCoordinates();
	obj13->computeTangents();
	obj13->createObjectBuffers();
	obj13->texture.readTexture("table.ppm");
	obj13->bump.readTexture("br-normal.ppm");
	obj13->translate(45, 0, 0);
	listObjects.push_back(*obj13);

	// Lit
	myObject3D* obj14 = new myObject3D();
	obj14->readMesh("bed.obj");
	obj14->computeNormals();
	obj14->computeSphereTextureCoordinates();
	obj14->computeTangents();
	obj14->createObjectBuffers();
	obj14->texture.readTexture("br-diffuse.ppm");
	obj14->bump.readTexture("br-normal.ppm");
	listObjects.push_back(*obj14);


	glUniform1i(glGetUniformLocation(shaderprogram1, "tex"), 1);	
	glUniform1i(glGetUniformLocation(shaderprogram1, "bump"), 2);

	/*{
	GLfloat verts[] = {1,1,1, 1,1,-1, 1,-1,1, 1,-1,-1, -1,1,1, -1,1,-1, -1,-1,1, -1,-1,-1};
	GLuint inds[] = {0,1,3, 0,3,2, 4,5,7, 4,7,6, 0,2,6, 0,6,4, 1,3,7, 1,7,5, 0,1,5, 0,5,4, 2,3,7, 2,7,6};
	vertices = vector<GLfloat> (verts, verts+24);
	indices = vector<GLuint> (inds, inds+36);

	glGenBuffers(2, buffers);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*4, &vertices.front(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*4, &indices.front(), GL_STATIC_DRAW);
	}*/


	glClearColor(0.4, 0.4, 0.4, 0);
}


int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutCreateWindow("My OpenGL Application");
	   
	glewInit() ; 
	glutReshapeWindow(Glut_w, Glut_h);
	
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(keyboard2);
	glutMotionFunc(mousedrag) ;
	glutMouseFunc(mouse) ;
	glutMouseWheelFunc(mouseWheel);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS) ;
	
	glEnable(GL_MULTISAMPLE);

	init();

	glutMainLoop();
	return 0;
}