
#include "Application.h"

//Include GLEW
#include <GL/glew.h>

//Include GLFW
#include <GLFW/glfw3.h>

//Include the standard C++ headers
#include <stdio.h>
#include <stdlib.h>

GLFWwindow* m_window;
const unsigned char FPS = 60; // FPS of this game
const unsigned int frameTime = 1000 / FPS; // time for each frame
unsigned Application::m_width;
unsigned Application::m_height;
float Application::screenUISizeX;
float Application::screenUISizeY;
unsigned char Application::sceneState;
unsigned char Application::roomState;
bool Application::skipIntro;
vector<string> Application::eList;
vector<string> Application::nameList;
int Application::playerGuesses = 3;
int Application::arcadeGameScore = 0;

//sound variables
Sound Application::soundManager;
FMOD::Sound *Application::soundList[SOUND_NUM];

bool Application::guardEvidences[4] = {false,false,false,false};
bool Application::janitorEvidences[4] = { false,false,false,false };
bool Application::arcadeEvidences[4] = { false,false,false,false };
bool Application::kidEvidences[4] = { false,false,false,false };
bool Application::oldguyEvidences[4] = { false,false,false,false };
bool Application::susCounter[4] = { false,false,false,false };

bool Application::IsFullscreen;
bool Application::CanPause;

//Define an error callback
static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
	_fgetchar();
}

//Define the key input callback
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

static void resize_callback(GLFWwindow* window, int width, int height)
{
	Application::m_width = width;
	Application::m_height = height;
	glViewport(0, 0, width, height);
}

