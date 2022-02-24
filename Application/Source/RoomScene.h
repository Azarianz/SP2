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

class RoomScene : public Scene
{
private:
	enum GEOMETRY_TYPE
	{
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
		GEO_GUARD,
		GEO_JANITOR,
		GEO_GAMER,
		GEO_KID,
		GEO_OLDMAN,
		//Room Layouts
		GEO_ROOML,	//Left
		GEO_ROOMR,	//Right
		//Room Based Furniture
		GEO_ROOM1_FURNITURE,	//Left 1
		GEO_ROOM2_FURNITURE,	//Right 1
		GEO_ROOM3_FURNITURE,	//Left 2
		GEO_ROOM4_FURNITURE,	//Right2
		NUM_GEOMETRY,
	};

	enum ENTITY_TYPE 
	{
		ENTITY_DEMENTIA_PILLS,
		ENTITY_ANTIDEPRESSANT_PILLS,
		ENTITY_GUNCASE,
		ENTITY_LAPTOP,
		ENTITY_CREEPYDRAWING,
		NUM_ENTITY
	};

	enum JOURNAL_PAGE
	{
		EVIDENCE_PAGE,
		PROFILE_PAGE
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
		//Light 1 (Bathroom)
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
		//Light 2 (Bedroom)
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
		U_LIGHTENABLED,
		U_NUMLIGHTS,
		U_COLOR_TEXTURE_ENABLED,
		U_COLOR_TEXTURE,
		U_TEXT_ENABLED,
		U_TEXT_COLOR,
		U_TOTAL,
	};

	Light light[2];

	MS modelStack, viewStack, projectionStack;

	unsigned m_parameters[U_TOTAL];
	unsigned m_vertexArrayID;
	Mesh* meshList[NUM_GEOMETRY];
	Entity entityList[NUM_ENTITY];

	unsigned m_programID;

	Camera3 camera;
	bool isJournalOpen;
	char journalPage;
	int evidencePage = 1;
	bool Pickup = false;
	bool Interacted = false;
	bool text = false;
	float rotateX = 0;
	float rotateZ = 0;
	bool Inspect;

	void RenderSkybox();
	void RenderMesh(Mesh* mesh, bool enableLight);
	void RenderEntity(Entity* entity, bool enableLight);
	void RenderMeshOnScreen(Mesh* mesh, float x, float y, float sizex, float sizey);
	void RenderText(Mesh* mesh, std::string text, Color color);
	void RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y);
	void RenderEvidenceObject(Entity* entity, float rangeX, float rangeY);
	void InspectEvidenceOnScreen(Mesh* mesh, float x, float y, float sizex, float sizey, float rotatez, float rotatex);
	void RenderPressEToInteract();
	void RenderHUD();
	void RenderJournal();
	void ResetJournal();
	void PrintEvidence();
	void RoomCollision1();
	void RoomCollision2();
	bool CreateButton(float buttonTop, float buttonBottom, float buttonRight, float buttonLeft);
	bool IsInDoorLInteraction();
	bool IsInDoorRInteraction();

public:
	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();
};

