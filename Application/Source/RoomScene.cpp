#include "RoomScene.h"
#include "GL\glew.h"

#include "shader.hpp"
#include "MeshBuilder.h"

#define LSPEED 20

void RoomScene::RenderMesh(Mesh* mesh, bool enableLight)
{
	Mtx44 MVP, modelView, modelView_inverse_transpose;

	MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
	modelView = viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MODELVIEW], 1, GL_FALSE, &modelView.a[0]);
	if (enableLight)
	{
		glUniform1i(m_parameters[U_LIGHTENABLED], 1);
		modelView_inverse_transpose = modelView.GetInverse().GetTranspose();
		glUniformMatrix4fv(m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE], 1, GL_FALSE, &modelView_inverse_transpose.a[0]);

		//load material
		glUniform3fv(m_parameters[U_MATERIAL_AMBIENT], 1, &mesh->material.kAmbient.r);
		glUniform3fv(m_parameters[U_MATERIAL_DIFFUSE], 1, &mesh->material.kDiffuse.r);
		glUniform3fv(m_parameters[U_MATERIAL_SPECULAR], 1, &mesh->material.kSpecular.r);
		glUniform1f(m_parameters[U_MATERIAL_SHININESS], mesh->material.kShininess);
	}
	else
	{
		glUniform1i(m_parameters[U_LIGHTENABLED], 0);
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

void RoomScene::RenderEntity(Entity* entity, bool enableLight)
{
	modelStack.PushMatrix();
	modelStack.Translate(entity->getTransform().x, entity->getTransform().y, entity->getTransform().z);
	modelStack.Scale(entity->getScale().x, entity->getScale().x, entity->getScale().x);

	Mtx44 MVP, modelView, modelView_inverse_transpose;

	MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
	modelView = viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MODELVIEW], 1, GL_FALSE, &modelView.a[0]);
	if (enableLight)
	{
		glUniform1i(m_parameters[U_LIGHTENABLED], 1);
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
		glUniform1i(m_parameters[U_LIGHTENABLED], 0);
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

void RoomScene::RenderText(Mesh* mesh, std::string text, Color color)
{
	if (!mesh || mesh->textureID <= 0) //Proper error check
	{
		return;
	}
	glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
	glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);
	glUniform1i(m_parameters[U_LIGHTENABLED], 0);
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

void RoomScene::RenderMeshOnScreen(Mesh* mesh, float x, float y, float sizex, float sizey)
{
	glDisable(GL_DEPTH_TEST);
	Mtx44 ortho;
	ortho.SetToOrtho(0, Application::screenUISizeX, 0, Application::screenUISizeY, -10, 10); //size of screen UI //size of screen UI //size of screen UI
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

void RoomScene::RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y)
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
	modelStack.Translate(x - text.size() * (0.6f * spacing), y, 0);
	modelStack.Scale(size, size, size);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
	glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);
	glUniform1i(m_parameters[U_LIGHTENABLED], 0);
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

void RoomScene::InspectEvidenceOnScreen(Mesh* mesh, float x, float y, float sizex, float sizey, float rotatez, float rotatex)
{
	Mtx44 ortho;
	ortho.SetToOrtho(0, 80, 0, 60, -10, 10); //size of screen UI
	projectionStack.PushMatrix();
	projectionStack.LoadMatrix(ortho);
	viewStack.PushMatrix();
	viewStack.LoadIdentity(); //No need camera for ortho mode
	modelStack.PushMatrix();
	modelStack.LoadIdentity(); //Reset modelStack
	modelStack.Translate(x, y, 0);
	modelStack.Scale(sizex, sizey, 1);
	modelStack.Rotate(rotatex, 1, 0, 0);
	modelStack.Rotate(rotatez, 0, 1, 0);
	RenderMesh(mesh, false);
	projectionStack.PopMatrix();
	viewStack.PopMatrix();
	modelStack.PopMatrix();
}

void RoomScene::RenderPressEToInteract()
{
	RenderTextOnScreen(meshList[GEO_TEXT], "[E] interact", Color(1, 1, 1), 3, 35, 10);
}

bool RoomScene::CreateButton(float buttonTop, float buttonBottom, float buttonRight, float buttonLeft)
{
	//Converting Viewport space to UI space
	double x, y;
	Application::GetCursorPos(&x, &y);
	unsigned w = Application::GetWindowWidth();
	unsigned h = Application::GetWindowHeight();
	float posX = static_cast<float>(x / 10); //convert (0,800) to (0,80)
	float posY = static_cast<float>(h / 10 - y / 10); //convert (600,0) to (0,60)
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

void RoomScene::RenderSkybox()
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

bool RoomScene::IsInDoorLInteraction()
{
	//LLayout Door interaction collision
	return ((camera.position.z >= 0) && (camera.position.z <= 2) &&
		(camera.position.x >= 4.5) && (camera.position.x <= 5));;
}

bool RoomScene::IsInDoorRInteraction()
{
	//RLayout Door interaction collision
	return ((camera.position.z >= -2.5) && (camera.position.z <= -0.5) &&
		(camera.position.x >= 4.5) && (camera.position.x <= 5));
}

void RoomScene::RenderHUD()
{
	string clues = "Clues found:" + std::to_string(Application::eList.size()) + "/20";
	string guess = "Guesses Left:" + std::to_string(Application::playerGuesses) + "/3";

	if (!isJournalOpen)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], guess, Color(1, 1, 1), 2, 8, Application::screenUISizeY - 5);
		RenderTextOnScreen(meshList[GEO_TEXT], "Find the Culprit", Color(1, 1, 1), 2, 8, Application::screenUISizeY - 8);
		RenderTextOnScreen(meshList[GEO_TEXT], "[J] Journal", Color(1, 1, 1), 2, Application::screenUISizeX - 16, Application::screenUISizeY - 5);
		RenderTextOnScreen(meshList[GEO_TEXT], clues, Color(1, 1, 1), 2, Application::screenUISizeX - 15, Application::screenUISizeY - 8);
	}
}

