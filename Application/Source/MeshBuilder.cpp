#include "MeshBuilder.h"
#include <GL\glew.h>
#define BIG_NUMBER 1000.f

/******************************************************************************/
/*!
\brief
Generate the vertices of a reference Axes; Use red for x-axis, green for y-axis, blue for z-axis
Then generate the VBO/IBO and store them in Mesh object

\param meshName - name of mesh
\param lengthX - x-axis should start at -lengthX / 2 and end at lengthX / 2
\param lengthY - y-axis should start at -lengthY / 2 and end at lengthY / 2
\param lengthZ - z-axis should start at -lengthZ / 2 and end at lengthZ / 2

\return Pointer to mesh storing VBO/IBO of reference axes
*/
/******************************************************************************/
Mesh* MeshBuilder::GenerateAxes(const std::string &meshName)
{
	Vertex v;
	std::vector<Vertex> vertex_buffer_data;
	std::vector<unsigned> index_buffer_data;

	//almost the same as GenerateQuad or GenerateCube
	v.pos.Set(0.f, 0.f, 0.f); v.color.Set(1, 0, 0);
	vertex_buffer_data.push_back(v);
	v.pos.Set(BIG_NUMBER, 0.f, 0.f); v.color.Set(1, 0, 0);
	vertex_buffer_data.push_back(v);
	v.pos.Set(0.f, 0.f, 0.f); v.color.Set(0, 1, 0);
	vertex_buffer_data.push_back(v);
	v.pos.Set(0.f, BIG_NUMBER, 0.f); v.color.Set(0, 1, 0);
	vertex_buffer_data.push_back(v);
	v.pos.Set(0.f, 0.f, BIG_NUMBER); v.color.Set(0, 0, 1);
	vertex_buffer_data.push_back(v);
	v.pos.Set(0.f, 0.f, 0.f); v.color.Set(0, 0, 1);
	vertex_buffer_data.push_back(v);


	for (int i = 0; i < 6; i++)
	{
		index_buffer_data.push_back(i);
	}

	Mesh* mesh = new Mesh(meshName);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(Vertex), &vertex_buffer_data[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data.size() * sizeof(GLuint), &index_buffer_data[0], GL_STATIC_DRAW);
	mesh->mode = Mesh::DRAW_LINES;
	mesh->indexSize = index_buffer_data.size();

	return mesh;
}

/******************************************************************************/
/*!
\brief
Generate the vertices of a quad; Use random color for each vertex
Then generate the VBO/IBO and store them in Mesh object

\param meshName - name of mesh
\param lengthX - width of quad
\param lengthY - height of quad

\return Pointer to mesh storing VBO/IBO of quad
*/
/******************************************************************************/
Mesh* MeshBuilder::GenerateQuad(const std::string& meshName, Color color, float size)
{
	Vertex v;
	v.color = color;
	std::vector<Vertex> vertex_buffer_data;
	std::vector<unsigned> index_buffer_data;

	//top right
	v.normal.Set(0.f, 0.f, 1.f);
	v.pos.Set(0.5f, 0.5f, 0.f);
	v.texCoord.Set(1, 1);
	vertex_buffer_data.push_back(v);

	//top left
	v.normal.Set(0.f, 0.f, 1.f);
	v.pos.Set(-0.5f, 0.5f, 0.f); 
	v.texCoord.Set(0, 1);
	vertex_buffer_data.push_back(v);

	//bottom left
	v.normal.Set(0.f, 0.f, 1.f);
	v.pos.Set(-0.5f, -0.5f, 0.f);
	v.texCoord.Set(0, 0);
	vertex_buffer_data.push_back(v);

	//bottom right
	v.normal.Set(0.f, 0.f, 1.f);
	v.pos.Set(0.5f, -0.5f, 0.f); 
	v.texCoord.Set(1, 0);
	vertex_buffer_data.push_back(v);

	//back face
	index_buffer_data.push_back(0);
	index_buffer_data.push_back(1);
	index_buffer_data.push_back(2);
	index_buffer_data.push_back(0);
	index_buffer_data.push_back(2);
	index_buffer_data.push_back(3);

	Mesh *mesh = new Mesh(meshName);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(Vertex), &vertex_buffer_data[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data.size() * sizeof(GLuint), &index_buffer_data[0], GL_STATIC_DRAW);
	mesh->mode = Mesh::DRAW_TRIANGLES;
	mesh->indexSize = index_buffer_data.size();
	return mesh;
}

