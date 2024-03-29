#pragma once
#include <glad/glad.h>

namespace Const
{
	const GLfloat Pi(3.14159F);
	const GLfloat halfPi(3.14159F / 2.0F);
	const GLfloat g(9.0F);
	const GLfloat Ts(1.0f / 240.0f);

	enum State
	{
		START_MENU,
		HIGHSCORE,
		BALL_DROPPING,
		FLIPPER,
		DEAD
	}gameState;
}