void RoomScene::RenderJournal()
{
	float journalButtonHeight = (Application::screenUISizeY / 10);
	float journalButtonWidth = (Application::screenUISizeX / 6);

	int buttonSpacing = 2;

	//so that button works with different resolution
	float tempScreenUISizeX = Application::GetWindowWidth() / 10;
	float tempScreenUISizeY = Application::GetWindowHeight() / 10;
	journalButtonHeight = tempScreenUISizeY / 10;
	journalButtonWidth = tempScreenUISizeX / 6;

	static bool lcButtonState = false;
	if (!lcButtonState && Application::IsMousePressed(0))
	{
		lcButtonState = true;
		//button1
		int tempCount = 1;
		if (CreateButton(tempScreenUISizeY - journalButtonHeight / 2 - buttonSpacing + journalButtonHeight / 2,
			tempScreenUISizeY - journalButtonHeight / 2 - buttonSpacing - journalButtonHeight / 2,
			(journalButtonWidth / 2 + journalButtonWidth * (tempCount + 0.45)) + (journalButtonWidth / 2 - buttonSpacing / 2),
			(journalButtonWidth / 2 + journalButtonWidth * (tempCount + 0.45)) - (journalButtonWidth / 2 - buttonSpacing / 2)))
		{
			std::cout << "journal page is: evidence" << std::endl;
			journalPage = EVIDENCE_PAGE;
		}
		++tempCount;
		if (CreateButton(tempScreenUISizeY - journalButtonHeight / 2 - buttonSpacing + journalButtonHeight / 2,
			tempScreenUISizeY - journalButtonHeight / 2 - buttonSpacing - journalButtonHeight / 2,
			(journalButtonWidth / 2 + journalButtonWidth * (tempCount + 0.35)) + (journalButtonWidth / 2 - buttonSpacing / 2),
			(journalButtonWidth / 2 + journalButtonWidth * (tempCount + 0.35)) - (journalButtonWidth / 2 - buttonSpacing / 2)))
		{
			std::cout << "journal page is: profile" << std::endl;
			journalPage = PROFILE_PAGE;
		}
	}
	else if (lcButtonState && !Application::IsMousePressed(0))
	{
		lcButtonState = false;
	}

	if (journalPage == EVIDENCE_PAGE)
	{
		RenderMeshOnScreen(meshList[GEO_JOURNAL_PAGE1], 40, 30, 40, 55);
		PrintEvidence();
	}
	else if (journalPage == PROFILE_PAGE)
	{
		RenderMeshOnScreen(meshList[GEO_JOURNAL_PAGE2], 40, 30, 40, 55);
		PrintProfiles();
	}
}

void RoomScene::ResetJournal()
{
	journalPage = EVIDENCE_PAGE;
	evidencePage = 1;
}

void RoomScene::PrintEvidence()
{
	int xpos = ((Application::screenUISizeX / 2)) - 7;
	int yOffset = ((Application::screenUISizeY / 1.7) + 7);
	static bool EButtonState = false;
	static bool QButtonState = false;

	if (!Application::eList.empty())
	{
		//evidencePage = 1;
		int index = (evidencePage - 1) * 5;
		int endIndex = evidencePage * 5;

		if (endIndex >= Application::eList.size()) {
			endIndex = (Application::eList.size());
		}

		//Add logic to print based on page number eg.(page 2: for (i = 8; i < i + 4; i++))
		for (int i = index; i <= (endIndex - 1); i++)
		{
			ss.str("");
			ss << "(" << Application::nameList[i] << ") " << Application::eList[i];
			RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 0, 0), 2, xpos, yOffset);
			yOffset -= 8;
		}

		if (!EButtonState && Application::IsKeyPressed('E'))
		{
			EButtonState = true;

			if (evidencePage >= 4) {
				evidencePage = 1;
			}
			else {
				evidencePage++;
			}

			index = (evidencePage - 1) * 5;
		}

		else if (EButtonState && !Application::IsKeyPressed('E'))
		{
			EButtonState = false;
		}

		if (!QButtonState && Application::IsKeyPressed('Q'))
		{
			QButtonState = true;

			if (evidencePage <= 1) {
				evidencePage = 4;
			}
			else {
				evidencePage--;
			}

			index = (evidencePage - 1) * 5;
		}

		else if (QButtonState && !Application::IsKeyPressed('Q'))
		{
			QButtonState = false;
		}

		RenderTextOnScreen(meshList[GEO_TEXT], "<Q", Color(1, 1, 1), 5, 5, 20);
		RenderTextOnScreen(meshList[GEO_TEXT], "E>", Color(1, 1, 1), 5, 71, 20);
	}

	const int i = 9;
	int arrag[i];
}