/******************************************************************************/
/*!
\brief
Generate the vertices of a cube; Use random color for each vertex
Then generate the VBO/IBO and store them in Mesh object

\param meshName - name of mesh
\param lengthX - width of cube
\param lengthY - height of cube
\param lengthZ - depth of cube

\return Pointer to mesh storing VBO/IBO of cube
*/
/******************************************************************************/
Mesh* MeshBuilder::GenerateCube(const std::string& meshName, Color color, float size)
{
	Vertex v;
	v.color = color;
	std::vector<Vertex> vertex_buffer_data;
	std::vector<unsigned> index_buffer_data;

	enum squareVertices
	{
		BACK_TOP_RIGHT,
		BACK_TOP_LEFT,
		BACK_BOTTOM_LEFT,
		BACK_BOTTOM_RIGHT,
		FRONT_TOP_RIGHT,
		FRONT_TOP_LEFT,
		FRONT_BOTTOM_LEFT,
		FRONT_BOTTOM_RIGHT
	};
	//back top right
	v.pos.Set(0.5f, 0.5f, -0.5f);
	v.texCoord.Set(0, 1);
	vertex_buffer_data.push_back(v);
	//back top left
	v.pos.Set(-0.5f, 0.5f, -0.5f);
	v.texCoord.Set(1, 1);
	vertex_buffer_data.push_back(v);
	//back bottom left
	v.pos.Set(-0.5f, -0.5f,- 0.5f);
	v.texCoord.Set(1, 0);
	vertex_buffer_data.push_back(v);
	//back bottom right
	v.pos.Set(0.5f, -0.5f, -0.5f);
	v.texCoord.Set(0, 0);
	vertex_buffer_data.push_back(v);

	//front top right
	v.pos.Set(0.5f, 0.5f, 0.5f);
	v.texCoord.Set(1, 1);
	vertex_buffer_data.push_back(v);
	//front top left
	v.pos.Set(-0.5f, 0.5f, 0.5f); v.color.Set(1, 0, 0);
	v.texCoord.Set(0, 1);
	vertex_buffer_data.push_back(v);
	//front bottom left
	v.pos.Set(-0.5f, -0.5f, 0.5f); v.color.Set(1, 0, 0);
	v.texCoord.Set(0, 0);
	vertex_buffer_data.push_back(v);
	//front bottom right
	v.pos.Set(0.5f, -0.5f, 0.5f); v.color.Set(1, 0, 0);
	v.texCoord.Set(1, 0);
	vertex_buffer_data.push_back(v);

	//back top right
	v.pos.Set(0.5f, 0.5f, -0.5f);
	v.texCoord.Set(1, 1);
	vertex_buffer_data.push_back(v);
	//back top left
	v.pos.Set(-0.5f, 0.5f, -0.5f);
	v.texCoord.Set(0, 1);
	vertex_buffer_data.push_back(v);
	//front top right
	v.pos.Set(0.5f, 0.5f, 0.5f);
	v.texCoord.Set(1, 0);
	vertex_buffer_data.push_back(v);
	//front top left
	v.pos.Set(-0.5f, 0.5f, 0.5f); v.color.Set(1, 0, 0);
	v.texCoord.Set(0, 0);
	vertex_buffer_data.push_back(v);

	//back bottom left
	v.pos.Set(-0.5f, -0.5f, -0.5f);
	v.texCoord.Set(0, 0);
	vertex_buffer_data.push_back(v);
	//back bottom right
	v.pos.Set(0.5f, -0.5f, -0.5f);
	v.texCoord.Set(1, 0);
	vertex_buffer_data.push_back(v);
	//front bottom left
	v.pos.Set(-0.5f, -0.5f, 0.5f); v.color.Set(1, 0, 0);
	v.texCoord.Set(0, 1);
	vertex_buffer_data.push_back(v);
	//front bottom right
	v.pos.Set(0.5f, -0.5f, 0.5f); v.color.Set(1, 0, 0);
	v.texCoord.Set(1, 1);
	vertex_buffer_data.push_back(v);

	//back face
	index_buffer_data.push_back(BACK_BOTTOM_RIGHT);
	index_buffer_data.push_back(BACK_BOTTOM_LEFT);
	index_buffer_data.push_back(BACK_TOP_RIGHT);
	index_buffer_data.push_back(BACK_BOTTOM_LEFT);
	index_buffer_data.push_back(BACK_TOP_LEFT);
	index_buffer_data.push_back(BACK_TOP_RIGHT);
	

	//left face
	index_buffer_data.push_back(FRONT_TOP_LEFT);
	index_buffer_data.push_back(BACK_TOP_LEFT);
	index_buffer_data.push_back(BACK_BOTTOM_LEFT);
	index_buffer_data.push_back(FRONT_TOP_LEFT);
	index_buffer_data.push_back(BACK_BOTTOM_LEFT);
	index_buffer_data.push_back(FRONT_BOTTOM_LEFT);

	//front face
	index_buffer_data.push_back(FRONT_TOP_RIGHT);
	index_buffer_data.push_back(FRONT_TOP_LEFT);
	index_buffer_data.push_back(FRONT_BOTTOM_LEFT);
	index_buffer_data.push_back(FRONT_TOP_RIGHT);
	index_buffer_data.push_back(FRONT_BOTTOM_LEFT);
	index_buffer_data.push_back(FRONT_BOTTOM_RIGHT);

	//bottom face
	index_buffer_data.push_back(12);
	index_buffer_data.push_back(13);
	index_buffer_data.push_back(15);
	index_buffer_data.push_back(12);
	index_buffer_data.push_back(15);
	index_buffer_data.push_back(14);

	//right face
	index_buffer_data.push_back(BACK_TOP_RIGHT);
	index_buffer_data.push_back(FRONT_TOP_RIGHT);
	index_buffer_data.push_back(FRONT_BOTTOM_RIGHT);
	index_buffer_data.push_back(BACK_TOP_RIGHT);
	index_buffer_data.push_back(FRONT_BOTTOM_RIGHT);
	index_buffer_data.push_back(BACK_BOTTOM_RIGHT);

	//top face
	index_buffer_data.push_back(8);
	index_buffer_data.push_back(9);
	index_buffer_data.push_back(11);
	index_buffer_data.push_back(8);
	index_buffer_data.push_back(11);
	index_buffer_data.push_back(10);



	Mesh* mesh = new Mesh(meshName);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(Vertex), &vertex_buffer_data[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data.size() * sizeof(GLuint), &index_buffer_data[0], GL_STATIC_DRAW);
	mesh->mode = Mesh::DRAW_TRIANGLES;
	mesh->indexSize = index_buffer_data.size();
	return mesh;
}

