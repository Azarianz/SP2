
#ifndef APPLICATION_H
#define APPLICATION_H

#include "timer.h"

//Include GLEW
#include <GL/glew.h>

//Include GLFW
#include <GLFW/glfw3.h>

#include "SceneUI.h"


class Application
{
public:
	static unsigned m_width;
	static unsigned m_height;

	Application();
	~Application();
	static bool IsMousePressed(unsigned short key);
	static void GetCursorPos(double* xpos, double* ypos);
	static int GetWindowWidth();
	static int GetWindowHeight();
	static void HideCursor();
	static void ShowCursor();
	void resize_callback(GLFWwindow* window, int w, int h);
	void Init();
	void Run();
	void Exit();
	static bool IsKeyPressed(unsigned short key);

private:

	//Declare a window object
	StopWatch m_timer;
};

#endif