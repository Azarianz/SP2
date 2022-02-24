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

class CorridorScene : public Scene
{
private:
	enum GEOMETRY_TYPE
	{
		GEO_QUAD,
		GEO_QUAD_BUTTON,
		GEO_LEFT,
		GEO_RIGHT,
		GEO_TOP,
		GEO_BOTTOM,
		GEO_FRONT,
		GEO_BACK,
		GEO_TEXT,
		GEO_JOURNAL_PAGE1,
		GEO_JOURNAL_PAGE2,
		//Corridor Stage + Assets
		GEO_CORRIDOR,
		GEO_OFFICER_M,
		GEO_OFFICER_F,
		NUM_GEOMETRY,
	};

	enum ENTITY_TYPE 
	{
		ENTITY_CLEANER_CART,
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
	float rotateSkybox;

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
	void RenderJournal();
	void ResetJournal();
	void RenderHUD();
	void PrintEvidence();
	void BoundsCheck();
	void RenderOfficers();
	bool CreateButton(float buttonTop, float buttonBottom, float buttonRight, float buttonLeft);
	bool IsInElevatorInteraction();
	bool IsInDoor1Interaction();
	bool IsInDoor2Interaction();
	bool IsInDoor3Interaction();
	bool IsInDoor4Interaction();
public:
	CorridorScene();
	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();
};

