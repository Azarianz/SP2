#include "CorridorScene.h"
#include "GL\glew.h"

#include "shader.hpp"
#include "MeshBuilder.h"

#define LSPEED 20

void CorridorScene::RenderMesh(Mesh* mesh, bool enableLight)
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

void CorridorScene::RenderEntity(Entity* entity, bool enableLight)
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

void CorridorScene::RenderText(Mesh* mesh, std::string text, Color color)
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

void CorridorScene::RenderMeshOnScreen(Mesh* mesh, float x, float y, float sizex, float sizey)
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

void CorridorScene::RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y)
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

bool CorridorScene::CreateButton(float buttonTop, float buttonBottom, float buttonRight, float buttonLeft)
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
		std::cout << "Hit!" << std::endl;
		//trigger user action or function
		return true;
	}
	else
	{
		std::cout << "Miss!" << std::endl;
		return false;
	}
}

void CorridorScene::RenderSkybox()
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

void CorridorScene::RenderPressEToInteract()
{
	RenderTextOnScreen(meshList[GEO_TEXT], "Press E to interact", Color(1, 1, 1), 3, 29, 10);
}

void CorridorScene::RenderHUD()
{
	string clues = "Clues found:" + std::to_string(Application::eList.size()) + "/20";
	string guess = "Guesses Left:" + std::to_string(Application::playerGuesses) + "/3";

	if (!isJournalOpen)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], guess, Color(1, 1, 1), 2, 8, Application::screenUISizeY - 10);
		RenderTextOnScreen(meshList[GEO_TEXT], "Find the Culprit", Color(1, 1, 1), 2, 8, Application::screenUISizeY - 13);
		RenderTextOnScreen(meshList[GEO_TEXT], "(J) Journal", Color(1, 1, 1), 2, Application::screenUISizeX - 16, Application::screenUISizeY - 10);
		RenderTextOnScreen(meshList[GEO_TEXT], clues, Color(1, 1, 1), 2, Application::screenUISizeX - 15, Application::screenUISizeY - 13);
	}
}

bool CorridorScene::IsInElevatorInteraction()
{
	//elvator interaction collision
	return ((camera.position.z >= 1.5) && (camera.position.z <= 4) &&
		(camera.position.x >= -5) && (camera.position.x <= -2.5));
}

bool CorridorScene::IsInDoor1Interaction()
{
	//door1 interaction collision
	return ((camera.position.z >= 16) && (camera.position.z <= 20) &&
		(camera.position.x >= -2) && (camera.position.x <= 0));
}

bool CorridorScene::IsInDoor2Interaction()
{
	//door2 interaction collision
	return ((camera.position.z >= 9) && (camera.position.z <= 11) &&
		(camera.position.x >= -2) && (camera.position.x <= 0));
}

bool CorridorScene::IsInDoor3Interaction()
{
	//door3 interaction collision
	return ((camera.position.z >= -10) && (camera.position.z <= -8) &&
		(camera.position.x >= -2) && (camera.position.x <= 0));
}

bool CorridorScene::IsInDoor4Interaction()
{
	//door4 interaction collision
	return ((camera.position.z >= -19) && (camera.position.z <= -16) &&
		(camera.position.x >= -2) && (camera.position.x <= 0));
}

