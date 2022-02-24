#include "LobbyScene.h"
#include "GL\glew.h"

#include "shader.hpp"
#include "MeshBuilder.h"

#define LSPEED 20

void LobbyScene::RenderMesh(Mesh* mesh, bool enableLight)
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

void LobbyScene::RenderEntity(Entity* entity, bool enableLight)
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

void LobbyScene::RenderText(Mesh* mesh, std::string text, Color color)
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

void LobbyScene::RenderMeshOnScreen(Mesh* mesh, float x, float y, float sizex, float sizey)
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

void LobbyScene::RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y)
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
	if (!isTalking)
	{
		modelStack.Translate(x - text.size() * (0.6f * spacing), y, 0);
	}
	else
	{
		modelStack.Translate(x, y, 0);
	}
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

void LobbyScene::RenderOfficers()
{
	//chief
	if (camera.position.x - 1.5 <= entityList[ENTITY_CHIEF].getTransform().x + interactOffset
		&& camera.position.z + 1.5 >= entityList[ENTITY_CHIEF].getTransform().z - interactOffset
		&& camera.position.x + 1.5 >= entityList[ENTITY_CHIEF].getTransform().x - interactOffset
		&& camera.position.z - 1.5 <= entityList[ENTITY_CHIEF].getTransform().z + interactOffset)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], ("You can have a look at them."), Color(1, 1, 1), 2, 35, 2);
		RenderTextOnScreen(meshList[GEO_TEXT], ("On your left, you'll find a table of confiscated evidences."), Color(1, 1, 1), 2, 22, 4.5);
		RenderTextOnScreen(meshList[GEO_TEXT], ("Go to the officers, they'll inform you about the suspects."), Color(1, 1, 1), 2, 25, 7);
		RenderTextOnScreen(meshList[GEO_TEXT], ("Welcome to the crime scene Detective."), Color(1, 1, 1), 2, 33, 9.5);
	}
	//officers
	{
		//janitor

		modelStack.PushMatrix();
		modelStack.Translate(-7, 0, 8.2);
		modelStack.Scale(1, 1, 1);
		modelStack.Rotate(90, 0, 1, 0);
		RenderMesh(meshList[GEO_OFFICER_F], true);
		modelStack.PopMatrix();
		if (camera.position.x > -8.5 && camera.position.x < -5.5 && camera.position.z > 6.5 && camera.position.z < 8.5 && !isTalking)
		{
			RenderTextOnScreen(meshList[GEO_TEXT], ("This is Gertrude, the janitor."), Color(1, 1, 1), 2, 35, 8);
			RenderTextOnScreen(meshList[GEO_TEXT], ("She's the last one seen to exit the victim's room."), Color(1, 1, 1), 2, 31, 5.5);
		}

		//guard
		modelStack.PushMatrix();
		modelStack.Translate(-5.5, 0, 3.2);
		modelStack.Scale(1, 1, 1);
		modelStack.Rotate(90, 0, 1, 0);
		RenderMesh(meshList[GEO_OFFICER_M], true);
		modelStack.PopMatrix();

		if (camera.position.x > -7 && camera.position.x < -4 && camera.position.z > 1.7 && camera.position.z < 4.7 && !isTalking)
		{
			RenderTextOnScreen(meshList[GEO_TEXT], ("Akkop, this is the detective."), Color(1, 1, 1), 2, 35, 8);
			RenderTextOnScreen(meshList[GEO_TEXT], ("Detective, this is Akkop. He was sitting"), Color(1, 1, 1), 2, 34, 5.5);
			RenderTextOnScreen(meshList[GEO_TEXT], ("with the victim before he died."), Color(1, 1, 1), 2, 34.5, 3);
		}

		//oldman
		modelStack.PushMatrix();
		modelStack.Translate(5.5, 0, 3.2);
		modelStack.Scale(1, 1, 1);
		modelStack.Rotate(-180, 0, 1, 0);
		RenderMesh(meshList[GEO_OFFICER_M], true);
		modelStack.PopMatrix();

		if (camera.position.x > 4 && camera.position.x < 7 && camera.position.z > 1.7 && camera.position.z < 4.7)
		{
			RenderTextOnScreen(meshList[GEO_TEXT], ("This is Izan."), Color(1, 1, 1), 2, 35, 8);
			RenderTextOnScreen(meshList[GEO_TEXT], ("He was the closest to the victim when he fell."), Color(1, 1, 1), 2, 31, 5.5);
		}
		//kid

		modelStack.PushMatrix();
		modelStack.Translate(4.5, 0, -5);
		modelStack.Scale(1, 1, 1);
		RenderMesh(meshList[GEO_OFFICER_F], true);
		modelStack.PopMatrix();

		if (camera.position.x > 3 && camera.position.x < 6 && camera.position.z > -6.5 && camera.position.z < -3.5)
		{
			RenderTextOnScreen(meshList[GEO_TEXT], ("This is Kevin."), Color(1, 1, 1), 2, 35, 8);
			RenderTextOnScreen(meshList[GEO_TEXT], ("He was caught having many run-ins with the victim."), Color(1, 1, 1), 2, 31, 5.5);
		}
		//arcade

		modelStack.PushMatrix();
		modelStack.Translate(2, 0, -14);
		modelStack.Scale(1, 1, 1);
		RenderMesh(meshList[GEO_OFFICER_M], true);
		modelStack.PopMatrix();

		if (camera.position.x > 0.5 && camera.position.x < 3.5 && camera.position.z > -15.5 && camera.position.z < -12.5 && !isTalking)
		{
			RenderTextOnScreen(meshList[GEO_TEXT], ("Good Evening Detective. This is Ivan."), Color(1, 1, 1), 2, 35, 8);
			RenderTextOnScreen(meshList[GEO_TEXT], ("He was just playing the arcade machine when the victim died."), Color(1, 1, 1), 2, 31, 5.5);
		}

		//lift
		modelStack.PushMatrix();
		modelStack.Translate(1.1, 0, 30.5);
		modelStack.Scale(1, 1, 1);
		RenderMesh(meshList[GEO_OFFICER_F], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(1.1, 0, 32);
		modelStack.Scale(1, 1, 1);
		modelStack.Rotate(-180, 0, 1, 0);
		RenderMesh(meshList[GEO_OFFICER_M], true);
		modelStack.PopMatrix();

		if (camera.position.x > -0.8 && camera.position.x < 5 && camera.position.z > 29 && camera.position.z < 40)
		{
			RenderTextOnScreen(meshList[GEO_TEXT], ("Hello sir. The chief will brief you on this case."), Color(1, 1, 1), 2, 30, 8);
			RenderTextOnScreen(meshList[GEO_TEXT], ("He'll be at the entrance."), Color(1, 1, 1), 2, 35, 5.5);
		}
	}
}

void LobbyScene::InspectEvidenceOnScreen(Mesh* mesh, float x, float y, float sizex, float sizey, float rotatez, float rotatex)
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

bool LobbyScene::CreateButton(float buttonTop, float buttonBottom, float buttonRight, float buttonLeft)
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

void LobbyScene::RenderSkybox()
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

void LobbyScene::RenderHUD() 
{
	string clues = "Clues found:" + std::to_string(Application::eList.size()) + "/20";
	string guess = "Guesses Left:" + std::to_string(Application::playerGuesses) + "/3";

	if (!isJournalOpen && !isTalking) 
	{
		RenderTextOnScreen(meshList[GEO_TEXT], guess, Color(1, 1, 1), 2, 8, Application::screenUISizeY - 10);
		RenderTextOnScreen(meshList[GEO_TEXT], "Find the Culprit", Color(1, 1, 1), 2, 8, Application::screenUISizeY - 13);
		RenderTextOnScreen(meshList[GEO_TEXT], "[J] Journal", Color(1, 1, 1), 2, Application::screenUISizeX - 16, Application::screenUISizeY - 10);
		RenderTextOnScreen(meshList[GEO_TEXT], clues, Color(1, 1, 1), 2, Application::screenUISizeX - 15, Application::screenUISizeY - 13);
	}
}

void LobbyScene::RenderPressToInteract(char btn, std::string input, float x, float y)
{
	ss.str("");
	ss << "[" << btn << "] " << input;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 3, x, y);
}

void LobbyScene::RenderJournal()
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

void LobbyScene::ResetJournal()
{
	journalPage = EVIDENCE_PAGE;
	evidencePage = 1;
}

void LobbyScene::ChatDialogueInit(std::string fileName, std::vector<std::string>& vec)
{
	std::string temp;
	std::ifstream inputFile(fileName); // "Text//OldManChat.txt"
	if (inputFile.is_open())
	{
		while (!inputFile.eof())
		{
			std::getline(inputFile, temp, ':');
			vec.push_back(temp);
			std::getline(inputFile, temp, '\n');
			vec.push_back(temp);
		}
	}
}

