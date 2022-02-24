#pragma once
#include"Scene.h"
#include"Mtx44.h"
#include "Application.h"
#include"Camera3.h"
#include "Mesh.h"
#include "MatrixStack.h"
#include "Light.h"
#include "Utility.h"
#include "LoadTGA.h"
#include "Entity.h"
#include <fstream>

class MainMenuScene : public Scene
{
private:
	enum GEOMETRY_TYPE
	{
		GEO_BACKGROUND,
		GEO_SELECT,
		GEO_LEFT,
		GEO_RIGHT,
		GEO_TOP,
		GEO_BOTTOM,
		GEO_FRONT,
		GEO_BACK,
		GEO_TEXT,
		NUM_GEOMETRY,
	};

	enum MENU_STATE
	{
		MAIN,
		INTRO,
		HOWTOPLAY,
		OPTIONS,
		RESOLUTION
	};

	enum UNIFORM_TYPE
	{
		U_MVP = 0,
		U_MODELVIEW,
		U_MODELVIEW_INVERSE_TRANSPOSE,
		U_MATERIAL_AMBIENT,
		U_MATERIAL_DIFFUSE,
		U_MATERIAL_SPECULAR,
		U_MATERIAL_SHININESS,
		U_COLOR_TEXTURE_ENABLED,
		U_COLOR_TEXTURE,
		U_TEXT_ENABLED,
		U_TEXT_COLOR,
		U_TOTAL,
	};

	MS modelStack, viewStack, projectionStack;

	unsigned m_parameters[U_TOTAL];
	unsigned m_vertexArrayID;
	Mesh* meshList[NUM_GEOMETRY];

	unsigned m_programID;

	Camera3 camera;

	//some variables for interactions
	char menuState; //what state the menu is in e.g. menuState = STATE_OPTIONS (is in options menu)
	float elapsedTime;
	float rotateSkybox;

	//color of text
	Color textColor;

	void RenderMesh(Mesh* mesh, bool enableLight);
	void RenderEntity(Entity* entity, bool enableLight);
	void RenderMeshOnScreen(Mesh* mesh, float x, float y, float sizex, float sizey);
	void RenderSkybox();
	void RenderText(Mesh* mesh, std::string text, Color color);
	void RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y);
	bool CreateButton(float buttonTop, float buttonBottom, float buttonRight, float buttonLeft);
	void RenderMainMenu();

public:
	MainMenuScene();
	~MainMenuScene();
	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();
};