void CorridorScene::RenderJournal()
{
	float journalButtonHeight = Application::screenUISizeY / 5;
	float journalButtonWidth = Application::screenUISizeX / 4;

	RenderMeshOnScreen(meshList[GEO_QUAD], Application::screenUISizeX / 2, Application::screenUISizeY / 2, Application::screenUISizeX, Application::screenUISizeY);

	int buttonSpacing = 4;

	RenderMeshOnScreen(meshList[GEO_QUAD_BUTTON], journalButtonWidth / 2, Application::screenUISizeY - journalButtonHeight / 2 - buttonSpacing, journalButtonWidth - buttonSpacing, journalButtonHeight);
	int tempCount = 1;
	RenderMeshOnScreen(meshList[GEO_QUAD_BUTTON], journalButtonWidth / 2 + journalButtonWidth * tempCount, Application::screenUISizeY - journalButtonHeight / 2 - buttonSpacing, journalButtonWidth - buttonSpacing, journalButtonHeight);
	++tempCount;
	RenderMeshOnScreen(meshList[GEO_QUAD_BUTTON], journalButtonWidth / 2 + journalButtonWidth * tempCount, Application::screenUISizeY - journalButtonHeight / 2 - buttonSpacing, journalButtonWidth - buttonSpacing, journalButtonHeight);
	++tempCount;
	RenderMeshOnScreen(meshList[GEO_QUAD_BUTTON], journalButtonWidth / 2 + journalButtonWidth * tempCount, Application::screenUISizeY - journalButtonHeight / 2 - buttonSpacing, journalButtonWidth - buttonSpacing, journalButtonHeight);

	//so that button works with different resolution
	float tempScreenUISizeX = Application::GetWindowWidth() / 10;
	float tempScreenUISizeY = Application::GetWindowHeight() / 10;
	journalButtonHeight = tempScreenUISizeY / 5;
	journalButtonWidth = tempScreenUISizeX / 4;

	static bool lcButtonState = false;
	if (!lcButtonState && Application::IsMousePressed(0))
	{
		lcButtonState = true;
		//button1
		if (CreateButton(tempScreenUISizeY - journalButtonHeight / 2 - buttonSpacing + journalButtonHeight / 2,
			tempScreenUISizeY - journalButtonHeight / 2 - buttonSpacing - journalButtonHeight / 2,
			journalButtonWidth / 2 + (journalButtonWidth / 2 - buttonSpacing / 2),
			journalButtonWidth / 2 - (journalButtonWidth / 2 - buttonSpacing / 2)))
		{
			std::cout << "journal page is: evidence" << std::endl;
			journalPage = EVIDENCE_PAGE;
		}
		int tempCount = 1;
		if (CreateButton(tempScreenUISizeY - journalButtonHeight / 2 - buttonSpacing + journalButtonHeight / 2,
			tempScreenUISizeY - journalButtonHeight / 2 - buttonSpacing - journalButtonHeight / 2,
			(journalButtonWidth / 2 + journalButtonWidth * tempCount) + (journalButtonWidth / 2 - buttonSpacing / 2),
			(journalButtonWidth / 2 + journalButtonWidth * tempCount) - (journalButtonWidth / 2 - buttonSpacing / 2)))
		{
			std::cout << "journal page is: profile" << std::endl;
			journalPage = PROFILE_PAGE;
		}
		++tempCount;
		if (CreateButton(tempScreenUISizeY - journalButtonHeight / 2 - buttonSpacing + journalButtonHeight / 2,
			tempScreenUISizeY - journalButtonHeight / 2 - buttonSpacing - journalButtonHeight / 2,
			(journalButtonWidth / 2 + journalButtonWidth * tempCount) + (journalButtonWidth / 2 - buttonSpacing / 2),
			(journalButtonWidth / 2 + journalButtonWidth * tempCount) - (journalButtonWidth / 2 - buttonSpacing / 2)))
		{
			std::cout << "journal page is: 3" << std::endl;
			journalPage = EVIDENCE_PAGE;
		}
		++tempCount;
		if (CreateButton(tempScreenUISizeY - journalButtonHeight / 2 - buttonSpacing + journalButtonHeight / 2,
			tempScreenUISizeY - journalButtonHeight / 2 - buttonSpacing - journalButtonHeight / 2,
			(journalButtonWidth / 2 + journalButtonWidth * tempCount) + (journalButtonWidth / 2 - buttonSpacing / 2),
			(journalButtonWidth / 2 + journalButtonWidth * tempCount) - (journalButtonWidth / 2 - buttonSpacing / 2)))
		{
			std::cout << "journal page is: 4" << std::endl;
			journalPage = EVIDENCE_PAGE;
		}
	}
	else if (lcButtonState && !Application::IsMousePressed(0))
	{
		lcButtonState = false;
	}

	if (journalPage == EVIDENCE_PAGE)
	{
		PrintEvidence();
	}
	else if (journalPage == PROFILE_PAGE)
	{

	}
}

void CorridorScene::ResetJournal()
{
}

void CorridorScene::PrintEvidence()
{
	int xpos = ((Application::screenUISizeX / 3.1));
	int yOffset = ((Application::screenUISizeY / 1.7) - 3);
	static bool EButtonState = false;
	static bool QButtonState = false;

	if (!Application::eList.empty())
	{
		//evidencePage = 1;
		int index = (evidencePage - 1) * 4;
		int endIndex = evidencePage * 4;

		if (endIndex >= Application::eList.size()) {
			endIndex = (Application::eList.size());
		}

		//Add logic to print based on page number eg.(page 2: for (i = 8; i < i + 4; i++))
		for (int i = index; i <= (endIndex - 1); i++)
		{
			RenderTextOnScreen(meshList[GEO_TEXT], Application::eList[i], Color(1, 1, 1), 3, xpos, yOffset);
			yOffset -= 8;
		}

		if (!EButtonState && Application::IsKeyPressed('E'))
		{
			cout << "E" << endl;
			EButtonState = true;

			if (evidencePage >= 4) {
				evidencePage = 1;
			}
			else {
				evidencePage++;
			}

			index = (evidencePage - 1) * 4;
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

			index = (evidencePage - 1) * 4;
		}

		else if (QButtonState && !Application::IsKeyPressed('Q'))
		{
			QButtonState = false;
		}

		RenderTextOnScreen(meshList[GEO_TEXT], "<", Color(1, 1, 1), 4, 5, 20);
		RenderTextOnScreen(meshList[GEO_TEXT], ">", Color(1, 1, 1), 4, 75, 20);
	}

	const int i = 9;
	int arrag[i];
}

