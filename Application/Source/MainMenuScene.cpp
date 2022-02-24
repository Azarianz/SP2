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
		RenderTextOnScreen(meshList[GEO_TEXT], "Title", textColor, 6, 40, 40);

		RenderTextOnScreen(meshList[GEO_TEXT], "Play Game", textColor, 2, 40, 30);
		RenderTextOnScreen(meshList[GEO_TEXT], "Options", textColor, 2, 40, 25);
		RenderTextOnScreen(meshList[GEO_TEXT], "How To Play", textColor, 2, 40, 20);
		RenderTextOnScreen(meshList[GEO_TEXT], "Quit", textColor, 2, 40, 15);

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
				if (Application::skipIntro == true)
				{
					Application::sceneState = Application::STATE_MAINMENU_EXIT;
				}
				else
				{
					elapsedTime = 0;
					menuState = INTRO;
				}

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
	else if (menuState == INTRO)
	{
		RenderMeshOnScreen(meshList[GEO_BACKGROUND], 0, 0, Application::screenUISizeX + 100, Application::screenUISizeY + 100);
		if (elapsedTime < 2)
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "June 13 2018", Color(1, 1, 1), 2.5, 40, 30);
		}
		else if (elapsedTime < 5)
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "It was a relaxing day on the cruise I boarded.", Color(1, 1, 1), 2.5, 40, 30);
			RenderTextOnScreen(meshList[GEO_TEXT], "I was taking a break from my job being a detective.", Color(1, 1, 1), 2.5, 40, 25);
		}
		else if (elapsedTime < 8)
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "Suddenly, a man fell over and died", Color(1, 1, 1), 2.5, 40, 30);
		}
		else if (elapsedTime < 11)
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "it seemed likely that the man was poisoned", Color(1, 1, 1), 2.5, 40, 30);
		}
		else if (elapsedTime < 14)
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "since I was on the ship", Color(1, 1, 1), 2.5, 40, 30);
			RenderTextOnScreen(meshList[GEO_TEXT], "I was assigned to the case", Color(1, 1, 1), 2.5, 40, 25);
		}
		else if (elapsedTime < 17)
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "and so begins my investigation", Color(1, 1, 1), 2.5, 40, 30);
		}
		else if (elapsedTime < 20)
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "Title", Color(1, 1, 1), 4, 40, 30);
		}
		else
		{
			Application::soundManager.ReleaseSound(Application::soundList[Application::SOUND_MAINMENU]);
			Application::soundManager.CreateSound(&Application::soundList[Application::SOUND_MAINGAME], "Sound/MainGame.wav");
			Application::soundManager.RunSound(Application::soundList[Application::SOUND_MAINGAME], true);
			Application::skipIntro = true;
			Application::sceneState = Application::STATE_MAINMENU_EXIT;
		}

		RenderTextOnScreen(meshList[GEO_TEXT], "Press 'B' to skip", Color(1, 1, 1), 2, 65, 5);

		if (Application::IsKeyPressed('B'))
		{
			elapsedTime = 20;
		}
	}
	else if (menuState == HOWTOPLAY)
	{
		int yOffset = 0;
		RenderTextOnScreen(meshList[GEO_TEXT], "How To Play", textColor, 5, 40, 50 + yOffset);
		yOffset -= 8;
		RenderTextOnScreen(meshList[GEO_TEXT], "WASD - Move", textColor, 2.5, 40, 50 + yOffset);
		yOffset -= 8;
		RenderTextOnScreen(meshList[GEO_TEXT], "E - Interact", textColor, 2.5, 40, 50 + yOffset);
		yOffset -= 8;
		RenderTextOnScreen(meshList[GEO_TEXT], "Left/Right Arrow - Rotate Inspect Object", textColor, 2.5, 40, 50 + yOffset);
		yOffset -= 8;
		RenderTextOnScreen(meshList[GEO_TEXT], "J - Journal", textColor, 2.5, 40, 50 + yOffset);
		RenderTextOnScreen(meshList[GEO_TEXT], "B or left click to leave this menu", textColor, 2, 21, 3);

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
		RenderTextOnScreen(meshList[GEO_TEXT], "Options", textColor, 5, 40, 40);
		RenderTextOnScreen(meshList[GEO_TEXT], "Fullscreen:", textColor, 2, 36.5, 30);
		RenderTextOnScreen(meshList[GEO_TEXT], "Change Resolution", textColor, 2, 40, 25);
		RenderTextOnScreen(meshList[GEO_TEXT], "Back", textColor, 2, 40, 20);

		if (Application::GetIsFullscreen())
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "On", textColor, 2, 50, 30);
		}
		else
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "Off", textColor, 2, 50, 30);
		}

		double x, y;
		static bool lcButtonState = true;
		Application::GetCursorPos(&x, &y);

		if ((y < (static_cast<double>(Application::GetWindowHeight()) / 10) * 5) &&
			(y > (static_cast<double>(Application::GetWindowHeight()) / 10) * 4) &&
			(x > (static_cast<double>(Application::GetWindowWidth()) / 5) * 2) &&
			(x < (static_cast<double>(Application::GetWindowWidth()) / 5) * 3))
		{
			RenderMeshOnScreen(meshList[GEO_SELECT], 28, 31, 1, 1);

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
		else if ((y < (static_cast<double>(Application::GetWindowHeight()) / 10) * 6) &&
			(y > (static_cast<double>(Application::GetWindowHeight()) / 10) * 5) &&
			(x > (static_cast<double>(Application::GetWindowWidth()) / 5) * 2) &&
			(x < (static_cast<double>(Application::GetWindowWidth()) / 5) * 3))
		{
			RenderMeshOnScreen(meshList[GEO_SELECT], 28, 26, 1, 1);

			if (!lcButtonState && Application::IsMousePressed(0))
			{
				menuState = RESOLUTION;
				lcButtonState = true;
			}
			else if (lcButtonState && !Application::IsMousePressed(0))
			{
				lcButtonState = false;
			}
		}
		else if ((y < (static_cast<double>(Application::GetWindowHeight()) / 10) * 7) &&
			(y > (static_cast<double>(Application::GetWindowHeight()) / 10) * 6) &&
			(x > (static_cast<double>(Application::GetWindowWidth()) / 5) * 2) &&
			(x < (static_cast<double>(Application::GetWindowWidth()) / 5) * 3))
		{
			RenderMeshOnScreen(meshList[GEO_SELECT], 28, 21, 1, 1);

			if (!lcButtonState && Application::IsMousePressed(0))
			{
				menuState = MAIN;
				lcButtonState = true;
			}
			else if (lcButtonState && !Application::IsMousePressed(0))
			{
				lcButtonState = false;
			}
		}

	}
	else if (menuState == RESOLUTION)
	{
		//RenderMeshOnScreen(meshList[GEO_BACKGROUND], 0, 0, Application::screenUISizeX, Application::screenUISizeY);
		RenderTextOnScreen(meshList[GEO_TEXT], "Resolution", textColor, 5, 40, 40);

		RenderTextOnScreen(meshList[GEO_TEXT], "1920 x 1080", textColor, 2, 40, 30);
		RenderTextOnScreen(meshList[GEO_TEXT], "1280 x 720", textColor, 2, 40, 25);
		RenderTextOnScreen(meshList[GEO_TEXT], "640 x 480", textColor, 2, 40, 20);
		RenderTextOnScreen(meshList[GEO_TEXT], "Back", textColor, 2, 40, 15);

		double x, y;
		static bool lcButtonState = true;
		Application::GetCursorPos(&x, &y);

		//1920 x 1080
		if ((y < (static_cast<double>(Application::GetWindowHeight()) / 10) * 5) &&
			(y > (static_cast<double>(Application::GetWindowHeight()) / 10) * 4) &&
			(x > (static_cast<double>(Application::GetWindowWidth()) / 5) * 2) &&
			(x < (static_cast<double>(Application::GetWindowWidth()) / 5) * 3))
		{
			RenderMeshOnScreen(meshList[GEO_SELECT], 30, 31, 1, 1);

			if (!lcButtonState && Application::IsMousePressed(0))
			{
				Application::SetResolution(1920, 1080);
				lcButtonState = true;
			}
			else if (lcButtonState && !Application::IsMousePressed(0))
			{
				lcButtonState = false;
			}
		}
		//1280 x 720
		else if ((y < (static_cast<double>(Application::GetWindowHeight()) / 10) * 6) &&
			(y > (static_cast<double>(Application::GetWindowHeight()) / 10) * 5) &&
			(x > (static_cast<double>(Application::GetWindowWidth()) / 5) * 2) &&
			(x < (static_cast<double>(Application::GetWindowWidth()) / 5) * 3))
		{
			RenderMeshOnScreen(meshList[GEO_SELECT], 30, 26, 1, 1);

			if (!lcButtonState && Application::IsMousePressed(0))
			{
				Application::SetResolution(1280, 720);
				lcButtonState = true;
			}
			else if (lcButtonState && !Application::IsMousePressed(0))
			{
				lcButtonState = false;
			}
		}
		//640 x 480
		else if ((y < (static_cast<double>(Application::GetWindowHeight()) / 10) * 7) &&
			(y > (static_cast<double>(Application::GetWindowHeight()) / 10) * 6) &&
			(x > (static_cast<double>(Application::GetWindowWidth()) / 5) * 2) &&
			(x < (static_cast<double>(Application::GetWindowWidth()) / 5) * 3))
		{
			RenderMeshOnScreen(meshList[GEO_SELECT], 30, 21, 1, 1);

			if (!lcButtonState && Application::IsMousePressed(0))
			{
				Application::SetResolution(640, 480);
				lcButtonState = true;
			}
			else if (lcButtonState && !Application::IsMousePressed(0))
			{
				lcButtonState = false;
			}
		}
		//back to options
		else if ((y < (static_cast<double>(Application::GetWindowHeight()) / 10) * 8) &&
			(y > (static_cast<double>(Application::GetWindowHeight()) / 10) * 7) &&
			(x > (static_cast<double>(Application::GetWindowWidth()) / 5) * 2) &&
			(x < (static_cast<double>(Application::GetWindowWidth()) / 5) * 3))
		{
			RenderMeshOnScreen(meshList[GEO_SELECT], 30, 16, 1, 1);

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
	}
}

