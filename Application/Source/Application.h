#ifndef APPLICATION_H
#define APPLICATION_H

#include "timer.h"

//Include GLEW
#include <GL/glew.h>

//Include GLFW
#include <GLFW/glfw3.h>

#include "SceneMiniGame.h"
#include "LobbyScene.h"
#include "CorridorScene.h"
#include "RoomScene.h"
#include "MainMenuScene.h"
#include <vector>
#include <string>

using namespace std;

class Application
{
public:
	enum SCENE_STATE
	{
		STATE_RUN_SCENE,
		STATE_MAINMENU_INIT,
		STATE_MAINMENU_EXIT,
		STATE_LOBBY,
		STATE_MINIGAME_INIT,
		STATE_MINIGAME_EXIT,
		STATE_CORRIDOR,
		STATE_ROOM1, //Arcade Guy's Room
		STATE_ROOM2, //Old Man's Room
		STATE_ROOM3, //Kid's Room
		STATE_ROOM4, //Victim's Room
	};

	enum SCENE_TYPE
	{
		SCENE_MAINMENU,
		SCENE_LOBBY,
		SCENE_MINIGAME,
		SCENE_CORRIDOR,
		SCENE_ROOM,
		SCENE_NUM
	};

	static unsigned m_width;
	static unsigned m_height;
	static float screenUISizeX;
	static float screenUISizeY;
	static unsigned char sceneState;
	static vector<string> eList;
	static int playerGuesses;

	Application();
	~Application();
	//mouse func
	static bool IsMousePressed(unsigned short key);
	static void GetCursorPos(double* xpos, double* ypos);
	static void HideCursor();
	static void ShowCursor();
	static void ResetCursor();

	//window width/height change
	static int GetWindowWidth();
	static int GetWindowHeight();

	//exit game
	static void ExitGame();

	//screen func
	static void Fullscreen();
	static void ExitFullscreen();
	static bool GetIsFullscreen();
	static void SetResolution(float width,float height);

	//other func
	void Init();
	void Run();
	void Exit();
	static bool IsKeyPressed(unsigned short key);
	static void AddEvidence(string text);

private:

	//Declare a window object
	StopWatch m_timer;

	//init some variables
	static bool IsFullscreen;
};

#endif