void RoomScene::PrintProfiles()
{
	int xpos = ((Application::screenUISizeX / 2)) - 4;
	int yOffset = ((Application::screenUISizeY / 2) - 8);
	static bool EButtonState = false;
	static bool QButtonState = false;

	switch (profilePage)
	{
		//gamer
	case 1:
		RenderMeshOnScreen(meshList[GEO_CHARACTER_PROFILE1], 40, 37.5, 25, 25);
		RenderTextOnScreen(meshList[GEO_TEXT], "The Arcade Gamer", Color(0, 0, 0), 2, xpos, yOffset);
		RenderTextOnScreen(meshList[GEO_TEXT], "Name: Ivan Sonny", Color(0, 0, 0), 2, xpos, yOffset - 4);
		RenderTextOnScreen(meshList[GEO_TEXT], "Age: 34 Years Old", Color(0, 0, 0), 2, xpos, yOffset - 8);
		RenderTextOnScreen(meshList[GEO_TEXT], "Last Seen: At the Arcade Machine", Color(0, 0, 0), 1.7f, xpos, yOffset - 12);
		RenderTextOnScreen(meshList[GEO_TEXT], "Desc: Depressed, mostly alone", Color(0, 0, 0), 1.7f, xpos, yOffset - 16);
		break;
		//guard
	case 2:
		RenderMeshOnScreen(meshList[GEO_CHARACTER_PROFILE2], 40, 37.5, 25, 25);
		RenderTextOnScreen(meshList[GEO_TEXT], "The Off-Duty Guard", Color(0, 0, 0), 2, xpos, yOffset);
		RenderTextOnScreen(meshList[GEO_TEXT], "Name: Akkop Poloka", Color(0, 0, 0), 2, xpos, yOffset - 4);
		RenderTextOnScreen(meshList[GEO_TEXT], "Age: 42 Years Old", Color(0, 0, 0), 2, xpos, yOffset - 8);
		RenderTextOnScreen(meshList[GEO_TEXT], "Last Seen: Drinking with the victim", Color(0, 0, 0), 1.7f, xpos - 1, yOffset - 12);
		RenderTextOnScreen(meshList[GEO_TEXT], "Desc: Alchoholic, Too Calm", Color(0, 0, 0), 1.7f, xpos, yOffset - 16);
		break;
		//janitor
	case 3:
		RenderMeshOnScreen(meshList[GEO_CHARACTER_PROFILE3], 40, 37.5, 25, 25);
		RenderTextOnScreen(meshList[GEO_TEXT], "The Janitor", Color(0, 0, 0), 2, xpos + 1, yOffset);
		RenderTextOnScreen(meshList[GEO_TEXT], "Name: Gertrude Heckler", Color(0, 0, 0), 2, xpos, yOffset - 4);
		RenderTextOnScreen(meshList[GEO_TEXT], "Age: 26 Years Old", Color(0, 0, 0), 2, xpos, yOffset - 8);
		RenderTextOnScreen(meshList[GEO_TEXT], "Last Seen: Throwing Victim's Bottle", Color(0, 0, 0), 1.7f, xpos - 1, yOffset - 12);
		RenderTextOnScreen(meshList[GEO_TEXT], "Desc: Grumpy, Hates Victim", Color(0, 0, 0), 1.7f, xpos, yOffset - 16);
		break;
		//kid
	case 4:
		RenderMeshOnScreen(meshList[GEO_CHARACTER_PROFILE4], 40, 37.5, 25, 25);
		RenderTextOnScreen(meshList[GEO_TEXT], "The Weird Kid", Color(0, 0, 0), 2, xpos + 1, yOffset);
		RenderTextOnScreen(meshList[GEO_TEXT], "Name: Kevin McCallister", Color(0, 0, 0), 2, xpos, yOffset - 4);
		RenderTextOnScreen(meshList[GEO_TEXT], "Age: 10 Years Old", Color(0, 0, 0), 2, xpos, yOffset - 8);
		RenderTextOnScreen(meshList[GEO_TEXT], "Last Seen: Playing with the Old Man", Color(0, 0, 0), 1.7f, xpos - 1, yOffset - 12);
		RenderTextOnScreen(meshList[GEO_TEXT], "Desc: Plays with imaginary friends", Color(0, 0, 0), 1.7f, xpos, yOffset - 16);
		break;
		//oldman
	case 5:
		RenderMeshOnScreen(meshList[GEO_CHARACTER_PROFILE5], 40, 37.5, 25, 25);
		RenderTextOnScreen(meshList[GEO_TEXT], "The Old Man", Color(0, 0, 0), 2, xpos + 1, yOffset);
		RenderTextOnScreen(meshList[GEO_TEXT], "Name: Izan Eisenhower", Color(0, 0, 0), 2, xpos, yOffset - 4);
		RenderTextOnScreen(meshList[GEO_TEXT], "Age: 80 Years Old", Color(0, 0, 0), 2, xpos, yOffset - 8);
		RenderTextOnScreen(meshList[GEO_TEXT], "Last Seen: Sitting with the Kid", Color(0, 0, 0), 1.7f, xpos, yOffset - 12);
		RenderTextOnScreen(meshList[GEO_TEXT], "Desc: Forgetful, has dementia", Color(0, 0, 0), 1.7f, xpos, yOffset - 16);
		break;
	default:
		break;
	}

	if (!EButtonState && Application::IsKeyPressed('E'))
	{
		EButtonState = true;

		if (profilePage >= 5) {
			profilePage = 1;
		}
		else {
			profilePage++;
		}
	}

	else if (EButtonState && !Application::IsKeyPressed('E'))
	{
		EButtonState = false;
	}

	if (!QButtonState && Application::IsKeyPressed('Q'))
	{
		QButtonState = true;

		if (profilePage <= 1) {
			profilePage = 5;
		}
		else {
			profilePage--;
		}
	}

	else if (QButtonState && !Application::IsKeyPressed('Q'))
	{
		QButtonState = false;
	}

	RenderTextOnScreen(meshList[GEO_TEXT], "<Q", Color(1, 1, 1), 5, 5, 20);
	RenderTextOnScreen(meshList[GEO_TEXT], "E>", Color(1, 1, 1), 5, 71, 20);
}

void RoomScene::RoomCollision1()
{
	if ((camera.position.z >= 3.4))
	{
		camera.position.z = 3.4;
		camera.target = camera.position + camera.view;
	}
	if ((camera.position.z <= -3.5) && (camera.position.x <= 0) && (camera.position.x >= -5))
	{
		camera.position.z = -3.5;
		camera.target = camera.position + camera.view;
	}
	if ((camera.position.x <= -0.05) && (camera.position.x >= 0) && (camera.position.z <= -0.2) && (camera.position.z >= -5))
	{
		camera.position.x = -0.05;
		camera.target = camera.position + camera.view;
	}
	if ((camera.position.z <= -4.2))
	{
		camera.position.z = -4.2;
		camera.target = camera.position + camera.view;
	}
	if ((camera.position.x >= 4.86))
	{
		camera.position.x = 4.86;
		camera.target = camera.position + camera.view;
	}
	if ((camera.position.x <= -4.2))
	{
		camera.position.x = -4.2;
		camera.target = camera.position + camera.view;
	}
	if ((camera.position.z <= 0.3) && (camera.position.x >= -0.1) && (camera.position.x <= 0.2))
	{
		camera.position.x = -0.1;
		camera.target = camera.position + camera.view;
	}
	if ((camera.position.z <= 0.3) && (camera.position.x >= 0.2) && (camera.position.x <= 0.7))
	{
		camera.position.x = 0.7;
		camera.target = camera.position + camera.view;
	}
	if ((camera.position.x >= 3.5) && (camera.position.z <= 0.3) && (camera.position.z >= -0.7))
	{
		camera.position.z = 0.1;
		camera.target = camera.position + camera.view;
	}
	if ((camera.position.x >= 3.5) && (camera.position.z <= -0.7) && (camera.position.z >= -1.1))
	{
		camera.position.z = -1.1;
		camera.target = camera.position + camera.view;
	}
	if ((camera.position.x <= 2.5) && (camera.position.x >= -0.1) && (camera.position.z <= 0.3) && (camera.position.z >= -0.7))
	{
		camera.position.z = 0.3;
		camera.target = camera.position + camera.view;
	}
	if ((camera.position.x <= 2.5) && (camera.position.x >= -0.1) && (camera.position.z <= -0.7) && (camera.position.z >= -1.1))
	{
		camera.position.z = -1.1;
		camera.target = camera.position + camera.view;
	}
}

