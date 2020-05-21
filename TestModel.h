#ifndef TEST_MODEL_CORNEL_BOX_H
#define TEST_MODEL_CORNEL_BOX_H

// Defines a simple test model: The Cornel Box

#include <glm/glm.hpp>
#include <vector>
#include <iostream>
#include "OBJ_Loader.h"

// Used to describe a triangular surface:
class Triangle
{
public:
	glm::vec3 v0;
	glm::vec3 v1;
	glm::vec3 v2;
	glm::vec3 normal;
	glm::vec3 color;

	Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 color)
		: v0(v0), v1(v1), v2(v2), color(color)
	{
		ComputeNormal();
	}

	void ComputeNormal()
	{
		glm::vec3 e1 = v1 - v0;
		glm::vec3 e2 = v2 - v0;
		normal = glm::normalize(glm::cross(e2, e1));
	}
};

// Loads the Cornell Box. It is scaled to fill the volume:
// -1 <= x <= +1
// -1 <= y <= +1
// -1 <= z <= +1
void LoadTestModel(std::vector<Triangle> &triangles)
{
	using glm::vec3;

	// Defines colors:
	vec3 red(0.75f, 0.15f, 0.15f);
	vec3 yellow(0.75f, 0.75f, 0.15f);
	vec3 green(0.15f, 0.75f, 0.15f);
	vec3 cyan(0.15f, 0.75f, 0.75f);
	vec3 blue(0.15f, 0.15f, 0.75f);
	vec3 purple(0.75f, 0.15f, 0.75f);
	vec3 white(0.75f, 0.75f, 0.75f);

	triangles.clear();
	triangles.reserve(5 * 2 * 3);

	// ---------------------------------------------------------------------------
	// Room

	float L = 555; // Length of Cornell Box side.

	vec3 A(L, 0, 0);
	vec3 B(0, 0, 0);
	vec3 C(L, 0, L);
	vec3 D(0, 0, L);

	vec3 E(L, L, 0);
	vec3 F(0, L, 0);
	vec3 G(L, L, L);
	vec3 H(0, L, L);

	// Floor:
	triangles.push_back(Triangle(C, B, A, green));
	triangles.push_back(Triangle(C, D, B, green));

	// Left wall
	triangles.push_back(Triangle(A, E, C, purple));
	triangles.push_back(Triangle(C, E, G, purple));

	// Right wall
	triangles.push_back(Triangle(F, B, D, yellow));
	triangles.push_back(Triangle(H, F, D, yellow));

	// Ceiling
	triangles.push_back(Triangle(E, F, G, cyan));
	triangles.push_back(Triangle(F, H, G, cyan));

	// Back wall
	triangles.push_back(Triangle(G, D, C, white));
	triangles.push_back(Triangle(G, H, D, white));

	// ---------------------------------------------------------------------------
	// Short block

	A = vec3(290, 0, 114);
	B = vec3(130, 0, 65);
	C = vec3(240, 0, 272);
	D = vec3(82, 0, 225);

	E = vec3(290, 165, 114);
	F = vec3(130, 165, 65);
	G = vec3(240, 165, 272);
	H = vec3(82, 165, 225);

	// Front
	triangles.push_back(Triangle(E, B, A, red));
	triangles.push_back(Triangle(E, F, B, red));

	// Front
	triangles.push_back(Triangle(F, D, B, red));
	triangles.push_back(Triangle(F, H, D, red));

	// BACK
	triangles.push_back(Triangle(H, C, D, red));
	triangles.push_back(Triangle(H, G, C, red));

	// LEFT
	triangles.push_back(Triangle(G, E, C, red));
	triangles.push_back(Triangle(E, A, C, red));

	// TOP
	triangles.push_back(Triangle(G, F, E, red));
	triangles.push_back(Triangle(G, H, F, red));

	// ---------------------------------------------------------------------------
	// Tall block

	A = vec3(423, 0, 247);
	B = vec3(265, 0, 296);
	C = vec3(472, 0, 406);
	D = vec3(314, 0, 456);

	E = vec3(423, 330, 247);
	F = vec3(265, 330, 296);
	G = vec3(472, 330, 406);
	H = vec3(314, 330, 456);

	// Front
	triangles.push_back(Triangle(E, B, A, blue));
	triangles.push_back(Triangle(E, F, B, blue));

	// Front
	triangles.push_back(Triangle(F, D, B, blue));
	triangles.push_back(Triangle(F, H, D, blue));

	// BACK
	triangles.push_back(Triangle(H, C, D, blue));
	triangles.push_back(Triangle(H, G, C, blue));

	// LEFT
	triangles.push_back(Triangle(G, E, C, blue));
	triangles.push_back(Triangle(E, A, C, blue));

	// TOP
	triangles.push_back(Triangle(G, F, E, blue));
	triangles.push_back(Triangle(G, H, F, blue));

	// ----------------------------------------------
	// Scale to the volume [-1,1]^3

	for (size_t i = 0; i < triangles.size(); ++i)
	{
		triangles[i].v0 *= 2 / L;
		triangles[i].v1 *= 2 / L;
		triangles[i].v2 *= 2 / L;

		triangles[i].v0 -= vec3(1, 1, 1);
		triangles[i].v1 -= vec3(1, 1, 1);
		triangles[i].v2 -= vec3(1, 1, 1);

		triangles[i].v0.x *= -1;
		triangles[i].v1.x *= -1;
		triangles[i].v2.x *= -1;

		triangles[i].v0.y *= -1;
		triangles[i].v1.y *= -1;
		triangles[i].v2.y *= -1;

		triangles[i].ComputeNormal();
	}
}