Mesh* MeshBuilder::GenerateCircle(const std::string& meshName, Color color, float radius, int sides, float size)
{
	float degreePerSlice = Math::DegreeToRadian(360.f / sides);
	Vertex v;
	std::vector<Vertex> vertex_buffer_data;
	std::vector<unsigned> index_buffer_data;

	v.pos.Set(0.f, 0.f, 0.f);
	v.color = color;
	vertex_buffer_data.push_back(v);

	for (int i = 0; i <= sides; ++i)
	{
		v.pos.Set(radius * cosf(i * degreePerSlice), 0.f, radius* sinf(i * degreePerSlice));
		v.color = color;
		vertex_buffer_data.push_back(v);
	}

	for (int i = 0; i <= sides + 1; ++i)
	{
		index_buffer_data.push_back(i);
		index_buffer_data.push_back(0);
	}

	Mesh* mesh = new Mesh(meshName);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(Vertex), &vertex_buffer_data[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data.size() * sizeof(GLuint), &index_buffer_data[0], GL_STATIC_DRAW);
	mesh->mode = Mesh::DRAW_TRIANGLE_STRIP;
	mesh->indexSize = index_buffer_data.size();

	return mesh;
}

Mesh* MeshBuilder::GenerateSphere(const std::string& meshName, Color color, unsigned numStacks, unsigned numSlices, float radius)
{
	Vertex v;
	std::vector<Vertex> vertex_buffer_data;
	std::vector<GLuint> index_buffer_data;

	float degreePerStack = 180.f / numStacks;
	float degreePerSlice = 360.f / numSlices;

	for (unsigned stack = 0; stack <= numStacks; ++stack)
	{
		float phi = -90.f + degreePerStack * stack;
		for (unsigned slice = 0; slice <= numSlices; ++slice)
		{
			float theta = degreePerSlice * slice;
			float x = radius * cos(Math::DegreeToRadian(phi)) * cos(Math::DegreeToRadian(theta));
			float y = radius * sin(Math::DegreeToRadian(phi));
			float z = radius * cos(Math::DegreeToRadian(phi)) * sin(Math::DegreeToRadian(theta));
			v.pos.Set(x, y, z);
			v.normal.Set(x,y,z);
			v.color = color;
			if (theta < numStacks / 2)
			{
				if (theta < 180)
				{
					v.texCoord.Set(0, 0);
				}
				else
				{
					v.texCoord.Set(1, 0);
				}
			}
			else
			{
				if (theta < 180)
				{
					v.texCoord.Set(0, 1);
				}
				else
				{
					v.texCoord.Set(1, 1);
				}
			}
			
			vertex_buffer_data.push_back(v);
		}
	}
	for (unsigned stack = 0; stack <= numStacks; ++stack)
	{
		for (unsigned slice = 0; slice <= numSlices; ++slice)
		{
			index_buffer_data.push_back(stack * (numSlices + 1) + slice);
			index_buffer_data.push_back((stack + 1) * (numSlices + 1) + slice);
		}
	}

	Mesh* mesh = new Mesh(meshName);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(Vertex), &vertex_buffer_data[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data.size() * sizeof(GLuint), &index_buffer_data[0], GL_STATIC_DRAW);
	mesh->indexSize = index_buffer_data.size();
	mesh->mode = Mesh::DRAW_TRIANGLE_STRIP;

	return mesh;
}

