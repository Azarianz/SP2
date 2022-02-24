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
		GEO_SUN,
		GEO_LEFT,
		GEO_RIGHT,
		GEO_TOP,
		GEO_BOTTOM,
		GEO_FRONT,
		GEO_BACK,
		GEO_JOURNAL_PAGE1,
		GEO_JOURNAL_PAGE2,
		GEO_TEXT,
		GEO_JANITOR,
		GEO_GAMER,
		GEO_LOBBY,
		GEO_TABLES,
		GEO_DIALOGUE,
		GEO_DIALOGUE2,
		GEO_OFFICERS,
		NUM_GEOMETRY,
	};

	enum JOURNAL_PAGE
	{
		EVIDENCE_PAGE,
		PROFILE_PAGE
	};

	enum ENTITY_TYPE
	{
		//Characters
		ENTITY_GUARD,
		ENTITY_JANITOR,
		ENTITY_GAMER,
		ENTITY_KID,
		ENTITY_OLDMAN,
		//Minigame
		ENTITY_MACHINE,
		//Evidence
		ENTITY_NOTES,
		ENTITY_ALCHOHOL_BOTTLE,
		ENTITY_BOTTLEMIX,
		ENTITY_KNIFE,
		ENTITY_PSYCHO_PILLS,
		ENTITY_CHIEF,
		NUM_ENTITY
	};

	enum UNIFORM_TYPE
	{
		//Models
		U_MVP = 0,
		U_MODELVIEW,
		U_MODELVIEW_INVERSE_TRANSPOSE,
		U_MATERIAL_AMBIENT,
		U_MATERIAL_DIFFUSE,
		U_MATERIAL_SPECULAR,
		U_MATERIAL_SHININESS,
		//Lights 1 (Window)
		U_LIGHT0_POSITION,
		U_LIGHT0_COLOR,
		U_LIGHT0_POWER,
		U_LIGHT0_KC,
		U_LIGHT0_KL,
		U_LIGHT0_KQ,
		U_LIGHT0_TYPE,
		U_LIGHT0_SPOTDIRECTION,
		U_LIGHT0_COSCUTOFF,
		U_LIGHT0_COSINNER,
		U_LIGHT0_EXPONENT,
		//Lights 2 (Bar)
		U_LIGHT1_POSITION,
		U_LIGHT1_COLOR,
		U_LIGHT1_POWER,
		U_LIGHT1_KC,
		U_LIGHT1_KL,
		U_LIGHT1_KQ,
		U_LIGHT1_TYPE,
		U_LIGHT1_SPOTDIRECTION,
		U_LIGHT1_COSCUTOFF,
		U_LIGHT1_COSINNER,
		U_LIGHT1_EXPONENT,
		//Lights 3 (Arcade)
		U_LIGHT2_POSITION,
		U_LIGHT2_COLOR,
		U_LIGHT2_POWER,
		U_LIGHT2_KC,
		U_LIGHT2_KL,
		U_LIGHT2_KQ,
		U_LIGHT2_TYPE,
		U_LIGHT2_SPOTDIRECTION,
		U_LIGHT2_COSCUTOFF,
		U_LIGHT2_COSINNER,
		U_LIGHT2_EXPONENT,
		//Lights 4 (Lift)
		U_LIGHT3_POSITION,
		U_LIGHT3_COLOR,
		U_LIGHT3_POWER,
		U_LIGHT3_KC,
		U_LIGHT3_KL,
		U_LIGHT3_KQ,
		U_LIGHT3_TYPE,
		U_LIGHT3_SPOTDIRECTION,
		U_LIGHT3_COSCUTOFF,
		U_LIGHT3_COSINNER,
		U_LIGHT3_EXPONENT,
		//Others
		U_LIGHTENABLED,
		U_NUMLIGHTS,
		U_COLOR_TEXTURE_ENABLED,
		U_COLOR_TEXTURE,
		U_TEXT_ENABLED,
		U_TEXT_COLOR,
		U_TOTAL,
	};

	Light light[4];

	MS modelStack, viewStack, projectionStack;

	float framePerSecond;

	int interval;
	int charId; // 0 = guard, 1 = janitor, 2 = Arcader, 3 = Kid, 4 = old guy
	int gossipId; // 0 = guard, 1 = janitor, 2 = Arcader, 3 = Kid, 4 = old guy
	int interrogateId; // 0 = guard, 1 = janitor, 2 = Arcader, 3 = Kid, 4 = old guy
	int interactOffset;
	bool canInteract;
	bool isTalking, isChatting, isAbleToTalk, isAbleToPin;
	int chatCounter = 0;
	int tempCounter = 0;
	bool isInterrogate;
	bool isGossiping;
	bool printGossip, printInterrogate, haveEvidence;
	std::ostringstream ss;
	std::string screenTxt, culpritText;
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
	bool Pickup = false;
	bool Interacted = false;
	bool text = false;
	float rotateX = 0;
	float rotateZ = 0;
	bool Inspect;

	void RenderMesh(Mesh* mesh, bool enableLight);
	void RenderEntity(Entity* entity, bool enableLight);
	void RenderMeshOnScreen(Mesh* mesh, float x, float y, float sizex, float sizey);
	void RenderText(Mesh* mesh, std::string text, Color color);
	void RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y);
	void RenderEvidenceObject(Entity* entity, float rangeX, float rangeY);
	void InspectEvidenceOnScreen(Mesh* mesh, float x, float y, float sizex, float sizey, float rotatez, float rotatex);
	void RenderSkybox();
	bool CreateButton(float buttonTop, float buttonBottom, float buttonRight, float buttonLeft);
	void RenderPressToInteract(char btn, std::string input, float x, float y);
	void RenderOfficers();
	void RenderJournal();
	void RenderHUD();
	void ResetJournal();
	void PrintEvidence();	//for journal
	void ChatDialogueInit(std::string, std::vector<std::string>& vec);
	void RenderInteraction();
	void Interaction();
	void TalkButtons();
	void CharacterPosCheck();
	void BoundsCheck();
	bool IsInArcadeMachineInteraction();
	bool IsInElevatorInteraction();
	bool declareCulprit(int id);

public:
	LobbyScene();
	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();
};

