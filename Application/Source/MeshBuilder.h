#ifndef MESH_BUILDER_H
#define MESH_BUILDER_H

#include "Mesh.h"
#include "Vertex.h"
#include "MyMath.h"
#include <vector>
#include "loadOBJ.h"

/******************************************************************************/
/*!
		Class MeshBuilder:
\brief	Provides methods to generate mesh of different shapes
*/
/******************************************************************************/
class MeshBuilder
{
public:
	static Mesh* GenerateAxes(const std::string &meshName);
	static Mesh* GenerateQuad(const std::string& meshName, Color color, float size = 1.f);
	static Mesh* GenerateCube(const std::string& meshName, Color color, float size = 1.f);
	static Mesh* GenerateCircle(const std::string& meshName, Color color, float radius, int sides, float size = 1.f);
	static Mesh* GenerateSphere(const std::string& meshName, Color color, unsigned numStacks, unsigned numSlices, float radius);
	static Mesh* GenerateCylinder(const std::string& meshName, Color color, float radius, int sides, float size = 1.f);
	static Mesh* GenerateCone(const std::string& meshName, Color color, float radius, int sides, int height, float size = 1.f);
	static Mesh* GenerateTorus(const std::string& meshName, Color color, unsigned numStacks, unsigned numSlices, float outerRadius, float innerRadius);
	static Mesh* GenerateOBJ(const std::string& meshName, const std::string& file_path);
	static Mesh* GenerateOBJMTL(const std::string& meshName, const std::string& file_path, const std::string& mtl_path);
	static Mesh* GenerateText(const std::string& meshName, unsigned numRow, unsigned numCol);
};

#endif