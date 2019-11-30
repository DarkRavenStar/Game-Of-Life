
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "AllHeader.h"

#include <GLFW/glfw3.h>

#include "GameOfLife.h"
#include "GameControl.h"

int WINDOW_WIDTH = 800;
int WINDOW_HEIGHT = 600;

int fps = 0;
int fpsLimit = 60;
bool play = false;

int xMinCoord = 0;
int xMaxCoord = cols;
int yMinCoord = 0;
int yMaxCoord = rows;

void gotoxy(int x, int y)
{
	static HANDLE h = NULL;
	if (!h)
		h = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD c = { x, y };
	SetConsoleCursorPosition(h, c);
}

void UpdateText()
{
	gotoxy(0, 0);
	std::cout << "Game of Life 0118856 Teena" << std::endl;
	gotoxy(0, 1);
	std::cout << "Use the GUI or Keyboard" << std::endl;
	gotoxy(0, 2);
	std::cout << "Use P to play or pause" << std::endl;
	gotoxy(0, 3);
	std::cout << "Use R to reset" << std::endl;
	gotoxy(0, 4);
	std::cout << "Use E to randomize grid" << std::endl;
	gotoxy(0, 5);
	std::cout << "Use W/S or UP/DOWN to increase/decrease FPS" << std::endl;
	gotoxy(0, 6);
	std::cout << "Use Mouse Scroll to zoom in and out" << std::endl;
	gotoxy(0, 7);
	std::cout << "Current FPS" << fpsLimit << "       " << std::endl;
}

void UpdateFPSText()
{
	gotoxy(0, 7);
	std::cout << "Current FPS" << fpsLimit << "       " << std::endl;
}

void onWindowResized(GLFWwindow* window, int width, int height)
{
	if (height == 0) height = 1;						// Prevent A Divide By Zero By making Height Equal One

	WINDOW_HEIGHT = height;
	WINDOW_WIDTH = width;

	glViewport(0.0f, 0.0f, WINDOW_WIDTH, WINDOW_HEIGHT);	// Reset The Current Viewport
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix
	glOrtho(0.0, WINDOW_WIDTH, WINDOW_HEIGHT, 0.0, -1, 1);
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();
}

void GUIInput(GLFWwindow* window, int button, int action, int mods)
{
	double x = 0.0;
	double y = 0.0;
	glfwGetCursorPos(window, &x, &y);

	int scaleX = 1;
	int scaleY = 1;

	float xMin = 0;
	float xMax = (float)(xMin + guiTexW * scaleX);

	float yMin = 0;
	float yMax = (float)(yMin + guiTexH * scaleY);

	int ratioY = (yMax - yMin) / 6;

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		if (x > xMin && x < xMax && y > 0 && y < ratioY * 1)
		{
			play = true;
		}

		if (x > xMin && x < xMax && y > ratioY * 1 && y < ratioY * 2)
		{
			play = false;
		}

		if (x > xMin && x < xMax && y > ratioY * 2 && y < ratioY * 3)
		{
			RandomizeGrid();
			play = false;
		}

		if (x > xMin && x < xMax && y > ratioY * 3 && y < ratioY * 4)
		{
			ResetGrid();
			play = false;
		}

		if (x > xMin && x < xMax && y > ratioY * 4 && y < ratioY * 5)
		{
			if (fpsLimit < 60)
			{
				fpsLimit++;
				UpdateFPSText();
			}
		}

		if (x > xMin && x < xMax && y > ratioY * 5 && y < ratioY * 6)
		{
			if (fpsLimit > 1)
			{
				fpsLimit--;
				UpdateFPSText();
			}
		}
	}
}

void KeyInput(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		play = !play;
	}

	if (key == GLFW_KEY_E && action == GLFW_PRESS)
	{
		RandomizeGrid();
		play = false;
	}

	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		ResetGrid();
		play = false;
	}

	if ((key == GLFW_KEY_W || key == GLFW_KEY_UP) && action == GLFW_PRESS)
	{
		if (fpsLimit < 60)
		{
			fpsLimit++;
			UpdateFPSText();
		}
	}

	if ((key == GLFW_KEY_S || key == GLFW_KEY_DOWN) && action == GLFW_PRESS)
	{
		if (fpsLimit > 1)
		{
			fpsLimit--;
			UpdateFPSText();
		}
	}
}

void GridInput(GLFWwindow* window)
{
	static double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	int leftMousebutton = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1);

	int xMin = WINDOW_WIDTH - WINDOW_HEIGHT;
	int xMax = WINDOW_WIDTH;
	int yMin = 0;
	int yMax = WINDOW_HEIGHT;

	int colOffset = xMaxCoord - xMinCoord;
	int rowOffset = yMaxCoord - yMinCoord;

	int xOffset = (xMax - xMin) / colOffset;
	int yOffset = (yMax - yMin) / rowOffset;

	int intX = ((xpos - xMin) / xOffset) + xMinCoord;
	int intY = ((ypos - yMin) / yOffset) + yMinCoord;

	GridNodeSelection(leftMousebutton, intX, intY);
}

void Zooming(GLFWwindow* window, double xoffset, double yoffset)
{
	if (yoffset < 0)
	{
		if (xMinCoord < cols / 2 - 1)
		{
			xMinCoord++;
		}

		if (xMaxCoord > cols / 2 + 1)
		{
			xMaxCoord--;
		}

		if (yMinCoord < rows / 2 - 1)
		{
			yMinCoord++;
		}

		if (yMaxCoord > rows / 2 + 1)
		{
			yMaxCoord--;
		}
	}
	else if (yoffset > 0)
	{
		if (xMinCoord > 0)
		{
			xMinCoord--;
		}

		if (xMaxCoord < cols - 1)
		{
			xMaxCoord++;
		}

		if (yMinCoord > 0)
		{
			yMinCoord--;
		}

		if (yMaxCoord < rows - 1)
		{
			yMaxCoord++;
		}
	}
}

void DrawGUI(int width, int height)
{
	ButtonGUI(0, 0, 1, 1);
}

int main()
{
	GLFWwindow* window;
	system("mode con: cols=80 lines=30");
	/* Initialize the GLFW library */
	if (!glfwInit())
		return -1;

	// Open an OpenGL window
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "OpenGL", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	// Hook window resize.
	glfwSetWindowSizeCallback(window, onWindowResized);
	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	onWindowResized(window, WINDOW_WIDTH, WINDOW_HEIGHT);

	glfwSetScrollCallback(window, Zooming);
	glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, 1);

	glfwSetKeyCallback(window, KeyInput);
	glfwSetMouseButtonCallback(window, GUIInput);

	glClearColor(0.2f, 0.2f, 0.2f, 0.5f);
	InitTexture();
	ResetGrid();
	RandomizeGrid();
	UpdateText();
	// Main loop
	while (!glfwWindowShouldClose(window))
	{
		glfwSwapInterval(1);
		// OpenGL rendering goes here...
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (glfwGetKey(window, GLFW_KEY_ESCAPE))
			break;

		if (play == true)
		{
			if (fps == 60 / fpsLimit)
			{
				CalculateGrid();
				fps = 0;
			}
			fps++;
		}

		GridInput(window);
		DrawGrid(WINDOW_WIDTH, WINDOW_HEIGHT, xMinCoord, xMaxCoord, yMinCoord, xMaxCoord);
		DrawGUI(WINDOW_WIDTH, WINDOW_HEIGHT);
		// Swap front and back rendering buffers
		glfwSwapBuffers(window);

		glfwPollEvents();
	}
	// Close window and terminate GLFW
	glfwTerminate();
}