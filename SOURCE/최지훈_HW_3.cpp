#include <Windows.h>

#define _USE_MATH_DEFINES
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)

#include <cmath>

#include "glSetup.h"

#include "include/GLFW/glfw3.h"
#include "include/GL/gl.h"
#include "include/GL/glut.h"

#include "include/glm/glm.hpp" //OpenGl Mathematics
#include "include/glm/gtc/type_ptr.hpp" //glm::value_ptr()
#include "include/glm/gtc/matrix_transform.hpp"

#pragma comment(lib, "lib/glfw3.lib")
#pragma comment(lib, "lib/opengl32.lib")
#pragma comment(lib, "lib/glut32.lib")

using namespace glm;

#include <iostream>
using namespace std;

#include <fstream>

// Camera configuration
vec3   eye(3, 5, 6);
vec3   center(0, 0, 0);
vec3   up(0, 1, 0);

// Light configuration
vec4   light(1.0,1.5,1.5, 0);   // Light position

float AXIS_LENGTH = 2;
float AXIS_LINE_WIDTH = 2;

GLfloat bgColor[4] = { 1, 1, 1, 1 };

//Selected example
int selection = 0;

vec4 p[36][18];

void 
loadRawTexture(const char * filename)
{
	unsigned char* data;
	data = new unsigned char[512 * 512 * 3];
	FILE* file;
	file = fopen(filename, "rb");

	fread(data, 512 * 512 * 3, 1, file);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	fclose(file);
	delete data;
}
GLuint texID[3]; 

void
drawShadedTorus() {
	glBegin(GL_QUADS);
	for (int i = 0; i < 36; i++)
	{
		for (int j = 0; j < 18; j++)
		{
			
			glNormal3f(p[i][j].x, p[i][j].y, p[i][j].z);
			glTexCoord2f(i / 36.0, j / 18.0);
			glVertex3f(p[i][j].x, p[i][j].y, p[i][j].z);

			glNormal3f(p[(i + 1) % 36][j].x, p[(i + 1) % 36][j].y, p[(i + 1) % 36][j].z);
			glTexCoord2f((i + 1) / 36.0, j / 18.0);
			glVertex3f(p[(i + 1) % 36][j].x, p[(i + 1) % 36][j].y, p[(i + 1) % 36][j].z);

			glNormal3f(p[(i + 1) % 36][(j + 1) % 18].x, p[(i + 1) % 36][(j + 1) % 18].y, p[(i + 1) % 36][(j + 1) % 18].z);
			glTexCoord2f((i + 1) / 36.0, (j + 1) / 18.0);
			glVertex3f(p[(i + 1) % 36][(j + 1) % 18].x, p[(i + 1) % 36][(j + 1) % 18].y, p[(i + 1) % 36][(j + 1) % 18].z);

			glNormal3f(p[i][(j + 1) % 18].x, p[i][(j + 1) % 18].y, p[i][(j + 1) % 18].z);
			glTexCoord2f(i / 36.0, (j + 1) / 18.0);
			glVertex3f(p[i][(j + 1) % 18].x, p[i][(j + 1) % 18].y, p[i][(j + 1) % 18].z);
			
		}
	}
	glEnd();
}

void
setTorusPoints()
{
	vec3 cenP(1.2, 1.2, 0.0);
	vec3 sP(0.5, 0, 0);
	vec3 yAxis(0, 1, 0);
	vec3 zAxis(0, 0, 1);

	float rTheta = 0.0f;
	float yrTheta = 0.0f;

	vec4 rV;
	mat4 tM = translate(mat4(1.0), cenP);

	for (int i = 0; i < 18; i++)
	{
		vec4 sP4(0.4, 0.0, 0.0, 1.0);

		mat4 R = rotate(mat4(1.0), radians(20.0f*i), zAxis);

		vec4 rotatedSP = R * sP4;

		rV = tM * rotatedSP;
		p[0][i] = rV;

	}

	for (int j = 0; j < 36; j++)
	{
		for (int i = 0; i < 18; i++)
		{
			mat4 R2 = rotate(mat4(1.0), radians(10.f*j), yAxis);
			vec4 rotateP(p[0][i].x, p[0][i].y, p[0][i].z, 1.0);
			vec4 rotatedCircle = R2 * rotateP;
			p[j][i] = rotatedCircle;
			glNormal3f(p[j][i].x, p[j][i].y, p[j][i].z);
			glVertex3f(p[j][i].x, p[j][i].y, p[j][i].z);
		}
	}
}


