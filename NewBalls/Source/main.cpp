#pragma once
#include <iostream>
#include <map>
#include <string>
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <math.h>
#include "Draw.h"
#include <time.h>
#include "ball.h"
#include "flipper.h"
#include "constants.h"
#include "Menu.h"
#include "shader.h"
#include "Text.h"

constexpr auto X_INIT = 0.2f;
constexpr auto Y_INIT = 0.8f;
const GLfloat SCREEN_HEIGHT(800.0f);
const GLfloat SCREEN_WIDTH(800.0f);

GLfloat xAdj = 0.02;
GLfloat Tfps = 1.0f / 60.0f;
GLfloat angleImpact = 0;
GLfloat d = 0;

clock_t t, ts;

using namespace Const;
int const numberOfSides = 20;

Ball ball(X_INIT, Y_INIT, 0.0f, 0.0f, 0.1f, 5.0f, 0.1f, 0.9f, numberOfSides);
flipper left(0.7f, 0.2f, 0.05f, -0.9f, -0.9f, 0, 10);
flipper right(0.7f, 0.2f, 0.05f, 0.9f, -0.9f, Pi, 10);
Menu menu(NONE);

int const Npins = 12;
GLfloat pinR = 0.05f;

GLfloat pins[Npins][2] = 
{
	{ -0.9f, 0.6f, },{ -0.3f, 0.6f },{ 0.3f, 0.6f },{ 0.9f, 0.6f },
	{ -0.9f, 0.2f, },{ -0.3f, 0.2f },{ 0.3f, 0.2f },{ 0.9f, 0.2f },
	{ -0.9f, -0.2f, },{ -0.3f, -0.2f },{ 0.3f, -0.2f },{ 0.9f, -0.2f }
};

GLuint ballVBO, ballVAO,
	   leftAxleVAO, leftAxleVBO, left4PointVAO, left4PointVBO, leftTipVAO, leftTipVBO,
	   rightAxleVAO, rightAxleVBO, right4PointVAO, right4PointVBO, rightTipVAO, rightTipVBO;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
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
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Flipper", nullptr, nullptr);
	glfwSetKeyCallback(window, keyBoard);

	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();

		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// build and compile our shader program
	// ------------------------------------
	Shader textShader("shaders/vsText.shader", "shaders/fsText.shader");
	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(SCREEN_WIDTH), 0.0f, static_cast<float>(SCREEN_HEIGHT));
	textShader.use();
	glUniformMatrix4fv(glGetUniformLocation(textShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	initFreeType();
	Shader ourShader("shaders/vs330.shader", "shaders/fs330.shader");

	// OpenGL state
	// ------------
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// generate arrays and buffers
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
	int pinHitIndex = Npins;
	float timeStart = glfwGetTime();

	// The main game loop
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.2, 0.5, 0.8, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		t = clock();

		switch (gameState)
		{
			case START_MENU:
				RenderText(textShader, "Flipper: ", 25.0f, 570.0f, 0.4f, glm::vec4(0.5, 0.8f, 0.2f, 1.0f));
				break;

			default:
				for (int p = 0; p < Npins; p++)
				{
					if (p == pinHitIndex)
					{
						float timeValue = glfwGetTime() - timeStart;
						float redValue = sin(timeValue * 50.0f) / timeValue;
						ourShader.setVec4("baseColor", glm::vec4(redValue, 0.0f, 0.0f, 1.0f));
					}
					else
					{
						ourShader.setVec4("baseColor", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
					}

					//ourShader.setVec2("position", glm::vec2(pins[p][0], pins[p][1]));
					Render(circleVertices[p], 0, pinVAO[p], pinVBO[p], numberOfSides + 2, ourShader);
				}

				ourShader.setVec4("baseColor", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

				Render(ball.getBallCircleVertices(), 0, ballVAO, ballVBO, numberOfSides + 2, ourShader);

				Render(left.Get4PointsVertices(), 0, left4PointVAO, left4PointVBO, 4, ourShader);
				Render(left.GetPartCircleVertices(true), 0, leftAxleVAO, leftAxleVBO, (numberOfSides + 2) / 2, ourShader);
				Render(left.GetPartCircleVertices(false), 0, leftTipVAO, leftTipVBO, (numberOfSides + 2) / 2, ourShader);

			    Render(right.Get4PointsVertices(), 0, right4PointVAO, right4PointVBO, 4, ourShader);
				Render(right.GetPartCircleVertices(true), 0, rightAxleVAO, rightAxleVBO, (numberOfSides + 2) / 2, ourShader);
				Render(right.GetPartCircleVertices(false), 0, rightTipVAO, rightTipVBO, (numberOfSides + 2) / 2, ourShader);

				RenderText(textShader, "Points: ", 25.0f, 570.0f, 0.4f, glm::vec4(0.5, 0.8f, 0.2f, 1.0f));

				break;
		}

		GLfloat r = ball.getR();

		// The simulation loop of the flipper game
		for (int iter = 0; iter < (int)(Tfps / Ts); iter++)
		{
			if (gameState == FLIPPER)
			{
				GLfloat xPos = ball.getX();
				GLfloat yPos = ball.getY();
				GLfloat impulse;

				for (int i = 0; i < Npins; i++)
				{
					if (ball.pinCollisionDetect(pins[i][0], pins[i][1], pinR))
					{
						timeStart = glfwGetTime();
						pinHitIndex = i;
						break;
					}
				}

				if (!ball.wallCollisionDetect())
				{
					if (left.FlipperCollisionDetect(xPos, yPos, r))
					{
						ball.bounce(left.getNormal(), xPos, yPos, left.getDisplacement());
						ball.hit(left.getImpulse(xPos, yPos), left.getNormal(), xPos, yPos);
					}
					else if (right.FlipperCollisionDetect(xPos, yPos, r))
					{
						ball.bounce(right.getNormal(), xPos, yPos, left.getDisplacement());
						ball.hit(right.getImpulse(xPos, yPos), right.getNormal(), xPos, yPos);
					}
				}

				ball.midair();

				left.updateFlip();
				right.updateFlip();
			}
		}

		// Swap front and back buffers
		glfwSwapBuffers(window);

		// Poll for and process events
		glfwPollEvents();

		while (((float)(clock() - t) / CLOCKS_PER_SEC) < Tfps) {};
	}

	glfwTerminate();
}