void LobbyScene::RenderInteraction()
{
	if (isTalking)
	{
		if (printInterrogate)//print interrogate dialogue
		{
			RenderMeshOnScreen(meshList[GEO_DIALOGUE2], 40, 12, 60, 13);
			switch (charId)
			{
			case 0: // Player interrogate guard
				ss.str("");
				ss << "Akkop P.";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 0, 0), 2, 13, 15.5);
				switch (interrogateId)
				{
				case 0: //interrogating about Janitor
					if (Application::CheckEvidence("Carmanadezpine pills"))
					{
						ss.str("");
						ss << "I have a medical condition.";
						RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 12, 11);
						Application::guardEvidences[0] = true;
						break;
					}
				case 1: //interrogating about Gamer
					if (Application::CheckEvidence("In the bar"))
					{
						ss.str("");
						ss << "I just want to rest.";
						RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 12, 11);
						ss.str("");
						ss << "Everyone here gets to relax, why can't I?";
						RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 12, 9);
						Application::guardEvidences[1] = true;
						break;
					}
				case 2: //interrogating about kid
					if (Application::CheckEvidence("Calm about incident"))
					{
						ss.str("");
						ss << "In my field of work, I've seen worse.";
						RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 12, 11);
						Application::guardEvidences[2] = true;
						break;
					}
				case 3: //interrogating about old man
					if (Application::CheckEvidence("Drug Dealing"))
					{
						ss.str("");
						ss << "He sells drugs. That's all I'll tell you.";
						RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 12, 11);
						ss.str("");
						ss << "And whatever he says about me is false.";
						RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 12, 9);
						Application::guardEvidences[3] = true;
						break;
					}
				}
				break;
			case 1: // Player interrogate janitor
				ss.str("");
				ss << "Gertrude H.";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 0, 0), 2, 13, 15.5);
				switch (interrogateId)
				{
				case 0://interrogating about guard
					if (Application::CheckEvidence("Anger towards the victim"))
					{
						ss.str("");
						ss << "Of course I'm angry. He's got no respect for me.";
						RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 12, 11);
						Application::janitorEvidences[0] = true;
						break;
					}
				case 1://interrogating about gamer
					if (Application::CheckEvidence("Access to the victims room"))
					{
						ss.str("");
						ss << "Yeah? I have to clean people's rooms?";
						RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 12, 11);
						Application::janitorEvidences[1] = true;
						break;
					}
				case 2://interrogating about kid
					if (Application::CheckEvidence("Trolley toxic detergent"))
					{
						ss.str("");
						ss << "Yeah! To clean people's rooms.";
						RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 12, 11);
						ss.str("");
						ss << "What else am I gonna use? Your piss!?";
						RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 12, 9);
						Application::AddEvidence("Access to the victims room", "Janitor");
						Application::janitorEvidences[2] = true;
						break;
					}
				case 3://interrogating about old man
					if (Application::CheckEvidence("Getting rid of the bottle"))
					{
						ss.str("");
						ss << "If there's an empty bottle,";
						RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 12, 11);
						ss.str("");
						ss << "I have to clean it. I am the cleaner...";
						RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 12, 9);
						Application::janitorEvidences[3] = true;
						break;
					}
				}
				break;
			case 2: // Player interrogate gamer
				ss.str("");
				ss << "Ivan S.";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 0, 0), 2, 13, 15.5);
				switch (interrogateId)
				{
				case 0://interrogating about guard
					if (Application::CheckEvidence("Antidepressant pills"))
					{
						if (!isDoneGaming)
						{
							ss.str("");
							ss << "I have really bad depression. don't really wanna";
							RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 12, 12.5);
							ss.str("");
							ss << "talk about it but I can slip an info for";
							RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 12, 10.5);
							ss.str("");
							ss << "you.. Not for free though. Beat my game";
							RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 12, 8.5);
							ss.str("");
							ss << "highscore of 1000 and I'll consider.";
							RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 12, 6.5);
						}
						else
						{
							ss.str("");
							ss << "Ooo you're not too shabby. Anyways,";
							RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 12, 12.5);
							ss.str("");
							ss << "I had a deal-in with the guard who wanted";
							RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 12, 10.5);
							ss.str("");
							ss << "some of my meds. Gave it to him for some";
							RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 12, 8.5);
							ss.str("");
							ss << "cash. If anything, he seems suspicious here.";
							RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 12, 6.5);
							Application::AddEvidence("Drug Dealing", "Guard");
						}
						Application::arcadeEvidences[0] = true;
						break;
					}
				case 1://interrogating about Janitor
					if (Application::CheckEvidence("Laptop/Phone Messages"))
					{
						ss.str("");
						ss << "It's from my company.";
						RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 12, 11);
						Application::arcadeEvidences[1] = true;
						break;
					}
				case 2://interrogating about kid
					if (Application::CheckEvidence("Pocket Knife"))
					{
						ss.str("");
						ss << "A man's gotta be prepared for anything.";
						RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 12, 11);
						ss.str("");
						ss << "What happens if you want to peel an apple?";
						RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 12, 9);
						Application::arcadeEvidences[2] = true;
						break;
					}
				case 3://interrogating about old man
					if (Application::CheckEvidence("Suspicious to everybody"))
					{
						ss.str("");
						ss << "And you believe them?";
						RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 12, 11);
						ss.str("");
						ss << "They're the suspicious ones I tell ya.";
						RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 12, 9);
						Application::arcadeEvidences[3] = true;
						break;
					}
				}
				break;
			case 3: // Player interrogate kid
				ss.str("");
				ss << "Kevin M.";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 0, 0), 2, 13, 15.5);
				switch (interrogateId)
				{
				case 0://interrogating about guard
					if (Application::CheckEvidence("Imaginary friends"))
					{
						ss.str("");
						ss << "Marv and Harry? They're here with me.";
						RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 12, 11);
						Application::kidEvidences[0] = true;
						break;
					}
				case 1://interrogating about Janitor
					if (Application::CheckEvidence("Guest list: Kids"))
					{
						ss.str("");
						ss << "THEY ARE HERE WITH ME! Am I right Marv?";
						RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 12, 11);
						Application::kidEvidences[1] = true;
						break;
					}
				case 2://interrogating about gamer
					if (Application::CheckEvidence("Drawing of demonic friends"))
					{
						ss.str("");
						ss << "That's a drawing I made for me and my friends.";
						RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 12, 11);
						Application::kidEvidences[2] = true;
						break;
					}
				case 3://interrogating about old man
					if (Application::CheckEvidence("Bottle of weird mixtures"))
					{
						ss.str("");
						ss << "It's part of the spy game.";
						RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 12, 11);
						ss.str("");
						ss << "We create mixtures to poison our enemy.";
						RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 12, 9);
						Application::kidEvidences[3] = true;
						break;
					}
				}
				break;
			case 4: // Player interrogate old man
				ss.str("");
				ss << "Izan E.";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 0, 0), 2, 13, 15.5);
				switch (interrogateId)
				{
				case 0://interrogating about pills
					if (Application::CheckEvidence("Dementia pills"))
					{
						ss.str("");
						ss << "Oh, my dementia pills.";
						RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 12, 11);
						ss.str("");
						ss << "Almost forgot to take them.";
						RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 12, 9);
						Application::oldguyEvidences[0] = true;
						break;
					}
				case 1://interrogating about letter
					if (Application::CheckEvidence("Written notes"))
					{
						ss.str("");
						ss << "My sweet sweet letter from my granddaughter.";
						RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 12, 11);
						Application::oldguyEvidences[1] = true;
						break;
					}
				case 2://interrogating about guest list
					if (Application::CheckEvidence("Guest list: daughter"))
					{
						ss.str("");
						ss << "Well my granddaughter is here I tell you.";
						RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 12, 11);
						Application::oldguyEvidences[2] = true;
						break;
					}
				case 3://interrogating about gun
					if (Application::CheckEvidence("Gun in his room"))
					{
						ss.str("");
						ss << "Back in my day, I used to be a veteran spy.";
						RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 12, 11);
						Application::oldguyEvidences[3] = true;
						break;
					}
				}
				break;
			}
		}
		else if (printGossip)//print gossip dialogues
		{
			RenderMeshOnScreen(meshList[GEO_DIALOGUE2], 40, 12, 60, 13);
			switch (charId)
			{
			case 0: // Player gossiping to guard
				ss.str("");
				ss << "Akkop P.";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 0), 2, 13, 15.5); //charId of the person we talking to
				switch (gossipId)
				{
				case 0: //Gossiping about Janitor
					ss.str("");
					ss << "She always looks angry except that one time";
					RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 14, 10.5);
					ss.str("");
					ss << "5 years ago when her son visited her.";
					RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 14, 8.5);
					break;
				case 1: //Gossiping about Gamer
					ss.str("");
					ss << "He looks angry.";
					RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 14, 10.5);
					Application::susCounter[0] = true;
					break;
				case 2: //Gossiping about kid
					ss.str("");
					ss << "That kid seems contented with his game.";
					RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 14, 10.5);
					break;
				case 3: //Gossiping about old man
					ss.str("");
					ss << "He's always there if his wife doesn't come to ";
					RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 14, 10.5);
					ss.str("");
					ss << "pick him up, he looks confused";
					RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 14, 8.5);
					break;
				}
				break;
			case 1: // Player gossiping to janitor
				ss.str("");
				ss << "Gertrude H.";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 0), 2, 13, 15.5);
				switch (gossipId)
				{
				case 0://Gossiping about guard
					ss.str("");
					ss << "He's been working here for a long time now.";
					RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 11, 11);
					ss.str("");
					ss << "I always see him drinking in the bar on his duty.";
					RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 11, 9);
					Application::AddEvidence("In the bar", "Guard");
					break;
				case 1://Gossiping about gamer
					ss.str("");
					ss << "I've seen him a few times when he is going out of";
					RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 11, 12.5);
					ss.str("");
					ss << "his room, always looking down while whispering";
					RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 11, 10.5);
					ss.str("");
					ss << "to himself about stopping it all. I've never seen";
					RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 11, 8.5);
					ss.str("");
					ss << "his face but I recognise that red hoodie.";
					RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 11, 6.5);
					Application::susCounter[1] = true;
					break;
				case 2://Gossiping about kid
					ss.str("");
					ss << "That kid is so annoying. I just can't";
					RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 12, 11);
					ss.str("");
					ss << "stand noisy kids.";
					RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 12, 9);
					break;
				case 3://Gossiping about old man
					ss.str("");
					ss << "He's always sitting at the lobby just";
					RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 12, 11);
					ss.str("");
					ss << "waiting for somebody.";
					RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 12, 9);
					break;
				}
				break;
			case 2: // Player gossiping to gamer
				ss.str("");
				ss << "Ivan S.";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 0), 2, 13, 15.5);
				switch (gossipId)
				{
				case 0://Gossiping about guard
					ss.str("");
					ss << "That guard has no emotions even till now";
					RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 11, 11);
					ss.str("");
					ss << "he just seems too relaxed. He must be the killer.";
					RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 11, 9);
					break;
				case 1://Gossiping about Janitor
					ss.str("");
					ss << "She always looks grumpy and unapproachable.";
					RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 11, 11);
					break;
				case 2://Gossiping about kid
					ss.str("");
					ss << "Everytime I'm in the hallway, the kid talks";
					RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 11, 11);
					ss.str("");
					ss << "to himself. He might have some mental issues";
					RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 11, 9);
					break;
				case 3://Gossiping about old man1
					ss.str("");
					ss << "Whenever I'm in the lobby, I always see him";
					RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 11, 11);
					ss.str("");
					ss << "sitting there looking.";
					RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 11, 9);
					break;
				}
				break;
			case 3: // Player gossiping to kid
				ss.str("");
				ss << "Kevin M.";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 0), 2, 13, 15.5);
				switch (gossipId)
				{
				case 0://Gossiping about guard
					ss.str("");
					ss << "I believe he's a robot. Whenever I say";
					RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 11, 11);
					ss.str("");
					ss << "hi to him, his replies are cold.";
					RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 11, 9);
					break;
				case 1://Gossiping about Janitor
					ss.str("");
					ss << "That lady is mean. She told me to be quiet";
					RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 11, 11);
					ss.str("");
					ss << "and to stop running around. She even threatened";
					RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 11, 9);
					ss.str("");
					ss << "to tie me up and leave to shut me up.";
					RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 11, 7);
					break;
				case 2://Gossiping about gamer
					ss.str("");
					ss << "I have never seen him before up until now.";
					RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 11, 11);
					ss.str("");
					ss << "He's always been away somewhere.";
					RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 11, 9);
					Application::susCounter[2] = true;
					break;
				case 3://Gossiping about old man
					ss.str("");
					ss << "The old guy is very friendly. When i was";
					RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 11, 12.5);
					ss.str("");
					ss << "playing with harry and marv, he offered to be ";
					RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 11, 10.5);
					ss.str("");
					ss << "the enemy. He's pretty funny too! He always";
					RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 11, 8.5);
					ss.str("");
					ss << "forgets his role and lines.";
					RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 11, 6.5);
					break;
				}
				break;
			case 4: // Player gossiping to old man
				ss.str("");
				ss << "Izan E.";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 0), 2, 13, 15.5);
				switch (gossipId)
				{
				case 0://Gossiping about guard
					ss.str("");
					ss << "I have never seen him before.";
					RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 11, 11);
					break;
				case 1://Gossiping about Janitor
					ss.str("");
					ss << "Oh the janitor? Well wait nevermind I forgot.";
					RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 11, 11);
					break;
				case 2://Gossiping about gamer
					ss.str("");
					ss << "Hmm? I dont remember anybody from the arcade.";
					RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 11, 11);
					Application::susCounter[3] = true;
					break;
				case 3://Gossiping about kid
					ss.str("");
					ss << "Nope I have no idea who he is but where is";
					RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 11, 11);
					ss.str("");
					ss << "my granddaughter?";
					RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 11, 9);
					break;
				}
				break;
			}
		}
		else if (isGossiping)//print gossip buttons
		{
			RenderMeshOnScreen(meshList[GEO_DIALOGUE], 40, 12, 60, 13);
			switch (charId)
			{
			case 0:
				ss.str("");
				ss << "Akkop P.";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 0), 2, 13, 15.5); //charId of the person we talking to
				ss.str("");
				ss << "Janitor";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 21, 11.5); //charId of others we gossiping about
				ss.str("");
				ss << "Gamer";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 52, 11.5);
				ss.str("");
				ss << "Kid";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 23, 7);
				ss.str("");
				ss << "Old Man";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 50, 7);
				break;
			case 1:
				ss.str("");
				ss << "Gertrude H.";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 0), 2, 13, 15.5);
				ss.str("");
				ss << "Guard";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 22, 11.5);
				ss.str("");
				ss << "Gamer";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 52, 11.5);
				ss.str("");
				ss << "Kid";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 23, 7);
				ss.str("");
				ss << "Old Man";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 50, 7);
				break;
			case 2:
				ss.str("");
				ss << "Ivan S.";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 0), 2, 13, 15.5);
				ss.str("");
				ss << "Guard";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 22, 11.5);
				ss.str("");
				ss << "Janitor";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 50, 11.5);
				ss.str("");
				ss << "Kid";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 23, 7);
				ss.str("");
				ss << "Old Man";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 50, 7);
				break;
			case 3:
				ss.str("");
				ss << "Kevin M.";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 0), 2, 13, 15.5);
				ss.str("");
				ss << "Guard";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 22, 11.5);
				ss.str("");
				ss << "Janitor";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 50, 11.5);
				ss.str("");
				ss << "Gamer";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 22, 7);
				ss.str("");
				ss << "Old Man";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 50, 7);
				break;
			case 4:
				ss.str("");
				ss << "Izan E.";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 0), 2, 13, 15.5);
				ss.str("");
				ss << "Guard";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 22, 11.5);
				ss.str("");
				ss << "Janitor";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 51, 11.5);
				ss.str("");
				ss << "Gamer";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 22, 7);
				ss.str("");
				ss << "Kid";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 53, 7);
				break;
			}
		}
		else if (isInterrogate)//Print Evidence buttons
		{
		RenderMeshOnScreen(meshList[GEO_DIALOGUE], 40, 12, 60, 13);
		switch (charId)
		{
		case 0:
			ss.str("");
			ss << "Akkop P.";
			RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 0, 0), 2, 13, 15.5);
			if (Application::CheckEvidence("Carmanadezpine pills") ||
				Application::CheckEvidence("In the bar") ||
				Application::CheckEvidence("Calm about incident") ||
				Application::CheckEvidence("Drug Dealing"))
			{
				if (Application::CheckEvidence("Carmanadezpine pills"))
				{
					ss.str("");
					ss << "Carmanadezpine pills";
					RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 13.5, 11.5);
				}
				if (Application::CheckEvidence("In the bar"))
				{
					ss.str("");
					ss << "In the bar";
					RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 49, 11.5);
				}
				if (Application::CheckEvidence("Calm about incident"))
				{
					ss.str("");
					ss << "Calm about incident";
					RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 14, 7);
				}
				if (Application::CheckEvidence("Drug Dealing"))
				{
					ss.str("");
					ss << "Drug Dealing";
					RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 47.5, 7);
				}
				haveEvidence = true;
			}
			else
			{
				RenderMeshOnScreen(meshList[GEO_DIALOGUE2], 40, 12, 60, 13);
				ss.str("");
				ss << "No evidence currently found";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 25, 11);
				haveEvidence = false;
			}
			break;
		case 1:
			ss.str("");
			ss << "Gertrude H.";
			RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 0, 0), 2, 13, 15.5);
			if (Application::CheckEvidence("Anger towards the victim") ||
				Application::CheckEvidence("Access to the victims room") ||
				Application::CheckEvidence("Trolley toxic detergent") ||
				Application::CheckEvidence("Getting rid of the bottle"))
			{
				if (Application::CheckEvidence("Anger towards the victim"))
				{
					ss.str("");
					ss << "Angry at victim";
					RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 16, 11.5);
				}
				if (Application::CheckEvidence("Access to the victims room"))
				{
					ss.str("");
					ss << "Access to victim's room";
					RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 41, 11.5);
				}
				if (Application::CheckEvidence("Trolley toxic detergent"))
				{
					ss.str("");
					ss << "Trolley toxic detergent";
					RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 11, 7);
				}
				if (Application::CheckEvidence("Getting rid of the bottle"))
				{
					ss.str("");
					ss << "Getting rid of bottle";
					RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 42, 7);
				}
				haveEvidence = true;
			}
			else
			{
				RenderMeshOnScreen(meshList[GEO_DIALOGUE2], 40, 12, 60, 13);
				ss.str("");
				ss << "No evidence currently found";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 25, 11);
				haveEvidence = false;
			}
			break;
		case 2:
			ss.str("");
			ss << "Ivan S.";
			RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 0, 0), 2, 13, 15.5);
			if (Application::CheckEvidence("Antidepressant pills") ||
				Application::CheckEvidence("Laptop/Phone Messages") ||
				Application::CheckEvidence("Pocket Knife") ||
				Application::CheckEvidence("Suspicious to everybody"))
			{
				if (Application::CheckEvidence("Antidepressant pills"))
				{
					ss.str("");
					ss << "Antidepressant pills";
					RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 13, 11.5);
				}
				if (Application::CheckEvidence("Laptop/Phone Messages"))
				{
					ss.str("");
					ss << "Laptop/Phone Messages";
					RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 43, 11.5);
				}
				if (Application::CheckEvidence("Pocket Knife"))
				{
					ss.str("");
					ss << "Pocket Knife";
					RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 19, 7);
				}
				if (Application::CheckEvidence("Suspicious to everybody"))
				{
					ss.str("");
					ss << "Suspicious to everybody";
					RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 41, 7);
				}
				haveEvidence = true;
			}
			else
			{
				RenderMeshOnScreen(meshList[GEO_DIALOGUE2], 40, 12, 60, 13);
				ss.str("");
				ss << "No evidence currently found";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 25, 11);
				haveEvidence = false;
			}
			break;
		case 3:
			ss.str("");
			ss << "Kevin M.";
			RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 0, 0), 2, 13, 15.5);
			if (Application::CheckEvidence("Imaginary friends") ||
				Application::CheckEvidence("Guest list: Kids") ||
				Application::CheckEvidence("Drawing of demonic friends") ||
				Application::CheckEvidence("Bottle of weird mixtures"))
			{
				if (Application::CheckEvidence("Imaginary friends"))
				{
					ss.str("");
					ss << "Imaginary friends";
					RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 15, 11.5);
				}
				if (Application::CheckEvidence("Guest list: Kids"))
				{
					ss.str("");
					ss << "Guest list: Kids";
					RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 45, 11.5);
				}
				if (Application::CheckEvidence("Drawing of demonic friends"))
				{
					ss.str("");
					ss << "Demonic Friends Drawing";
					RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 11.5, 7);
				}
				if (Application::CheckEvidence("Bottle of weird mixtures"))
				{
					ss.str("");
					ss << "Bottle of weird mixtures";
					RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 41, 7);
				}
				haveEvidence = true;
			}
			else
			{
				RenderMeshOnScreen(meshList[GEO_DIALOGUE2], 40, 12, 60, 13);
				ss.str("");
				ss << "No evidence currently found";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 25, 11);
				haveEvidence = false;
			}
			break;
		case 4:
			ss.str("");
			ss << "Izan E.";
			RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 0, 0), 2, 13, 15.5);
			if (Application::CheckEvidence("Dementia pills") ||
				Application::CheckEvidence("Written notes") ||
				Application::CheckEvidence("Guest list: daughter") ||
				Application::CheckEvidence("Gun in his room"))
			{
				if (Application::CheckEvidence("Dementia pills"))
				{
					ss.str("");
					ss << "Dementia pills";
					RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 17, 11.5);
				}
				if (Application::CheckEvidence("Written notes"))
				{
					ss.str("");
					ss << "Written notes";
					RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 48, 11.5);
				}
				if (Application::CheckEvidence("Guest list: daughter"))
				{
					ss.str("");
					ss << "Guest list: Daughter";
					RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 13, 7);
				}
				if (Application::CheckEvidence("Gun in his room"))
				{
					ss.str("");
					ss << "Gun in his room";
					RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 46.5, 7);
				}
				haveEvidence = true;
			}
			else
			{
				RenderMeshOnScreen(meshList[GEO_DIALOGUE2], 40, 12, 60, 13);
				ss.str("");
				ss << "No evidence currently found";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 25, 11);
				haveEvidence = false;
			}
			break;
		}
		}
		else if (isChatting)//print chat dialogues
		{
			RenderMeshOnScreen(meshList[GEO_DIALOGUE2], 40, 12, 60, 13);
			switch (charId)
			{
			case 0:
				if (chatCounter < guardChat.size() - 1)
				{
					if (guardChat[chatCounter] == "D")
					{
						ss.str("");
						ss << "Detective";
						RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 0, 1), 2, 15, 15.5); //charId of the person we talking to
						ss.str("");
						tempCounter = chatCounter + 1;
						ss << guardChat[tempCounter];
						RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 11, 10);
					}
					else if (guardChat[chatCounter] == "G")
					{
						ss.str("");
						ss << "Akkop P.";
						RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 2, 15, 15.5);//charId of the person we talking to
						ss.str("");
						tempCounter = chatCounter + 1;
						ss << guardChat[tempCounter];
						RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 11, 10);
					}

					if (chatCounter == tempCounter)
					{
						chatCounter++;
					}
				}
				else
				{
					Application::AddEvidence("Calm about incident", "Guard");
					isChatting = false;
					isGossiping = false;
					isTalking = true;
					printGossip = false;
					chatCounter = 0;
				}
				break;
			case 1:
				if (chatCounter < janitorChat.size() - 1)
				{
					if (janitorChat[chatCounter] == "D")
					{
						ss.str("");
						ss << "Detective";
						RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 0, 1), 2, 15, 15.5);//charId of the person we talking to
						ss.str("");
						tempCounter = chatCounter + 1;
						ss << janitorChat[tempCounter];
						RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 11, 10);
					}
					else if (janitorChat[chatCounter] == "J")
					{
						ss.str("");
						ss << "Gertrude H.";
						RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 2, 15, 15.5); //charId of the person we talking to
						ss.str("");
						tempCounter = chatCounter + 1;
						ss << janitorChat[tempCounter];
						RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 11, 10);
					}

					if (chatCounter == tempCounter)
					{
						chatCounter++;
					}
				}
				else
				{
					Application::AddEvidence("Anger towards the victim", "Janitor");
					isChatting = false;
					isGossiping = false;
					isTalking = true;
					printGossip = false;
					chatCounter = 0;
				}
				break;
			case 2:
				if (chatCounter < gamerChat.size() - 1)
				{
					if (gamerChat[chatCounter] == "D")
					{
						ss.str("");
						ss << "Detective";
						RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 0, 1), 2, 15, 15.5); //charId of the person we talking to
						ss.str("");
						tempCounter = chatCounter + 1;
						ss << gamerChat[tempCounter];
						RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 11, 10);
					}
					else if (gamerChat[chatCounter] == "A")
					{
						ss.str("");
						ss << "Ivan S.";
						RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 2, 15, 15.5);//charId of the person we talking to
						ss.str("");
						tempCounter = chatCounter + 1;
						ss << gamerChat[tempCounter];
						RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 11, 10);
					}

					if (chatCounter == tempCounter)
					{
						chatCounter++;
					}
				}
				else
				{
					isChatting = false;
					isGossiping = false;
					isTalking = true;
					printGossip = false;
					chatCounter = 0;
				}
				break;
			case 3:
				if (chatCounter < kidChat.size() - 1)
				{
					if (kidChat[chatCounter] == "D")
					{
						ss.str("");
						ss << "Detective";
						RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 0, 1), 2, 15, 15.5); //charId of the person we talking to
						ss.str("");
						tempCounter = chatCounter + 1;
						ss << kidChat[tempCounter];
						RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 11, 10);
					}
					else if (kidChat[chatCounter] == "K")
					{
						ss.str("");
						ss << "Kevin M.";
						RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 2, 15, 15.5); //charId of the person we talking to
						ss.str("");
						tempCounter = chatCounter + 1;
						ss << kidChat[tempCounter];
						RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 11, 10);
					}

					if (chatCounter == tempCounter)
					{
						chatCounter++;
					}
				}
				else
				{
					Application::AddEvidence("Imaginary friends", "Kid");
					isChatting = false;
					isGossiping = false;
					isTalking = true;
					printGossip = false;
					chatCounter = 0;
				}
				break;
			case 4:
				if (chatCounter < oldManChat.size() - 1)
				{
					if (oldManChat[chatCounter] == "D")
					{
						ss.str("");
						ss << "Detective";
						RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 0, 1), 2, 15, 15.5);//charId of the person we talking to
						ss.str("");
						tempCounter = chatCounter + 1;
						ss << oldManChat[tempCounter];
						RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 11, 10);
					}
					else if (oldManChat[chatCounter] == "O")
					{
						ss.str("");
						ss << "Izan E.";
						RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 2, 15, 15.5);//charId of the person we talking to
						ss.str("");
						tempCounter = chatCounter + 1;
						ss << oldManChat[tempCounter];
						RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 11, 10);
					}

					if (chatCounter == tempCounter)
					{
						chatCounter++;
					}
				}
				else
				{
					isChatting = false;
					isGossiping = false;
					isTalking = true;
					printGossip = false;
					chatCounter = 0;
				}
				break;
			}
		}
		else//print chat options
		{
			RenderMeshOnScreen(meshList[GEO_DIALOGUE], 40, 12, 60, 13);
			switch (charId)
			{
			case 0:
				ss.str("");
				ss << "Akkop P.";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 13, 15.5);
				break;
			case 1:
				ss.str("");
				ss << "Gertrude H.";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 13, 15.5);
				break;
			case 2:
				ss.str("");
				ss << "Ivan S.";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 13, 15.5);
				break;
			case 3:
				ss.str("");
				ss << "Kevin M.";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 13, 15.5);
				break;
			case 4:
				ss.str("");
				ss << "Izan E.";
				RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 13, 15.5);
				break;
			}
			ss.str("");
			ss << "Chat";
			RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 23, 11.5);
			ss.str("");
			ss << "Interrogate";
			RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 49, 11.5);
			ss.str("");
			ss << "Gossip";
			RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 22, 7);
			ss.str("");
			ss << "Leave";
			RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 2, 52, 7);
		}
	}
}

