#include "SceneMiniGame.h"
#include "GL\glew.h"

#include "shader.hpp"
#include "MeshBuilder.h"

void SceneMiniGame::RenderMesh(Mesh* mesh, bool enableLight)
{
	Mtx44 MVP, modelView, modelView_inverse_transpose;

	MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
	modelView = viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MODELVIEW], 1, GL_FALSE, &modelView.a[0]);

	if (mesh->textureID > 0)
	{
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh->textureID);
		glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	}
	else
	{
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 0);
	}
	mesh->Render(); //this line should only be called once
	if (mesh->textureID > 0)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void SceneMiniGame::RenderEntity(Entity* entity, bool enableLight)
{
	modelStack.PushMatrix();
	modelStack.Translate(entity->getTransform().x, entity->getTransform().y, entity->getTransform().z);
	modelStack.Scale(entity->getScale().x, entity->getScale().x, entity->getScale().x);

	Mtx44 MVP, modelView, modelView_inverse_transpose;

	MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
	modelView = viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MODELVIEW], 1, GL_FALSE, &modelView.a[0]);

	if (entity->getMesh()->textureID > 0)
	{
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, entity->getMesh()->textureID);
		glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	}
	else
	{
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 0);
	}
	entity->getMesh()->Render(); //this line should only be called once
	if (entity->getMesh()->textureID > 0)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	modelStack.PopMatrix();
}

void SceneMiniGame::RenderText(Mesh* mesh, std::string text, Color color)
{
	if (!mesh || mesh->textureID <= 0) //Proper error check
	{
		return;
	}
	glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
	glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);
	glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->textureID);
	glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	for (unsigned i = 0; i < text.length(); ++i)
	{
		Mtx44 characterSpacing;
		characterSpacing.SetToTranslation(i * 1.0f, 0, 0); //1.0f is the spacing of each character, you may change this value
		Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * characterSpacing;
		glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
		mesh->Render((unsigned)text[i] * 6, 6);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
}

void SceneMiniGame::RenderMeshOnScreen(Mesh* mesh, float x, float y, float sizex, float sizey)
{
	glDisable(GL_DEPTH_TEST);
	Mtx44 ortho;
	ortho.SetToOrtho(0, Application::screenUISizeX, 0, Application::screenUISizeY, -10, 10); //size of screen UI
	projectionStack.PushMatrix();
	projectionStack.LoadMatrix(ortho);
	viewStack.PushMatrix();
	viewStack.LoadIdentity(); //No need camera for ortho mode
	modelStack.PushMatrix();
	modelStack.LoadIdentity(); //Reset modelStack
	modelStack.Translate(x, y, 0);
	modelStack.Scale(sizex, sizey, 1);
	RenderMesh(mesh, false);
	projectionStack.PopMatrix();
	viewStack.PopMatrix();
	modelStack.PopMatrix();
	glEnable(GL_DEPTH_TEST);
}

void SceneMiniGame::RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y)
{
	float spacing = 1.0f;

	if (!mesh || mesh->textureID <= 0) //Proper error check
	{
		return;
	}
	glDisable(GL_DEPTH_TEST);
	Mtx44 ortho;
	ortho.SetToOrtho(0, Application::screenUISizeX, 0, Application::screenUISizeY, -10, 10); //size of screen UI
	projectionStack.PushMatrix();
	projectionStack.LoadMatrix(ortho);
	viewStack.PushMatrix();
	viewStack.LoadIdentity(); //No need camera for ortho mode
	modelStack.PushMatrix();
	modelStack.LoadIdentity(); //Reset modelStack
	modelStack.Translate(x - text.size() * (0.5f * spacing), y, 0);
	modelStack.Scale(size, size, size);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
	glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);
	glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->textureID);
	glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	for (unsigned i = 0; i < text.length(); ++i)
	{
		Mtx44 characterSpacing;
		characterSpacing.SetToTranslation(0.5f + i * spacing, 0.5f, 0); //1.0f is the spacing of each character, you may change this value
		Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * characterSpacing;
		glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
		mesh->Render((unsigned)text[i] * 6, 6);
	}

		glBindTexture(GL_TEXTURE_2D, 0);
		glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
		projectionStack.PopMatrix();
		viewStack.PopMatrix();
		modelStack.PopMatrix();
		glEnable(GL_DEPTH_TEST);
}

