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
		GEO_AXES,
		GEO_QUAD,
		GEO_SPHERE,
		GEO_SUN,
		GEO_LEFT,
		GEO_RIGHT,
		GEO_TOP,
		GEO_BOTTOM,
		GEO_FRONT,
		GEO_BACK,
		GEO_TEXT,
		GEO_GUARD,
		GEO_JANITOR,
		GEO_GAMER,
		GEO_KID,
		GEO_OLDMAN,
		//Corridor Stage + Assets
		GEO_CORRIDOR,
		NUM_GEOMETRY,
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

	unsigned m_parameters[U_TOTAL];
	unsigned m_vertexArrayID;
	Mesh* meshList[NUM_GEOMETRY];

	//unsigned m_indexBuffer [Num_Geometry];
	//unsigned m_vertexBuffer[Num_Geometry];
	//unsigned m_colorBuffer[Num_Geometry];
	unsigned m_programID;

	Camera3 camera;

	void RenderSkybox();
	void RenderMesh(Mesh* mesh, bool enableLight);
	void RenderEntity(Entity* entity, bool enableLight);
	void RenderMeshOnScreen(Mesh* mesh, float x, float y, float sizex, float sizey);
	void RenderText(Mesh* mesh, std::string text, Color color);
	void RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y);
	bool CreateButton(float buttonTop, float buttonBottom, float buttonRight, float buttonLeft);

public:
	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();
};