void LobbyScene::Interaction()
{
	//interaction delay to prevent spam
	if (canInteract == false)
	{
		interval++;
		if (interval >= 30) {
			canInteract = true;
			interval = 0;
		}
	}

	CharacterPosCheck();
}

void LobbyScene::TalkButtons()
{
	double x = Application::m_width / 10 / 3;
	double y = Application::m_height / 10 / 4;

	if (printGossip)
	{
		isChatting = false;
		isGossiping = false;
		isTalking = true;
		isInterrogate = false;
		printGossip = false;
		printInterrogate = false;
	}
	else if (printInterrogate)
	{
		isChatting = false;
		isGossiping = false;
		isTalking = true;
		isInterrogate = false;
		printGossip = false;
		printInterrogate = false;
	}
	else if (isGossiping) //if gossiping
	{
		if (CreateButton(y, y - y / 3, x + (x / 4) * 2, x - x / 1.6)) //1st gossip char
		{
			gossipId = 0;
		}
		else if (CreateButton(y, y - y / 3, x / 1.6 + x * 2, x)) //2md gossip char
		{
			gossipId = 1;
		}
		else if (CreateButton(y - y / 3, y - (y / 3) * 2, x + (x / 4) * 2, x - x / 1.6)) //2md gossip char
		{
			gossipId = 2;
		}
		else if (CreateButton(y - y / 3, y - (y / 3) * 2, x / 1.6 + x * 2, x)) //2md gossip char
		{
			gossipId = 3;
		}
		printGossip = true;
	}
	else if (isInterrogate) //if interrogating
	{
		if (CreateButton(y, y - y / 3, x + (x / 4) * 2, x - x / 1.6)) //1st gossip char
		{
			switch (charId)
			{
			case 0:
				if (Application::CheckEvidence("Carmanadezpine pills"))
				{
					interrogateId = 0;
					printInterrogate = true;
				}
				break;
			case 1:
				if (Application::CheckEvidence("Anger towards the victim"))
				{
					interrogateId = 0;
					printInterrogate = true;
				}
				break;
			case 2:
				if (Application::CheckEvidence("Antidepressant pills"))
				{
					interrogateId = 0;
					printInterrogate = true;
				}
				break;
			case 3:
				if (Application::CheckEvidence("Imaginary friends"))
				{
					interrogateId = 0;
					printInterrogate = true;
				}
				break;
			case 4:
				if (Application::CheckEvidence("Dementia pills"))
				{
					interrogateId = 0;
					printInterrogate = true;
				}
				break;
			}
		}
		else if (CreateButton(y, y - y / 3, x / 1.6 + x * 2, x)) //2md gossip char
		{
			switch (charId)
			{
			case 0:
				if (Application::CheckEvidence("In the bar"))
				{
					interrogateId = 1;
					printInterrogate = true;
				}
				break;
			case 1:
				if (Application::CheckEvidence("Access to the victims room"))
				{
					interrogateId = 1;
					printInterrogate = true;
				}
				break;
			case 2:
				if (Application::CheckEvidence("Laptop/Phone Messages"))
				{
					interrogateId = 1;
					printInterrogate = true;
				}
				break;
			case 3:
				if (Application::CheckEvidence("Guest list: Kids"))
				{
					interrogateId = 1;
					printInterrogate = true;
				}
				break;
			case 4:
				if (Application::CheckEvidence("Written notes"))
				{
					interrogateId = 1;
					printInterrogate = true;
				}
				break;
			}
		}
		else if (CreateButton(y - y / 3, y - (y / 3) * 2, x + (x / 4) * 2, x - x / 1.6)) //2md gossip char
		{
			switch (charId)
			{
			case 0:
				if (Application::CheckEvidence("Calm about incident"))
				{
					interrogateId = 2;
					printInterrogate = true;
				}
				break;
			case 1:
				if (Application::CheckEvidence("Trolley toxic detergent"))
				{
					interrogateId = 2;
					printInterrogate = true;
				}
				break;
			case 2:
				if (Application::CheckEvidence("Pocket Knife"))
				{
					interrogateId = 2;
					printInterrogate = true;
				}
				break;
			case 3:
				if (Application::CheckEvidence("Drawing of demonic friends"))
				{
					interrogateId = 2;
					printInterrogate = true;
				}
				break;
			case 4:
				if (Application::CheckEvidence("Guest list: daughter"))
				{
					interrogateId = 2;
					printInterrogate = true;
				}
				break;
			}
		}
		else if (CreateButton(y - y / 3, y - (y / 3) * 2, x / 1.6 + x * 2, x)) //2md gossip char
		{
		switch (charId)
		{
		case 0:
			if (Application::CheckEvidence("Drug Dealing"))
			{
				interrogateId = 3;
				printInterrogate = true;
			}
			break;
		case 1:
			if (Application::CheckEvidence("Getting rid of the bottle"))
			{
				interrogateId = 3;
				printInterrogate = true;
			}
			break;
		case 2:
			if (Application::CheckEvidence("Suspicious to everybody"))
			{
				interrogateId = 3;
				printInterrogate = true;
			}
			break;
		case 3:
			if (Application::CheckEvidence("Bottle of weird mixtures"))
			{
				interrogateId = 3;
				printInterrogate = true;
			}
			break;
		case 4:
			if (Application::CheckEvidence("Gun in his room"))
			{
				interrogateId = 3;
				printInterrogate = true;
			}
			break;
		}
		}
		if (!haveEvidence)
		{
			isChatting = false;
			isGossiping = false;
			isTalking = true;
			isInterrogate = false;
			printGossip = false;
			printInterrogate = false;
		}
	}
	else if (isChatting) // Continue chat button
	{
		chatCounter++;
	}
	else
	{
		if (CreateButton(y , y - y / 3, x + (x / 4) * 2, x - x / 1.6)) //Chatting button
		{
			isChatting = true;
		}
		else if (CreateButton(y, y - y / 3 ,x / 1.6 + x * 2, x)) //Interrogate button
		{
			std::cout << "Interrogating..." << std::endl;
			isInterrogate = true;
		}
		else if (CreateButton(y - y / 3, y - (y / 3) * 2, x + (x / 4) * 2, x - x / 1.6)) //Gossip button
		{
			isGossiping = true;
		}
		else if (CreateButton(y - y / 3, y - (y / 3) * 2, x / 1.6 + x * 2, x)) // Leave button
		{
			isChatting = false;
			isGossiping = false;
			isTalking = false;
			isInterrogate = false;
			Application::ResetCursor();
			camera.EnableControl();
			Application::HideCursor();
		}
	}
}

