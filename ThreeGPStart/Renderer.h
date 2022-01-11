#pragma once

#include "ExternalLibraryHeaders.h"

#include "Helper.h"
#include "Mesh.h"
#include "Camera.h"



class Renderer
{
private:
	// Program object - to host shaders
	GLuint terrainProgram{ 0 };
	GLuint cubeProgram{ 0 };
	GLuint jeepProgram{ 0 };
	GLuint skyboxProgram{ 0 };
	//Cube
	GLuint c_VAO{ 0 };
	GLuint c_numElements{ 0 };
	//Jeep
	GLuint tex{0};
	GLuint j_VAO{ 0 };
	GLuint j_numElements{ 0 };
	//Terrain
	GLuint t_tex{ 0 };
	GLuint t_VAO{ 0 };
	//Skybox
	GLuint s_numElements{0};
	GLuint s_VAO{0};
	GLuint s_texture{ 0 };
	GLuint skyboxMap{ 0 };
	


	// Vertex Array Object to wrap all render settings
	

	// Number of elments to use when rendering
	GLuint m_numElements{ 0 };
	

	bool m_wireframe{ false };

	GLuint CreateProgram(std::string, std::string);

	bool Swap = false;
	bool NoiseGen = true;
	bool ExtraNoise = false;
	float NoiseVal{0};
	size_t Index{ 0 };

public:
	Renderer();
	~Renderer();

	// Draw GUI
	void DefineGUI();

	// Create and / or load geometry, this is like 'level load'
	bool InitialiseGeometry();

	// Render the scene
	void Render(const Helpers::Camera& camera, float deltaTime);
};

