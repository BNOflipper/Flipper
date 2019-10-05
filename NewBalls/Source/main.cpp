#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <time.h>
#include "ball.h"
#include "flipper.h"
#include "constants.h"
#include "Menu.h"
#include "ShaderUtil.h"
#include <iostream>

constexpr auto X_INIT = 0.2f;
constexpr auto Y_INIT = 0.8f;

GLfloat xAdj = 0.02;
GLfloat Tfps = 1.0f / 60.0f;
GLfloat angleImpact = 0;
GLfloat d = 0;

clock_t t, ts;

using namespace Const;

Ball ball(X_INIT, Y_INIT, 0.0f, 0.0f, 0.1f, 5.0f, 0.01f, 0.6f);
// GLfloat dAxleTip, GLfloat r_left, GLfloat r_right, GLfloat x_axle, GLfloat y_axle, bool clockwise, GLuint numberOfSides
flipper left(0.7f, 0.1f, 0.05f, -0.9f, -0.9f, false, 10);
flipper right(0.7f, 0.1f, 0.05f, 0.9f, -0.9f, true, 10);
Menu menu(NONE);

int const Npins = 12;
int const numberOfSides = 20;
GLfloat pinR = 0.05f;

GLfloat pins[Npins][2] = 
{
	{ -0.9f, 0.6f, },{ -0.3f, 0.6f },{ 0.3f, 0.6f },{ 0.9f, 0.6f },
	{ -0.9f, 0.2f, },{ -0.3f, 0.2f },{ 0.3f, 0.2f },{ 0.9f, 0.2f },
	{ -0.9f, -0.2f, },{ -0.3f, -0.2f },{ 0.3f, -0.2f },{ 0.9f, -0.2f }
};

void Render(GLfloat vertices[], GLuint drawmode, GLuint VAO, GLuint VBO, GLuint N)
{
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 2 * N * sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);

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

	glBindVertexArray(0);
}

void keyBoard(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_REPEAT || action == GLFW_PRESS )
	{
		switch (gameState)
		{
			case START_MENU:
				switch (key)
				{
					case GLFW_KEY_ENTER: menu.Select(); break;
					case GLFW_KEY_UP: menu.Up(); break;
					case GLFW_KEY_DOWN: menu.Down(); break;
					default: break;
				}
				break;

			case HIGHSCORE:
				switch (key)
				{
					case GLFW_KEY_ENTER: gameState = START_MENU; break;
					default: break;
				}
				break;

			case BALL_DROPPING:
				switch (key)
				{
					case GLFW_KEY_RIGHT: ball.AdjustDropPosition(xAdj, 0); break;
					case GLFW_KEY_LEFT:  ball.AdjustDropPosition(-xAdj, 0); break;
					case GLFW_KEY_ENTER: gameState = FLIPPER; break;
					default: break;
				}
				break;

			case FLIPPER:
				switch (key)
				{
					case GLFW_KEY_RIGHT: right.flip(); break;
					case GLFW_KEY_LEFT:  left.flip(); break;
					default: break;
				}
				break;

			case DEAD:
				switch (key)
				{
					case GLFW_KEY_ENTER: 
						ball.ResetPosition(X_INIT, Y_INIT);
						gameState = BALL_DROPPING; break;
					default: break;
				}
			default:
				break;
		} 
	}
}