bool LobbyScene::declareCulprit(int id) {
	// 0 = guard, 1 = janitor, 2 = Arcader, 3 = Kid, 4 = old guy

	//culprit is 2 (arcade gamer)
	if (id == 2) 
	{
		return true;
	}
	//guess wrongly
	else
	{
		Application::playerGuesses--;
		return false;
	}
}

void LobbyScene::CharacterPosCheck()
{
	interactOffset = 1.5;

	//interaction position check
	if (camera.position.x <= entityList[ENTITY_GUARD].getTransform().x + interactOffset
		&& camera.position.z >= entityList[ENTITY_GUARD].getTransform().z - interactOffset
		&& camera.position.x >= entityList[ENTITY_GUARD].getTransform().x - interactOffset
		&& camera.position.z <= entityList[ENTITY_GUARD].getTransform().z + interactOffset)
	{
		isAbleToTalk = true;
		if (Application::EnoughEvidence(Application::guardEvidences) && entityList[ENTITY_GUARD].getPinable())
		{
			isAbleToPin = true;
			if (canInteract && Application::IsKeyPressed('C'))
			{
				if (declareCulprit(ENTITY_GUARD)) //if actual culprit (guard)
				{
					Application::sceneState = Application::STATE_GAMEWIN;
				}
				else
				{
					entityList[ENTITY_GUARD].setPinable(false);
					isAbleToPin = false;
				}
			}
		}
		if (canInteract && Application::IsKeyPressed('E') && !isJournalOpen)
		{
			charId = ENTITY_GUARD;
			isTalking = true;
			canInteract = false;
			camera.DisableControl();
			Application::ShowCursor();
		}
	}
	else if (camera.position.x <= entityList[ENTITY_JANITOR].getTransform().x + interactOffset
		&& camera.position.z >= entityList[ENTITY_JANITOR].getTransform().z - interactOffset
		&& camera.position.x >= entityList[ENTITY_JANITOR].getTransform().x - interactOffset
		&& camera.position.z <= entityList[ENTITY_JANITOR].getTransform().z + interactOffset)
	{
		isAbleToTalk = true;
		if (Application::EnoughEvidence(Application::janitorEvidences) && entityList[ENTITY_JANITOR].getPinable())
		{
			isAbleToPin = true;
			if (canInteract && Application::IsKeyPressed('C'))
			{
				if (declareCulprit(ENTITY_JANITOR)) //if actual culprit (janitor)
				{
					Application::sceneState = Application::STATE_GAMEWIN;
				}
				else
				{
					entityList[ENTITY_JANITOR].setPinable(false);
					isAbleToPin = false;
				}
			}
		}
		if (canInteract && Application::IsKeyPressed('E') && !isJournalOpen)
		{
			charId = ENTITY_JANITOR;
			isTalking = true;
			canInteract = false;
			camera.DisableControl();
			Application::ShowCursor();
		}
	}
	else if (camera.position.x <= entityList[ENTITY_GAMER].getTransform().x + interactOffset
		&& camera.position.z >= entityList[ENTITY_GAMER].getTransform().z - interactOffset
		&& camera.position.x >= entityList[ENTITY_GAMER].getTransform().x - interactOffset
		&& camera.position.z <= entityList[ENTITY_GAMER].getTransform().z + interactOffset)
	{
		isAbleToTalk = true;
		if (Application::EnoughEvidence(Application::arcadeEvidences) && entityList[ENTITY_GAMER].getPinable())
		{
			isAbleToPin = true;
			if (canInteract && Application::IsKeyPressed('C'))
			{
				if (declareCulprit(ENTITY_GAMER)) //if actual culprit (janitor)
				{
					Application::sceneState = Application::STATE_GAMEWIN;
				}
				else
				{
					entityList[ENTITY_GAMER].setPinable(false);
					isAbleToPin = false;
				}
			}
		}

		if (canInteract && Application::IsKeyPressed('E') && !isJournalOpen)
		{
			charId = ENTITY_GAMER;
			isTalking = true;
			canInteract = false;
			camera.DisableControl();
			Application::ShowCursor();
		}
	}
	else if (camera.position.x <= entityList[ENTITY_KID].getTransform().x + interactOffset
		&& camera.position.z >= entityList[ENTITY_KID].getTransform().z - interactOffset
		&& camera.position.x >= entityList[ENTITY_KID].getTransform().x - interactOffset
		&& camera.position.z <= entityList[ENTITY_KID].getTransform().z + interactOffset)
	{
		isAbleToTalk = true;
		if (Application::EnoughEvidence(Application::kidEvidences) && entityList[ENTITY_KID].getPinable())
		{
			isAbleToPin = true;
			if (canInteract && Application::IsKeyPressed('C'))
			{
				if (declareCulprit(ENTITY_KID)) //if actual culprit (kid)
				{
					Application::sceneState = Application::STATE_GAMEWIN;
				}
				else
				{
					entityList[ENTITY_KID].setPinable(false);
					isAbleToPin = false;
				}
			}
		}
		if (canInteract && Application::IsKeyPressed('E') && !isJournalOpen)
		{
			charId = ENTITY_KID;
			isTalking = true;
			canInteract = false;
			camera.DisableControl();
			Application::ShowCursor();
		}
	}
	else if (camera.position.x <= entityList[ENTITY_OLDMAN].getTransform().x + interactOffset
		&& camera.position.z >= entityList[ENTITY_OLDMAN].getTransform().z - interactOffset
		&& camera.position.x >= entityList[ENTITY_OLDMAN].getTransform().x - interactOffset
		&& camera.position.z <= entityList[ENTITY_OLDMAN].getTransform().z + interactOffset)
	{
	isAbleToTalk = true;
	if (Application::EnoughEvidence(Application::oldguyEvidences) && entityList[ENTITY_OLDMAN].getPinable())
	{
		isAbleToPin = true;
		if (canInteract && Application::IsKeyPressed('C'))
		{
			if (declareCulprit(ENTITY_OLDMAN)) //if actual culprit (oldman)
			{
				Application::sceneState = Application::STATE_GAMEWIN;
			}
			else
			{
				entityList[ENTITY_OLDMAN].setPinable(false);
				isAbleToPin = false;
			}
		}
	}
		if (canInteract && Application::IsKeyPressed('E') && !isJournalOpen)
		{
			charId = ENTITY_OLDMAN;
			isTalking = true;
			canInteract = false;
			camera.DisableControl();
			Application::ShowCursor();
		}
	}
	else
	{
		isAbleToTalk = false;
		isAbleToPin = false;
	}
}

