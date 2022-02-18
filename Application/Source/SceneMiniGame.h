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

class SceneMiniGame : public Scene
{
private:
	enum MINIGAME_STATE
	{
		EXITCONFIRMATION,
		MAINMENU,
		MINIGAME,
		GAMEOVER,
	};

	enum ITEM_TYPE
	{
		GOLD,
		DYNAMITE
	};

	enum GEOMETRY_TYPE
	{
		GEO_TEXT,
		GEO_QUAD,
		GEO_GOLD,
		GEO_DYNAMITE,
		GEO_HEART,
		GEO_OVER,
		GEO_LOGO,
		NUM_GEOMETRY
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

	float framePerSecond;

	unsigned m_parameters[U_TOTAL];
	unsigned m_vertexArrayID;
	Mesh* meshList[NUM_GEOMETRY];
	unsigned m_programID;

	Camera3 camera;
	char miniGameState;
	unsigned score;
	char health;
	float gameSpeed;
	float timeElapsed;
	float timeElapsedTextToggle;

	float noX, noY, noSizeX, noSizeY, yesX, yesY, yesSizeX, yesSizeY; //variables for button

	//platform variables
	float platformTransformX;;
	float platformTransformY;
	float platformWidth;
	float platformHeight;
	float platformSpeed;

	float initialPlatformSpeed;
	float initialItemSpeed;

	//item variables
	float itemSpawnY;
	float itemSpawnX[5];
	float itemTransformY[5];
	char itemType[5];
	float itemSpawnDelay[5];
	float itemSpeed;


	void RenderMesh(Mesh* mesh, bool enableLight);
	void RenderEntity(Entity* entity, bool enableLight);
	void RenderMeshOnScreen(Mesh* mesh, float x, float y, float sizex, float sizey);
	void RenderText(Mesh* mesh, std::string text, Color color);
	void RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y);
	bool CreateButton(float buttonTop, float buttonBottom, float buttonRight, float buttonLeft);
	void ResetGameVariables();

public:
	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();
};