bool SceneMiniGame::CreateButton(float buttonTop, float buttonBottom, float buttonRight, float buttonLeft)
{
	//Converting Viewport space to UI space
	double x, y;
	Application::GetCursorPos(&x, &y);
	unsigned w = Application::GetWindowWidth();
	unsigned h = Application::GetWindowHeight();
	float posX = static_cast<float>(x / 10); //convert (0,800) to (0,80)
	float posY = static_cast<float>(h / 10 - y / 10); //convert (600,0) to (0,60)
	std::cout << "posX:" << posX << " , posY:" << posY << std::endl;
	if (posX > buttonLeft && posX < buttonRight && posY > buttonBottom  && posY < buttonTop)
	{
		//trigger user action or function
		return true;
	}
	else
	{
		return false;
	}
}

void SceneMiniGame::ResetGameVariables()
{
	score = 0;
	health = 3;
	timeElapsed = 0;
	timeElapsedTextToggle = 0;
	gameSpeed = 1;//game speed in times so 1x game speed

	//platform variables
	platformTransformX = 80 / 2;
	platformTransformY = 10;

	platformHeight = 2;
	platformWidth = 10;
	platformSpeed = initialPlatformSpeed;

	//item variables
	for (int i = 1; i <= sizeof(itemSpawnX) / sizeof(float); ++i)
	{
		itemSpawnX[i - 1] = 13 * i;
	}

	itemSpawnY = 52;

	for (int i = 0; i < sizeof(itemTransformY) / sizeof(float); ++i)
	{
		itemTransformY[i] = itemSpawnY;
	}

	srand(time(0));

	for (int i = 0; i < sizeof(itemType) / sizeof(char); ++i)
	{
		itemType[i] = rand() % 2;
	}

	//item spawn delay randomization
	for (int i = 0; i < sizeof(itemSpawnDelay) / sizeof(float); ++i)
	{
		itemSpawnDelay[i] = (rand() % 3) + ((rand() % 100) * 0.01);
	}

	itemSpeed = initialItemSpeed;
}

