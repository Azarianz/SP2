#include "MainMenuScene.h"
#include "GL\glew.h"

#include "shader.hpp"
#include "MeshBuilder.h"

#define LSPEED 20

void MainMenuScene::RenderMesh(Mesh* mesh, bool enableLight)
{
	Mtx44 MVP, modelView, modelView_inverse_transpose;

	MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
	modelView = viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MODELVIEW], 1, GL_FALSE, &modelView.a[0]);
	if (enableLight)
	{
		modelView_inverse_transpose = modelView.GetInverse().GetTranspose();
		glUniformMatrix4fv(m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE], 1, GL_FALSE, &modelView_inverse_transpose.a[0]);

		//load material
		glUniform3fv(m_parameters[U_MATERIAL_AMBIENT], 1, &mesh->material.kAmbient.r);
		glUniform3fv(m_parameters[U_MATERIAL_DIFFUSE], 1, &mesh->material.kDiffuse.r);
		glUniform3fv(m_parameters[U_MATERIAL_SPECULAR], 1, &mesh->material.kSpecular.r);
		glUniform1f(m_parameters[U_MATERIAL_SHININESS], mesh->material.kShininess);
	}

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

void MainMenuScene::RenderEntity(Entity* entity, bool enableLight)
{
	modelStack.PushMatrix();
	modelStack.Translate(entity->getTransform().x, entity->getTransform().y, entity->getTransform().z);
	modelStack.Rotate(entity->getRotationAngle(), entity->getRotationAxis().x, entity->getRotationAxis().y, entity->getRotationAxis().z);
	modelStack.Scale(entity->getScale().x, entity->getScale().x, entity->getScale().x);

	Mtx44 MVP, modelView, modelView_inverse_transpose;

	MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
	modelView = viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MODELVIEW], 1, GL_FALSE, &modelView.a[0]);
	if (enableLight)
	{
		modelView_inverse_transpose = modelView.GetInverse().GetTranspose();
		glUniformMatrix4fv(m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE], 1, GL_FALSE, &modelView_inverse_transpose.a[0]);

		//load material
		glUniform3fv(m_parameters[U_MATERIAL_AMBIENT], 1, &entity->getMesh()->material.kAmbient.r);
		glUniform3fv(m_parameters[U_MATERIAL_DIFFUSE], 1, &entity->getMesh()->material.kDiffuse.r);
		glUniform3fv(m_parameters[U_MATERIAL_SPECULAR], 1, &entity->getMesh()->material.kSpecular.r);
		glUniform1f(m_parameters[U_MATERIAL_SHININESS], entity->getMesh()->material.kShininess);
	}
	else
	{
	}

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

void MainMenuScene::RenderText(Mesh* mesh, std::string text, Color color)
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

void MainMenuScene::RenderMeshOnScreen(Mesh* mesh, float x, float y, float sizex, float sizey)
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

