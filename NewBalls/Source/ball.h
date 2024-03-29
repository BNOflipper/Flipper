#pragma once
#include "Draw.h"
#include <glad/glad.h>
#include <math.h>
#include "constants.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace Const;

class Ball 
{
	GLfloat m;
	GLfloat c;
	GLfloat vx;
	GLfloat vy;
	GLfloat x;
	GLfloat y;
	GLfloat r;
	GLfloat A;
	int numberOfSides;

public:

	Ball(GLfloat x_init, GLfloat y_init, GLfloat vx_init, GLfloat vy_init,
		 GLfloat radius, GLfloat mass, GLfloat c_air, GLfloat a, int numberOfSides)
	{
		m = mass;
		r = radius;
		x = x_init;
		y = y_init;
		vx = vx_init;
		vy = vy_init;
		c = c_air / mass;
		A = a;
		this->numberOfSides = numberOfSides;
	}

	void midair()
	{
		vy -= Ts * (c*vy + g);
		vx -= Ts * c*vx;

		y += Ts * vy;
		x += Ts * vx;
	}

	void bounce(glm::vec2 normal, GLfloat x_bounce, GLfloat y_bounce, GLfloat displacement)
	{
		glm::vec2 refl = A * glm::reflect(glm::vec2(vx, vy), normal);
		vx = refl.x;
		vy = refl.y;

		glm::vec2 displacementVector = displacement * normal;
		x += displacementVector.x;
		y += displacementVector.y;;
	}

	void hit(GLfloat I, glm::vec2 normal, GLfloat x_bounce, GLfloat y_bounce)
	{
		vx = I * normal.x / m + vx;
		vy = I * normal.y / m + vy;
	}

	bool pinCollisionDetect(GLfloat pinx, GLfloat piny, GLfloat pinr)
	{
		glm::vec2 normal(x - pinx, y - piny);
		GLfloat normalLength = glm::length(normal);

		if (normalLength < pinr + r)
		{
			GLfloat displacement = pinr + r - normalLength + 0.001f;
			bounce(glm::normalize(normal), vx, vy, displacement);

			return true;
		}

		return false;
	}

	bool wallCollisionDetect()
	{
		if (x < r - 1.0f)
		{
			bounce(glm::vec2(1.0f, 0.0f), r - 1.0f, y, 0.001f);
		}
		else if (x > 1.0f  - r)
		{
			bounce(glm::vec2(-1.0f, 0.0f), 1.0f - r, y, 0.001f);
		}
		else if (y < -1.0f + r)
		{
			//gameState = DEAD;
			bounce(glm::vec2(0.0f, 1.0f), x, y, 0.001f);
		}
		else if (y > 1.0f - r) 
		{
			bounce(glm::vec2(0.0f, -1.0f), x, 1.0f - r, 0.001f);
		}
		else
		{
			return false;
		}

		return true;
	}

	GLfloat * getBallCircleVertices()
	{
		return getCircleVertices(x, y, r, numberOfSides);
	}

	void AdjustDropPosition(GLfloat xAdj, GLfloat yAdj)
	{
		this->x += xAdj;
		this->y += yAdj;
	}

	void ResetPosition(GLfloat x, GLfloat y)
	{
		this->x = x;
		this->y = y;
		vx = 0;
		vy = 0;
	}

	GLfloat getX()
	{
		return x;
	}

	GLfloat getY()
	{
		return y;
	}

	GLfloat getR()
	{
		return r;
	}
};