void CorridorScene::Init()
{
	// Init VBO here
	Mtx44 projection;

	projection.SetToPerspective(60.f, 4.f / 3.f, 0.1f, 1000.f);
	projectionStack.LoadMatrix(projection);

	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	camera.Init(Vector3(0, 1.5f, 0), Vector3(5, 1.5f, 0), Vector3(0, 1,0));

	light[0].type = Light::LIGHT_SPOT;
	light[0].position.Set(0, 20, 0);
	light[0].color.Set(1, 1, 1);
	light[0].power = 2;
	light[0].kC = 1.f;
	light[0].kL = 0.01f;
	light[0].kQ = 0.001f;
	light[0].cosCutoff = cos(Math::DegreeToRadian(45));
	light[0].cosInner = cos(Math::DegreeToRadian(30));
	light[0].exponent = 3.f;
	light[0].spotDirection.Set(0.f, 1.f, 0.f);

	//set background color
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	//generate default VAO for now
	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);

	//load vertex and fragment shaders
	m_programID = LoadShaders("Shader//Texture.vertexshader", "Shader//Text.fragmentshader");

	m_parameters[U_LIGHT0_TYPE] = glGetUniformLocation(m_programID, "lights[0].type");
	m_parameters[U_LIGHT0_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[0].spotDirection");
	m_parameters[U_LIGHT0_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[0].cosCutoff");
	m_parameters[U_LIGHT0_COSINNER] = glGetUniformLocation(m_programID, "lights[0].cosInner");
	m_parameters[U_LIGHT0_EXPONENT] = glGetUniformLocation(m_programID, "lights[0].exponent");
	m_parameters[U_NUMLIGHTS] = glGetUniformLocation(m_programID, "numLights");
	m_parameters[U_MVP] = glGetUniformLocation(m_programID, "MVP");
	m_parameters[U_MODELVIEW] = glGetUniformLocation(m_programID, "MV");
	m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE] = glGetUniformLocation(m_programID, "MV_inverse_transpose");
	m_parameters[U_MATERIAL_AMBIENT] = glGetUniformLocation(m_programID, "material.kAmbient");
	m_parameters[U_MATERIAL_DIFFUSE] = glGetUniformLocation(m_programID, "material.kDiffuse");
	m_parameters[U_MATERIAL_SPECULAR] = glGetUniformLocation(m_programID, "material.kSpecular");
	m_parameters[U_MATERIAL_SHININESS] = glGetUniformLocation(m_programID, "material.kShininess");
	m_parameters[U_LIGHT0_POSITION] = glGetUniformLocation(m_programID, "lights[0].position_cameraspace");
	m_parameters[U_LIGHT0_COLOR] = glGetUniformLocation(m_programID, "lights[0].color");
	m_parameters[U_LIGHT0_POWER] = glGetUniformLocation(m_programID, "lights[0].power");
	m_parameters[U_LIGHT0_KC] = glGetUniformLocation(m_programID, "lights[0].kC");
	m_parameters[U_LIGHT0_KL] = glGetUniformLocation(m_programID, "lights[0].kL");
	m_parameters[U_LIGHT0_KQ] = glGetUniformLocation(m_programID, "lights[0].kQ");
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
	glUniform1i(m_parameters[U_NUMLIGHTS], 1);
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

	meshList[GEO_SPHERE] = MeshBuilder::GenerateSphere("Sphere", Color(0.5, 1.0, 1.0), 20, 20, 1);
	meshList[GEO_SPHERE]->textureID = LoadTGA("Image//color.tga");

	meshList[GEO_AXES] = MeshBuilder::GenerateAxes("reference");

	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureID = LoadTGA("Image//arial.tga");

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
		meshList[GEO_GUARD] = MeshBuilder::GenerateOBJMTL("guard", "OBJ//Guard.obj", "OBJ//Guard.mtl");
		meshList[GEO_GUARD]->textureID = LoadTGA("Image//PolygonOffice_Texture_01_A.tga");

		meshList[GEO_GAMER] = MeshBuilder::GenerateOBJMTL("gamer", "OBJ//Gamer.obj", "OBJ//Gamer.mtl");
		meshList[GEO_GAMER]->textureID = LoadTGA("Image//PolygonCity_Texture_03_B.tga");

		meshList[GEO_JANITOR] = MeshBuilder::GenerateOBJMTL("janitor", "OBJ//Janitor.obj", "OBJ//Janitor.mtl");
		meshList[GEO_JANITOR]->textureID = LoadTGA("Image//PolygonOffice_Texture_02_C.tga");

		meshList[GEO_OLDMAN] = MeshBuilder::GenerateOBJMTL("Old Man", "OBJ//OldMan.obj", "OBJ//OldMan.mtl");
		meshList[GEO_OLDMAN]->textureID = LoadTGA("Image//PolygonCity_Texture_01_C.tga");

		meshList[GEO_KID] = MeshBuilder::GenerateOBJMTL("Kid", "OBJ//Kid.obj", "OBJ//Kid.mtl");
		meshList[GEO_KID]->textureID = LoadTGA("Image//PolygonKids_Texture_01_A.tga");
	}

	//Corridor Stage + Assets
	{
		meshList[GEO_CORRIDOR] = MeshBuilder::GenerateOBJMTL("Corridor", "OBJ//ship_corridor.obj", "OBJ//ship_corridor.mtl");
		meshList[GEO_CORRIDOR]->textureID = LoadTGA("Image//PolygonOffice_Texture_01_AMachine.tga");
	}

}

