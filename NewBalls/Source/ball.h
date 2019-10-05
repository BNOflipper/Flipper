#pragma once
#include <GL/glew.h>
#include "Draw.h"
#include <math.h>
#include "constants.h"
#include "ShaderUtil.h"
#include <iostream>

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
	GLfloat tMidAir;

public:

	Ball(GLfloat x_init, GLfloat y_init, GLfloat vx_init, GLfloat vy_init, GLfloat radius, GLfloat mass, GLfloat c_air, GLfloat a) 
	{
		m = mass;
		r = radius;
		x = x_init;
		y = y_init;
		vx = vx_init;
		vy = vy_init;
		c = c_air;
		A = a;
		tMidAir = 0;
	}

	void midair()
	{
		vy -= (Ts / m) * (c*vy + m * g);
		vx -= Ts * c*vx / m;

		y += Ts * vy;
		x += Ts * vx;

		tMidAir += Ts;
	}

	void bounce(GLfloat theta, GLfloat x_bounce, GLfloat y_bounce)
	{
		GLfloat v;
		GLfloat rollAngle;
		GLfloat ballDir = atan2(vy, vx);
		GLfloat dirAngle = -ballDir + 2 * theta - Pi;
		v = sqrt(pow(vx, 2) + pow(vy, 2));
		vy = v * sin(dirAngle);
		vx = v * cos(dirAngle);

		if (tMidAir > 0.05f || theta < 0)
		{
			vy *= A;
			vx *= A;
		}
		else
		{
			GLfloat F_roll = -0.01f * c;
			if (theta <= halfPi)
			{
				rollAngle = theta - halfPi;
			}
			else
			{
				rollAngle = theta + halfPi;
			}

			vx += Ts / m * (F_roll*vx + m * g * cos(rollAngle) * fabs(sin(rollAngle)));
			vy += Ts / m * (F_roll*vy + m * g * fabs(sin(rollAngle)) * sin(rollAngle));
		}

		x = x_bounce + Ts * vx;
		y = y_bounce + Ts * vy;
		tMidAir = 0;
	}

	void hit(GLfloat I, GLfloat theta, GLfloat x_bounce, GLfloat y_bounce)
	{
		vx = I*cos(theta) / m + vx;
		vy = I*sin(theta) / m + vy;
		x = x_bounce + Ts * vx;
		y = y_bounce + Ts * vy;
		tMidAir = 0;
	}

	bool pinCollisionDetect(GLfloat pinx, GLfloat piny, GLfloat pinr)
	{
		GLfloat d = sqrtf(powf(pinx - x, 2.0f) + powf(piny - y, 2.0f));

		if (d <= pinr + r)
		{
			GLfloat theta = atan2(y - piny, x - pinx);
			bounce(theta, pinx + (pinr + r) * cos(theta), piny + (pinr + r)  * sin(theta));
			return true;
		}

		return false;
	}

	bool wallCollisionDetect()
	{
		GLfloat theta;
		if (x <= r - 1.0f)
		{
			theta = 0;
			bounce(theta, r - 1.0f, y);
		}
		else if (x >= 1.0f  - r)
		{
			theta = Pi;
			bounce(theta, 1.0f - r, y);
		}
		else if (y <= -1.0f + r)
		{
			gameState = DEAD;
		}
		else if (y >= 1.0f - r) {
			theta = -Pi / 2.0f;
			bounce(theta, x, 1.0f - r);
		}
		else
		{
			return false;
		}
		return true;
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