Mesh* MeshBuilder::GenerateCylinder(const std::string& meshName, Color color, float radius, int sides, float size)
{
	float degreePerSlice = Math::DegreeToRadian(360.f / sides);
	int height = 1;
	int stacks = 1;
	int stackHeight = height / stacks;
	Vertex v;
	std::vector<Vertex> vertex_buffer_data;
	std::vector<unsigned> index_buffer_data;

	//cylinder side
	for (int i = 0; i <= sides; ++i)
	{
		v.pos.Set(radius * cosf(i * degreePerSlice), -height * 0.5f + stacks * stackHeight, radius * sinf(i * degreePerSlice));
		v.normal.Set(cosf(i * degreePerSlice), 0.f, sinf(i * degreePerSlice));
		v.color = color;
		vertex_buffer_data.push_back(v);

		v.pos.Set(radius * cosf(i * degreePerSlice), -height * 0.5f + (stacks + 1) * stackHeight, radius * sinf(i * degreePerSlice));
		v.normal.Set(cosf(i * degreePerSlice), 0.f, sinf(i * degreePerSlice));
		v.color = color;
		vertex_buffer_data.push_back(v);
	}

	for (int i = 0; i <= sides * 2 + 1; ++i)
	{
		index_buffer_data.push_back(i);
	}

	Mesh* mesh = new Mesh(meshName);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(Vertex), &vertex_buffer_data[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data.size() * sizeof(GLuint), &index_buffer_data[0], GL_STATIC_DRAW);
	mesh->mode = Mesh::DRAW_TRIANGLE_STRIP;
	mesh->indexSize = index_buffer_data.size();

	return mesh;
}

