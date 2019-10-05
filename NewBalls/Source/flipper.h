#pragma once
#include <GL/glew.h>
#include "Draw.h"
#include <math.h>
#include "constants.h"

using namespace Const;

class flipper 
{
	GLfloat x;
	GLfloat xTip;
	GLfloat y;
	GLfloat yTip;
	GLfloat r;
	GLfloat rTip;
	GLfloat d;
	GLfloat angle;
	GLfloat angleSpeed;
	GLfloat e;
	GLfloat axleVertices[100];
	GLfloat middleVertices[8];
	GLfloat tipVertices[100];
	GLuint nSides;
	GLuint nVertices;
	bool clockWise;
	bool moving;

public:
	flipper(GLfloat dAxleTip, GLfloat r_left, GLfloat r_right, GLfloat x_axle, GLfloat y_axle, bool clockwise, GLuint numberOfSides)
	{
		x = x_axle;
		y = y_axle;
		yTip = y_axle;
		r = r_left;
		rTip = r_right;
		d = dAxleTip;
		angleSpeed = 0;
		moving = false;
		clockWise = clockwise;
		e = getEdge();
		nSides = numberOfSides;

		if (clockwise)
		{
			angle = Pi;
			xTip = x - d;
		}
		else
		{
			angle = 0;
			xTip = x + d;
		}

		nVertices = nSides + 2;

		// Define local vertices for axle half circle
		axleVertices[0] = 0.0f;
		axleVertices[1] = 0.0f;
		axleVertices[2] = 0.0f;
		axleVertices[3] = r;

		for (int i = 2; i < nVertices; i++)
		{
			axleVertices[2 * i] = r * cos(halfPi + i * Pi / nSides);
			axleVertices[2 * i + 1] = r * sin(halfPi + i * Pi / nSides);
		}

		// Define middle frame verticess
		middleVertices[0] = 0.0f;
		middleVertices[1] = r;
		middleVertices[2] = d;
		middleVertices[3] = rTip;
		middleVertices[4] = d;
		middleVertices[5] = -rTip;
		middleVertices[6] = 0.0f;
		middleVertices[7] = -r;

		// Define vertices for tip half circle
		tipVertices[0] = 0.0f;
		tipVertices[1] = 0.0f;
		tipVertices[2] = 0.0f;
		tipVertices[3] = -rTip;

		for (int i = 2; i < nVertices; i++)
		{
			tipVertices[2 * i] = rTip * cos(-halfPi + i * Pi / nSides);
			tipVertices[2 * i + 1] = rTip * sin(-halfPi + i * Pi / nSides);
		}
	}

	GLfloat *  Get4PointsVertices()
	{
		static GLfloat vertices[8];

		for (int i = 0; i < 4; i++)
		{
			vertices[2 * i] = x + middleVertices[2 * i] * cos(angle) - middleVertices[2 * i + 1] * sin(angle);
			vertices[2 * i + 1] = y + middleVertices[2 * i] * sin(angle) + middleVertices[2 * i + 1] * cos(angle);
		}

		return vertices;
	}

	GLfloat * GetPartCircleVertices(bool select)
	{
		static GLfloat vertices[100];

		if (select)
		{
			for (int i = 0; i < nVertices; i++)
			{
				vertices[2 * i] = x + axleVertices[2 * i] * cos(angle) - axleVertices[2 * i + 1] * sin(angle);
				vertices[2 * i + 1] = y + axleVertices[2 * i] * sin(angle) + axleVertices[2 * i + 1] * cos(angle);
			}
		}
		else
		{
			for (int i = 0; i < nVertices; i++)
			{
				vertices[2 * i] = xTip + tipVertices[2 * i] * cos(angle) - tipVertices[2 * i + 1] * sin(angle);
				vertices[2 * i + 1] = yTip + tipVertices[2 * i] * sin(angle) + tipVertices[2 * i + 1] * cos(angle);
			}
		}

		return vertices;
	}

    void flip()
	{
		if (!clockWise)
		{
			angleSpeed = (Pi / 4.0f - angle) * 20.0f;
		}
		else
		{
			angleSpeed = (3.0f * Pi / 4.0f - angle) * 20.0f;
		}
		moving = true;
	}