void CorridorScene::Update(double dt)
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

	if (Application::IsKeyPressed('I'))
		light[0].position.z -= (float)(LSPEED * dt);
	if (Application::IsKeyPressed('K'))
		light[0].position.z += (float)(LSPEED * dt);
	if (Application::IsKeyPressed('J'))
		light[0].position.x -= (float)(LSPEED * dt);
	if (Application::IsKeyPressed('L'))
		light[0].position.x += (float)(LSPEED * dt);
	if (Application::IsKeyPressed('O'))
		light[0].position.y -= (float)(LSPEED * dt);
	if (Application::IsKeyPressed('P'))
		light[0].position.y += (float)(LSPEED * dt);

	if (Application::IsKeyPressed('5'))
	{
		light[0].type = Light::LIGHT_POINT;
		glUniform1i(m_parameters[U_LIGHT0_TYPE], light[0].type);
	}
	else if (Application::IsKeyPressed('6'))
	{
		light[0].type = Light::LIGHT_DIRECTIONAL;
		glUniform1i(m_parameters[U_LIGHT0_TYPE], light[0].type);
	}
	else if (Application::IsKeyPressed('7'))
	{
		light[0].type = Light::LIGHT_SPOT;
		glUniform1i(m_parameters[U_LIGHT0_TYPE], light[0].type);
	}

	//Mouse Inputs
	{
		static bool bLButtonState = false;
		if (!bLButtonState && Application::IsMousePressed(0))
		{
			bLButtonState = true;
			std::cout << "LBUTTON DOWN" << std::endl;

			CreateButton(25 + 10, 25, 30 + 20, 30);
		}
		else if (bLButtonState && !Application::IsMousePressed(0))
		{
			bLButtonState = false;
			std::cout << "LBUTTON UP" << std::endl;
		}
		static bool bRButtonState = false;
		if (!bRButtonState && Application::IsMousePressed(1))
		{
			bRButtonState = true;
			std::cout << "RBUTTON DOWN" << std::endl;
		}
		else if (bRButtonState && !Application::IsMousePressed(1))
		{
			bRButtonState = false;
			std::cout << "RBUTTON UP" << std::endl;
		}

	}

	//Journal
	{
		static bool jButtonState = false;
		if (!jButtonState && Application::IsKeyPressed('J') && !isJournalOpen)
		{
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
			camera.EnableControl();
			Application::HideCursor();
			isJournalOpen = false;
			jButtonState = true;
		}
	}

	//Check Door Interaction Collision
	{
		if ((IsInDoor1Interaction()) && Application::IsKeyPressed('E')) {
			Application::ResetCursor();
			Application::ShowCursor();
			Application::sceneState = Application::STATE_ROOM_INIT;
			Application::roomState = Application::ROOM4;
		}

		if ((IsInDoor2Interaction()) && Application::IsKeyPressed('E')) {
			Application::ResetCursor();
			Application::ShowCursor();
			Application::sceneState = Application::STATE_ROOM_INIT;
			Application::roomState = Application::ROOM4;
		}

		if ((IsInDoor3Interaction()) && Application::IsKeyPressed('E')) {
			Application::ResetCursor();
			Application::ShowCursor();
			Application::sceneState = Application::STATE_ROOM_INIT;
			Application::roomState = Application::ROOM4;
		}

		if ((IsInDoor4Interaction()) && Application::IsKeyPressed('E')) {
			Application::ResetCursor();
			Application::ShowCursor();
			Application::sceneState = Application::STATE_ROOM_INIT;
			Application::roomState = Application::ROOM4;
		}

		if (IsInElevatorInteraction() && Application::IsKeyPressed('E')) {
			Application::ResetCursor();
			Application::ShowCursor();
			Application::sceneState = Application::STATE_LOBBY;
		}
	}

	rotateSkybox += 5 * dt;

	if (rotateSkybox >= 360)
	{
		rotateSkybox = 0;
	}

	framePerSecond = 1.f / dt;
}