void SceneMiniGame::Init()
{
	// Init VBO here
	Mtx44 projection;

	projection.SetToPerspective(45.f, 4.f / 3.f, 0.1f, 1000.f);
	projectionStack.LoadMatrix(projection);

	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	camera.Init(Vector3(4, 3, 3), Vector3(0, 0, 0), Vector3(0, 1, 0));

	//button variables
	noX = 25;
	noY = 15;
	noSizeX = 20;
	noSizeY = 7.5;

	yesX = 55;
	yesY = 15;
	yesSizeX = 20;
	yesSizeY = 7.5;

	miniGameState = MAINMENU;

	initialItemSpeed = 15;
	initialPlatformSpeed = 20;

	//set background color
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	//generate default VAO for now
	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);

	//load vertex and fragment shaders
	m_programID = LoadShaders("Shader//Texture.vertexshader", "Shader//Text.fragmentshader");

	m_parameters[U_MVP] = glGetUniformLocation(m_programID, "MVP");
	m_parameters[U_MODELVIEW] = glGetUniformLocation(m_programID, "MV");
	m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE] = glGetUniformLocation(m_programID, "MV_inverse_transpose");
	m_parameters[U_MATERIAL_AMBIENT] = glGetUniformLocation(m_programID, "material.kAmbient");
	m_parameters[U_MATERIAL_DIFFUSE] = glGetUniformLocation(m_programID, "material.kDiffuse");
	m_parameters[U_MATERIAL_SPECULAR] = glGetUniformLocation(m_programID, "material.kSpecular");
	m_parameters[U_MATERIAL_SHININESS] = glGetUniformLocation(m_programID, "material.kShininess");

	// Get a handle for our "colorTexture" uniform
	m_parameters[U_COLOR_TEXTURE_ENABLED] = glGetUniformLocation(m_programID, "colorTextureEnabled");
	m_parameters[U_COLOR_TEXTURE] = glGetUniformLocation(m_programID, "colorTexture");

	// Get a handle for our "textColor" uniform
	m_parameters[U_TEXT_ENABLED] = glGetUniformLocation(m_programID, "textEnabled");
	m_parameters[U_TEXT_COLOR] = glGetUniformLocation(m_programID, "textColor");

	//use our shader
	glUseProgram(m_programID);

	//Enable depth test
	glEnable(GL_DEPTH_TEST);

	//Get a handle for our "MVP" uniform
	m_parameters[U_MVP] = glGetUniformLocation(m_programID, "MVP");

	Mesh::SetMaterialLoc(m_parameters[U_MATERIAL_AMBIENT],
		m_parameters[U_MATERIAL_DIFFUSE],
		m_parameters[U_MATERIAL_SPECULAR],
		m_parameters[U_MATERIAL_SHININESS]);

	meshList[GEO_QUAD] = MeshBuilder::GenerateQuad("quad", Color(0.5, 0.5, 0.5), 1.f);

	meshList[GEO_GOLD] = MeshBuilder::GenerateQuad("gold", Color(0.5, 0.5, 0.5), 1.f);
	meshList[GEO_GOLD]->textureID = LoadTGA("Image//gold.tga");

	meshList[GEO_DYNAMITE] = MeshBuilder::GenerateQuad("dynamite", Color(0.5, 0.5, 0.5), 1.f);
	meshList[GEO_DYNAMITE]->textureID = LoadTGA("Image//dynamite.tga");

	meshList[GEO_HEART] = MeshBuilder::GenerateQuad("heart", Color(0.5, 0.5, 0.5), 1.f);
	meshList[GEO_HEART]->textureID = LoadTGA("Image//heart.tga");

	meshList[GEO_OVER] = MeshBuilder::GenerateQuad("gameover", Color(0.5, 0.5, 0.5), 1.f);
	meshList[GEO_OVER]->textureID = LoadTGA("Image//gameover.tga");

	meshList[GEO_LOGO] = MeshBuilder::GenerateQuad("logo", Color(0.5, 0.5, 0.5), 1.f);
	meshList[GEO_LOGO]->textureID = LoadTGA("Image//minigamelogo.tga");

	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureID = LoadTGA("Image//arial.tga");
}