void Application::HideCursor()
{
	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Application::ShowCursor()
{
	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void Application::ResetCursor()
{
	glfwSetCursorPos(m_window, m_width / 2, m_height / 2);
}

void Application::ExitGame()
{
	glfwSetWindowShouldClose(m_window, GL_TRUE);
}

void Application::Fullscreen()
{
	IsFullscreen = true;
	glfwSetWindowMonitor(m_window, glfwGetPrimaryMonitor(), 0, 0, m_width, m_height, 60);
}

void Application::ExitFullscreen()
{
	IsFullscreen = false;
	glfwSetWindowMonitor(m_window, NULL, 50, 50, m_width, m_height, 60);
}

bool Application::GetIsFullscreen()
{
	return IsFullscreen;
}

void Application::SetResolution(float width, float height)
{
	resize_callback(m_window, width, height);

	if (IsFullscreen)
	{
		Fullscreen();
	}
	else
	{
		ExitFullscreen();
	}
}

bool Application::IsKeyPressed(unsigned short key)
{
    return ((GetAsyncKeyState(key) & 0x8001) != 0);
}

bool Application::IsMousePressed(unsigned short key) //0 - Left, 1 - Right, 2 - Middle
{
	return glfwGetMouseButton(m_window, key) != 0;
}

void Application::GetCursorPos(double* xpos, double* ypos)
{
	glfwGetCursorPos(m_window, xpos, ypos);
}

int Application::GetWindowWidth()
{
	return m_width;
}

int Application::GetWindowHeight()
{
	return m_height;
}

void Application::AddEvidence(string text, string name)
{
	bool isExisting = true; //check for existing evidence
	if (!eList.empty())
	{
		for (int i = 0; i <= eList.size() - 1; i++)
		{
			if (eList[i] == text) {
				isExisting = true;
				break;
			}
			else {
				isExisting = false;
			}
		}
		if (!isExisting) {
			eList.push_back(text);
			nameList.push_back(name);
		}
	}
	else
	{
		eList.push_back(text);
		nameList.push_back(name);
	}
}

bool Application::CheckEvidence(string text)
{
	for (int i = 0; i <= eList.size() - 1; i++)
	{
		if (eList[i] == text)
		{
			return true;
		}
	}
	return false;
}

bool Application::EnoughEvidence(bool b[4])
{
	int check = 0;
	for (int i = 0; i <= sizeof(b) / sizeof(b[0]); i++) 
	{
		if (b[i] == true) {
			check++;
		}

		if (check >= 4) {
			check = 4;
			break;
		}
	}

	if (check == 4) {
		return true;
	}
	else{
		return false;
	}
}

void Application::SetCanPause(bool isAble)
{
	CanPause = isAble;
}

void Application::Init()
{
	/*AddEvidence("Evidence 01: Insert Text Here");
	AddEvidence("Evidence 02: Insert Text Here");
	AddEvidence("Evidence 03: Insert Text Here");
	AddEvidence("Evidence 04: Insert Text Here");
	AddEvidence("Evidence 05: Insert Text Here");
	AddEvidence("Evidence 06: Insert Text Here");
	AddEvidence("Evidence 07: Insert Text Here");
	AddEvidence("Evidence 08: Insert Text Here");
	AddEvidence("Evidence 09: Insert Text Here");*/

	//Set the error callback
	glfwSetErrorCallback(error_callback);

	//Initialize GLFW
	if (!glfwInit())
	{
		exit(EXIT_FAILURE);
	}

	//Set the GLFW window creation hints - these are optional
	glfwWindowHint(GLFW_SAMPLES, 4); //Request 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //Request a specific OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); //Request a specific OpenGL version
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL 


	//Create a window and create its OpenGL context
	m_width = 1280;
	m_height = 720;
	screenUISizeX = 80;
	screenUISizeY = 60;
	m_window = glfwCreateWindow(m_width, m_height, "Test Window", NULL, NULL);

	//If the window couldn't be created
	if (!m_window)
	{
		fprintf( stderr, "Failed to open GLFW window.\n" );
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	//This function makes the context of the specified window current on the calling thread. 
	glfwMakeContextCurrent(m_window);

	//Sets the key callback
	//glfwSetKeyCallback(m_window, key_callback);

	glewExperimental = true; // Needed for core profile
	//Initialize GLEW
	GLenum err = glewInit();

	//If GLEW hasn't initialized
	if (err != GLEW_OK) 
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		//return -1;
	}

	//initialize callback with GLFW
	glfwSetWindowSizeCallback(m_window, resize_callback);

	//init some game variables
	sceneState = STATE_MAINMENU_INIT;
	prevState = STATE_LOBBY;
	roomState = 0;
	skipIntro = false;
	CanPause = true;
}

void Application::Run()
{
	//Main Loop
	Scene* sceneList[SCENE_NUM];
	sceneList[SCENE_MAINMENU] = nullptr;
	sceneList[SCENE_LOBBY] = nullptr;
	sceneList[SCENE_MINIGAME] = nullptr;
	sceneList[SCENE_CORRIDOR] = nullptr;
	sceneList[SCENE_ROOM] = nullptr;
	sceneList[SCENE_GAMEOVER] = nullptr;
	Scene* scene = nullptr;

	m_timer.startTimer();    // Start timer to calculate how long it takes to render this frame
	while (!glfwWindowShouldClose(m_window))
	{
		if (sceneState != STATE_RUN_SCENE)
		{
			if((sceneState != STATE_MAINMENU_EXIT) && (sceneState != STATE_MAINMENU_INIT))
			{
				prevState = sceneState;
			}

			//Game
			if (playerGuesses > 0) 
			{
				if (sceneState == STATE_MAINMENU_INIT)
				{
					if (sceneList[SCENE_MAINMENU] == nullptr)
					{
						//Close Previous Scene
						if (sceneList[SCENE_GAMEOVER] != nullptr)
						{
							delete sceneList[SCENE_GAMEOVER];
							sceneList[SCENE_GAMEOVER] = nullptr;
						}

						sceneList[SCENE_MAINMENU] = new MainMenuScene();
						sceneList[SCENE_MAINMENU]->Init();
						scene = sceneList[SCENE_MAINMENU];
						sceneState = STATE_RUN_SCENE;
					}
					else
					{
						sceneState = STATE_RUN_SCENE;
					}
				}
				else if (sceneState == STATE_MAINMENU_EXIT)
				{
					if (sceneList[SCENE_MAINMENU] != nullptr)
					{
						sceneList[SCENE_MAINMENU]->Exit();
						delete sceneList[SCENE_MAINMENU];
						sceneList[SCENE_MAINMENU] = nullptr;
					}
					sceneState = prevState;
				}
				else if (sceneState == STATE_LOBBY)
				{
					if (sceneList[SCENE_LOBBY] == nullptr)
					{
						//Close Previous Scene
						if (sceneList[SCENE_CORRIDOR] != nullptr)
						{
							delete sceneList[SCENE_CORRIDOR];
							sceneList[SCENE_CORRIDOR] = nullptr;
						}

						sceneList[SCENE_LOBBY] = new LobbyScene();
						sceneList[SCENE_LOBBY]->Init();
						scene = sceneList[SCENE_LOBBY];
						sceneState = STATE_RUN_SCENE;
					}
					else 
					{
						sceneList[SCENE_LOBBY]->Init();
						scene = sceneList[SCENE_LOBBY];
						sceneState = STATE_RUN_SCENE;
					}
				}
				else if (sceneState == STATE_MINIGAME_INIT)
				{
					if (sceneList[SCENE_MINIGAME] == nullptr)
					{
						prevWindowWidth = m_width;
						prevWindowHeight = m_height;
						sceneList[SCENE_MINIGAME] = new SceneMiniGame();
						SetResolution(800, 600);

						sceneList[SCENE_MINIGAME]->Init();
						scene = sceneList[SCENE_MINIGAME];
						sceneState = STATE_RUN_SCENE;
					}
				}
				else if (sceneState == STATE_MINIGAME_EXIT)
				{
					if (sceneList[SCENE_MINIGAME] != nullptr)
					{
						SetResolution(prevWindowWidth, prevWindowHeight);

						sceneList[SCENE_MINIGAME]->Exit();
						delete sceneList[SCENE_MINIGAME];
						sceneList[SCENE_MINIGAME] = nullptr;

						sceneState = STATE_LOBBY;

					}
				}
				else if (sceneState == STATE_CORRIDOR_INIT)
				{
					if (sceneList[SCENE_CORRIDOR] == nullptr) {
						//Close Previous Scene
						if (sceneList[SCENE_LOBBY] != nullptr)
						{
							delete sceneList[SCENE_LOBBY];
							sceneList[SCENE_LOBBY] = nullptr;
						}
						sceneList[SCENE_CORRIDOR] = new CorridorScene();
						sceneList[SCENE_CORRIDOR]->Init();
						scene = sceneList[SCENE_CORRIDOR];
						sceneState = STATE_RUN_SCENE;
					}
					else 
					{
						sceneList[SCENE_CORRIDOR]->Init();
						scene = sceneList[SCENE_CORRIDOR];
						sceneState = STATE_RUN_SCENE;
					}
				}
				else if (sceneState == STATE_ROOM_INIT)
				{
					if (sceneList[SCENE_ROOM] == nullptr)
					{
						sceneList[SCENE_ROOM] = new RoomScene();
						sceneList[SCENE_ROOM]->Init();
						scene = sceneList[SCENE_ROOM];
						sceneState = STATE_RUN_SCENE;
					}
					else
					{
						sceneList[SCENE_ROOM]->Init();
						scene = sceneList[SCENE_ROOM];
						sceneState = STATE_RUN_SCENE;
					}
				}
				else if (sceneState == STATE_ROOM_EXIT)
				{
					if (sceneList[SCENE_ROOM] != nullptr)
					{
						sceneList[SCENE_ROOM]->Exit();
						delete sceneList[SCENE_ROOM];
						sceneList[SCENE_ROOM] = nullptr;

						roomState = 0;
						sceneState = STATE_CORRIDOR_INIT;
					}
				}

				//Game End (Correct Guess)
				else if (sceneState == STATE_GAMEWIN)
				{
					if (sceneList[SCENE_GAMEOVER] == nullptr)
					{
						if (sceneList[SCENE_LOBBY] != nullptr) {
							delete sceneList[SCENE_LOBBY];
							sceneList[SCENE_LOBBY] = nullptr;
						}

						sceneList[SCENE_GAMEOVER] = new GameEndScene();
						sceneList[SCENE_GAMEOVER]->Init();
						scene = sceneList[SCENE_GAMEOVER];
						playerGuesses = 3;
						sceneState = STATE_RUN_SCENE;
					}
				}
			}

			else if (playerGuesses <= 0) 
			{
				//Game End (Correct Guess)
				if (sceneState == STATE_GAMEWIN)
				{
					if (sceneList[SCENE_GAMEOVER] == nullptr)
					{
						if (sceneList[SCENE_LOBBY] != nullptr) {
							delete sceneList[SCENE_LOBBY];
							sceneList[SCENE_LOBBY] = nullptr;
						}

						sceneList[SCENE_GAMEOVER] = new GameEndScene();
						sceneList[SCENE_GAMEOVER]->Init();
						scene = sceneList[SCENE_GAMEOVER];
						playerGuesses = 3;
						sceneState = STATE_RUN_SCENE;
					}
				}

				//Game End (Out of Guesses)
				else if (sceneState == STATE_GAMELOSE)
				{
					if (sceneList[SCENE_GAMEOVER] == nullptr)
					{
						if (sceneList[SCENE_LOBBY] != nullptr) {
							delete sceneList[SCENE_LOBBY];
							sceneList[SCENE_LOBBY] = nullptr;
						}

						sceneList[SCENE_GAMEOVER] = new GameEndScene();
						sceneList[SCENE_GAMEOVER]->Init();
						scene = sceneList[SCENE_GAMEOVER];
						playerGuesses = 3;
						sceneState = STATE_RUN_SCENE;
					}
				}
			}
		}

		else 
		{
			scene->Update(m_timer.getElapsedTime());
			scene->Render();

			static bool isEscapePressed = false;
			if (IsKeyPressed(VK_ESCAPE) && !isEscapePressed && CanPause)
			{
				isEscapePressed = true;
				sceneState = STATE_MAINMENU_INIT;
			}
			else if (!IsKeyPressed(VK_ESCAPE) && isEscapePressed)
			{
				isEscapePressed = false;
			}
		}

		//Swap buffers
		glfwSwapBuffers(m_window);
		//Get and organize events, like keyboard and mouse input, window resizing, etc...
		glfwPollEvents();
		m_timer.waitUntil(frameTime);       // Frame rate limiter. Limits each frame to a specified time in ms.   

	}

	for (int i = 0; i < SOUND_NUM; ++i)
	{
		soundManager.ReleaseSound(soundList[i]);
	}

	for (int i = 0; i < SCENE_NUM; ++i)
	{
		if (sceneList[i] != nullptr)
		{
			sceneList[i]->Exit();
			delete sceneList[i];
		}
	}
}

void Application::Exit()
{
	//Close OpenGL window and terminate GLFW
	glfwDestroyWindow(m_window);
	//Finalize and clean up GLFW
	glfwTerminate();
}