void RoomScene::RoomCollision2()
{
	if ((camera.position.z >= 4.5))
	{
		camera.position.z = 4.5;
		camera.target = camera.position + camera.view;
	}
	if ((camera.position.z <= -3.9))
	{
		camera.position.z = -3.9;
		camera.target = camera.position + camera.view;
	}
	if ((camera.position.x <= -4))
	{
		camera.position.x = -4;
		camera.target = camera.position + camera.view;
	}
	if ((camera.position.x >= 4.4))
	{
		camera.position.x = 4.4;
		camera.target = camera.position + camera.view;
	}
	if ((camera.position.z >= 3) && (camera.position.x <= -0.27))
	{
		camera.position.z = 3;
		camera.target = camera.position + camera.view;
	}
	if ((camera.position.z >= 0.1) && (camera.position.x >= -0.27) && (camera.position.x <= 0))
	{
		camera.position.x = -0.27;
		camera.target = camera.position + camera.view;
	}
	if ((camera.position.z >= 0.1) && (camera.position.x >= 0) && (camera.position.x <= 0.7))
	{
		camera.position.x = 0.7;
		camera.target = camera.position + camera.view;
	}
	if ((camera.position.x >= 3.2) && (camera.position.z >= 0.1) && (camera.position.z <= 0.3))
	{
		camera.position.z = 0.1;
		camera.target = camera.position + camera.view;
	}
	if ((camera.position.x >= 3.2) && (camera.position.z >= 0.3) && (camera.position.z <= 0.9))
	{
		camera.position.z = 0.9;
		camera.target = camera.position + camera.view;
	}
	if ((camera.position.x >= -0.27) && (camera.position.x <= 2.2) && (camera.position.z >= 0.1) && (camera.position.z <= 0.3))
	{
		camera.position.z = 0.1;
		camera.target = camera.position + camera.view;
	}
	if ((camera.position.x >= -0.27) && (camera.position.x <= 2.2) && (camera.position.z >= 0.3) && (camera.position.z <= 0.9))
	{
		camera.position.z = 0.9;
		camera.target = camera.position + camera.view;
	}
}

void RoomScene::RenderEvidenceObject(Entity* entity, float rangeX, float rangeZ) {
	//inspect
	{
		if (Pickup == false)
		{
			RenderEntity(entity, true);
		}

		if (camera.position.x >= entity->getTransform().x - rangeX
			&& camera.position.x <= entity->getTransform().x + rangeX
			&& camera.position.z >= entity->getTransform().z - rangeZ
			&& camera.position.z <= entity->getTransform().z + rangeZ)
		{
			if (!Inspect)
			{
				RenderTextOnScreen(meshList[GEO_TEXT], "[F] inspect", Color(1, 1, 1), 3, 35, 10);

				if (Application::IsKeyPressed('F') && (Interacted == false) && !isJournalOpen)
				{
					if (entity == &entityList[ENTITY_DEMENTIA_PILLS])
					{
						Application::AddEvidence("Dementia pills", "Old Man");
					}
					else if (entity == &entityList[ENTITY_ANTIDEPRESSANT_PILLS])
					{
						Application::AddEvidence("Antidepressant pills", "Gamer");
					}
					else if (entity == &entityList[ENTITY_LAPTOP])
					{
						Application::AddEvidence("Laptop/Phone Messages", "Gamer");
					}
					else if (entity == &entityList[ENTITY_GUNCASE])
					{
						Application::AddEvidence("Gun in his room", "Old Man");
					}
					else if (entity == &entityList[ENTITY_CREEPYDRAWING])
					{
						Application::AddEvidence("Drawing of demonic friends", "Kid");
					}
					Inspect = true;
					Interacted = true;
					Pickup = true;
					Application::SetCanPause(false);
					camera.DisableControl();

					return;
				}
				else if (!Application::IsKeyPressed('F') && (Interacted == true))
				{
					Interacted = false;
				}
			}
			else if (Inspect)
			{
				if (Application::IsKeyPressed('F') && (Interacted == false) && !isJournalOpen)
				{
					text = false;
					Inspect = false;
					Pickup = false;
					Interacted = true;
					Application::SetCanPause(true);
					camera.EnableControl();
				}
				else if (!Application::IsKeyPressed('F') && (Interacted == true))
				{
					Interacted = false;
				}

				InspectEvidenceOnScreen(entity->getMesh(), 40, 30, 60, 60, rotateZ, rotateX);

				RenderTextOnScreen(meshList[GEO_TEXT], "[F] stop inspecting", Color(1, 1, 1), 3, 30, 10);
				RenderTextOnScreen(meshList[GEO_TEXT], "Arrow Keys To Turn The Object", Color(1, 1, 1), 1.5, 38, 5);
			}
		}
	}
}

