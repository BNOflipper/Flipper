#pragma once
#include <glad/glad.h>
#include "Draw.h"
#include <math.h>
#include "constants.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace Const;

class flipper
{
	GLfloat x_axle;
	GLfloat y_axle;
	GLfloat r_axle;
	GLfloat x_tip;
	GLfloat y_tip;
	GLfloat r_tip;
	GLfloat dAxleTip;
	GLfloat angle;
	GLfloat angleSpeed;
	GLfloat angleMax;
	GLfloat angleMin;
	GLfloat axleVertices[100];
	GLfloat middleVertices[8];
	GLfloat tipVertices[100];
	GLfloat globalAxleVertices[100];
	GLfloat globalMiddleVertices[8];
	GLfloat globalTipVertices[100];
	GLfloat displacements;
	GLuint nSides;
	GLuint nVertices;
	glm::vec2 closestNormal;
	glm::vec2 normal[2];
	glm::vec2 origo[2];
	glm::vec2 surfaces[2];
	bool moving;
	bool hit;

public:
	flipper(GLfloat dAxleTip, GLfloat max_thickness, GLfloat min_thickness,
		    GLfloat x_axle, GLfloat y_axle, GLfloat angle_init, GLuint numberOfSides)
	{
		this->x_axle = x_axle;
		this->y_axle = y_axle;
		r_axle = max_thickness / 2.0f;
		r_tip = min_thickness / 2.0f;
		this->dAxleTip = dAxleTip;
		angleSpeed = 0;
		angle = angle_init;
		nSides = numberOfSides;
		x_tip = x_axle + dAxleTip * cos(angle_init);
		y_tip = y_axle + dAxleTip * sin(angle_init);
		moving = false;
		hit = false;

		if (angle_init < halfPi)
		{
			angleMin = angle_init;
			angleMax = angle_init + Pi / 4.0f;
		}
		else
		{
			angleMax = angle_init;
			angleMin = angle_init - Pi / 4.0f;
		}

		nVertices = nSides + 2;

		// Define local vertices for axle half circle
		axleVertices[0] = 0.0f;
		axleVertices[1] = 0.0f;
		axleVertices[2] = 0.0f;
		axleVertices[3] = r_axle;

		for (int i = 2; i < nVertices; i++)
		{
			axleVertices[2 * i] = r_axle * cos(halfPi + i * Pi / nSides);
			axleVertices[2 * i + 1] = r_axle * sin(halfPi + i * Pi / nSides);
		}

		// Define middle frame verticess
		middleVertices[0] = 0.0f;
		middleVertices[1] = r_axle;
		middleVertices[2] = dAxleTip;
		middleVertices[3] = r_tip;
		middleVertices[4] = dAxleTip;
		middleVertices[5] = -r_tip;
		middleVertices[6] = 0.0f;
		middleVertices[7] = -r_axle;

		// Define vertices for tip half circle
		tipVertices[0] = 0.0f;
		tipVertices[1] = 0.0f;
		tipVertices[2] = 0.0f;
		tipVertices[3] = -r_tip;

		for (int i = 2; i < nVertices; i++)
		{
			tipVertices[2 * i] = r_tip * cos(-halfPi + i * Pi / nSides);
			tipVertices[2 * i + 1] = r_tip * sin(-halfPi + i * Pi / nSides);
		}

		updateVertices();
	}

	GLfloat *  Get4PointsVertices()
	{
		return globalMiddleVertices;
	}

	GLfloat * GetPartCircleVertices(bool select)
	{
		if (select)
		{
			return globalAxleVertices;
		}
		else
		{
			return globalTipVertices;
		}
	}

    void flip()
	{
		if (angle < Pi / 4.0f)
		{
			angleSpeed = (Pi / 4.0f - angle) * 20.0f;
		}
		else
		{
			angleSpeed = (3.0f * Pi / 4.0f - angle) * 20.0f;
		}

		moving = true;
	}