void LoadObj(std::vector<Triangle> &triangles)
{
	using glm::vec3;

	// Defines colors:
	vec3 red(0.75f, 0.15f, 0.15f);
	vec3 yellow(0.75f, 0.75f, 0.15f);
	vec3 green(0.15f, 0.75f, 0.15f);
	vec3 cyan(0.15f, 0.75f, 0.75f);
	vec3 blue(0.15f, 0.15f, 0.75f);
	vec3 purple(0.75f, 0.15f, 0.75f);
	vec3 white(0.75f, 0.75f, 0.75f);

	vec3 wall(0.952, 0.549, 0.698);
	vec3 floor(0.890, 0.803, 0.709);

	triangles.clear();
	triangles.reserve(1000000);

	float L = 550; // Length of Cornell Box side.

	vec3 A(L, 0, 0);
	vec3 B(0, 0, 0);
	vec3 C(L, 0, L);
	vec3 D(0, 0, L);

	vec3 E(L, L, 0);
	vec3 F(0, L, 0);
	vec3 G(L, L, L);
	vec3 H(0, L, L);

	//Floor:
	triangles.push_back(Triangle(C, B, A, floor));
	triangles.push_back(Triangle(C, D, B, floor));

	// Left wall
	triangles.push_back(Triangle(A, E, C, wall));
	triangles.push_back(Triangle(C, E, G, wall));

	// Right wall
	triangles.push_back(Triangle(F, B, D, wall));
	triangles.push_back(Triangle(H, F, D, wall));

	// Ceiling
	triangles.push_back(Triangle(E, F, G, wall));
	triangles.push_back(Triangle(F, H, G, wall));

	// Back wall
	triangles.push_back(Triangle(G, D, C, wall));
	triangles.push_back(Triangle(G, H, D, wall));

	objl::Loader loader;
	loader.LoadFile("obj/test.obj");

	// Go through each loaded mesh and out its contents
	for (int i = 0; i < loader.LoadedMeshes.size(); i++)
	{
		// Copy one of the loaded meshes to be our current mesh
		objl::Mesh curMesh = loader.LoadedMeshes[i];

		vec3 A;
		vec3 B;
		vec3 C;

		std::cout << curMesh.Indices.size() << std::endl;
		for (int j = 0; j < curMesh.Indices.size(); j += 3)
		{
			int a = curMesh.Indices[j];
			A = vec3(curMesh.Vertices[a].Position.X+5, curMesh.Vertices[a].Position.Y, curMesh.Vertices[a].Position.Z+2);
			A *= 50.0f;

			int b = curMesh.Indices[j + 1];
			B = vec3(curMesh.Vertices[b].Position.X+5, curMesh.Vertices[b].Position.Y, curMesh.Vertices[b].Position.Z+2);
			B *= 50.0f;

			int c = curMesh.Indices[j + 2];
			C = vec3(curMesh.Vertices[c].Position.X+5, curMesh.Vertices[c].Position.Y, curMesh.Vertices[c].Position.Z+2);
			C *= 50.0f;

			triangles.push_back(Triangle(A, B, C, vec3(0.545,0.721,0.933)));
		}
	}

	for (size_t i = 0; i < triangles.size(); ++i)
	{
		triangles[i].v0 *= 2 / L;
		triangles[i].v1 *= 2 / L;
		triangles[i].v2 *= 2 / L;

		triangles[i].v0 -= vec3(1, 1, 1);
		triangles[i].v1 -= vec3(1, 1, 1);
		triangles[i].v2 -= vec3(1, 1, 1);

		triangles[i].v0.x *= -1;
		triangles[i].v1.x *= -1;
		triangles[i].v2.x *= -1;

		triangles[i].v0.y *= -1;
		triangles[i].v1.y *= -1;
		triangles[i].v2.y *= -1;
		triangles[i].ComputeNormal();

		if(i>9){
			triangles[i].normal = -triangles[i].normal;
		}
	}
}
#endif