void RoomScene::Init()
{
		// Init VBO here
		Mtx44 projection;

		projection.SetToPerspective(60.f, 4.f / 3.f, 0.1f, 1000.f);
		projectionStack.LoadMatrix(projection);

		glDisable(GL_CULL_FACE);
		glEnable(GL_BLEND);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//Light Settings
		{
			//Arcade Guy Room
			if (Application::roomState == Application::ROOM1) 
			{
				//Light 1 (Bathroom L)
				light[0].type = Light::LIGHT_SPOT;
				light[0].position.Set(3, 3.5, -3.5);
				light[0].color.Set(1, 1, 1);
				light[0].power = 1.4f;
				light[0].kC = 1.f;
				light[0].kL = 0.01f;
				light[0].kQ = 0.001f;
				light[0].cosCutoff = cos(Math::DegreeToRadian(45));
				light[0].cosInner = cos(Math::DegreeToRadian(30));
				light[0].exponent = 3.f;
				light[0].spotDirection.Set(0.f, 1.f, 0.f);

				//Light 2 (Bedroom L)
				light[1].type = Light::LIGHT_SPOT;
				light[1].position.Set(-2.8, 7, 3);
				light[1].color.Set(1, 1, 1);
				light[1].power = 1.5f;
				light[1].kC = 1.f;
				light[1].kL = 0.01f;
				light[1].kQ = 0.001f;
				light[1].cosCutoff = cos(Math::DegreeToRadian(45));
				light[1].cosInner = cos(Math::DegreeToRadian(30));
				light[1].exponent = 3.f;
				light[1].spotDirection.Set(0.f, 1.f, 0.f);
			}

			//Kid Room
			if (Application::roomState == Application::ROOM3)
			{
				//Light 1 (Bathroom L)
				light[0].type = Light::LIGHT_SPOT;
				light[0].position.Set(3, 3.5, -3.5);
				light[0].color.Set(1, 1, 1);
				light[0].power = 1.4f;
				light[0].kC = 1.f;
				light[0].kL = 0.01f;
				light[0].kQ = 0.001f;
				light[0].cosCutoff = cos(Math::DegreeToRadian(45));
				light[0].cosInner = cos(Math::DegreeToRadian(30));
				light[0].exponent = 3.f;
				light[0].spotDirection.Set(0.f, 1.f, 0.f);
			}

			//Old Man
			if (Application::roomState == Application::ROOM2)
			{
				//Light 1 (Bathroom R)
				light[0].type = Light::LIGHT_SPOT;
				light[0].position.Set(2.5, 3.5, 2.5);
				light[0].color.Set(1, 1, 1);
				light[0].power = 1.4f;
				light[0].kC = 1.f;
				light[0].kL = 0.01f;
				light[0].kQ = 0.001f;
				light[0].cosCutoff = cos(Math::DegreeToRadian(45));
				light[0].cosInner = cos(Math::DegreeToRadian(30));
				light[0].exponent = 3.f;
				light[0].spotDirection.Set(0.f, 1.f, 0.f);

				//Light 2 (Bedroom R)
				light[1].type = Light::LIGHT_SPOT;
				light[1].position.Set(-2.5, 2.6, 2.5);
				light[1].color.Set(1, 1, 1);
				light[1].power = 1.5f;
				light[1].kC = 1.f;
				light[1].kL = 0.01f;
				light[1].kQ = 0.001f;
				light[1].cosCutoff = cos(Math::DegreeToRadian(50));
				light[1].cosInner = cos(Math::DegreeToRadian(30));
				light[1].exponent = 3.f;
				light[1].spotDirection.Set(0.f, 1.f, 0.f);
			}

			//Victim Room
			if (Application::roomState == Application::ROOM4)
			{
				//Light 1 (Bathroom R)
				light[0].type = Light::LIGHT_SPOT;
				light[0].position.Set(2.5, 3.5, 2.5);
				light[0].color.Set(1, 1, 1);
				light[0].power = 1.4f;
				light[0].kC = 1.f;
				light[0].kL = 0.01f;
				light[0].kQ = 0.001f;
				light[0].cosCutoff = cos(Math::DegreeToRadian(45));
				light[0].cosInner = cos(Math::DegreeToRadian(30));
				light[0].exponent = 3.f;
				light[0].spotDirection.Set(0.f, 1.f, 0.f);

				//Light 2 (Bedroom R)
				light[1].type = Light::LIGHT_SPOT;
				light[1].position.Set(-2, 2.6f, -3);
				light[1].color.Set(1, 1, 1);
				light[1].power = 1.5f;
				light[1].kC = 1.f;
				light[1].kL = 0.01f;
				light[1].kQ = 0.001f;
				light[1].cosCutoff = cos(Math::DegreeToRadian(45));
				light[1].cosInner = cos(Math::DegreeToRadian(30));
				light[1].exponent = 3.f;
				light[1].spotDirection.Set(0.f, 1.f, 0.f);
			}
		}

		//set background color
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

		//generate default VAO for now
		glGenVertexArrays(1, &m_vertexArrayID);
		glBindVertexArray(m_vertexArrayID);

		if (Application::roomState == Application::ROOM1 ||
			Application::roomState == Application::ROOM3)
		{
			camera.Init(Vector3(4, 1.5, 1), Vector3(0, 1.5, 1), Vector3(0, 1, 0));
		}

		if (Application::roomState == Application::ROOM2 ||
			Application::roomState == Application::ROOM4)
		{
			camera.Init(Vector3(5, 1.5, -1.5), Vector3(0, 1.5, -1.5), Vector3(0, 1, 0));
		}

		//load vertex and fragment shaders
		m_programID = LoadShaders("Shader//Texture.vertexshader", "Shader//Text.fragmentshader");
		m_parameters[U_MVP] = glGetUniformLocation(m_programID, "MVP");
		m_parameters[U_MODELVIEW] = glGetUniformLocation(m_programID, "MV");
		m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE] = glGetUniformLocation(m_programID, "MV_inverse_transpose");
		m_parameters[U_MATERIAL_AMBIENT] = glGetUniformLocation(m_programID, "material.kAmbient");
		m_parameters[U_MATERIAL_DIFFUSE] = glGetUniformLocation(m_programID, "material.kDiffuse");
		m_parameters[U_MATERIAL_SPECULAR] = glGetUniformLocation(m_programID, "material.kSpecular");
		m_parameters[U_MATERIAL_SHININESS] = glGetUniformLocation(m_programID, "material.kShininess");

		//Lights mpram
		{
			m_parameters[U_LIGHT0_TYPE] = glGetUniformLocation(m_programID, "lights[0].type");
			m_parameters[U_LIGHT0_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[0].spotDirection");
			m_parameters[U_LIGHT0_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[0].cosCutoff");
			m_parameters[U_LIGHT0_COSINNER] = glGetUniformLocation(m_programID, "lights[0].cosInner");
			m_parameters[U_LIGHT0_EXPONENT] = glGetUniformLocation(m_programID, "lights[0].exponent");
			m_parameters[U_LIGHT0_POSITION] = glGetUniformLocation(m_programID, "lights[0].position_cameraspace");
			m_parameters[U_LIGHT0_COLOR] = glGetUniformLocation(m_programID, "lights[0].color");
			m_parameters[U_LIGHT0_POWER] = glGetUniformLocation(m_programID, "lights[0].power");
			m_parameters[U_LIGHT0_KC] = glGetUniformLocation(m_programID, "lights[0].kC");
			m_parameters[U_LIGHT0_KL] = glGetUniformLocation(m_programID, "lights[0].kL");
			m_parameters[U_LIGHT0_KQ] = glGetUniformLocation(m_programID, "lights[0].kQ");

			m_parameters[U_LIGHT1_TYPE] = glGetUniformLocation(m_programID, "lights[1].type");
			m_parameters[U_LIGHT1_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[1].spotDirection");
			m_parameters[U_LIGHT1_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[1].cosCutoff");
			m_parameters[U_LIGHT1_COSINNER] = glGetUniformLocation(m_programID, "lights[1].cosInner");
			m_parameters[U_LIGHT1_EXPONENT] = glGetUniformLocation(m_programID, "lights[1].exponent");
			m_parameters[U_LIGHT1_POSITION] = glGetUniformLocation(m_programID, "lights[1].position_cameraspace");
			m_parameters[U_LIGHT1_COLOR] = glGetUniformLocation(m_programID, "lights[1].color");
			m_parameters[U_LIGHT1_POWER] = glGetUniformLocation(m_programID, "lights[1].power");
			m_parameters[U_LIGHT1_KC] = glGetUniformLocation(m_programID, "lights[1].kC");
			m_parameters[U_LIGHT1_KL] = glGetUniformLocation(m_programID, "lights[1].kL");
			m_parameters[U_LIGHT1_KQ] = glGetUniformLocation(m_programID, "lights[1].kQ");
		}

		m_parameters[U_NUMLIGHTS] = glGetUniformLocation(m_programID, "numLights");
		m_parameters[U_LIGHTENABLED] = glGetUniformLocation(m_programID, "lightEnabled");

		// Get a handle for our "colorTexture" uniform
		m_parameters[U_COLOR_TEXTURE_ENABLED] = glGetUniformLocation(m_programID, "colorTextureEnabled");
		m_parameters[U_COLOR_TEXTURE] = glGetUniformLocation(m_programID, "colorTexture");

		// Get a handle for our "textColor" uniform
		m_parameters[U_TEXT_ENABLED] = glGetUniformLocation(m_programID, "textEnabled");
		m_parameters[U_TEXT_COLOR] = glGetUniformLocation(m_programID, "textColor");

		//use our shader
		glUseProgram(m_programID);

		// Make sure you pass uniform parameters after glUseProgram()
		glUniform1i(m_parameters[U_NUMLIGHTS], 2);

		//Lights glparam
		{
			//light[0].type = Light::LIGHT_DIRECTIONAL;
			glUniform1i(m_parameters[U_LIGHT0_TYPE], light[0].type);
			glUniform3fv(m_parameters[U_LIGHT0_COLOR], 1, &light[0].color.r);
			glUniform1f(m_parameters[U_LIGHT0_POWER], light[0].power);

			//light[0].type = Light::LIGHT_POINT;
			glUniform1f(m_parameters[U_LIGHT0_KC], light[0].kC);
			glUniform1f(m_parameters[U_LIGHT0_KL], light[0].kL);
			glUniform1f(m_parameters[U_LIGHT0_KQ], light[0].kQ);

			//light[0].type = Light::LIGHT_SPOT;
			glUniform1f(m_parameters[U_LIGHT0_COSCUTOFF], light[0].cosCutoff);
			glUniform1f(m_parameters[U_LIGHT0_COSINNER], light[0].cosInner);
			glUniform1f(m_parameters[U_LIGHT0_EXPONENT], light[0].exponent);

			//light[1].type = Light::LIGHT_DIRECTIONAL;
			glUniform1i(m_parameters[U_LIGHT1_TYPE], light[1].type);
			glUniform3fv(m_parameters[U_LIGHT1_COLOR], 1, &light[1].color.r);
			glUniform1f(m_parameters[U_LIGHT1_POWER], light[1].power);

			//light[1].type = Light::LIGHT_POINT;
			glUniform1f(m_parameters[U_LIGHT1_KC], light[1].kC);
			glUniform1f(m_parameters[U_LIGHT1_KL], light[1].kL);
			glUniform1f(m_parameters[U_LIGHT1_KQ], light[1].kQ);

			//light[1].type = Light::LIGHT_SPOT;
			glUniform1f(m_parameters[U_LIGHT1_COSCUTOFF], light[1].cosCutoff);
			glUniform1f(m_parameters[U_LIGHT1_COSINNER], light[1].cosInner);
			glUniform1f(m_parameters[U_LIGHT1_EXPONENT], light[1].exponent);
		}

		//Enable depth test
		glEnable(GL_DEPTH_TEST);

		//Get a handle for our "MVP" uniform
		m_parameters[U_MVP] = glGetUniformLocation(m_programID, "MVP");

		Mesh::SetMaterialLoc(m_parameters[U_MATERIAL_AMBIENT],
			m_parameters[U_MATERIAL_DIFFUSE],
			m_parameters[U_MATERIAL_SPECULAR],
			m_parameters[U_MATERIAL_SHININESS]);

		meshList[GEO_QUAD] = MeshBuilder::GenerateQuad("quad", Color(0.5, 0.5, 0.5), 1.f);

		meshList[GEO_QUAD_BUTTON] = MeshBuilder::GenerateQuad("quad", Color(0, 0, 0), 1.f);

		meshList[GEO_SUN] = MeshBuilder::GenerateSphere("Sphere", Color(1.0, 1.0, 1.0), 20, 20, 0.5);

		meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
		meshList[GEO_TEXT]->textureID = LoadTGA("Image//typewriter.tga");

		//Journal
		{
			meshList[GEO_JOURNAL_PAGE1] = MeshBuilder::GenerateQuad("Journal Page 1", Color(1, 1, 1), 1.f);
			meshList[GEO_JOURNAL_PAGE1]->textureID = LoadTGA("Image//journal_1.tga");

			meshList[GEO_JOURNAL_PAGE2] = MeshBuilder::GenerateQuad("Journal Page 2", Color(1, 1, 1), 1.f);
			meshList[GEO_JOURNAL_PAGE2]->textureID = LoadTGA("Image//journal_2.tga");

			meshList[GEO_CHARACTER_PROFILE1] = MeshBuilder::GenerateQuad("Gamer Profile", Color(1, 1, 1), 1.f);
			meshList[GEO_CHARACTER_PROFILE1]->textureID = LoadTGA("Image//profile//gamer_profile.tga");

			meshList[GEO_CHARACTER_PROFILE2] = MeshBuilder::GenerateQuad("Guard Profile", Color(1, 1, 1), 1.f);
			meshList[GEO_CHARACTER_PROFILE2]->textureID = LoadTGA("Image//profile//guard_profile.tga");

			meshList[GEO_CHARACTER_PROFILE3] = MeshBuilder::GenerateQuad("Janitor Profile", Color(1, 1, 1), 1.f);
			meshList[GEO_CHARACTER_PROFILE3]->textureID = LoadTGA("Image//profile//janitor_profile.tga");

			meshList[GEO_CHARACTER_PROFILE4] = MeshBuilder::GenerateQuad("Kid Profile", Color(1, 1, 1), 1.f);
			meshList[GEO_CHARACTER_PROFILE4]->textureID = LoadTGA("Image//profile//kid_profile.tga");

			meshList[GEO_CHARACTER_PROFILE5] = MeshBuilder::GenerateQuad("Old Man Profile", Color(1, 1, 1), 1.f);
			meshList[GEO_CHARACTER_PROFILE5]->textureID = LoadTGA("Image//profile//oldman_profile.tga");
		}

		//evidence
		{
			entityList[ENTITY_DEMENTIA_PILLS].setMesh(MeshBuilder::GenerateOBJMTL("dementia pills", "OBJ//evidence//bottle_pills.obj", "OBJ//evidence//bottle_pills.mtl"));
			entityList[ENTITY_DEMENTIA_PILLS].getMesh()->textureID = LoadTGA("Image//PolygonOffice_Texture_03_B.tga");
			entityList[ENTITY_DEMENTIA_PILLS].setTransform(Vector3(0.5, 1.02, 1.9)); //transform by default is 0,0,0

			entityList[ENTITY_ANTIDEPRESSANT_PILLS].setMesh(MeshBuilder::GenerateOBJMTL("anti-depressants", "OBJ//evidence//pills.obj", "OBJ//evidence//pills.mtl"));
			entityList[ENTITY_ANTIDEPRESSANT_PILLS].getMesh()->textureID = LoadTGA("Image//PolygonOffice_Texture_03_B.tga");
			entityList[ENTITY_ANTIDEPRESSANT_PILLS].setTransform(Vector3(-1, 0.5f, -3.4f)); //transform by default is 0,0,0

			entityList[ENTITY_GUNCASE].setMesh(MeshBuilder::GenerateOBJMTL("", "OBJ//evidence//gun_briefcase.obj", "OBJ//evidence//gun_briefcase.mtl"));
			entityList[ENTITY_GUNCASE].getMesh()->textureID = LoadTGA("Image//PolygonOffice_Texture_03_B.tga");
			entityList[ENTITY_GUNCASE].setTransform(Vector3(-4.3, 0.3, 3)); //transform by default is 0,0,0

			entityList[ENTITY_LAPTOP].setMesh(MeshBuilder::GenerateOBJMTL("cleaner cart", "OBJ//evidence//laptop.obj", "OBJ//evidence//laptop.mtl"));
			entityList[ENTITY_LAPTOP].getMesh()->textureID = LoadTGA("Image//PolygonOffice_Texture_03_B.tga");
			entityList[ENTITY_LAPTOP].setTransform(Vector3(-3, 0.7, 1.5)); //transform by default is 0,0,0

			entityList[ENTITY_CREEPYDRAWING].setMesh(MeshBuilder::GenerateOBJMTL("creepy drawing", "OBJ//evidence//creepy_drawing.obj", "OBJ//evidence//creepy_drawing.mtl"));
			entityList[ENTITY_CREEPYDRAWING].getMesh()->textureID = LoadTGA("Image//creepy_drawing.tga");
			entityList[ENTITY_CREEPYDRAWING].setTransform(Vector3(4.5, 1, -3)); //transform by default is 0,0,0
		}

		//Skybox
		{
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
		}

		//Room Stage + Assets
		{
			meshList[GEO_ROOML] = MeshBuilder::GenerateOBJMTL("Left Room Layout", "OBJ//ship_roomL.obj", "OBJ//ship_roomL.mtl");
			meshList[GEO_ROOML]->textureID = LoadTGA("Image//PolygonOffice_Texture_02_A.tga");
			meshList[GEO_ROOMR] = MeshBuilder::GenerateOBJMTL("Right Room Layout", "OBJ//ship_roomR.obj", "OBJ//ship_roomR.mtl");
			meshList[GEO_ROOMR]->textureID = LoadTGA("Image//PolygonOffice_Texture_02_A.tga");

			//Room Furnitures
			meshList[GEO_ROOM1_FURNITURE] = MeshBuilder::GenerateOBJMTL("Left1 Furniture", "OBJ//ship_room1_furniture.obj", "OBJ//ship_room1_furniture.mtl");
			meshList[GEO_ROOM1_FURNITURE]->textureID = LoadTGA("Image//PolygonOffice_Texture_02_A.tga");
			meshList[GEO_ROOM2_FURNITURE] = MeshBuilder::GenerateOBJMTL("Right1 Furniture", "OBJ//ship_room2_furniture.obj", "OBJ//ship_room2_furniture.mtl");
			meshList[GEO_ROOM2_FURNITURE]->textureID = LoadTGA("Image//PolygonOffice_Texture_02_A.tga");
		}

		//hide and reset the cursor
		Application::ResetCursor();
		Application::HideCursor();
}

void RoomScene::Update(double dt)
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

	//Rotate Inspect Item
	{
		//rotate item
		if (Application::IsKeyPressed(VK_UP))
		{
			rotateX += 90 * dt;
		}
		else if (Application::IsKeyPressed(VK_DOWN))
		{
			rotateX -= 90 * dt;
		}
		if (Application::IsKeyPressed(VK_LEFT))
		{
			rotateZ += 110 * dt;
		}
		else if (Application::IsKeyPressed(VK_RIGHT))
		{
			rotateZ -= 110 * dt;
		}
	}

	//Journal
	{
		static bool jButtonState = false;
		if (!jButtonState && Application::IsKeyPressed('J') && !isJournalOpen && !Inspect)
		{
			Application::SetCanPause(false);
			camera.DisableControl();
			Application::ShowCursor();
			isJournalOpen = true;
			jButtonState = true;
		}
		else if (jButtonState && !Application::IsKeyPressed('J'))
		{
			jButtonState = false;
		}
		else if (!jButtonState && Application::IsKeyPressed('J') && isJournalOpen)
		{
			Application::SetCanPause(true);
			camera.EnableControl();
			Application::HideCursor();
			isJournalOpen = false;
			jButtonState = true;
			ResetJournal();
		}
	}

	//Check Door Interaction Collision
	if ((IsInDoorLInteraction() || IsInDoorRInteraction())
		&& Application::IsKeyPressed('E')) {
		Application::ResetCursor();
		Application::ShowCursor();
		Application::sceneState = Application::STATE_ROOM_EXIT;
	}
}

void RoomScene::Render()
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

	Position lightPosition_cameraspace = viewStack.Top() * light[0].position;
	glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, &lightPosition_cameraspace.x);
	Vector3 spotDirection_cameraspace = viewStack.Top() * light[0].spotDirection;
	glUniform3fv(m_parameters[U_LIGHT0_SPOTDIRECTION], 1, &spotDirection_cameraspace.x);

	lightPosition_cameraspace = viewStack.Top() * light[1].position;
	glUniform3fv(m_parameters[U_LIGHT1_POSITION], 1, &lightPosition_cameraspace.x);
	spotDirection_cameraspace = viewStack.Top() * light[1].spotDirection;
	glUniform3fv(m_parameters[U_LIGHT1_SPOTDIRECTION], 1, &spotDirection_cameraspace.x);

	RenderSkybox();

	//Stage + Assets
	{	
		//Room 1 (Arcade Guy)
		if (Application::roomState == Application::ROOM1) {
			modelStack.PushMatrix();
			modelStack.Translate(0, 0, 0);
			modelStack.Scale(1, 1, 1);
			RenderMesh(meshList[GEO_ROOML], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(0, 0, 0);
			modelStack.Scale(1, 1, 1);
			RenderMesh(meshList[GEO_ROOM1_FURNITURE], true);
			modelStack.PopMatrix();

			RenderEvidenceObject(&entityList[ENTITY_ANTIDEPRESSANT_PILLS], 1, 1);
		}

		//Room 2 (Old Man)
		else if (Application::roomState == Application::ROOM2) {			
			modelStack.PushMatrix();
			modelStack.Translate(0, 0, 0);
			modelStack.Scale(1, 1, 1);
			RenderMesh(meshList[GEO_ROOMR], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(0, 0, 0);
			modelStack.Scale(1, 1, 1);
			RenderMesh(meshList[GEO_ROOM2_FURNITURE], true);
			modelStack.PopMatrix();

			RenderEvidenceObject(&entityList[ENTITY_GUNCASE], 1, 1);
			RenderEvidenceObject(&entityList[ENTITY_DEMENTIA_PILLS], 1, 1);
		}

		//Room 3 (Kid):
		else if (Application::roomState == Application::ROOM3){
			modelStack.PushMatrix();
			modelStack.Translate(0, 0, 0);
			modelStack.Scale(1, 1, 1);
			RenderMesh(meshList[GEO_ROOML], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(0, 0, 0);
			modelStack.Scale(1, 1, 1);
			RenderMesh(meshList[GEO_ROOM1_FURNITURE], true);
			modelStack.PopMatrix();

			RenderEvidenceObject(&entityList[ENTITY_CREEPYDRAWING], 1, 1);
		}

		//Room 4 (Victim's Room): WIP
		else if (Application::roomState == Application::ROOM4) {
			modelStack.PushMatrix();
			modelStack.Translate(0, 0, 0);
			modelStack.Scale(1, 1, 1);
			RenderMesh(meshList[GEO_ROOMR], true);
			modelStack.PopMatrix();

			modelStack.PushMatrix();
			modelStack.Translate(0, 0, 0);
			modelStack.Scale(1, 1, 1);
			RenderMesh(meshList[GEO_ROOM2_FURNITURE], true);
			modelStack.PopMatrix();

			RenderEvidenceObject(&entityList[ENTITY_LAPTOP], 1, 1);
		}
	}

	//TODO COLLISIONS FOR ROOM WALLS (WITHOUT FURNITURE)
	//Left Layout
	if (Application::roomState == Application::ROOM1 ||
		Application::roomState == Application::ROOM3) {

		if (IsInDoorLInteraction())
		{
			RenderPressEToInteract();
		}
		RoomCollision1();
	}

	//Right Layout
	if (Application::roomState == Application::ROOM2 ||
		Application::roomState == Application::ROOM4) {
		if (IsInDoorRInteraction())
		{
			RenderPressEToInteract();
		}
		RoomCollision2();
	}

	if (isJournalOpen)
	{
		RenderJournal();
	}

	RenderHUD();
}

void RoomScene::Exit()
{
	// Cleanup VBO here
	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);
}