//Light
void
setupLight()
{
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	GLfloat ambient[4] = { 0.1, 0.1, 0.1, 1 };
	GLfloat diffuse[4] = { 1.0, 1.0, 1.0, 1 };
	GLfloat specular[4] = { 1.0, 1.0, 1.0, 1 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0, GL_POSITION, value_ptr(light));
}

//Material
void
setupMaterial()
{
	//Make it possible to change a subset of material parameters
	glColorMaterial(GL_FRONT, GL_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	//Material
	GLfloat mat_ambient[4] = { 0.1, 0.1, 0.1, 1 };
	GLfloat mat_specular[4] = { 0.5, 0.5, 0.5, 1 };
	GLfloat mat_shininess = 25;

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
}


void
init()
{
	glEnable(GL_TEXTURE_2D);
	glGenTextures(3, texID);

	glBindTexture(GL_TEXTURE_2D, texID[0]);
	glBindTexture(GL_TEXTURE_2D, texID[1]);
	glBindTexture(GL_TEXTURE_2D, texID[2]);

	//Keyboard
	cout << "Keyboard input: 1 key to select marble texture" << endl;
	cout << "Keyboard input: 2 key to select wood texture" << endl;
	cout << "Keyboard input: 3 key to select raw texture" << endl;
}


void
render(GLFWwindow* window)
{
	//Background color
	glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(eye[0], eye[1], eye[2], center[0], center[1], center[2], up[0], up[1], up[2]);
	
	//Axes
	glDisable(GL_LIGHTING);
	drawAxes(AXIS_LENGTH, AXIS_LINE_WIDTH*dpiScaling);

	glColor3f(1, 1, 1);

	setupLight();

	setupMaterial();

	glEnable(GL_TEXTURE_2D);
	setTorusPoints();
	drawShadedTorus();
	glDisable(GL_TEXTURE_2D);

	switch (selection)
	{
	case 0:
		drawShadedTorus();
		break;
	case 1:
		loadRawTexture("Material/marble.raw");
		break;
	case 2: 
		loadRawTexture("Material/wood.raw");
		break;
	case 3: 
		loadRawTexture("Material/check.raw");
		break;
	}
}


void
keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		switch (key)
		{
			//Quit
		case GLFW_KEY_Q:
		case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GL_TRUE); break;

		case GLFW_KEY_1: selection = 1; break;
		case GLFW_KEY_2: selection = 2; break;
		case GLFW_KEY_3: selection = 3; break;
		}
	}
}

int
main(int argc, char* argv[])
{

	//Initialize the OpenGL system
	GLFWwindow* window = initializeOpenGL(argc, argv, bgColor);
	if (window == NULL) return -1;

	//Callbacks
	glfwSetKeyCallback(window, keyboard);

	//Depth test
	glEnable(GL_DEPTH_TEST);

	//Normal vectors are normalized after transformation.
	glEnable(GL_NORMALIZE);

	//Viewport and perspective setting
	reshape(window, windowW, windowH);

	//Initializeation - Main loop - Finalization
	init();

	//Main loop
	float previous = glfwGetTime();
	float elapsed = 0;
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents(); //Events

						  //Time apssed during a single loop
		float now = glfwGetTime();
		float delta = now - previous;
		previous = now;

		//Time passed after the previous frame
		elapsed += delta;

		render(window); //Draw one frame
		glfwSwapBuffers(window); //Swap buffers
	}

	//Terminate the glfw system
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}