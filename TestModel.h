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

// Load the obj file and part up into triangles
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

	vec3 wall(0.952, 0.549, 0.698);	 // Wall color
	vec3 floor(0.890, 0.803, 0.709); // Floor color

	triangles.clear();
	triangles.reserve(1000000);

	float L = 550; // Length of Cornell Box side.

	// Add the cornell box to the scene
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

	//OBJ-loader. Used from https://github.com/Bly7/OBJ-Loader
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

		for (int j = 0; j < curMesh.Indices.size(); j += 3)
		{
			int a = curMesh.Indices[j];
			A = vec3(curMesh.Vertices[a].Position.X + 5, curMesh.Vertices[a].Position.Y, curMesh.Vertices[a].Position.Z + 2);
			A *= 50.0f; // Scaling position of the vertice

			int b = curMesh.Indices[j + 1];
			B = vec3(curMesh.Vertices[b].Position.X + 5, curMesh.Vertices[b].Position.Y, curMesh.Vertices[b].Position.Z + 2);
			B *= 50.0f; // Scaling position of the vertice

			int c = curMesh.Indices[j + 2];
			C = vec3(curMesh.Vertices[c].Position.X + 5, curMesh.Vertices[c].Position.Y, curMesh.Vertices[c].Position.Z + 2);
			C *= 50.0f; // Scaling position of the vertice

			triangles.push_back(Triangle(A, B, C, vec3(0.545, 0.721, 0.933)));
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

		// Change direction of normal of the teapot.
		if (i > 9)
		{
			triangles[i].normal = -triangles[i].normal;
		}
	}
}
#endif