void LobbyScene::BoundsCheck()
{
	//room left
	if ((camera.position.x <= -8.5) && (camera.position.z <= 18.5) && (camera.position.z >= -15))
	{
		camera.position.x = -8.5;
		camera.target = camera.position + camera.view;
	}
	//room back
	if ((camera.position.z <= -14.5) && (camera.position.x <= 7) && (camera.position.x >= -9))
	{
		camera.position.z = -14.5;
		camera.target = camera.position + camera.view;
	}
	//room right
	if ((camera.position.x >= 6.5) && (camera.position.z <= 18.5) && (camera.position.z >= -14.5))
	{
		camera.position.x = 6.5;
		camera.target = camera.position + camera.view;
	}
	//room left front
	if ((camera.position.z >= 17.5) && (camera.position.z <= 18) && (camera.position.x <= -2.5) && (camera.position.x >= -9))
	{
		camera.position.z = 17.5;
		camera.target = camera.position + camera.view;
	}
	if ((camera.position.z >= 18) && (camera.position.z <= 18.5) && (camera.position.x <= -2.4) && (camera.position.x >= -5.5))
	{
		camera.position.x = -2.4;
		camera.target = camera.position + camera.view;
	}
	if ((camera.position.z >= 18) && (camera.position.z <= 19) && (camera.position.x <= -2.5) && (camera.position.x >= -5.5))
	{
		camera.position.z = 19;
		camera.target = camera.position + camera.view;
	}
	//room right front
	if ((camera.position.z >= 17.5) && (camera.position.z <= 18) && (camera.position.x <= 6.5) && (camera.position.x >= 0.5))
	{
		camera.position.z = 17.5;
		camera.target = camera.position + camera.view;
	}
	if ((camera.position.z >= 18) && (camera.position.z <= 18.5) && (camera.position.x <= 6.5) && (camera.position.x >= 0.4))
	{
		camera.position.x = 0.4;
		camera.target = camera.position + camera.view;
	}
	if ((camera.position.z >= 18) && (camera.position.z <= 19) && (camera.position.x <= 3.5) && (camera.position.x >= 0.5))
	{
		camera.position.z = 19;
		camera.target = camera.position + camera.view;
	}
	//corridor right
	if ((camera.position.x >= 1.5) && (camera.position.z <= 33) && (camera.position.z >= 19))
	{
		camera.position.x = 1.5;
		camera.target = camera.position + camera.view;
	}
	//corridor back
	if ((camera.position.z >= 32.5) && (camera.position.x <= 1.5) && (camera.position.x >= -5))
	{
		camera.position.z = 32.5;
		camera.target = camera.position + camera.view;
	}
	//coridor left
	if ((camera.position.x <= -4.6) && (camera.position.x >= -5) && (camera.position.z <= 32.5) && (camera.position.z >= 30))
	{
		camera.position.x = -4.6;
		camera.target = camera.position + camera.view;
	}
	if ((camera.position.x <= -4.6) && (camera.position.x >= -5) && (camera.position.z <= 29) && (camera.position.z >= 19))
	{
		camera.position.x = -4.6;
		camera.target = camera.position + camera.view;
	}

	//elvator
	if ((camera.position.z <= 28.9) && (camera.position.z >= 28) && (camera.position.x <= -5.2) && (camera.position.x >= -7.5))
	{
		camera.position.z = 28.9;
		camera.target = camera.position + camera.view;
	}
	if ((camera.position.x <= -7.1) && (camera.position.z <= 31) && (camera.position.z >= 28))
	{
		camera.position.x = -7.1;
		camera.target = camera.position + camera.view;
	}
	if ((camera.position.z >= 30.5) && (camera.position.z <= 31) && (camera.position.x <= -5) && (camera.position.x >= -8))
	{
		camera.position.z = 30.5;
		camera.target = camera.position + camera.view;
	}
}

