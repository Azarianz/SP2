
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
vector<string> Application::eList;
int Application::playerGuesses = 3;
bool Application::IsFullscreen;


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
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	glfwSetWindowMonitor(m_window, glfwGetPrimaryMonitor(), 0, 0, 1280, 720, 60);
}

void Application::ExitFullscreen()
{
	IsFullscreen = false;
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	glfwSetWindowMonitor(m_window, nullptr, 0, 0, 1280, 720, 60);
}

bool Application::GetIsFullscreen()
{
	return IsFullscreen;
}

bool Application::IsKeyPressed(unsigned short key)
{
    return ((GetAsyncKeyState(key) & 0x8001) != 0);
}

Application::Application()
{
}

Application::~Application()
{
}

void Application::resize_callback(GLFWwindow* window, int w, int h)
{
	Application::m_width = w;
	Application::m_height = h;
	glViewport((w - m_width / 2), 0, w, h);
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

void Application::AddEvidence(string text) 
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
		}
	}
	else
	{
		eList.push_back(text);
	}
}

void Application::Init()
{
	AddEvidence("Evidence 01: Insert Text Here");
	AddEvidence("Evidence 02: Insert Text Here");
	AddEvidence("Evidence 03: Insert Text Here");
	AddEvidence("Evidence 04: Insert Text Here");
	AddEvidence("Evidence 05: Insert Text Here");
	AddEvidence("Evidence 06: Insert Text Here");
	AddEvidence("Evidence 07: Insert Text Here");
	AddEvidence("Evidence 08: Insert Text Here");
	AddEvidence("Evidence 09: Insert Text Here");

	//Set the error callback
	glfwSetErrorCallback(error_callback);

	//initialize callback with GLFW
	//glfwSetWindowSizeCallback(m_window, resize_callback);

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

	//init some game variables
	sceneState = STATE_MAINMENU_INIT;
}

void Application::Run()
{
	//Main Loop
	Scene* sceneList[SCENE_NUM];
	sceneList[SCENE_MAINMENU] = new MainMenuScene();
	sceneList[SCENE_LOBBY] = new LobbyScene();
	sceneList[SCENE_MINIGAME] = nullptr;
	sceneList[SCENE_CORRIDOR] = new CorridorScene();
	sceneList[SCENE_ROOM] = new RoomScene();
	Scene* scene = sceneList[SCENE_LOBBY];
	/*sceneList[SCENE_CORRIDOR]->Init();
	sceneList[SCENE_ROOM]->Init();*/
	/*sceneList[SCENE_LOBBY]->Init();*/
	sceneList[SCENE_MAINMENU]->Init();

	m_timer.startTimer();    // Start timer to calculate how long it takes to render this frame
	while (!glfwWindowShouldClose(m_window))
	{
		if (playerGuesses > 0) {
			if (sceneState == STATE_MAINMENU_INIT)
			{
				scene = sceneList[SCENE_MAINMENU];
			}
			if (sceneState == STATE_MAINMENU_EXIT)
			{
				sceneList[SCENE_LOBBY]->Init();
				sceneState = STATE_LOBBY;
				scene = sceneList[SCENE_LOBBY];
			}
			else if (sceneState == STATE_LOBBY)
			{
				scene = sceneList[SCENE_LOBBY];
			}
			else if (sceneState == STATE_MINIGAME_INIT)
			{
				if (sceneList[SCENE_MINIGAME] == nullptr)
				{
					glfwDestroyWindow(m_window);
					m_width = 800;
					m_height = 600;
					m_window = glfwCreateWindow(m_width, m_height, "Test Window", NULL, NULL);
					glfwMakeContextCurrent(m_window);

					sceneList[SCENE_MINIGAME] = new SceneMiniGame();
					sceneList[SCENE_MINIGAME]->Init();
					scene = sceneList[SCENE_MINIGAME];
				}
			}
			else if (sceneState == STATE_MINIGAME_EXIT)
			{
				if (sceneList[SCENE_MINIGAME] != nullptr)
				{
					glfwDestroyWindow(m_window);
					m_width = 1280;
					m_height = 720;
					m_window = glfwCreateWindow(m_width, m_height, "Test Window", NULL, NULL);
					glfwMakeContextCurrent(m_window);

					sceneList[SCENE_MINIGAME]->Exit();
					delete sceneList[SCENE_MINIGAME];
					sceneList[SCENE_MINIGAME] = nullptr;

					sceneList[SCENE_LOBBY]->Init();
					scene = sceneList[SCENE_LOBBY];
					sceneState = STATE_LOBBY;
				}
			}
			else if (sceneState == STATE_CORRIDOR)
			{
				scene = sceneList[SCENE_CORRIDOR];
			}
			else if (sceneState == STATE_ROOM1 ||
				sceneState == STATE_ROOM2 ||
				sceneState == STATE_ROOM3 ||
				sceneState == STATE_ROOM4)
			{
				scene = sceneList[SCENE_ROOM];
			}
		}
		else {
			//Game Over Code
		}
	}


		scene->Update(m_timer.getElapsedTime());
		scene->Render();
		//Swap buffers
		glfwSwapBuffers(m_window);
		//Get and organize events, like keyboard and mouse input, window resizing, etc...
		glfwPollEvents();
        m_timer.waitUntil(frameTime);       // Frame rate limiter. Limits each frame to a specified time in ms.   

		if (IsKeyPressed(VK_ESCAPE))
		{
			ExitGame();
		}

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
