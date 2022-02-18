
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

void Application::Init()
{
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
	sceneState = SCENE_LOBBY;
}

void Application::Run()
{
	//Main Loop
	Scene* scene1 = new LobbyScene();
	Scene* sceneMiniGame = nullptr;
	Scene* scene3 = new CorridorScene();
	Scene* scene4 = new RoomScene();
	Scene* scene = scene1;
	scene4->Init();
	scene3->Init();
	scene1->Init();

	m_timer.startTimer();    // Start timer to calculate how long it takes to render this frame
	while (!glfwWindowShouldClose(m_window) && !IsKeyPressed(VK_ESCAPE))
	{
		if (sceneState == SCENE_LOBBY)
		{
			scene = scene1;
		}
		else if (sceneState == SCENE_MINIGAMEINIT)
		{
			if (sceneMiniGame == nullptr)
			{
				glfwDestroyWindow(m_window);
				m_width = 800;
				m_height = 600;
				m_window = glfwCreateWindow(m_width, m_height, "Test Window", NULL, NULL);
				glfwMakeContextCurrent(m_window);
				sceneMiniGame = new SceneMiniGame();
				sceneMiniGame->Init();
				scene = sceneMiniGame;
			}
		}
		else if (sceneState == SCENE_MINIGAMEEXIT)
		{
			if (sceneMiniGame != nullptr)
			{
				glfwDestroyWindow(m_window);
				m_width = 1280;
				m_height = 720;
				m_window = glfwCreateWindow(m_width, m_height, "Test Window", NULL, NULL);
				glfwMakeContextCurrent(m_window);

				sceneMiniGame->Exit();
				delete sceneMiniGame;
				sceneMiniGame = nullptr;

				scene1->Init();
				scene = scene1;
				sceneState = SCENE_LOBBY;
			}
		}
		else if (sceneState == SCENE_CORRIDOR) 
		{
			scene = scene3;
		}
		else if (sceneState == SCENE_ROOM1 ||
			sceneState == SCENE_ROOM2 || 
			sceneState == SCENE_ROOM3 || 
			sceneState == SCENE_ROOM4) 
		{
			scene = scene4;
		}


		scene->Update(m_timer.getElapsedTime());
		scene->Render();
		//Swap buffers
		glfwSwapBuffers(m_window);
		//Get and organize events, like keyboard and mouse input, window resizing, etc...
		glfwPollEvents();
        m_timer.waitUntil(frameTime);       // Frame rate limiter. Limits each frame to a specified time in ms.   

	} //Check if the ESC key had been pressed or if the window had been closed
	scene1->Exit();
	delete scene1;
	if (sceneMiniGame != nullptr)
	{
		sceneMiniGame->Exit();
		delete sceneMiniGame;
	}
	scene3->Exit();
	delete scene3;
	scene4->Exit();
	delete scene4;
}

void Application::Exit()
{
	//Close OpenGL window and terminate GLFW
	glfwDestroyWindow(m_window);
	//Finalize and clean up GLFW
	glfwTerminate();
}