bool LobbyScene::IsInArcadeMachineInteraction()
{
	//arcade interaction colision
	return(camera.position.x >= entityList[ENTITY_MACHINE].getTransform().x - 1) &&
		(camera.position.z >= entityList[ENTITY_MACHINE].getTransform().z - 1) &&
		(camera.position.x <= entityList[ENTITY_MACHINE].getTransform().x + 1) &&
		(camera.position.z < entityList[ENTITY_MACHINE].getTransform().z + 1);
}

bool LobbyScene::IsInElevatorInteraction()
{
	//elvator interaction collision
	return ((camera.position.z >= 28.9) && (camera.position.z <= 30.5) &&
		(camera.position.x >= -7.1) && (camera.position.x <= -5.2));
}

void LobbyScene::PrintEvidence()
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

void LobbyScene::PrintProfiles()
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

void LobbyScene::RenderEvidenceObject(Entity* entity, float rangeX, float rangeZ) {
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
				RenderTextOnScreen(meshList[GEO_TEXT], "Press F To Inspect", Color(1, 1, 1), 4, 25, 6);

				if (Application::IsKeyPressed('F') && (Interacted == false) && !isJournalOpen)
				{
					if (entity == &entityList[ENTITY_PSYCHO_PILLS])
					{
						Application::AddEvidence("Carmanadezpine pills", "Guard");
					}
					else if (entity == &entityList[ENTITY_ALCHOHOL_BOTTLE])
					{
						Application::AddEvidence("Getting rid of the bottle", "Janitor");
					}
					else if (entity == &entityList[ENTITY_BOTTLEMIX])
					{
						Application::AddEvidence("Bottle of weird mixtures", "Kid");
					}
					else if (entity == &entityList[ENTITY_NOTES])
					{
						Application::AddEvidence("Written notes", "Old Man");
					}
					else if (entity == &entityList[ENTITY_KNIFE])
					{
						Application::AddEvidence("Pocket Knife", "Gamer");
					}
					Inspect = true;
					Interacted = true;
					Pickup = true;
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
					camera.EnableControl();
				}
				else if (!Application::IsKeyPressed('F') && (Interacted == true))
				{
					Interacted = false;
				}

				RenderTextOnScreen(meshList[GEO_TEXT], "Press F To Stop Inspecting", Color(1, 1, 1), 3, 27, 8);
				RenderTextOnScreen(meshList[GEO_TEXT], "Arrow Keys To Turn The Object", Color(1, 1, 1), 1.5, 60, 30);

				InspectEvidenceOnScreen(entity->getMesh(), 40, 20, 60, 60, rotateZ, rotateX);
			}
		}
	}
}