	void updateFlip()
	{
		if (moving)
		{
			GLfloat angleAcc = -0.05f * copysign(1.0f, cos(angle));

			if (!hit)
			{
				angleSpeed += angleAcc;
			}
			else
			{
				angleSpeed += 10 * angleAcc;
			}

			angle += Ts * angleSpeed;

			if (angle > angleMax)
			{
				angle = angleMax;
				angleSpeed = 0.0f;
			}
			else if (angle < angleMin)
			{
				angle = angleMin;
				angleSpeed = 0.0f;
			}

			updateVertices();
		}
	}

	void updateVertices()
	{
		x_tip = x_axle + dAxleTip * cos(angle);
		y_tip = y_axle + dAxleTip * sin(angle);

		for (int i = 0; i < 4; i++)
		{
			globalMiddleVertices[2 * i] = x_axle + middleVertices[2 * i] * cos(angle) - middleVertices[2 * i + 1] * sin(angle);
			globalMiddleVertices[2 * i + 1] = y_axle + middleVertices[2 * i] * sin(angle) + middleVertices[2 * i + 1] * cos(angle);
		}

		for (int i = 0; i < nVertices; i++)
		{
			globalAxleVertices[2 * i] = x_axle + axleVertices[2 * i] * cos(angle) - axleVertices[2 * i + 1] * sin(angle);
			globalAxleVertices[2 * i + 1] = y_axle + axleVertices[2 * i] * sin(angle) + axleVertices[2 * i + 1] * cos(angle);
		}

		for (int i = 0; i < nVertices; i++)
		{
			globalTipVertices[2 * i] = x_tip + tipVertices[2 * i] * cos(angle) - tipVertices[2 * i + 1] * sin(angle);
			globalTipVertices[2 * i + 1] = y_tip + tipVertices[2 * i] * sin(angle) + tipVertices[2 * i + 1] * cos(angle);
		}

		origo[0] = glm::vec2(globalMiddleVertices[0], globalMiddleVertices[1]);
		origo[1] = glm::vec2(globalMiddleVertices[6], globalMiddleVertices[7]);

		surfaces[0] = glm::vec2(globalMiddleVertices[2] - origo[0].x, globalMiddleVertices[3] - origo[0].y);
		surfaces[1] = glm::vec2(globalMiddleVertices[4] - origo[1].x, globalMiddleVertices[5] - origo[1].y);

		normal[0] = glm::normalize(glm::vec2(-surfaces[0].y, surfaces[0].x));
		normal[1] = glm::normalize(glm::vec2(surfaces[1].y, -surfaces[1].x));
	}

	bool FlipperCollisionDetect(GLfloat x_in, GLfloat y_in, GLfloat min_distance)
	{
		for (int i = 0; i < 2; i++)
		{
			glm::vec2 localVector(x_in - origo[i].x, y_in - origo[i].y);
			GLfloat surfPosition = glm::dot(surfaces[i], localVector) / dAxleTip;

			if (surfPosition < dAxleTip && surfPosition > 0)
			{
				GLfloat distance = abs(glm::dot(normal[i], localVector));

				if (distance < min_distance)
				{
					closestNormal = normal[i];
					displacements = min_distance - distance + 0.001f;
					hit = true;
					return true;
				}
			}
		}

		glm::vec2 normalTip(x_in - x_tip, y_in - y_tip);
		GLfloat normalLength = glm::length(normalTip);

		if (glm::length(normalTip) < min_distance + r_tip)
		{
			closestNormal = glm::normalize(normalTip);
			displacements = r_tip + min_distance - normalLength + 0.001f;
			hit = true;
			return true;
		}

		hit = false;
		return false;
	}

	GLfloat getImpulse(GLfloat x_bounce, GLfloat y_bounce)
	{
		GLfloat I;
		GLfloat dAxle;

		if (((angleSpeed < 0.0f) && angle < halfPi) || ((angleSpeed > 0.0f) && angle > halfPi))
		{
			I = 0;
		}
		else
		{
			dAxle = sqrt(pow(fabs(x_bounce - x_axle), 2.0) + pow(fabs(y_bounce - y_axle), 2.0));
			I = 5.0f * dAxle * fabs(angleSpeed);
		}

		return I;
	}

	glm::vec2 getNormal()
	{
		return closestNormal;
	}

	GLfloat getDisplacement()
	{
		return displacements;
	}
};