void MainMenuScene::RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y)
{
	float spacing = 0.6f;

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
	modelStack.Translate(x - (text.size() * (0.5f * spacing))*size, y, 0);
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
		std::stringstream ss;
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

bool MainMenuScene::CreateButton(float buttonTop, float buttonBottom, float buttonRight, float buttonLeft)
{
	//Converting Viewport space to UI space
	double x, y;
	Application::GetCursorPos(&x, &y);
	unsigned w = Application::GetWindowWidth();
	unsigned h = Application::GetWindowHeight();
	float posX = static_cast<float>(x / 10); //convert (0,800) to (0,80)
	float posY = static_cast<float>(h / 10 - y / 10); //convert (600,0) to (0,60)
	if ((posX > buttonLeft) &&
		(posX < buttonRight) &&
		(posY > buttonBottom) &&
		(posY < buttonTop))
	{
		//trigger user action or function
		return true;
	}
	else
	{
		return false;
	}
}

void MainMenuScene::RenderMainMenu()
{
	if (menuState == MAIN)
	{
		RenderMeshOnScreen(meshList[GEO_BACKGROUND], 0, 0, Application::screenUISizeX, Application::screenUISizeY);
		RenderTextOnScreen(meshList[GEO_TEXT], "Title", Color(1, 1, 1), 5, 40, 40);

		RenderTextOnScreen(meshList[GEO_TEXT], "Play Game", Color(1, 1, 1), 2, 40, 30);
		RenderTextOnScreen(meshList[GEO_TEXT], "Options", Color(1, 1, 1), 2, 40, 25);
		RenderTextOnScreen(meshList[GEO_TEXT], "How To Play", Color(1, 1, 1), 2, 40, 20);
		RenderTextOnScreen(meshList[GEO_TEXT], "Quit", Color(1, 1, 1), 2, 40, 15);

		double x, y;
		static bool lcButtonState = true;
		Application::GetCursorPos(&x, &y);

		//start game
		if ((y < (static_cast<double>(Application::GetWindowHeight()) / 10) * 5) &&
			(y > (static_cast<double>(Application::GetWindowHeight()) / 10) * 4) &&
			(x > (static_cast<double>(Application::GetWindowWidth()) / 5) * 2) &&
			(x < (static_cast<double>(Application::GetWindowWidth()) / 5) * 3))
		{
			RenderMeshOnScreen(meshList[GEO_SELECT], 30, 31, 1, 1);

			if (!lcButtonState && Application::IsMousePressed(0))
			{
				Application::sceneState = Application::STATE_MAINMENU_EXIT;
				lcButtonState = true;
			}
			else if (lcButtonState && !Application::IsMousePressed(0))
			{
				lcButtonState = false;
			}
		}
		//options
		else if ((y < (static_cast<double>(Application::GetWindowHeight()) / 10) * 6) &&
			(y > (static_cast<double>(Application::GetWindowHeight()) / 10) * 5) &&
			(x > (static_cast<double>(Application::GetWindowWidth()) / 5) * 2) &&
			(x < (static_cast<double>(Application::GetWindowWidth()) / 5) * 3))
		{
			RenderMeshOnScreen(meshList[GEO_SELECT], 30, 26, 1, 1);

			if (!lcButtonState && Application::IsMousePressed(0))
			{
				menuState = OPTIONS;
				lcButtonState = true;
			}
			else if (lcButtonState && !Application::IsMousePressed(0))
			{
				lcButtonState = false;
			}
		}
		//how to play
		else if ((y < (static_cast<double>(Application::GetWindowHeight()) / 10) * 7) &&
			(y > (static_cast<double>(Application::GetWindowHeight()) / 10) * 6) &&
			(x > (static_cast<double>(Application::GetWindowWidth()) / 5) * 2) &&
			(x < (static_cast<double>(Application::GetWindowWidth()) / 5) * 3))
		{
			RenderMeshOnScreen(meshList[GEO_SELECT], 30, 21, 1, 1);

			if (!lcButtonState && Application::IsMousePressed(0))
			{
				menuState = HOWTOPLAY;
				lcButtonState = true;
			}
			else if (lcButtonState && !Application::IsMousePressed(0))
			{
				lcButtonState = false;
			}
		}
		//exit game
		else if ((y < (static_cast<double>(Application::GetWindowHeight()) / 10) * 8) &&
			(y > (static_cast<double>(Application::GetWindowHeight()) / 10) * 7) &&
			(x > (static_cast<double>(Application::GetWindowWidth()) / 5) * 2) &&
			(x < (static_cast<double>(Application::GetWindowWidth()) / 5) * 3))
		{
			RenderMeshOnScreen(meshList[GEO_SELECT], 30, 16, 1, 1);

			if (!lcButtonState && Application::IsMousePressed(0))
			{
				Application::ExitGame();

				lcButtonState = true;
			}
			else if (lcButtonState && !Application::IsMousePressed(0))
			{
				lcButtonState = false;
			}
		}
	}
	else if (menuState == HOWTOPLAY)
	{
		int yOffset = 0;
		RenderTextOnScreen(meshList[GEO_TEXT], "How To Play", Color(1, 1, 1), 5, 40, 50 + yOffset);
		yOffset -= 8;
		RenderTextOnScreen(meshList[GEO_TEXT], "WASD - Move", Color(1, 1, 1), 2.5, 40, 50 + yOffset);
		yOffset -= 8;
		RenderTextOnScreen(meshList[GEO_TEXT], "E - Interact", Color(1, 1, 1), 2.5, 40, 50 + yOffset);
		yOffset -= 8;
		RenderTextOnScreen(meshList[GEO_TEXT], "Left/Right Arrow - Rotate Inspect Object", Color(1, 1, 1), 2.5, 40, 50 + yOffset);
		yOffset -= 8;
		RenderTextOnScreen(meshList[GEO_TEXT], "J - Journal", Color(1, 1, 1), 2.5, 40, 50 + yOffset);
		RenderTextOnScreen(meshList[GEO_TEXT], "B or left click to leave this menu", Color(0.8, 0.8, 0.8), 2, 21, 3);

		static bool buttonState = true;
		if (!buttonState && Application::IsMousePressed(0) || (Application::IsKeyPressed('B')))
		{
			menuState = MAIN;
			buttonState = true;
		}
		else if (buttonState && !Application::IsMousePressed(0) || (Application::IsKeyPressed('B')))
		{
			buttonState = false;
		}
	}
	else if (menuState == OPTIONS)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Options", Color(1, 1, 1), 5, 40, 40);
		RenderTextOnScreen(meshList[GEO_TEXT], "Fullscreen:", Color(1, 1, 1), 2, 40, 30);
		if (Application::GetIsFullscreen())
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "On", Color(1, 1, 1), 2, 47, 30);
		}
		else
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "Off", Color(1, 1, 1), 2, 47, 30);
		}

		double x, y;
		static bool lcButtonState = true;
		Application::GetCursorPos(&x, &y);

		if ((y < (static_cast<double>(Application::GetWindowHeight()) / 10) * 5) &&
			(y > (static_cast<double>(Application::GetWindowHeight()) / 10) * 4) &&
			(x > (static_cast<double>(Application::GetWindowWidth()) / 5) * 2) &&
			(x < (static_cast<double>(Application::GetWindowWidth()) / 5) * 3))
		{
			RenderMeshOnScreen(meshList[GEO_SELECT], 30, 31, 1, 1);

			if (!lcButtonState && Application::IsMousePressed(0))
			{
				if (Application::GetIsFullscreen())
				{
					Application::ExitFullscreen();
				}
				else
				{
					Application::Fullscreen();
				}

				lcButtonState = true;
			}
			else if (lcButtonState && !Application::IsMousePressed(0))
			{
				lcButtonState = false;
			}
		}

	}
}