void LobbyScene::Init()
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
		//1st Light (Window)
		light[0].type = Light::LIGHT_SPOT;
		light[0].position.Set(18, 20, 0);
		light[0].color.Set(1, 1, 1);
		light[0].power = 2;
		light[0].kC = 1.f;
		light[0].kL = 0.01f;
		light[0].kQ = 0.001f;
		light[0].cosCutoff = cos(Math::DegreeToRadian(45));
		light[0].cosInner = cos(Math::DegreeToRadian(30));
		light[0].exponent = 3.f;
		light[0].spotDirection.Set(0.f, 1.f, 0.f);

		//2nd Light (Bar)
		light[1].type = Light::LIGHT_SPOT;
		light[1].position.Set(-15, 12, 3);
		light[1].color.Set(1, 1, 1);
		light[1].power = 2.5;
		light[1].kC = 1.f;
		light[1].kL = 0.01f;
		light[1].kQ = 0.001f;
		light[1].cosCutoff = cos(Math::DegreeToRadian(60));
		light[1].cosInner = cos(Math::DegreeToRadian(45));
		light[1].exponent = 3.f;
		light[1].spotDirection.Set(0.f, 1.f, 0.f);

		//3rd Light (Arcade)
		light[2].type = Light::LIGHT_SPOT;
		light[2].position.Set(4.5, 4, -14);
		light[2].color.Set(1, 1, 1);
		light[2].power = 1.8f;
		light[2].kC = 1.f;
		light[2].kL = 0.01f;
		light[2].kQ = 0.001f;
		light[2].cosCutoff = cos(Math::DegreeToRadian(45));
		light[2].cosInner = cos(Math::DegreeToRadian(30));
		light[2].exponent = 3.f;
		light[2].spotDirection.Set(0.f, 1.f, 0.f);

		//4th Light (Lift)
		light[3].type = Light::LIGHT_SPOT;
		light[3].position.Set(-3, 9, 27);
		light[3].color.Set(1, 1, 1);
		light[3].power = 1.8f;
		light[3].kC = 1.f;
		light[3].kL = 0.01f;
		light[3].kQ = 0.001f;
		light[3].cosCutoff = cos(Math::DegreeToRadian(45));
		light[3].cosInner = cos(Math::DegreeToRadian(30));
		light[3].exponent = 3.f;
		light[3].spotDirection.Set(0.f, 1.f, 0.f);
	}


	//set background color
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	//generate default VAO for now
	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);

	//load vertex and fragment shaders
	m_programID = LoadShaders("Shader//Texture.vertexshader", "Shader//Text.fragmentshader");

	//Lights m_params
	{
		m_parameters[U_MVP] = glGetUniformLocation(m_programID, "MVP");
		m_parameters[U_MODELVIEW] = glGetUniformLocation(m_programID, "MV");
		m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE] = glGetUniformLocation(m_programID, "MV_inverse_transpose");
		m_parameters[U_MATERIAL_AMBIENT] = glGetUniformLocation(m_programID, "material.kAmbient");
		m_parameters[U_MATERIAL_DIFFUSE] = glGetUniformLocation(m_programID, "material.kDiffuse");
		m_parameters[U_MATERIAL_SPECULAR] = glGetUniformLocation(m_programID, "material.kSpecular");
		m_parameters[U_MATERIAL_SHININESS] = glGetUniformLocation(m_programID, "material.kShininess");

		//1st Light (Window)
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
		
		//2nd Light (Bar)
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

		//3rd Light (Arcade)
		m_parameters[U_LIGHT2_TYPE] = glGetUniformLocation(m_programID, "lights[2].type");
		m_parameters[U_LIGHT2_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[2].spotDirection");
		m_parameters[U_LIGHT2_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[2].cosCutoff");
		m_parameters[U_LIGHT2_COSINNER] = glGetUniformLocation(m_programID, "lights[2].cosInner");
		m_parameters[U_LIGHT2_EXPONENT] = glGetUniformLocation(m_programID, "lights[2].exponent");
		m_parameters[U_LIGHT2_POSITION] = glGetUniformLocation(m_programID, "lights[2].position_cameraspace");
		m_parameters[U_LIGHT2_COLOR] = glGetUniformLocation(m_programID, "lights[2].color");
		m_parameters[U_LIGHT2_POWER] = glGetUniformLocation(m_programID, "lights[2].power");
		m_parameters[U_LIGHT2_KC] = glGetUniformLocation(m_programID, "lights[2].kC");
		m_parameters[U_LIGHT2_KL] = glGetUniformLocation(m_programID, "lights[2].kL");
		m_parameters[U_LIGHT2_KQ] = glGetUniformLocation(m_programID, "lights[2].kQ");

		//4th Light (Lift)
		m_parameters[U_LIGHT3_TYPE] = glGetUniformLocation(m_programID, "lights[3].type");
		m_parameters[U_LIGHT3_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[3].spotDirection");
		m_parameters[U_LIGHT3_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[3].cosCutoff");
		m_parameters[U_LIGHT3_COSINNER] = glGetUniformLocation(m_programID, "lights[3].cosInner");
		m_parameters[U_LIGHT3_EXPONENT] = glGetUniformLocation(m_programID, "lights[3].exponent");
		m_parameters[U_LIGHT3_POSITION] = glGetUniformLocation(m_programID, "lights[3].position_cameraspace");
		m_parameters[U_LIGHT3_COLOR] = glGetUniformLocation(m_programID, "lights[3].color");
		m_parameters[U_LIGHT3_POWER] = glGetUniformLocation(m_programID, "lights[3].power");
		m_parameters[U_LIGHT3_KC] = glGetUniformLocation(m_programID, "lights[3].kC");
		m_parameters[U_LIGHT3_KL] = glGetUniformLocation(m_programID, "lights[3].kL");
		m_parameters[U_LIGHT3_KQ] = glGetUniformLocation(m_programID, "lights[3].kQ");
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
	glUniform1i(m_parameters[U_NUMLIGHTS], 4);

	//Lights glParams
	{
		//Light 0 (Window)
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
		}

		//Light 1 (Bar)
		{
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

		//Light 2 (Arcade)
		{
			//light[2].type = Light::LIGHT_DIRECTIONAL;
			glUniform1i(m_parameters[U_LIGHT2_TYPE], light[2].type);
			glUniform3fv(m_parameters[U_LIGHT2_COLOR], 1, &light[2].color.r);
			glUniform1f(m_parameters[U_LIGHT2_POWER], light[2].power);

			//light[2].type = Light::LIGHT_POINT;
			glUniform1f(m_parameters[U_LIGHT2_KC], light[2].kC);
			glUniform1f(m_parameters[U_LIGHT2_KL], light[2].kL);
			glUniform1f(m_parameters[U_LIGHT2_KQ], light[2].kQ);

			//light[2].type = Light::LIGHT_SPOT;
			glUniform1f(m_parameters[U_LIGHT2_COSCUTOFF], light[2].cosCutoff);
			glUniform1f(m_parameters[U_LIGHT2_COSINNER], light[2].cosInner);
			glUniform1f(m_parameters[U_LIGHT2_EXPONENT], light[2].exponent);
		}

		//Light 3 (Lift)
		{
			//light[3].type = Light::LIGHT_DIRECTIONAL;
			glUniform1i(m_parameters[U_LIGHT3_TYPE], light[3].type);
			glUniform3fv(m_parameters[U_LIGHT3_COLOR], 1, &light[3].color.r);
			glUniform1f(m_parameters[U_LIGHT3_POWER], light[3].power);

			//light[3].type = Light::LIGHT_POINT;
			glUniform1f(m_parameters[U_LIGHT3_KC], light[3].kC);
			glUniform1f(m_parameters[U_LIGHT3_KL], light[3].kL);
			glUniform1f(m_parameters[U_LIGHT3_KQ], light[3].kQ);

			//light[3].type = Light::LIGHT_SPOT;
			glUniform1f(m_parameters[U_LIGHT3_COSCUTOFF], light[3].cosCutoff);
			glUniform1f(m_parameters[U_LIGHT3_COSINNER], light[3].cosInner);
			glUniform1f(m_parameters[U_LIGHT3_EXPONENT], light[3].exponent);
		}
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

	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureID = LoadTGA("Image//Typewriter.tga");

	meshList[GEO_OFFICER_M]=MeshBuilder::GenerateOBJMTL("CHIEF", "OBJ//officer_male.obj", "OBJ//officer_male.mtl");
	meshList[GEO_OFFICER_M]->textureID = LoadTGA("Image//PolygonOffice_Texture_01_A.tga");

	meshList[GEO_OFFICER_F] = MeshBuilder::GenerateOBJMTL("CHIEF", "OBJ//officer_female.obj", "OBJ//officer_female.mtl");
	meshList[GEO_OFFICER_F]->textureID = LoadTGA("Image//PolygonOffice_Texture_01_A.tga");

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

	//Dialogue BG
	{
		meshList[GEO_DIALOGUE] = MeshBuilder::GenerateQuad("dialogue", Color(0.5, 0.5, 0.5), 1.f);
		meshList[GEO_DIALOGUE]->textureID = LoadTGA("Image//dialogue_start.tga");
		meshList[GEO_DIALOGUE2] = MeshBuilder::GenerateQuad("dialogue2", Color(0.5, 0.5, 0.5), 1.f);
		meshList[GEO_DIALOGUE2]->textureID = LoadTGA("Image//dialogue_gossip.tga");
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

	//Main Characters
	{
		//gamer npc
		entityList[ENTITY_GAMER].setMesh(MeshBuilder::GenerateOBJMTL("gamer", "OBJ//Gamer.obj", "OBJ//Gamer.mtl"));
		entityList[ENTITY_GAMER].getMesh()->textureID = LoadTGA("Image//PolygonCity_Texture_03_B.tga");
		entityList[ENTITY_GAMER].setTransform(Vector3(0.0f, 0.f, -14.f)); //transform by default is 0,0,0

		//janitor npc
		entityList[ENTITY_JANITOR].setMesh(MeshBuilder::GenerateOBJMTL("janitor", "OBJ//Janitor.obj", "OBJ//Janitor.mtl"));
		entityList[ENTITY_JANITOR].getMesh()->textureID = LoadTGA("Image//PolygonOffice_Texture_02_C.tga");
		entityList[ENTITY_JANITOR].setTransform(Vector3(-7.f, 0.f, 7.f)); //transform by default is 0,0,0
		entityList[ENTITY_JANITOR].setRotationAngle(float(90.f));
		entityList[ENTITY_JANITOR].setRotationAxis(Vector3(0.f, 3.f, 0.f));

		//old man npc
		entityList[ENTITY_OLDMAN].setMesh(MeshBuilder::GenerateOBJMTL("Old Man", "OBJ//OldMan.obj", "OBJ//OldMan.mtl"));
		entityList[ENTITY_OLDMAN].getMesh()->textureID = LoadTGA("Image//PolygonCity_Texture_01_C.tga");
		entityList[ENTITY_OLDMAN].setTransform(Vector3(5.6f, 0.f, 0.5f)); //transform by default is 0,0,0
		//kid npc
		entityList[ENTITY_KID].setMesh(MeshBuilder::GenerateOBJMTL("Kid", "OBJ//Kid.obj", "OBJ//Kid.mtl"));
		entityList[ENTITY_KID].getMesh()->textureID = LoadTGA("Image//PolygonKids_Texture_01_A.tga");
		entityList[ENTITY_KID].setTransform(Vector3(5.5f, 0.f, -5.f)); //transform by default is 0,0,0
		//guard npc
		entityList[ENTITY_GUARD].setMesh(MeshBuilder::GenerateOBJMTL("guard", "OBJ//Guard.obj", "OBJ//Guard.mtl"));
		entityList[ENTITY_GUARD].getMesh()->textureID = LoadTGA("Image//PolygonOffice_Texture_01_A.tga");
		entityList[ENTITY_GUARD].setTransform(Vector3(-5.6f, 0.f, 1.3f)); //transform by default is 0,0,0
		entityList[ENTITY_GUARD].setRotationAngle(float(-90.f));
		entityList[ENTITY_GUARD].setRotationAxis(Vector3(0.f, 1.f, 0.f));
		//chief
		entityList[ENTITY_CHIEF].setMesh(MeshBuilder::GenerateOBJMTL("CHIEF", "OBJ//officer_male.obj", "OBJ//officer_male.mtl"));
		entityList[ENTITY_CHIEF].getMesh()->textureID = LoadTGA("Image//PolygonOffice_Texture_01_A.tga");
		entityList[ENTITY_CHIEF].setTransform(Vector3(-1.1f, 0.f, 14.5f)); //transform by default is 0,0,0
	}
	
	//Lobby Stage + Assets
	{
		meshList[GEO_LOBBY] = MeshBuilder::GenerateOBJMTL("Dining Hall", "OBJ//ship_dininghall.obj", "OBJ//ship_dininghall.mtl");
		meshList[GEO_LOBBY]->textureID = LoadTGA("Image//PolygonOffice_Texture_02_A.tga");

		meshList[GEO_TABLES] = MeshBuilder::GenerateOBJMTL("Tables", "OBJ//dininghall_tables.obj", "OBJ//dininghall_tables.mtl");
		meshList[GEO_TABLES]->textureID = LoadTGA("Image//PolygonOffice_Texture_02_A.tga");

		//Arcade Machine
		entityList[ENTITY_MACHINE].setMesh(MeshBuilder::GenerateOBJMTL("Arcade Machine", "OBJ//arcade_machine.obj", "OBJ//arcade_machine.mtl"));
		entityList[ENTITY_MACHINE].getMesh()->textureID = LoadTGA("Image//PolygonOffice_Texture_01_AMachine.tga");
		entityList[ENTITY_MACHINE].setTransform(Vector3(4.5f, 0.f, -14.f));
	}

	//evidence
	{
		entityList[ENTITY_PSYCHO_PILLS].setMesh(MeshBuilder::GenerateOBJMTL("psycho pills", "OBJ//evidence//psycho_pills.obj", "OBJ//evidence//psycho_pills.mtl"));
		entityList[ENTITY_PSYCHO_PILLS].getMesh()->textureID = LoadTGA("Image//PolygonOffice_Texture_04_C.tga");
		entityList[ENTITY_PSYCHO_PILLS].setTransform(Vector3(-8, 0.9, 15)); //transform by default is 0,0,0

		entityList[ENTITY_NOTES].setMesh(MeshBuilder::GenerateOBJMTL("oldman notes", "OBJ//evidence//writing_notes.obj", "OBJ//evidence//writing_notes.mtl"));
		entityList[ENTITY_NOTES].getMesh()->textureID = LoadTGA("Image//PolygonOffice_Texture_03_B.tga");
		entityList[ENTITY_NOTES].setTransform(Vector3(-8, 0.9f, 10.f)); //transform by default is 0,0,0

		entityList[ENTITY_ALCHOHOL_BOTTLE].setMesh(MeshBuilder::GenerateOBJMTL("alchohol bottle", "OBJ//evidence//drinking_bottle.obj", "OBJ//evidence//drinking_bottle.mtl"));
		entityList[ENTITY_ALCHOHOL_BOTTLE].getMesh()->textureID = LoadTGA("Image//PolygonTown_Texture_01_A.tga");
		entityList[ENTITY_ALCHOHOL_BOTTLE].setTransform(Vector3(-8, 0.9f, 11.5f)); //transform by default is 0,0,0

		entityList[ENTITY_BOTTLEMIX].setMesh(MeshBuilder::GenerateOBJMTL("bottle mix", "OBJ//evidence//water_bottle.obj", "OBJ//evidence//water_bottle.mtl"));
		entityList[ENTITY_BOTTLEMIX].getMesh()->textureID = LoadTGA("Image//PolygonTown_Texture_01_A.tga");
		entityList[ENTITY_BOTTLEMIX].setTransform(Vector3(-8, 0.8f, 13.f)); //transform by default is 0,0,0

		entityList[ENTITY_KNIFE].setMesh(MeshBuilder::GenerateOBJMTL("pocket knife", "OBJ//evidence//cutlery_knife.obj", "OBJ//evidence//cutlery_knife.mtl"));
		entityList[ENTITY_KNIFE].getMesh()->textureID = LoadTGA("Image//PolygonTown_Texture_01_A.tga");
		entityList[ENTITY_KNIFE].setTransform(Vector3(-8, 0.9f, 14.f)); //transform by default is 0,0,0
	}

	isJournalOpen = false;
	journalPage = EVIDENCE_PAGE;
	rotateSkybox = 0;

	//hide and reset the cursor
	Application::ResetCursor();
	if (!isTalking)
	{
		Application::HideCursor();
	}
}

LobbyScene::LobbyScene()
{
	camera.Init(Vector3(-7, 1.5f, 30), Vector3(0, 1.5, 30), Vector3(0, 1, 0));

	//Init dialogues
	ChatDialogueInit("Text//GuardChat.txt", guardChat);// file format to be like this "Text//OldManChat.txt" and vector to store into
	ChatDialogueInit("Text//JanitorChat.txt", janitorChat);
	ChatDialogueInit("Text//GamerChat.txt", gamerChat);
	ChatDialogueInit("Text//KidChat.txt", kidChat);
	ChatDialogueInit("Text//OldManChat.txt", oldManChat);

	Application::AddEvidence("Guest list: Kids", "Kid");
	Application::AddEvidence("Guest list: daughter", "Old Man");
}

void LobbyScene::Update(double dt)
{
	camera.Update(dt);

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

	//Mouse Inputs
	{
		static bool bLButtonState = false;
		if (!bLButtonState && Application::IsMousePressed(0))
		{
			bLButtonState = true;
			TalkButtons();
		}
		else if (bLButtonState && !Application::IsMousePressed(0))
		{
			bLButtonState = false;
		}
	}

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
		if (!jButtonState && Application::IsKeyPressed('J') && !isJournalOpen && !isTalking)
		{
			camera.DisableControl();
			Application::SetCanPause(false);
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
			camera.EnableControl();
			Application::HideCursor();
			Application::SetCanPause(true);
			isJournalOpen = false;
			jButtonState = true;
			ResetJournal();
		}
	}
	if (!isJournalOpen)
	{

		if (IsInArcadeMachineInteraction() && Application::IsKeyPressed('E') && !isJournalOpen)
		{
			Application::sceneState = Application::STATE_MINIGAME_INIT;
		}

		if (IsInElevatorInteraction() && Application::IsKeyPressed('E') && !isJournalOpen) 
		{
			Application::sceneState = Application::STATE_CORRIDOR_INIT;
		}
	}

	//Debug test pin culprit
	if (Application::IsKeyPressed(VK_F1))
	{
		Application::playerGuesses--;
	}

	if (Application::playerGuesses <= 0) {
		Application::sceneState = Application::STATE_GAMELOSE;
	}

	Interaction();
	BoundsCheck();

	rotateSkybox -= 5 * dt;
	if (rotateSkybox <= -360)
	{
		rotateSkybox = 0;
	}

	if (Application::EnoughEvidence(Application::susCounter))
	{
		Application::AddEvidence("Suspicious to everybody", "Gamer");
	}
	if (Application::arcadeGameScore >= 1000)
	{
		isDoneGaming = true;
	}
}

void LobbyScene::Render()
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

	//2nd Light Parameters
	lightPosition_cameraspace = viewStack.Top() * light[1].position;
	glUniform3fv(m_parameters[U_LIGHT1_POSITION], 1, &lightPosition_cameraspace.x);
	spotDirection_cameraspace = viewStack.Top() * light[1].spotDirection;
	glUniform3fv(m_parameters[U_LIGHT1_SPOTDIRECTION], 1, &spotDirection_cameraspace.x);

	//3rd Light Parameters
	lightPosition_cameraspace = viewStack.Top() * light[2].position;
	glUniform3fv(m_parameters[U_LIGHT2_POSITION], 1, &lightPosition_cameraspace.x);
	spotDirection_cameraspace = viewStack.Top() * light[2].spotDirection;
	glUniform3fv(m_parameters[U_LIGHT2_SPOTDIRECTION], 1, &spotDirection_cameraspace.x);

	//4th Light Parameters
	lightPosition_cameraspace = viewStack.Top() * light[3].position;
	glUniform3fv(m_parameters[U_LIGHT3_POSITION], 1, &lightPosition_cameraspace.x);
	spotDirection_cameraspace = viewStack.Top() * light[3].spotDirection;
	glUniform3fv(m_parameters[U_LIGHT3_SPOTDIRECTION], 1, &spotDirection_cameraspace.x);

	modelStack.PushMatrix();
	modelStack.Rotate(rotateSkybox, 0,1,0);
	RenderSkybox();
	modelStack.PopMatrix();

	//Stage + Assets
	{
		modelStack.PushMatrix();
		modelStack.Translate(-4, 0, 12);
		modelStack.Scale(1, 1, 1);
		RenderMesh(meshList[GEO_LOBBY], true);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(-4, 0, 12);
		modelStack.Scale(1, 1, 1);
		RenderMesh(meshList[GEO_TABLES], true);
		modelStack.PopMatrix();
	}

	RenderEntity(&entityList[ENTITY_GUARD], true);
	RenderEntity(&entityList[ENTITY_JANITOR], true);
	RenderEntity(&entityList[ENTITY_GAMER], true);
	RenderEntity(&entityList[ENTITY_KID], true);
	RenderEntity(&entityList[ENTITY_OLDMAN], true);
	RenderEntity(&entityList[ENTITY_MACHINE], true);
	RenderEntity(&entityList[ENTITY_CHIEF], true);
	RenderEvidenceObject(&entityList[ENTITY_PSYCHO_PILLS], 0.5f, 0.5f);
	RenderEvidenceObject(&entityList[ENTITY_NOTES], 0.5f, 0.5f);
	RenderEvidenceObject(&entityList[ENTITY_ALCHOHOL_BOTTLE], 0.5f, 0.5f);
	RenderEvidenceObject(&entityList[ENTITY_BOTTLEMIX], 0.5f, 0.5f);
	RenderEvidenceObject(&entityList[ENTITY_KNIFE], 0.5f, 0.5f);

	RenderOfficers();


	if (isJournalOpen)
	{
		RenderJournal();
	}

	if (IsInArcadeMachineInteraction() ||
		IsInElevatorInteraction())
	{
		RenderPressToInteract('E', "interact", 35, 10);
	}

	if (isTalking)
	{
		RenderInteraction();
	}
	else
	{
		if (isAbleToTalk)
		{
			RenderPressToInteract('E', "talk", 35, 10);

		}
		if (isAbleToPin)
		{
			RenderPressToInteract('C', "pin culprit", 30, 15);
		}
	}
	RenderHUD();

	if (isJournalOpen)
	{
		RenderJournal();
	}
}

void LobbyScene::Exit()
{
	// Cleanup VBO here
	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);
}