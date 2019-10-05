#pragma once
#include <GL/glew.h>

namespace Const
{
	const GLfloat Pi(3.14159F);
	const GLfloat halfPi(3.14159F / 2.0F);
	const GLfloat g(8.0F);
	const GLfloat Ts(1.0f / 240.0f);
	const GLfloat SCREEN_HEIGHT(800.0f);
	const GLfloat SCREEN_WIDTH(800.0f);

	enum State
	{
		START_MENU,
		HIGHSCORE,
		BALL_DROPPING,
		FLIPPER,
		DEAD
	}gameState;
}