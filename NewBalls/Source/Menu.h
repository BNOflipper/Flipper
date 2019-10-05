#pragma once
#include <GL/glew.h>
#include "Draw.h"
#include <math.h>
#include "constants.h"

using namespace Const;

enum Button
{
	NONE,
	PLAY,
	HIGH_SCORE,
	EXIT
};

class Menu
{
	Button button;

public:
	Menu(Button startButton)
	{
		button = startButton;
	}

	void Up()
	{
		switch (button)
		{
			case NONE:
				button = PLAY;
				break;

			case PLAY:
				button = EXIT;
				break;

			case HIGH_SCORE:
				button = PLAY;
				break;

			case EXIT:
				button = HIGH_SCORE;
				break;
		}
	}

	void Down()
	{
		switch (button)
		{
			case NONE:
				button = PLAY;
				break;

			case PLAY:
				button = HIGH_SCORE;
				break;

			case HIGH_SCORE:
				button = EXIT;
				break;

			case EXIT:
				button = PLAY;
				break;
		}
	}

	void Select()
	{
		switch (button)
		{
			case PLAY:
				gameState = BALL_DROPPING;
				break;

			case HIGH_SCORE:
				gameState = HIGHSCORE;
				break;

			case EXIT:
				glfwTerminate();
				break;
		}
	}

	void Draw(ShaderUtil& s)
	{
		GLfloat x = SCREEN_WIDTH / 4.0f;
		GLfloat width = SCREEN_WIDTH / 2.0f;;
		GLfloat hight = SCREEN_HEIGHT / 8;
		GLfloat spacing = hight / 10.0f;
		GLfloat d = hight / 20.0f;
		GLfloat selectMultiplier = 4.0f;
		GLfloat playBtnFrame = d;
		GLfloat highScoreBtnFrame = d;
		GLfloat exitBtnFrame = d;

		switch (button)
		{
			case PLAY: 
				playBtnFrame *= selectMultiplier;
				break;

			case HIGH_SCORE:
				highScoreBtnFrame *= selectMultiplier;
				break;

			case EXIT:
				exitBtnFrame *= selectMultiplier;

			default:
				break;
		}
		
		drawFrame(x, 3 * hight + 2 * spacing, width, hight, playBtnFrame, s);
		drawFrame(x, 2 * hight + spacing, width, hight, highScoreBtnFrame, s);
		drawFrame(x, hight, width, hight, exitBtnFrame, s);
	}
};