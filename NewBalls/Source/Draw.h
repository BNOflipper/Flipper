#pragma once
#include <GL/glew.h>
#include <math.h>
#include "ShaderUtil.h"
#include <array>
#include "constants.h"
#include <iostream>

using namespace Const;

GLfloat * getCircleVertices(GLfloat x, GLfloat y, GLfloat r, GLint numberOfSides)
{
	GLint numberOfVertices = numberOfSides + 2;
	static GLfloat circleVertices[100];

	circleVertices[0] = x;
	circleVertices[1] = y;

	for (int i = 1; i < numberOfVertices; i++)
	{
		circleVertices[2 * i] = x + (r * cos(i *  2.0f * Pi / numberOfSides));
		circleVertices[2 * i + 1] = y + (r * sin(i * 2.0f * Pi / numberOfSides));
	}

	return circleVertices;
}

void drawRectangle(GLfloat x, GLfloat y, GLfloat width, GLfloat hight, ShaderUtil& s)
{

	GLfloat Vertices[8] = 
	{ 
		x, y,
		x + width, y,
		x + width, y + hight,
		x, y + hight
	};
}

void drawFrame(GLfloat x, GLfloat y, GLfloat width, GLfloat hight, GLfloat d, ShaderUtil& s)
{
	drawRectangle(x, y, d, hight, s);
	drawRectangle(x + d, y + hight - d, width - 2 * d, d, s);
	drawRectangle(x + width - d, y, d, hight, s);
	drawRectangle(x + d, y, width - 2 * d, d, s);
}