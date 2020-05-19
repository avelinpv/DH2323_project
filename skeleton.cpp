#include <iostream>
#include <glm/glm.hpp>
#include <SDL.h>
#include "SDLauxiliary.h"
#include "TestModel.h"
#include "OBJ_Loader.h"

#include <math.h>
#include <cmath>
#include <stdlib.h>
#include <iostream>

using namespace std;
using glm::ivec2;
using glm::mat3;
using glm::vec2;
using glm::vec3;

// ----------------------------------------------------------------------------
// GLOBAL VARIABLES

const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 500;
SDL_Surface *screen;
int t;
vector<Triangle> triangles;
vec3 cameraPos(0, 0, -3.001f);
const int focalLength = SCREEN_HEIGHT;
mat3 R;
float yaw = 0; // Yaw angle controlling camera rotation around y-axis
float moveFrac = 0.01f;
vec3 presentColor; // Color of current pixel
float depthBuffer[SCREEN_HEIGHT][SCREEN_WIDTH];
float shadowMap[SCREEN_HEIGHT][SCREEN_WIDTH];
vec3 lightPos(0, -0.5, -0.7);
vec3 lightPower = 7.1f * vec3(1, 1, 1);
vec3 indirectLightPowerPerArea = 0.5f * vec3(1, 1, 1);
vec3 currentNormal;
vec3 currentReflectance;

// ----------------------------------------------------------------------------
// STRUCT

struct Pixel
{
	int x;
	int y;
	float zinv;
	float shadowInv;
	vec3 pos3d;
};

struct Vertex
{
	vec3 position;
};

// ----------------------------------------------------------------------------
// FUNCTIONS

void Update();
void Draw();
void VertexShader(const Vertex &v, Pixel &p);						  // Pixel
void Interpolate(Pixel a, Pixel b, vector<Pixel> &result);			  // Pixel
void DrawLineSDL(SDL_Surface *surface, Pixel a, Pixel b, vec3 color); // Pixel
void DrawPolygonEdges(const vector<vec3> &vertices);
void ComputePolygonRows(const vector<Pixel> &vertexPixels, vector<Pixel> &leftPixels, vector<Pixel> &rightPixels);
void DrawRows(const vector<Pixel> &leftPixels, const vector<Pixel> &rightPixels); // Pixel
void DrawPolygon(const vector<Vertex> &vertices);
void PixelShader(const Pixel &p);

// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
	LoadObj(triangles);

	screen = InitializeSDL(SCREEN_WIDTH, SCREEN_HEIGHT);
	t = SDL_GetTicks(); // Set start value for timer.

	while (NoQuitMessageSDL())
	{
		Update();
		Draw();
	}

	SDL_SaveBMP(screen, "screenshot2.bmp"); //Take screenshot
	return 0;
}

void Update()
{
	// Compute frame time:
	int t2 = SDL_GetTicks();
	float dt = float(t2 - t);
	t = t2;
	cout << "Render time: " << dt << " ms." << endl;

	Uint8 *keystate = SDL_GetKeyState(0);

	vec3 right(R[0][0], R[0][1], R[0][2]);
	vec3 down(R[1][0], R[1][1], R[1][2]);
	vec3 forward(R[2][0], R[2][1], R[2][2]);

	if (keystate[SDLK_UP])
	{
		cameraPos += forward * moveFrac;
	}

	if (keystate[SDLK_DOWN])
	{
		cameraPos -= forward * moveFrac;
	}

	if (keystate[SDLK_RIGHT])
	{
		yaw -= moveFrac;
		R = mat3(
			cos(yaw), 0, sin(yaw),
			0, 1, 0,
			-sin(yaw), 0, cos(yaw));
	}

	if (keystate[SDLK_LEFT])
	{
		yaw += moveFrac;
		R = mat3(
			cos(yaw), 0, sin(yaw),
			0, 1, 0,
			-sin(yaw), 0, cos(yaw));
	}

	if (keystate[SDLK_RSHIFT])
	{
	}

	if (keystate[SDLK_RCTRL])
	{
	}

	if (keystate[SDLK_w])
	{
		cameraPos += forward * moveFrac;
	}

	if (keystate[SDLK_s])
	{
		cameraPos -= forward * moveFrac;
	}

	if (keystate[SDLK_d])
	{
		yaw -= moveFrac;
		R = mat3(
			cos(yaw), 0, sin(yaw),
			0, 1, 0,
			-sin(yaw), 0, cos(yaw));
	}

	if (keystate[SDLK_a])
	{
		yaw += moveFrac;
		R = mat3(
			cos(yaw), 0, sin(yaw),
			0, 1, 0,
			-sin(yaw), 0, cos(yaw));
	}

	if (keystate[SDLK_e])
	{
		cameraPos += down * moveFrac;
	}

	if (keystate[SDLK_q])
	{
		cameraPos -= down * moveFrac;
	}
}