Mesh* MeshBuilder::GenerateCone(const std::string& meshName, Color color, float radius, int sides, int height, float size)
{
	float degreePerSlice = Math::DegreeToRadian(360.f / sides);
	Vertex v;
	std::vector<Vertex> vertex_buffer_data;
	std::vector<unsigned> index_buffer_data;

	v.pos.Set(0, height * 0.5f, 0);
	v.color = color;
	vertex_buffer_data.push_back(v);

	v.pos.Set(0, -height * 0.5f, 0);
	v.normal.Set(0, -1, 0);
	v.color = color;
	vertex_buffer_data.push_back(v);

	for (int i = 0; i <= sides; ++i)
	{
		v.pos.Set(radius * cosf(i * degreePerSlice), -height * 0.5, radius * sinf(i * degreePerSlice));
		v.normal.Set(0, -1, 0);
		v.color = color;
		vertex_buffer_data.push_back(v);
	}

	for (int i = 1; i <= sides + 2; ++i)
	{
		index_buffer_data.push_back(1);
		index_buffer_data.push_back(i);
	}

	for (int i = 0; i <= sides; ++i)
	{
		v.pos.Set(radius * cosf(i * degreePerSlice), -height* 0.5, radius * sinf(i * degreePerSlice));
		v.normal.Set(height * cosf(i * degreePerSlice), radius, height * sinf(i * degreePerSlice));
		v.normal.Normalize();
		v.color = color;
		vertex_buffer_data.push_back(v);
	}

	for (int i = sides + 2; i <= sides * 2 + 3; ++i)
	{
		index_buffer_data.push_back(i);
		index_buffer_data.push_back(0);
	}

	

	Mesh* mesh = new Mesh(meshName);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(Vertex), &vertex_buffer_data[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data.size() * sizeof(GLuint), &index_buffer_data[0], GL_STATIC_DRAW);
	mesh->mode = Mesh::DRAW_TRIANGLE_STRIP;
	mesh->indexSize = index_buffer_data.size();

	return mesh;
}

Mesh* MeshBuilder::GenerateTorus(const std::string& meshName, Color color, unsigned numStacks, unsigned numSlices, float outerRadius, float innerRadius)
{
	Vertex v;
	std::vector<Vertex> vertex_buffer_data;
	std::vector<GLuint> index_buffer_data;

	float degreePerStack = 360.f / numStacks;
	float degreePerSlice = 360.f / numSlices;

	float x1, z1;
	float x2, y2, z2;
	for (unsigned stack = 0; stack <= numStacks; ++stack)
	{
		float phi = -90.f + degreePerStack * stack;
		for (unsigned slice = 0; slice <= numSlices; ++slice)
		{
			z1 = outerRadius * cosf(Math::DegreeToRadian(stack * degreePerStack));
			x1 = outerRadius * sinf(Math::DegreeToRadian(stack * degreePerStack));
			z2 = (outerRadius + innerRadius * cosf(Math::DegreeToRadian(slice * degreePerSlice))) * cosf(Math::DegreeToRadian(stack * degreePerStack));
			y2 = innerRadius * sin(Math::DegreeToRadian(slice * degreePerSlice));
			x2 = (outerRadius + innerRadius * cosf(Math::DegreeToRadian(slice * degreePerSlice))) * sinf(Math::DegreeToRadian(stack * degreePerStack));

			v.pos.Set(x2, y2, z2);
			v.color = color;
			v.normal.Set(x2 - x1, y2, z2 - z1);
			v.normal.Normalize();
			vertex_buffer_data.push_back(v);
		}
	}
	for (unsigned stack = 0; stack <= numStacks; ++stack)
	{
		for (unsigned slice = 0; slice <= numSlices; ++slice)
		{
			index_buffer_data.push_back(stack * (numSlices + 1) + slice);
			index_buffer_data.push_back((stack + 1) * (numSlices + 1) + slice);
		}
	}

	Mesh* mesh = new Mesh(meshName);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(Vertex), &vertex_buffer_data[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data.size() * sizeof(GLuint), &index_buffer_data[0], GL_STATIC_DRAW);
	mesh->indexSize = index_buffer_data.size();
	mesh->mode = Mesh::DRAW_TRIANGLE_STRIP;

	return mesh;
}

