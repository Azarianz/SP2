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
#include <iomanip> 
#include <sstream>
#include <fstream>
#include <vector>

class LobbyScene : public Scene
{
private:
	enum GEOMETRY_TYPE
	{
		GEO_AXES,
		GEO_QUAD,
		GEO_QUAD_BUTTON,
		GEO_SPHERE,
		GEO_SUN,
		GEO_LEFT,
		GEO_RIGHT,
		GEO_TOP,
		GEO_BOTTOM,
		GEO_FRONT,
		GEO_BACK,
		GEO_TEXT,
		GEO_JANITOR,
		GEO_GAMER,
		GEO_LOBBY,
		GEO_TABLES,
		GEO_DIALOGUE,
		GEO_DIALOGUE2,
		NUM_GEOMETRY,
	};

	enum JOURNAL_PAGE
	{
		EVIDENCE_PAGE,
		PROFILE_PAGE
	};

	enum ENTITY_TYPE
	{
		ENTITY_GUARD,
		ENTITY_JANITOR,
		ENTITY_GAMER,
		ENTITY_KID,
		ENTITY_OLDMAN,
		ENTITY_MACHINE,
		NUM_ENTITY
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
		U_LIGHT0_POSITION,
		U_LIGHT0_COLOR,
		U_LIGHT0_POWER,
		U_LIGHT0_KC,
		U_LIGHT0_KL,
		U_LIGHT0_KQ,
		U_LIGHTENABLED,
		U_LIGHT0_TYPE,
		U_LIGHT0_SPOTDIRECTION,
		U_LIGHT0_COSCUTOFF,
		U_LIGHT0_COSINNER,
		U_LIGHT0_EXPONENT,
		U_NUMLIGHTS,
		U_COLOR_TEXTURE_ENABLED,
		U_COLOR_TEXTURE,
		U_TEXT_ENABLED,
		U_TEXT_COLOR,
		U_TOTAL,
	};

	Light light[1];

	MS modelStack, viewStack, projectionStack;

	float framePerSecond;

	int interval;
	int charId; // 0 = guard, 1 = janitor, 2 = Arcader, 3 = Kid, 4 = old guy
	int gossipId; // 0 = guard, 1 = janitor, 2 = Arcader, 3 = Kid, 4 = old guy
	bool canInteract;
	int interactOffset;
	bool isTalking;
	bool isChatting;
	bool isDoneChat;
	int chatCounter = 0;
	int tempCounter = 0;
	bool isInterrogate;
	bool isGossiping;
	bool printGossip;
	std::ostringstream ss;
	std::string screenTxt;
	std::vector <std::string> guardChat;
	std::vector <std::string> janitorChat;
	std::vector <std::string> gamerChat;
	std::vector <std::string> kidChat;
	std::vector <std::string> oldManChat;

	unsigned m_parameters[U_TOTAL];
	unsigned m_vertexArrayID;
	Mesh* meshList[NUM_GEOMETRY];
	Entity entityList[NUM_ENTITY];

	unsigned m_programID;

	Camera3 camera;

	bool isJournalOpen;
	char journalPage;
	float rotateSkybox;
	int evidencePage = 1;

	void RenderMesh(Mesh* mesh, bool enableLight);
	void RenderEntity(Entity* entity, bool enableLight);
	void RenderMeshOnScreen(Mesh* mesh, float x, float y, float sizex, float sizey);
	void RenderText(Mesh* mesh, std::string text, Color color);
	void RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y);
	void RenderSkybox();
	bool CreateButton(float buttonTop, float buttonBottom, float buttonRight, float buttonLeft);
	void RenderPressEToInteract();
	void RenderJournal();
	void ResetJournal();
	void PrintEvidence();
	void ChatDialogueInit(std::string, std::vector<std::string>& vec);
	void RenderInteraction();
	void Interaction();
	void TalkButtons();
	void CharacterPosCheck();
	void BoundsCheck();
	bool IsInArcadeMachineInteraction();
	bool IsInElevatorInteraction();

public:
	LobbyScene();
	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();
};