void Draw()
{
	SDL_FillRect(screen, 0, 0);
	if (SDL_MUSTLOCK(screen))
		SDL_LockSurface(screen);

	for (int y = 0; y < SCREEN_HEIGHT; ++y)
	{
		for (int x = 0; x < SCREEN_WIDTH; ++x)
		{
			depthBuffer[y][x] = 0;
			shadowMap[y][x] = 0;
		}
	}

	for (int i = 0; i < triangles.size(); ++i)
	{
		presentColor = triangles[i].color;
		currentNormal = triangles[i].normal;
		currentReflectance = triangles[i].color;
		vector<Vertex> vertices(3);
		vertices[0].position = triangles[i].v0;
		vertices[1].position = triangles[i].v1;
		vertices[2].position = triangles[i].v2;
		DrawPolygon(vertices);
	}

	if (SDL_MUSTLOCK(screen))
		SDL_UnlockSurface(screen);

	SDL_UpdateRect(screen, 0, 0, 0, 0);
}

//Pixel
void VertexShader(const Vertex &v, Pixel &p)
{
	vec3 vPrime = (v.position - cameraPos) * R;
	vec3 vLight = (v.position - lightPos);

	p.zinv = 1.0f / vPrime.z;
	p.shadowInv = 1.0f / vLight.z;
	p.x = focalLength * (vPrime.x / vPrime.z) + (SCREEN_WIDTH / 2);
	p.y = focalLength * (vPrime.y / vPrime.z) + (SCREEN_HEIGHT / 2);
	p.pos3d = v.position;
}

void PixelShader(const Pixel &p)
{
	int x = p.x;
	int y = p.y;

	if (p.shadowInv > shadowMap[x][y])
	{
		shadowMap[x][y] = p.shadowInv;
		vec3 shadowColor = vec3(0); //presentColor + 0.5f*(vec3(0) - presentColor);
		PutPixelSDL(screen, x, y, shadowColor);
	}

	if (p.zinv > depthBuffer[x][y])
	{
		depthBuffer[x][y] = p.zinv;
		vec3 rVec = glm::normalize(lightPos - p.pos3d);
		float r = glm::length(lightPos - p.pos3d);
		vec3 n = currentNormal;
		float area = 4 * M_PI * r * r;

		vec3 D = lightPower * max(glm::dot(rVec, n), 0.0f) / area;
		vec3 R = currentReflectance * (D + indirectLightPowerPerArea);

		PutPixelSDL(screen, x, y, R);
	}
	// else if (p.zinv < depthBuffer[x][y])
	// {
	// 	vec3 shadowColor = presentColor + 0.5f*(vec3(0) - presentColor);
	// 	PutPixelSDL(screen, x, y, shadowColor);
	// }
}

