#ifndef APPLICATION_H
#define APPLICATION_H

#include "timer.h"

//Include GLEW
#include <GL/glew.h>

//Include GLFW
#include <GLFW/glfw3.h>

#include "SceneMiniGame.h"
#include "LobbyScene.h"


class Application
{
public:
	enum SCENE_STATE
	{
		SCENE_LOBBY,
		SCENE_MINIGAME,
		SCENE_MINIGAMEEXIT
	};

	static unsigned m_width;
	static unsigned m_height;
	static unsigned char sceneState;

	Application();
	~Application();
	static bool IsMousePressed(unsigned short key);
	static void GetCursorPos(double* xpos, double* ypos);
	static int GetWindowWidth();
	static int GetWindowHeight();
	static void HideCursor();
	static void ShowCursor();
	static void ResetCursor();
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