int main(void)
{
	// Init GLFW
	glfwInit();
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Flipper", nullptr, nullptr);
	glfwSetKeyCallback(window, keyBoard);

	int screenWidth, screenHeight;
	glfwGetFramebufferSize(window, &screenWidth, &screenHeight);

	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();

		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);

	// Initialize GLEW to setup the OpenGL Function pointers
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	ShaderUtil shaderUtil;
	shaderUtil.Load("shaders/vs400.shader", "shaders/fs400.shader");

	GLuint ballVBO, ballVAO,
		   leftAxleVAO, leftAxleVBO, left4PointVAO, left4PointVBO, leftTipVAO, leftTipVBO, 
		   rightAxleVAO, rightAxleVBO, right4PointVAO, right4PointVBO, rightTipVAO, rightTipVBO;
	glGenVertexArrays(1, &ballVAO);
	glGenBuffers(1, &ballVBO);
	glGenVertexArrays(1, &leftAxleVAO);
	glGenBuffers(1, &leftAxleVBO);
	glGenVertexArrays(1, &left4PointVAO);
	glGenBuffers(1, &left4PointVBO);
	glGenVertexArrays(1, &leftTipVAO);
	glGenBuffers(1, &leftTipVBO);
	glGenVertexArrays(1, &rightAxleVAO);
	glGenBuffers(1, &rightAxleVBO);
	glGenVertexArrays(1, &right4PointVAO);
	glGenBuffers(1, &right4PointVBO);
	glGenVertexArrays(1, &rightTipVAO);
	glGenBuffers(1, &rightTipVBO);
	shaderUtil.Use();

	GLfloat *circleVerticesPtr;
	GLfloat circleVertices[Npins][2 * numberOfSides + 4];
	GLuint pinVBO[Npins], pinVAO[Npins];

	for (int p = 0; p < Npins; p++)
	{
		circleVerticesPtr = getCircleVertices(pins[p][0], pins[p][1], pinR, numberOfSides);

		for (int i = 0; i < 2 * numberOfSides + 4; i++)
		{
			circleVertices[p][i] = circleVerticesPtr[i];
		}

		glGenVertexArrays(1, &pinVAO[p]);
		glGenBuffers(1, &pinVBO[p]);
	}

	gameState = BALL_DROPPING;

	// The main game loop
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.2, 0.5, 0.8, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		t = clock();

		switch (gameState)
		{
			case START_MENU:
				menu.Draw(shaderUtil);
				break;

			default:
				for (int p = 0; p < Npins; p++)
				{
					Render(circleVertices[p], 0, pinVAO[p], pinVBO[p], numberOfSides + 2);
				}

				Render(getCircleVertices(ball.getX(), ball.getY(), ball.getR(), numberOfSides), 0, ballVAO, ballVBO, numberOfSides + 2);

				Render(left.Get4PointsVertices(), 2, left4PointVAO, left4PointVBO, 4);
				Render(left.GetPartCircleVertices(true), 0, leftAxleVAO, leftAxleVBO, (numberOfSides + 2) / 2);
				Render(left.GetPartCircleVertices(false), 0, leftTipVAO, leftTipVBO, (numberOfSides + 2) / 2);

			    Render(right.Get4PointsVertices(), 2, right4PointVAO, right4PointVBO, 4);
				Render(right.GetPartCircleVertices(true), 0, rightAxleVAO, rightAxleVBO, (numberOfSides + 2) / 2);
				Render(right.GetPartCircleVertices(false), 0, rightTipVAO, rightTipVBO, (numberOfSides + 2) / 2);
				break;
		}

		// Swap front and back buffers
		glfwSwapBuffers(window);

		// Poll for and process events
		glfwPollEvents();

		// The simulation loop of the flipper game
		for (int iter = 0; iter < (int)(Tfps / Ts); iter++)
		{
			if (gameState == FLIPPER)
			{
				bool hit = false;
				bool leftHit = false;
				bool rightHit = false;
				GLfloat xPos = ball.getX();
				GLfloat yPos = ball.getY();
				GLfloat r = ball.getR();
				GLfloat impulse;
				GLfloat x_bounce;
				GLfloat y_bounce;

				for (int i = 0; i < Npins; i++)
				{
					if (!hit)
					{
						hit = ball.pinCollisionDetect(pins[i][0], pins[i][1], pinR);
					}
				}

				if (!hit)
				{
					hit = ball.wallCollisionDetect();
				}

				if (!hit)
				{
					d = left.getDistance(xPos, yPos);

					if (d < r)
					{
						angleImpact = left.getAngle(xPos, yPos);
						x_bounce = xPos + (r - d) * cos(angleImpact);
						y_bounce = yPos + (r - d) * sin(angleImpact);
						impulse = left.getImpulse(x_bounce, y_bounce);

						if (impulse > 0)
							ball.hit(impulse, x_bounce, y_bounce, angleImpact);
						else
							ball.bounce(angleImpact, x_bounce, y_bounce);
						leftHit = true;
					}
				}

				if (!hit || !leftHit)
				{
					d = right.getDistance(xPos, yPos);

					if (d < r)
					{
						angleImpact = right.getAngle(xPos, yPos);
						x_bounce = xPos + (r - d) * cos(angleImpact);
						y_bounce = yPos + (r - d) * sin(angleImpact);
						impulse = right.getImpulse(x_bounce, y_bounce);

						if (impulse > 0)
							ball.hit(impulse, x_bounce, y_bounce, angleImpact);
						else
							ball.bounce(angleImpact, x_bounce, y_bounce);
						rightHit = true;
					}
				}

				if (!hit || !leftHit || !rightHit)
				{
					ball.midair();
				}

				left.updateFlip(leftHit);
				right.updateFlip(rightHit);
			}
		}

		while (((float)(clock() - t) / CLOCKS_PER_SEC) < Tfps) {};
	}

	shaderUtil.Delete();
	glfwTerminate();
}