void Interpolate(Pixel a, Pixel b, vector<Pixel> &result)
{
	int N = result.size();
	float xStep = (b.x - a.x) / float(glm::max(N - 1, 1));
	float yStep = (b.y - a.y) / float(glm::max(N - 1, 1));
	float zStep = (b.zinv - a.zinv) / float(glm::max(N - 1, 1));

	b.pos3d = (b.pos3d * b.zinv);
	a.pos3d = (a.pos3d * a.zinv);
	vec3 posStep = vec3(b.pos3d - a.pos3d) / float(glm::max(N - 1, 1));

	for (int i = 0; i < N; i++)
	{
		result[i].x = a.x + i * xStep;
		result[i].y = a.y + i * yStep;
		result[i].zinv = a.zinv + i * zStep;
		result[i].pos3d = a.pos3d + float(i) * posStep;
		result[i].pos3d /= result[i].zinv;
	}
}

void DrawLineSDL(SDL_Surface *surface, Pixel a, Pixel b, vec3 color)
{
	Pixel delta;
	delta.x = glm::abs(a.x - b.x);
	delta.y = glm::abs(a.y - b.y);
	int pixels = glm::max(delta.x, delta.y) + 1;
	vector<Pixel> line(pixels);
	Interpolate(a, b, line);
	for (int i = 0; i < line.size(); i++)
	{
		PixelShader(line[i]);
	}
}

void ComputePolygonRows(const vector<Pixel> &vertexPixels, vector<Pixel> &leftPixels, vector<Pixel> &rightPixels)
{
	// 1. Find max and min y-value of the polygon
	// and compute the number of rows it occupies.
	int ymin = numeric_limits<int>::max();
	int ymax = -numeric_limits<int>::max();
	for (int i = 0; i < vertexPixels.size(); i++)
	{
		ymin = glm::min(vertexPixels[i].y, ymin);
		ymax = glm::max(vertexPixels[i].y, ymax);
	}

	int rows = ymax - ymin + 1;

	// 2. Resize leftPixels and rightPixels
	// so that they have an element for each row.
	leftPixels.resize(rows);
	rightPixels.resize(rows);

	// 3. Initialize the x-coordinates in leftPixels
	// to some really large value and the x-coordinates
	// in rightPixels to some really small value.
	for (int i = 0; i < rows; ++i)
	{
		leftPixels[i].x += numeric_limits<int>::max();
		rightPixels[i].x -= numeric_limits<int>::max();
	}

	// 4. Loop through all edges of the polygon and use
	// linear interpolation to find the x-coordinate for
	// each row it occupies. Update the corresponding
	// values in rightPixels and leftPixels.
	for (int i = 0; i < vertexPixels.size(); i++)
	{
		int next = (i + 1) % vertexPixels.size();
		int ydif = abs(vertexPixels[i].y - vertexPixels[next].y) + 1;
		vector<Pixel> line(ydif);
		Interpolate(vertexPixels[i], vertexPixels[next], line);

		for (int k = 0; k < line.size(); k++)
		{
			int index = line[k].y - ymin;
			if (leftPixels[index].x > line[k].x)
			{
				leftPixels[index] = line[k];
			}
			if (rightPixels[index].x < line[k].x)
			{
				rightPixels[index] = line[k];
			}
		}
	}
}

void DrawRows(const vector<Pixel> &leftPixels, const vector<Pixel> &rightPixels)
{
	for (int i = 0; i < leftPixels.size(); i++)
	{
		DrawLineSDL(screen, leftPixels[i], rightPixels[i], presentColor);
	}
}

void DrawPolygon(const vector<Vertex> &vertices)
{
	int V = vertices.size();
	vector<Pixel> vertexPixels(V);
	for (int i = 0; i < V; ++i)
	{
		VertexShader(vertices[i], vertexPixels[i]);
	}
	vector<Pixel> leftPixels;
	vector<Pixel> rightPixels;
	ComputePolygonRows(vertexPixels, leftPixels, rightPixels);
	DrawRows(leftPixels, rightPixels);
}