void MainMenuScene::Init()
{
	// Init VBO here
	Mtx44 projection;

	projection.SetToPerspective(60.f, 4.f / 3.f, 0.1f, 1000.f);
	projectionStack.LoadMatrix(projection);

	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);

	camera.Init(Vector3(4, 3, 3), Vector3(0, 0, 0), Vector3(0, 1, 0));
	camera.DisableControl();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

	meshList[GEO_BACKGROUND] = MeshBuilder::GenerateQuad("quad", Color(0, 0, 0), 1.f);

	meshList[GEO_SELECT] = MeshBuilder::GenerateQuad("quad", Color(1, 1, 1), 1.f);

	meshList[GEO_FRONT] = MeshBuilder::GenerateQuad("front", Color(1, 1, 1), 1.f);
	meshList[GEO_FRONT]->textureID = LoadTGA("Image//front.tga");

	meshList[GEO_BACK] = MeshBuilder::GenerateQuad("back", Color(1, 1, 1), 1.f);
	meshList[GEO_BACK]->textureID = LoadTGA("Image//back.tga");

	meshList[GEO_LEFT] = MeshBuilder::GenerateQuad("left", Color(1, 1, 1), 1.f);
	meshList[GEO_LEFT]->textureID = LoadTGA("Image//left.tga");

	meshList[GEO_RIGHT] = MeshBuilder::GenerateQuad("right", Color(1, 1, 1), 1.f);
	meshList[GEO_RIGHT]->textureID = LoadTGA("Image//right.tga");

	meshList[GEO_TOP] = MeshBuilder::GenerateQuad("top", Color(1, 1, 1), 1.f);
	meshList[GEO_TOP]->textureID = LoadTGA("Image//top.tga");

	meshList[GEO_BOTTOM] = MeshBuilder::GenerateQuad("bottom", Color(1, 1, 1), 1.f);
	meshList[GEO_BOTTOM]->textureID = LoadTGA("Image//bottom.tga");

	meshList[GEO_SUN] = MeshBuilder::GenerateSphere("Sphere", Color(1.0, 1.0, 1.0), 20, 20, 0.5);

	meshList[GEO_SPHERE] = MeshBuilder::GenerateSphere("Sphere", Color(0.5, 1.0, 1.0), 20, 20, 1);
	meshList[GEO_SPHERE]->textureID = LoadTGA("Image//color.tga");

	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureID = LoadTGA("Image//arial.tga");


	//hide and reset the cursor
	Application::ResetCursor();
	Application::ShowCursor();

	menuState = MAIN;
}

void MainMenuScene::Update(double dt)
{
	if (Application::IsKeyPressed('1'))
	{
		glEnable(GL_CULL_FACE);
	}
	else if (Application::IsKeyPressed('2'))
	{
		glDisable(GL_CULL_FACE);
	}
	else if (Application::IsKeyPressed('3'))
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //default fill mode
	}
	else if (Application::IsKeyPressed('4'))
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //wireframe mode
	}
	camera.Update(dt);
}

void MainMenuScene::Render()
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

	RenderMainMenu();
}

void MainMenuScene::Exit()
{
	// Cleanup VBO here
	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);
}