void CorridorScene::Render()
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

	if (light[0].type == Light::LIGHT_DIRECTIONAL)
	{
		Vector3 lightDir(light[0].position.x, light[0].position.y, light[0].position.z);
		Vector3 lightDirection_cameraspace = viewStack.Top() * lightDir;
		glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, &lightDirection_cameraspace.x);
	}
	else if (light[0].type == Light::LIGHT_SPOT)
	{
		Position lightPosition_cameraspace = viewStack.Top() * light[0].position;
		glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, &lightPosition_cameraspace.x);
		Vector3 spotDirection_cameraspace = viewStack.Top() * light[0].spotDirection;
		glUniform3fv(m_parameters[U_LIGHT0_SPOTDIRECTION], 1, &spotDirection_cameraspace.x);
	}
	else
	{
		Position lightPosition_cameraspace = viewStack.Top() * light[0].position;
		glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, &lightPosition_cameraspace.x);
	}

	RenderMesh(meshList[GEO_AXES], false);

	modelStack.PushMatrix();
	modelStack.Translate(light[0].position.x, light[0].position.y, light[0].position.z);
	RenderMesh(meshList[GEO_SUN], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Rotate(rotateSkybox, 0, 1, 0);
	RenderSkybox();
	modelStack.PopMatrix();

	//Main Characters
	{
		modelStack.PushMatrix();
		modelStack.Translate(0, 0, -2);
		modelStack.Scale(1, 1, 1);
		RenderMesh(meshList[GEO_GUARD], false);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(2, 0, -2);
		modelStack.Scale(1, 1, 1);
		RenderMesh(meshList[GEO_GAMER], false);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(4, 0, -2);
		modelStack.Scale(1, 1, 1);
		RenderMesh(meshList[GEO_KID], false);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(-2, 0, -2);
		modelStack.Scale(1, 1, 1);
		RenderMesh(meshList[GEO_JANITOR], false);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(-4, 0, -2);
		modelStack.Scale(1, 1, 1);
		RenderMesh(meshList[GEO_OLDMAN], false);
		modelStack.PopMatrix();
	}

	//Stage + Assets
	{
		modelStack.PushMatrix();
		modelStack.Translate(0, 0, 0);
		modelStack.Scale(1, 1, 1);
		RenderMesh(meshList[GEO_CORRIDOR], true);
		modelStack.PopMatrix();
	}
	
	if (IsInElevatorInteraction() || 
		IsInDoor1Interaction() ||
		IsInDoor2Interaction() ||
		IsInDoor3Interaction() ||
		IsInDoor4Interaction())
	{
		RenderPressEToInteract();
	}

	if (isJournalOpen)
	{
		RenderJournal();
	}

	RenderHUD();

	RenderTextOnScreen(meshList[GEO_TEXT], std::to_string(framePerSecond), Color(0, 1, 0), 4, 0, 0);
	//RenderTextOnScreen(meshList[GEO_TEXT], std::to_string(camera.position.x), Color(0, 1, 0), 4, 0, 0);
	//RenderTextOnScreen(meshList[GEO_TEXT], std::to_string(camera.position.z), Color(0, 1, 0), 4, 0, 2);
	//RenderMeshOnScreen(meshList[GEO_QUAD], 40, 30, 20, 10);

}

void CorridorScene::Exit()
{
	// Cleanup VBO here
	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);
}