	void updateFlip(bool flipperHit)
	{
		if (moving)
		{
			GLfloat angleAcc = -0.05f * copysign(1.0f, cos(angle));

			if (!flipperHit)
			{
				angleSpeed += angleAcc;
			}
			else
			{
				angleSpeed += 10 * angleAcc;
			}

			if (angle < 0.0f || angle > Pi)
			{
				angleSpeed = 0.0f;
				moving = false;
				if (!clockWise)
				{
					angle = 0.0f;
				}
				else
				{
					angle = Pi;
				}
			}
			else
			{
				angle += Ts * angleSpeed;
				if (angle > Pi / 4.0f && !clockWise)
				{
					angle = Pi / 4.0f;
					angleSpeed = 0.0f;
				}
				else if (angle < 3.0f * Pi / 4.0f && clockWise)
				{
					angle = 3.0f * Pi / 4.0f;
					angleSpeed = 0.0f;
				}
			}

			xTip = x + d * cos(angle);
			yTip = y + d * sin(angle);
		}
	}

	GLfloat getDistance(GLfloat x_in, GLfloat y_in)
	{
		GLfloat K, Y0, angleTip, xMin, distance, dx, y0;
		e = getEdge();

		if (!clockWise && (x_in < e))
		{
			dx = x_in - x;
			y0 = y + r;

			if (x_in < e - rTip * cos(angle))
			{
			    K = (yTip + rTip - y0) / d;
				xMin = (dx + K * (y_in - y0)) / (powf(K, 2.0f) + 1.0f);
				distance = sqrt(powf(dx - xMin, 2.0f) + powf(y_in - K * xMin - y0, 2.0f));
			}
			else
			{
				angleTip = atan2(y_in - yTip, x_in - xTip);
				distance = sqrt(pow(xTip + rTip * cos(angleTip) - x_in, 2.0f) + pow(yTip + rTip * sin(angleTip) - y_in, 2.0f));
			}
		}
		else if (clockWise && (x_in > e))
		{
			dx = x_in - xTip;
			y0 = yTip + rTip;

			if (x_in > e - rTip * cos(angle))
			{
				K = (y + r - y0) / d;
				xMin = (dx + K * (y_in - y0)) / (powf(K, 2.0f) + 1.0f);
				distance = sqrt(powf(dx - xMin, 2.0f) + powf(y_in - K * xMin - y0, 2.0f));
			}
			else
			{
				angleTip = atan2(y_in - yTip, x_in - xTip);
				distance = sqrt(pow(xTip + rTip * cos(angleTip) - x_in, 2.0f) + pow(yTip + rTip * sin(angleTip) - y_in, 2.0f));
			}
		}
		else
		{
			distance = 1.0f;
		}

		return distance;
	}

	GLfloat getAngle(GLfloat x_in, GLfloat y_in)
	{
		GLfloat halfPi = Pi / 2.0f;
		GLfloat angleSurf, angleImpact;
		if (((x_in < e - rTip * cos(angle)) && !clockWise) || ((x_in > e + rTip * cos(angle)) && clockWise))
		{
			angleSurf = atan2(yTip + rTip - y - r, xTip - x);
			if (angleSurf >= -halfPi && angleSurf <= halfPi)
			{
				angleImpact = angleSurf + halfPi;
			}
			else if (angleSurf > halfPi)
			{
				angleImpact = angleSurf - halfPi;
			}
			else
			{
				angleImpact = angleSurf + 3.0f * halfPi;
			}
			/*else
			{
				angleSurf = atan2(yTip - rTip - y + r, xTip - x);
				if (angleSurf >= -halfPi && angleSurf <= halfPi)
				{
					angleImpact = angleSurf - halfPi;
				}
				else if (angleSurf > halfPi)
				{
					angleImpact = angleSurf - 3 * halfPi;
				}
				else
				{
					angleImpact = angleSurf - halfPi;;
				}
			}*/
		}
		else
		{
			angleImpact = atan2(y_in - yTip, x_in - xTip);
		}

		return angleImpact;
	}

	GLfloat getImpulse(GLfloat x_bounce, GLfloat y_bounce)
	{
		GLfloat I;
		GLfloat dAxle;
		if (((angleSpeed < 2.0) && !clockWise) || ((angleSpeed > -2.0f) && clockWise))
		{
			I = 0;
		}
		else
		{
			dAxle = sqrt(pow(fabs(x_bounce - x), 2.0) + pow(fabs(y_bounce - y), 2.0));
			I = dAxle * 0.00001f * fabs(angleSpeed);
		}
		return I;
	}

	GLfloat getEdge()
	{
		return xTip + rTip * cos(angle);
	}
};