void MainMenuScene::RenderSkybox()
{
	float scale = 400;

	modelStack.PushMatrix();
	modelStack.Translate(camera.position.x, -scale / 2 + 0.5, camera.position.z);
	modelStack.Scale(scale, scale, scale);
	modelStack.Rotate(-90, 1, 0, 0);
	RenderMesh(meshList[GEO_BOTTOM], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(camera.position.x, scale / 2 - 0.5, camera.position.z);
	modelStack.Scale(scale, scale, scale);
	modelStack.Rotate(270, 0, 1, 0);
	modelStack.Rotate(90, 1, 0, 0);
	RenderMesh(meshList[GEO_TOP], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(camera.position.x, 0, -scale / 2 + 0.5 + camera.position.z);
	modelStack.Scale(scale, scale, scale);
	RenderMesh(meshList[GEO_LEFT], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(camera.position.x, 0, scale / 2 - 0.5 + camera.position.z);
	modelStack.Scale(scale, scale, scale);
	modelStack.Rotate(180, 0, 1, 0);
	RenderMesh(meshList[GEO_RIGHT], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-scale / 2 + 0.5 + camera.position.x, 0, camera.position.z);
	modelStack.Scale(scale, scale, scale);
	modelStack.Rotate(90, 0, 1, 0);
	RenderMesh(meshList[GEO_BACK], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(scale / 2 - 0.5 + camera.position.x, 0, camera.position.z);
	modelStack.Scale(scale, scale, scale);
	modelStack.Rotate(90, 0, 1, 0);
	RenderMesh(meshList[GEO_FRONT], false);
	modelStack.PopMatrix();
}

MainMenuScene::MainMenuScene()
{
	enum READ_INDICATOR
	{
		FULLSCREEN = 0,
		WIDTH,
		HEIGHT
	};

	ifstream file("settings.csv");
	if (!file.is_open())
	{
		std::cout << "Impossible to open settings.csv" << std::endl;
		return;
	}

	std::string input;
	char readIndicator = -1;

	while (file >> input)
	{
		if (input == "Fullscreen:")
		{
			readIndicator = FULLSCREEN;
		}
		else if (input == "Width:")
		{
			readIndicator = WIDTH;
		}
		else if (input == "Height:")
		{
			readIndicator = HEIGHT;
		}

		if (readIndicator == FULLSCREEN)
		{
			if (input[input.size() - 1] == '1')
			{
				Application::Fullscreen();
			}
		}
		else if (readIndicator == WIDTH)
		{
			if (input == "Width:")
			{
				break;
			}

			bool isValid = true;
			for (int i = 0; i < input.size(); ++i)
			{
				if (!std::isdigit(input[i]))
				{
					std::cout << "Screen width data in settings.csv is invalid" << std::endl;
					isValid = false;
					break;
				}
			}

			if (isValid)
			{
				Application::m_width = stof(input);
			}
			else
			{
				Application::m_width = 1280;
			}
		}
		else if (readIndicator == HEIGHT)
		{
			if (input == "Height:")
			{
				break;
			}

			bool isValid = true;
			for (int i = 0; i < input.size(); ++i)
			{
				if (!std::isdigit(input[i]))
				{
					std::cout << "Screen height data in settings.csv is invalid" << std::endl;
					isValid = false;
					break;
				}
			}

			if (isValid)
			{
				Application::m_height = stof(input);
			}
			else
			{
				Application::m_height = 720;
			}
		}
	}
	Application::SetResolution(Application::m_width, Application::m_height);
}

MainMenuScene::~MainMenuScene()
{
	ofstream file("settings.csv");

	if (Application::GetIsFullscreen())
	{
		file << "Fullscreen: 1\n";
	}
	else
	{
		file << "Fullscreen: 0\n";
	}

	file << "Width: " << Application::m_width << "\n";
	file << "Height: " << Application::m_height << "\n";

}

void MainMenuScene::Init()
{
	// Init VBO here
	Mtx44 projection;

	projection.SetToPerspective(40.f, 4.f / 3.f, 0.1f, 1000.f);
	projectionStack.LoadMatrix(projection);

	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);

	camera.Init(Vector3(0, 1, 3), Vector3(3, 2, 0), Vector3(0, 1, 0));
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

	textColor = Color(0, 0, 0);

	meshList[GEO_BACKGROUND] = MeshBuilder::GenerateQuad("quad", Color(0, 0, 0), 1.f);

	meshList[GEO_SELECT] = MeshBuilder::GenerateQuad("quad", textColor, 1.f);

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

	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureID = LoadTGA("Image//typewriter.tga");

	//hide and reset the cursor
	Application::ResetCursor();
	Application::ShowCursor();

	menuState = MAIN;

	//main menu bgm
	if (Application::skipIntro == false)
	{
		Application::soundManager.CreateSound(&Application::soundList[Application::SOUND_MAINMENU], "Sound/MainMenu.wav");
		Application::soundManager.RunSound(Application::soundList[Application::SOUND_MAINMENU], true);
	}
}

void MainMenuScene::Update(double dt)
{
	if (menuState == INTRO)
	{
		elapsedTime += dt;
	}

	rotateSkybox -= 3 * dt;
	if (rotateSkybox <= -360)
	{
		rotateSkybox = 0;
	}
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

	modelStack.PushMatrix();
	modelStack.Translate(0, -40, 0);
	modelStack.Rotate(rotateSkybox, 0, 1, 0);
	RenderSkybox();
	modelStack.PopMatrix();

	RenderMainMenu();
}

void MainMenuScene::Exit()
{
	// Cleanup VBO here
	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);
}