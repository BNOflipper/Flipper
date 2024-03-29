#pragma once
#include <glad/glad.h>
#include <math.h>
#include "constants.h"
#include "shader.h"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace Const;

void Render(GLfloat vertices[], GLuint drawmode, GLuint VAO, GLuint VBO, GLuint N, Shader ourShader)
{
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 2 * N * sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	ourShader.use();

	switch (drawmode)
	{
		case 0:
			glDrawArrays(GL_TRIANGLE_FAN, 0, N);
			break;
		case 1:
			glDrawArrays(GL_TRIANGLES, 0, N);
			break;
		case 2:
			glDrawArrays(GL_QUADS, 0, N);
			break;
		default:
			break;
	}
}

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

void drawRectangle(GLfloat x, GLfloat y, GLfloat width, GLfloat hight)
{
	GLfloat Vertices[8] = 
	{ 
		x, y,
		x + width, y,
		x + width, y + hight,
		x, y + hight
	};
}

void drawFrame(GLfloat x, GLfloat y, GLfloat width, GLfloat hight, GLfloat d)
{
	drawRectangle(x, y, d, hight);
	drawRectangle(x + d, y + hight - d, width - 2 * d, d);
	drawRectangle(x + width - d, y, d, hight);
	drawRectangle(x + d, y, width - 2 * d, d);
}