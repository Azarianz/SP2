#ifndef APPLICATION_H
#define APPLICATION_H

#include "timer.h"

//Include GLEW
#include <GL/glew.h>

//Include GLFW
#include <GLFW/glfw3.h>

//include audio manager
#include "Sound.h"

#include "SceneMiniGame.h"
#include "LobbyScene.h"
#include "CorridorScene.h"
#include "RoomScene.h"
#include "MainMenuScene.h"
#include "GameEndScene.h"
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
		STATE_CORRIDOR_INIT,
		STATE_CORRIDOR_EXIT,
		STATE_ROOM_INIT,
		STATE_ROOM_EXIT,
		STATE_GAMEWIN,
		STATE_GAMELOSE,
	};

	enum ROOM_STATE
	{
		ROOM1, //Arcade Guy's Room
		ROOM2, //Old Man's Room
		ROOM3, //Kid's Room
		ROOM4, //Victim's Room
	};

	enum SCENE_TYPE
	{
		SCENE_MAINMENU,
		SCENE_LOBBY,
		SCENE_MINIGAME,
		SCENE_CORRIDOR,
		SCENE_ROOM,
		SCENE_GAMEOVER,
		SCENE_NUM
	};

	enum SOUNDS
	{
		SOUND_MAINMENU,
		SOUND_FOOTSTEP,
		SOUND_MINIGAME,
		SOUND_MAINGAME,
		SOUND_NUM
	};

	static unsigned m_width;
	static unsigned m_height;
	static float screenUISizeX;
	static float screenUISizeY;
	static unsigned char sceneState;
	static unsigned char roomState;
	static bool skipIntro;
	static vector<string> eList;
	static int playerGuesses;

	static bool guardEvidences[4];
	static bool janitorEvidences[4];
	static bool arcadeEvidences[4];
	static bool kidEvidences[4];
	static bool oldguyEvidences[4];

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
	static bool CheckEvidence(string text);
	static bool EnoughEvidence(bool b[4]);
	static void SetCanPause(bool isAble);

	//sound manager
	static Sound soundManager;	
	static FMOD::Sound *soundList[SOUND_NUM];

private:

	//Declare a window object
	StopWatch m_timer;

	//init some variables
	static bool IsFullscreen;
	unsigned char prevState;
	static bool CanPause;

	//some resolution variables for minigame
	unsigned prevWindowWidth;
	unsigned prevWindowHeight;
};

#endif