void SceneMiniGame::Update(double dt)
{
	if (miniGameState == EXITCONFIRMATION)
	{
		static bool bLButtonState = false;
		if (!bLButtonState && Application::IsMousePressed(0))
		{
			bLButtonState = true;

			//no button
			if (CreateButton(noY + noSizeY / 2, noY - noSizeY / 2, noX + noSizeX / 2, noX - noSizeX / 2))
			{
				ResetGameVariables();
				miniGameState = MAINMENU;
			}

			//yes button
			if (CreateButton(yesY + yesSizeY / 2, yesY - yesSizeY / 2, yesX + yesSizeX / 2, yesX - yesSizeX / 2))
			{
				miniGameState = MAINMENU;
				Application::sceneState = Application::STATE_MINIGAME_EXIT;
			}

		}
		else if (bLButtonState && !Application::IsMousePressed(0))
		{
			bLButtonState = false;
		}
	}
	else if (miniGameState == MAINMENU)
	{
		timeElapsed += dt;
		timeElapsedTextToggle += dt;

		
		if (timeElapsedTextToggle > 1)
		{
			timeElapsedTextToggle = 0;
		}
		
		if (timeElapsed > 3)
		{
			ResetGameVariables();
			timeElapsed = 0;
			miniGameState = MINIGAME;
		}
	}
	else if (miniGameState == MINIGAME)
	{
		timeElapsed += dt;
		if ((timeElapsed > 3 * gameSpeed)&& (gameSpeed < 5))
		{
			timeElapsed = 0;
			gameSpeed += 0.2;
			platformSpeed = initialPlatformSpeed * gameSpeed;
			itemSpeed = initialItemSpeed * gameSpeed;
		}

		if (Application::IsKeyPressed('A') && (platformTransformX > 0 + platformWidth / 2))
		{
			platformTransformX -= platformSpeed * dt;
		}
		else if (Application::IsKeyPressed('D') && (platformTransformX < 80 - platformWidth /2))
		{
			platformTransformX += platformSpeed * dt;
		}

		char tempCount = 0;

		itemSpawnDelay[tempCount] -= dt;
		tempCount++;
		itemSpawnDelay[tempCount] -= dt;
		tempCount++;
		itemSpawnDelay[tempCount] -= dt;
		tempCount++;
		itemSpawnDelay[tempCount] -= dt;
		tempCount++;
		itemSpawnDelay[tempCount] -= dt;

		tempCount = 0;

		if (itemSpawnDelay[tempCount] < 0)
		{
			itemTransformY[tempCount] -= itemSpeed * dt;
		}
		tempCount++;
		if (itemSpawnDelay[tempCount] < 0)
		{
			itemTransformY[tempCount] -= itemSpeed * dt;
		}
		tempCount++;
		if (itemSpawnDelay[tempCount] < 0)
		{
			itemTransformY[tempCount] -= itemSpeed * dt;
		}
		tempCount++;
		if (itemSpawnDelay[tempCount] < 0)
		{
			itemTransformY[tempCount] -= itemSpeed * dt;
		}
		tempCount++;
		if (itemSpawnDelay[tempCount] < 0)
		{
			itemTransformY[tempCount] -= itemSpeed * dt;
		}

		tempCount = 0;

		if ((itemTransformY[tempCount] < platformTransformY + platformHeight) &&
			(itemTransformY[tempCount] > platformTransformY - platformHeight) &&
			(itemSpawnX[tempCount] < platformTransformX + platformWidth / 2) &&
			(itemSpawnX[tempCount] > platformTransformX - platformWidth / 2))
		{
			if (itemType[tempCount] == GOLD)
			{
				score += 100;
			}
			else
			{
				health -= 1;
			}

			srand(time(0));
			itemType[tempCount] = rand() % 2;
			itemSpawnDelay[tempCount] = (rand() % 3) + ((rand() % 100) * 0.01);
			itemTransformY[tempCount] = itemSpawnY;
		}
		++tempCount;
		if ((itemTransformY[tempCount] < platformTransformY + platformHeight) &&
			(itemTransformY[tempCount] > platformTransformY - platformHeight) &&
			(itemSpawnX[tempCount] < platformTransformX + platformWidth / 2) &&
			(itemSpawnX[tempCount] > platformTransformX - platformWidth / 2))
		{
			if (itemType[tempCount] == GOLD)
			{
				score += 100;
			}
			else
			{
				health -= 1;
			}

			srand(time(0));
			itemType[tempCount] = rand() % 2;
			itemSpawnDelay[tempCount] = (rand() % 3) + ((rand() % 100) * 0.01);
			itemTransformY[tempCount] = itemSpawnY;
		}
		++tempCount;
		if ((itemTransformY[tempCount] < platformTransformY + platformHeight) &&
			(itemTransformY[tempCount] > platformTransformY - platformHeight) &&
			(itemSpawnX[tempCount] < platformTransformX + platformWidth / 2) &&
			(itemSpawnX[tempCount] > platformTransformX - platformWidth / 2))
		{
			if (itemType[tempCount] == GOLD)
			{
				score += 100;
			}
			else
			{
				health -= 1;
			}

			srand(time(0));
			itemType[tempCount] = rand() % 2;
			itemSpawnDelay[tempCount] = (rand() % 3) + ((rand() % 100) * 0.01);
			itemTransformY[tempCount] = itemSpawnY;
		}
		++tempCount;
		if ((itemTransformY[tempCount] < platformTransformY + platformHeight) &&
			(itemTransformY[tempCount] > platformTransformY - platformHeight) &&
			(itemSpawnX[tempCount] < platformTransformX + platformWidth / 2) &&
			(itemSpawnX[tempCount] > platformTransformX - platformWidth / 2))
		{
			if (itemType[tempCount] == GOLD)
			{
				score += 100;
			}
			else
			{
				health -= 1;
			}

			srand(time(0));
			itemType[tempCount] = rand() % 2;
			itemSpawnDelay[tempCount] = (rand() % 3) + ((rand() % 100) * 0.01);
			itemTransformY[tempCount] = itemSpawnY;
		}
		++tempCount;
		if ((itemTransformY[tempCount] < platformTransformY + platformHeight) &&
			(itemTransformY[tempCount] > platformTransformY - platformHeight) &&
			(itemSpawnX[tempCount] < platformTransformX + platformWidth / 2) &&
			(itemSpawnX[tempCount] > platformTransformX - platformWidth / 2))
		{
			if (itemType[tempCount] == GOLD)
			{
				score += 100;
			}
			else
			{
				health -= 1;
			}

			srand(time(0));
			itemType[tempCount] = rand() % 2;
			itemSpawnDelay[tempCount] = (rand() % 3) + ((rand() % 100) * 0.01);
			itemTransformY[tempCount] = itemSpawnY;
		}

		tempCount = 0;

		if (itemTransformY[tempCount] < 0)
		{
			srand(time(0));
			itemType[tempCount] = rand() % 2;
			itemSpawnDelay[tempCount] = (rand() % 3) + ((rand() % 100) * 0.01);
			itemTransformY[tempCount] = itemSpawnY;
		}
		++tempCount;
		if (itemTransformY[tempCount] < 0)
		{
			srand(time(0));
			itemType[tempCount] = rand() % 2;
			itemSpawnDelay[tempCount] = (rand() % 3) + ((rand() % 100) * 0.01);
			itemTransformY[tempCount] = itemSpawnY;
		}
		++tempCount;
		if (itemTransformY[tempCount] < 0)
		{
			srand(time(0));
			itemType[tempCount] = rand() % 2;
			itemSpawnDelay[tempCount] = (rand() % 3) + ((rand() % 100) * 0.01);
			itemTransformY[tempCount] = itemSpawnY;
		}
		++tempCount;
		if (itemTransformY[tempCount] < 0)
		{
			srand(time(0));
			itemType[tempCount] = rand() % 2;
			itemSpawnDelay[tempCount] = (rand() % 3) + ((rand() % 100) * 0.01);
			itemTransformY[tempCount] = itemSpawnY;
		}
		++tempCount;
		if (itemTransformY[tempCount] < 0)
		{
			srand(time(0));
			itemType[tempCount] = rand() % 2;
			itemSpawnDelay[tempCount] = (rand() % 3) + ((rand() % 100) * 0.01);
			itemTransformY[tempCount] = itemSpawnY;
		}
		
		if (health == 0)
		{
			timeElapsed = 0;
			miniGameState = GAMEOVER;
		}
	}
	else if (GAMEOVER)
	{
		timeElapsed += dt;

		if (timeElapsed > 2)
		{
			ResetGameVariables();
			miniGameState = MAINMENU;
		}
	}

	if ((miniGameState != EXITCONFIRMATION) && (Application::IsKeyPressed('B')))
	{
		Application::ResetCursor();
		Application::ShowCursor();
		miniGameState = EXITCONFIRMATION;
	}

	framePerSecond = 1.f / dt;
}