Mesh* MeshBuilder::GenerateOBJ(const std::string& meshName, const std::string& file_path)
{
	//Read vertices, texcoords & normals from OBJ
	std::vector<Position> vertices;
	std::vector<TexCoord> uvs;
	std::vector<Vector3> normals;
	bool success = LoadOBJ(file_path.c_str(), vertices, uvs, normals);
	if (!success)
		return NULL;

	//Index the vertices, texcoords & normals properly
	std::vector<Vertex> vertex_buffer_data;
	std::vector<GLuint> index_buffer_data;
	IndexVBO(vertices, uvs, normals, index_buffer_data, vertex_buffer_data);

	Mesh* mesh = new Mesh(meshName);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(Vertex), &vertex_buffer_data[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data.size() * sizeof(GLuint), &index_buffer_data[0], GL_STATIC_DRAW);
	mesh->indexSize = index_buffer_data.size();
	mesh->mode = Mesh::DRAW_TRIANGLES;

	return mesh;
}

Mesh* MeshBuilder::GenerateOBJMTL(const std::string& meshName, const std::string& file_path, const std::string& mtl_path)
{
	//Read vertices, texcoords & normals from OBJ
	std::vector<Position> vertices;
	std::vector<TexCoord> uvs;

	std::vector<Vector3> normals;
	std::vector<Material> materials;
	bool success = LoadOBJMTL(file_path.c_str(), mtl_path.c_str(),
		vertices, uvs, normals, materials);
	if (!success)
		return NULL;
	//Index the vertices, texcoords & normals properly
	std::vector<Vertex> vertex_buffer_data;
	std::vector<GLuint> index_buffer_data;
	IndexVBO(vertices, uvs, normals, index_buffer_data,
		vertex_buffer_data);
	Mesh* mesh = new Mesh(meshName);
	for (Material& material : materials)
		mesh->materials.push_back(material);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() *
		sizeof(Vertex), &vertex_buffer_data[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data.size()
		* sizeof(GLuint), &index_buffer_data[0], GL_STATIC_DRAW);
	mesh->indexSize = index_buffer_data.size();
	mesh->mode = Mesh::DRAW_TRIANGLES;
	return mesh;
}

Mesh* MeshBuilder::GenerateText(const std::string& meshName, unsigned numRow, unsigned numCol)
{
	Vertex v;
	std::vector<Vertex> vertex_buffer_data;
	std::vector<unsigned> index_buffer_data;

	float width = 1.f / numCol;
	float height = 1.f / numRow;
	unsigned offset = 0;
	for (unsigned row = 0; row < numRow; ++row)
	{
		for (unsigned col = 0; col < numCol; ++col)
		{
			//top right
			v.normal.Set(0.f, 0.f, 1.f);
			v.pos.Set(0.5f, 0.5f, 0.f);
			v.texCoord.Set(width * (col + 1), height * (1 + (numRow - row - 1)));
			vertex_buffer_data.push_back(v);

			//top left
			v.normal.Set(0.f, 0.f, 1.f);
			v.pos.Set(-0.5f, 0.5f, 0.f);
			v.texCoord.Set(width * col, height * (1 + (numRow - row - 1)));
			vertex_buffer_data.push_back(v);

			//bottom left
			v.normal.Set(0.f, 0.f, 1.f);
			v.pos.Set(-0.5f, -0.5f, 0.f);
			v.texCoord.Set(width * col, height * (numRow - row - 1));
			vertex_buffer_data.push_back(v);

			//bottom right
			v.normal.Set(0.f, 0.f, 1.f);
			v.pos.Set(0.5f, -0.5f, 0.f);
			v.texCoord.Set(width * (1 + col), height * (numRow - row - 1));
			vertex_buffer_data.push_back(v);
			
			index_buffer_data.push_back(0 + offset);
			index_buffer_data.push_back(1 + offset);
			index_buffer_data.push_back(2 + offset);
			index_buffer_data.push_back(0 + offset);
			index_buffer_data.push_back(2 + offset);
			index_buffer_data.push_back(3 + offset);

			offset += 4;
		}
	}

	//back face
	index_buffer_data.push_back(0);
	index_buffer_data.push_back(1);
	index_buffer_data.push_back(2);
	index_buffer_data.push_back(0);
	index_buffer_data.push_back(2);
	index_buffer_data.push_back(3);


	Mesh* mesh = new Mesh(meshName);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(Vertex), &vertex_buffer_data[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data.size() * sizeof(GLuint), &index_buffer_data[0], GL_STATIC_DRAW);
	mesh->mode = Mesh::DRAW_TRIANGLES;
	mesh->indexSize = index_buffer_data.size();
	return mesh;
}