void SceneMiniGame::Render()
{

	//clear color and depth buffer every frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Render VBO here
	Mtx44 MVP;

	viewStack.LoadIdentity();
	viewStack.LookAt(camera.position.x, camera.position.y, camera.position.z,
		camera.target.x, camera.target.y, camera.target.z,
		camera.up.x, camera.up.y, camera.up.z);
	modelStack.LoadIdentity();
	if (miniGameState == EXITCONFIRMATION)
	{
		meshList[GEO_QUAD] = MeshBuilder::GenerateQuad("quad", Color(0, 0, 0), 1.f);

		RenderMeshOnScreen(meshList[GEO_QUAD], 40, 30, 60, 50);
		RenderTextOnScreen(meshList[GEO_TEXT], "Are you sure?" ,Color(1, 1, 1), 5, 16, 35);

		meshList[GEO_QUAD] = MeshBuilder::GenerateQuad("quad", Color(1, 1, 1), 1.f);


		RenderMeshOnScreen(meshList[GEO_QUAD], noX, noY, noSizeX, noSizeY);
		RenderTextOnScreen(meshList[GEO_TEXT], "No", Color(0, 0, 0), 3, 22, 14);

		RenderMeshOnScreen(meshList[GEO_QUAD], yesX, yesY, yesSizeX, yesSizeY);
		RenderTextOnScreen(meshList[GEO_TEXT], "Yes", Color(0, 0, 0), 3, 51.5, 14);
	}
	else if (miniGameState == MAINMENU)
	{
		RenderMeshOnScreen(meshList[GEO_LOGO], 40, 30, 80, 60);
		RenderMeshOnScreen(meshList[GEO_GOLD], 40, 20, 10, 10);

		if (timeElapsedTextToggle > 0.5)
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "Insert Coin", Color(1, 1, 1), 3, 30, 5);
		}
	}
	else if (miniGameState == MINIGAME)
	{
		//items
		char tempCount = 0;
		if (itemType[tempCount] == GOLD)
		{
			RenderMeshOnScreen(meshList[GEO_GOLD], itemSpawnX[tempCount], itemTransformY[tempCount], 3, 3);
		}
		else
		{
			RenderMeshOnScreen(meshList[GEO_DYNAMITE], itemSpawnX[tempCount], itemTransformY[tempCount], 3, 3);
		}
		++tempCount;
		if (itemType[tempCount] == GOLD)
		{
			RenderMeshOnScreen(meshList[GEO_GOLD], itemSpawnX[tempCount], itemTransformY[tempCount], 3, 3);
		}
		else
		{
			RenderMeshOnScreen(meshList[GEO_DYNAMITE], itemSpawnX[tempCount], itemTransformY[tempCount], 3, 3);
		}
		++tempCount;
		if (itemType[tempCount] == GOLD)
		{
			RenderMeshOnScreen(meshList[GEO_GOLD], itemSpawnX[tempCount], itemTransformY[tempCount], 3, 3);
		}
		else
		{
			RenderMeshOnScreen(meshList[GEO_DYNAMITE], itemSpawnX[tempCount], itemTransformY[tempCount], 3, 3);
		}
		++tempCount;
		if (itemType[tempCount] == GOLD)
		{
			RenderMeshOnScreen(meshList[GEO_GOLD], itemSpawnX[tempCount], itemTransformY[tempCount], 3, 3);
		}
		else
		{
			RenderMeshOnScreen(meshList[GEO_DYNAMITE], itemSpawnX[tempCount], itemTransformY[tempCount], 3, 3);
		}
		++tempCount;
		if (itemType[tempCount] == GOLD)
		{
			RenderMeshOnScreen(meshList[GEO_GOLD], itemSpawnX[tempCount], itemTransformY[tempCount], 3, 3);
		}
		else
		{
			RenderMeshOnScreen(meshList[GEO_DYNAMITE], itemSpawnX[tempCount], itemTransformY[tempCount], 3, 3);
		}

		//header
		meshList[GEO_QUAD] = MeshBuilder::GenerateQuad("quad", Color(1, 1, 1), 1.f);
		RenderMeshOnScreen(meshList[GEO_QUAD], 40, 55, 80, 10);

		//tray/player
		RenderMeshOnScreen(meshList[GEO_QUAD], platformTransformX, platformTransformY, 10, 2);

		//ui on top of screen
		std::stringstream tempStream;
		std::string tempString;
		tempStream << "Score " << std::setfill('0') << std::setw(8) << score;
		tempString = tempStream.str();
		tempStream.str(std::string());

		RenderTextOnScreen(meshList[GEO_TEXT], tempString, Color(0, 0, 0), 3, 10, 55);

		tempStream << "Game Speed " << gameSpeed << "x";
		tempString = tempStream.str();

		RenderTextOnScreen(meshList[GEO_TEXT], tempString, Color(0, 0, 0), 3, 10, 50);

		if (health > 0)
		{
			RenderMeshOnScreen(meshList[GEO_HEART], 65, 55, 4, 4);
		}
		if (health > 1)
		{
			RenderMeshOnScreen(meshList[GEO_HEART], 70, 55, 4, 4);
		}
		if (health > 2)
		{
			RenderMeshOnScreen(meshList[GEO_HEART], 75, 55, 4, 4);
		}
	}
	else if (miniGameState == GAMEOVER)
	{
		RenderMeshOnScreen(meshList[GEO_OVER], 40, 30, 80, 60);
	}

	if (miniGameState != EXITCONFIRMATION)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Press 'B' to exit", Color(1, 1, 1), 2, 50, 1);
	}
	
}

void SceneMiniGame::Exit()
{
	// Cleanup VBO here
	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);
}