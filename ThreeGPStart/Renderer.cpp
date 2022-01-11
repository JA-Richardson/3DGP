#include "Renderer.h"
#include "Camera.h"
#include "ImageLoader.h"

GLuint j_VAO;


Renderer::Renderer() 
{

}

// On exit must clean up any OpenGL resources e.g. the program, the buffers
Renderer::~Renderer()
{
	// TODO: clean up any memory used including OpenGL objects via glDelete* calls
	glDeleteProgram(terrainProgram);
	glDeleteBuffers(1, &j_VAO);
}

// Use IMGUI for a simple on screen GUI
void Renderer::DefineGUI()
{
	// Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	ImGui::Begin("3GP");						// Create a window called "3GP" and append into it.

	ImGui::Text("Visibility.");					// Display some text (you can use a format strings too)	

	ImGui::Checkbox("Wireframe", &m_wireframe);	// A checkbox linked to a member variable

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		
	ImGui::End();
}

// Load, compile and link the shaders and create a program object to host them
GLuint Renderer::CreateProgram(std::string vertPath, std::string fragPath)
{
	// Create a new program (returns a unqiue id)
	GLuint program = glCreateProgram();

	// Load and create vertex and fragment shaders
	GLuint vertex_shader{ Helpers::LoadAndCompileShader(GL_VERTEX_SHADER, vertPath )};
	GLuint fragment_shader{ Helpers::LoadAndCompileShader(GL_FRAGMENT_SHADER, fragPath )};
	if (vertex_shader == 0 || fragment_shader == 0)
		return false;

	// Attach the vertex shader to this program (copies it)
	glAttachShader(program, vertex_shader);

	// The attibute location 0 maps to the input stream "vertex_position" in the vertex shader
	// Not needed if you use (location=0) in the vertex shader itself
	//glBindAttribLocation(m_program, 0, "vertex_position");

	// Attach the fragment shader (copies it)
	glAttachShader(program, fragment_shader);

	// Done with the originals of these as we have made copies
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	// Link the shaders, checking for errors
	if (!Helpers::LinkProgramShaders(program))
		return 0;

	return program;
}

float Noise(int x, int y)
{
	int n = x + y * 57;  // 57 is the seed – can be tweaked
	n = (n >> 13) ^ n;
	int nn = (n * (n * n * 60493 + 19990303) + 1376312589) & 0x7fffffff;
	return 1.0f - ((float)nn / 1073741824.0f);
}


// Load / create geometry into OpenGL buffers	
bool Renderer::InitialiseGeometry()
{
	// Load and compile shaders into m_program
	
	cubeProgram = CreateProgram("Data/Shaders/cube_vertex_shader.vert", "Data/Shaders/cube_fragment_shader.frag");
	terrainProgram = CreateProgram("Data/Shaders/vertex_shader.vert", "Data/Shaders/fragment_shader.frag");
	jeepProgram = CreateProgram("Data/Shaders/jeep_vertex_shader.vert", "Data/Shaders/jeep_fragment_shader.frag");
	skyboxProgram = CreateProgram("Data/Shaders/skybox_vertex_shader.vert", "Data/Shaders/skybox_fragment_shader.frag");

	//Cube
	glm::vec3 cubeMaxValues = { 10, 10, 10 };
	glm::vec3 cubeMinValues = { -10, -10, -10 };

	glm::vec3 cubeCorners[8] =
	{
		{cubeMinValues.x, cubeMinValues.y, cubeMaxValues.z},
		{cubeMaxValues.x, cubeMinValues.y, cubeMaxValues.z},
		{cubeMinValues.x, cubeMaxValues.y, cubeMaxValues.z},
		{cubeMaxValues.x, cubeMaxValues.y, cubeMaxValues.z},

		{cubeMinValues.x, cubeMinValues.y, cubeMinValues.z},
		{cubeMaxValues.x, cubeMinValues.y, cubeMinValues.z},
		{cubeMinValues.x, cubeMaxValues.y, cubeMinValues.z},
		{cubeMaxValues.x, cubeMaxValues.y, cubeMinValues.z},
	};

	std::vector<glm::vec3> cubeVertices;
	std::vector<GLuint> cubeElements;
	std::vector<glm::vec3> cubeColours;
	//red
	cubeColours.push_back({ 1, 0, 0 });
	cubeColours.push_back({ 1, 0, 0 });
	cubeColours.push_back({ 1, 0, 0 });
	cubeColours.push_back({ 1, 0, 0 });
	//blue
	cubeColours.push_back({ 0, 1, 0 });
	cubeColours.push_back({ 0, 1, 0 });
	cubeColours.push_back({ 0, 1, 0 });
	cubeColours.push_back({ 0, 1, 0 });
	//green
	cubeColours.push_back({ 0, 0, 1 });
	cubeColours.push_back({ 0, 0, 1 });
	cubeColours.push_back({ 0, 0, 1 });
	cubeColours.push_back({ 0, 0, 1 });
	//yellow
	cubeColours.push_back({ 1, 1, 0 });
	cubeColours.push_back({ 1, 1, 0 });
	cubeColours.push_back({ 1, 1, 0 });
	cubeColours.push_back({ 1, 1, 0 });
	//orange
	cubeColours.push_back({ 1, 0.5, 0 });
	cubeColours.push_back({ 1, 0.5, 0 });
	cubeColours.push_back({ 1, 0.5, 0 });
	cubeColours.push_back({ 1, 0.5, 0 });
	//white
	cubeColours.push_back({ 1, 1, 1 });
	cubeColours.push_back({ 1, 1, 1 });
	cubeColours.push_back({ 1, 1, 1 });
	cubeColours.push_back({ 1, 1, 1 });
	//front face
	cubeVertices.push_back(cubeCorners[0]);//0
	cubeVertices.push_back(cubeCorners[1]);//1
	cubeVertices.push_back(cubeCorners[2]);//2
	cubeVertices.push_back(cubeCorners[3]);//3

	cubeElements.push_back(0);
	cubeElements.push_back(1);
	cubeElements.push_back(2);
	cubeElements.push_back(1);
	cubeElements.push_back(3);
	cubeElements.push_back(2);
	//back face
	cubeVertices.push_back(cubeCorners[4]);//4
	cubeVertices.push_back(cubeCorners[5]);//5
	cubeVertices.push_back(cubeCorners[6]);//6
	cubeVertices.push_back(cubeCorners[7]);//7

	cubeElements.push_back(5);
	cubeElements.push_back(4);
	cubeElements.push_back(6);
	cubeElements.push_back(5);
	cubeElements.push_back(6);
	cubeElements.push_back(7);
	//right face
	cubeVertices.push_back(cubeCorners[1]);//8
	cubeVertices.push_back(cubeCorners[3]);//9
	cubeVertices.push_back(cubeCorners[5]);//10
	cubeVertices.push_back(cubeCorners[7]);//11

	cubeElements.push_back(8);
	cubeElements.push_back(10);
	cubeElements.push_back(11);
	cubeElements.push_back(8);
	cubeElements.push_back(11);
	cubeElements.push_back(9);
	//left face
	cubeVertices.push_back(cubeCorners[0]);//12
	cubeVertices.push_back(cubeCorners[2]);//13
	cubeVertices.push_back(cubeCorners[4]);//14
	cubeVertices.push_back(cubeCorners[6]);//15

	cubeElements.push_back(12);
	cubeElements.push_back(15);
	cubeElements.push_back(14);
	cubeElements.push_back(12);
	cubeElements.push_back(13);
	cubeElements.push_back(15);

	//bottom face
	cubeVertices.push_back(cubeCorners[0]);//16
	cubeVertices.push_back(cubeCorners[1]);//17
	cubeVertices.push_back(cubeCorners[4]);//18
	cubeVertices.push_back(cubeCorners[5]);//19

	cubeElements.push_back(16);
	cubeElements.push_back(18);
	cubeElements.push_back(17);
	cubeElements.push_back(19);
	cubeElements.push_back(17);
	cubeElements.push_back(18);
	//top face
	cubeVertices.push_back(cubeCorners[2]);//20
	cubeVertices.push_back(cubeCorners[3]);//21
	cubeVertices.push_back(cubeCorners[6]);//22
	cubeVertices.push_back(cubeCorners[7]);//23

	cubeElements.push_back(21);
	cubeElements.push_back(23);
	cubeElements.push_back(22);
	cubeElements.push_back(20);
	cubeElements.push_back(21);
	cubeElements.push_back(22);

	GLuint positionsVBO;
	glGenBuffers(1, &positionsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, positionsVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)* cubeVertices.size(), cubeVertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLuint coloursVBO;
	glGenBuffers(1, &coloursVBO);
	glBindBuffer(GL_ARRAY_BUFFER, coloursVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)* cubeColours.size(), cubeColours.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	GLuint cubeElementEBO;
	glGenBuffers(1, &cubeElementEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeElementEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)* cubeElements.size(), cubeElements.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	c_numElements = cubeElements.size();
	
	glGenVertexArrays(1, &c_VAO);
	glBindVertexArray(c_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, positionsVBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);
	glBindBuffer(GL_ARRAY_BUFFER, coloursVBO);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeElementEBO);
	glBindVertexArray(0);

	//Jeep
	Helpers::ModelLoader loader;
	if (!loader.LoadFromFile("Data\\Models\\Jeep\\jeep.obj"))
	return false;

	Helpers::ImageLoader Jeep;
	if (Jeep.Load("Data\\Models\\Jeep\\jeep_rood.jpg"))
	{
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Jeep.Width(), Jeep.Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, Jeep.GetData());
		glGenerateMipmap(GL_TEXTURE_2D);

	}
	else
	{
		MessageBox(NULL, L"Texture not found", L"Error, you're an idiot", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}


	for (const Helpers::Mesh& mesh : loader.GetMeshVector())
	{
		j_numElements = mesh.elements.size();

		GLuint jeepPositionsVBO;
		glGenBuffers(1, &jeepPositionsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, jeepPositionsVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.vertices.size(), mesh.vertices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		GLuint jeepColoursVBO;
		glGenBuffers(1, &jeepColoursVBO);
		glBindBuffer(GL_ARRAY_BUFFER, jeepColoursVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * mesh.uvCoords.size(), mesh.uvCoords.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		GLuint jeepElementEBO;
		glGenBuffers(1, &jeepElementEBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, jeepElementEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * mesh.elements.size(), mesh.elements.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glGenVertexArrays(1, &j_VAO);
		glBindVertexArray(j_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, jeepPositionsVBO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(
			0,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
		);
		glBindVertexArray(0);
		glBindVertexArray(j_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, jeepColoursVBO);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(
			1,
			2,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
		);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, jeepElementEBO);
		glBindVertexArray(0);

	}

	////Terrain + Height map + texture + noise
	std::vector<glm::vec3> vertices;
	std::vector<GLuint> elements;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> colours;	
	std::vector<glm::vec2> UVCoords;

	float numCellX = 500;
	float numCellZ = 500;

	int numVertX = numCellX + 1;
	int numVertZ = numCellZ + 1;
	Helpers::ImageLoader HeightMap;
	if (!HeightMap.Load("Data\\Heightmaps\\sf1.gif"))
	{

		for (int i = 0; i < numVertX; i++)
		{
			for (int j = 0; j < numVertZ; j++)
			{
				vertices.push_back(glm::vec3(i * 8, 0, j * 8));

				UVCoords.push_back(glm::vec2(j / numCellZ, i / numCellX));

				normals.push_back(glm::vec3(0, 0, 0));
			}
		}
	}
	else
	{
		float vertexXtoImage = ((float)HeightMap.Width() -1) / numVertX;
		float vertexZtoImage = ((float)HeightMap.Height() -1) / numVertZ;

		BYTE* imageData = HeightMap.GetData();

		 for (size_t x = 0; x < numVertX; x++)
		{
			 for (size_t z = 0; z < numVertZ; z++)
			{
				int imagex = vertexXtoImage * (numVertX - x);
				int imagez = vertexZtoImage * z;

				size_t offset = ((size_t)imagex + (size_t)imagez * HeightMap.Width()) * 4;
				BYTE height = imageData[offset];

				vertices.push_back(glm::vec3(x * 8, (float)height, z * 8));
				UVCoords.push_back(glm::vec2(x / numCellZ, z / numCellX));
				normals.push_back(glm::vec3(0, 0, 0));


			}
		}
	}

	for (int cellZ = 0; cellZ < numCellZ; cellZ++)
	{
		for (int cellX = 0; cellX < numCellX; cellX++)
		{
			int startVertIndex = (cellZ * numVertX) + cellX;
			if (Swap)
			{
				elements.push_back(startVertIndex);
				elements.push_back(startVertIndex + 1);
				elements.push_back(startVertIndex + numVertX);

				elements.push_back(startVertIndex + 1);
				elements.push_back(startVertIndex + numVertX + 1);
				elements.push_back(startVertIndex + numVertX);
			}
			else
			{
				elements.push_back(startVertIndex);
				elements.push_back(startVertIndex + numVertX + 1);
				elements.push_back(startVertIndex + numVertX);

				elements.push_back(startVertIndex + 1);
				elements.push_back(startVertIndex + numVertX + 1);
				elements.push_back(startVertIndex);
			}
			Swap = !Swap;
		}
		Swap = !Swap;
	}

	if (NoiseGen)
	{
		for (int i = 0; i < numVertX; i++)
		{
			for (int j = 0; j < numVertZ; j++)
			{
				NoiseVal = Noise(i, j);
				NoiseVal = NoiseVal + 1.25 / 2;
				glm::vec3 NoiseVec = vertices[Index];

				if (!ExtraNoise)
				{
					NoiseVal = NoiseVal * 2;
				}

				NoiseVec.y = NoiseVec.y + NoiseVal;
				vertices[Index] = NoiseVec;
				Index++;

			}
		}
	}

	Helpers::ImageLoader Terrain;
	if (Terrain.Load("Data\\Textures\\redblue.jpg"))
	{
		glGenTextures(1, &t_tex);
		glBindTexture(GL_TEXTURE_2D, t_tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Terrain.Width(), Terrain.Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, Terrain.GetData());
		glGenerateMipmap(GL_TEXTURE_2D);

	}
	else
	{
		MessageBox(NULL, L"Texture not found", L"Error, you're an idiot", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	for (glm::vec3& n : normals)
		n = glm::vec3(0, 0, 0);

	for (size_t index = 0; index < elements.size(); index += 3)
	{
		glm::vec3 v0{ vertices[elements[index]] };
		glm::vec3 v1{ vertices[elements[index + 1]] };
		glm::vec3 v2{ vertices[elements[index + 2]] };

		glm::vec3 side1 = v1 - v0;
		glm::vec3 side2 = v2 - v0;

		glm::vec3 TriNorm = glm::normalize(glm::cross(side1, side2));

		normals[elements[index]] += TriNorm;
		normals[elements[index + 1]] += TriNorm;
		normals[elements[index + 2]] += TriNorm;
	}

	for (size_t normIndex = 0; normIndex < normals.size(); normIndex++)
	{
		glm::normalize(normals[normIndex]);
	}
	
	 m_numElements = elements.size();

	GLuint TerrainVBO;
	glGenBuffers(1, &TerrainVBO);
	glBindBuffer(GL_ARRAY_BUFFER, TerrainVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLuint TerrainColVBO;
	glGenBuffers(1, &TerrainColVBO);
	glBindBuffer(GL_ARRAY_BUFFER, TerrainColVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * UVCoords.size(), UVCoords.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLuint elementEBO;
	glGenBuffers(1, &elementEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * elements.size(), elements.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	GLuint normalbuffer;
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

	glGenVertexArrays(1, &t_VAO);
	glBindVertexArray(t_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, TerrainVBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);
	glBindVertexArray(0);
	glBindVertexArray(t_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, TerrainColVBO);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1,
		2,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);
	glBindVertexArray(0);
	glBindVertexArray(t_VAO);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glVertexAttribPointer(
		2,                                // attribute
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized
		0,                                // stride
		(void*)0                          // array buffer offset
	);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementEBO);
	glBindVertexArray(0);

	//Skybox
	std::vector<GLfloat> skyboxVerts =
	{
		-10.0f,  10.0f, -10.0f,
		-10.0f, -10.0f, -10.0f,
		 10.0f, -10.0f, -10.0f,
		 10.0f, -10.0f, -10.0f,
		 10.0f,  10.0f, -10.0f,
		-10.0f,  10.0f, -10.0f,

		-10.0f, -10.0f,  10.0f,
		-10.0f, -10.0f, -10.0f,
		-10.0f,  10.0f, -10.0f,
		-10.0f,  10.0f, -10.0f,
		-10.0f,  10.0f,  10.0f,
		-10.0f, -10.0f,  10.0f,

		 10.0f, -10.0f, -10.0f,
		 10.0f, -10.0f,  10.0f,
		 10.0f,  10.0f,  10.0f,
		 10.0f,  10.0f,  10.0f,
		 10.0f,  10.0f, -10.0f,
		 10.0f, -10.0f, -10.0f,

		-10.0f, -10.0f,  10.0f,
		-10.0f,  10.0f,  10.0f,
		 10.0f,  10.0f,  10.0f,
		 10.0f,  10.0f,  10.0f,
		 10.0f, -10.0f,  10.0f,
		-10.0f, -10.0f,  10.0f,

		-10.0f,  10.0f, -10.0f,
		 10.0f,  10.0f, -10.0f,
		 10.0f,  10.0f,  10.0f,
		 10.0f,  10.0f,  10.0f,
		-10.0f,  10.0f,  10.0f,
		-10.0f,  10.0f, -10.0f,

		-10.0f, -10.0f, -10.0f,
		-10.0f, -10.0f,  10.0f,
		 10.0f, -10.0f, -10.0f,
		 10.0f, -10.0f, -10.0f,
		-10.0f, -10.0f,  10.0f,
		 10.0f, -10.0f,  10.0f
	};

	Helpers::ImageLoader skyboxRight;
	Helpers::ImageLoader skyboxLeft;
	Helpers::ImageLoader skyboxTop;
	Helpers::ImageLoader skyboxBottom;
	Helpers::ImageLoader skyboxFront;
	Helpers::ImageLoader skyboxBack;

	if (!skyboxRight.Load("Data\\Models\\Sky\\Mars\\Mar_R.dds"))
	{
		MessageBox(NULL, L"Texture not found", L"Error, you're an idiot", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	else if (!skyboxLeft.Load("Data\\Models\\Sky\\Mars\\Mar_L.dds"))
	{
		MessageBox(NULL, L"Texture not found", L"Error, you're an idiot", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	else if (!skyboxTop.Load("Data\\Models\\Sky\\Mars\\Mar_U.dds"))
	{
		MessageBox(NULL, L"Texture not found", L"Error, you're an idiot", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	else if (!skyboxBottom.Load("Data\\Models\\Sky\\Mars\\Mar_D.dds"))
	{
		MessageBox(NULL, L"Texture not found", L"Error, you're an idiot", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	else if (!skyboxFront.Load("Data\\Models\\Sky\\Mars\\Mar_F.dds"))
	{
		MessageBox(NULL, L"Texture not found", L"Error, you're an idiot", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	else if (!skyboxBack.Load("Data\\Models\\Sky\\Mars\\Mar_B.dds"))
	{
		MessageBox(NULL, L"Texture not found", L"Error, you're an idiot", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	glGenTextures(1, &skyboxMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxMap);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, skyboxRight.Width(), skyboxRight.Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, skyboxRight.GetData());
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, skyboxLeft.Width(), skyboxLeft.Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, skyboxLeft.GetData());
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, skyboxTop.Width(), skyboxTop.Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, skyboxTop.GetData());
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, skyboxBottom.Width(), skyboxBottom.Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, skyboxBottom.GetData());
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, skyboxFront.Width(), skyboxFront.Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, skyboxFront.GetData());
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, skyboxBack.Width(), skyboxBack.Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, skyboxBack.GetData());
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);



	GLuint skyMeshVBO;
	glGenBuffers(1, &skyMeshVBO);
	glBindBuffer(GL_ARRAY_BUFFER, skyMeshVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * skyboxVerts.size(), skyboxVerts.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glGenVertexArrays(1, &s_VAO);
	//glBindVertexArray(0);
	glBindVertexArray(s_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, skyMeshVBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glBindVertexArray(0);
};



// Render the scene. Passed the delta time since last called.
void Renderer::Render(const Helpers::Camera& camera, float deltaTime)
{			
	// Configure pipeline settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Wireframe mode controlled by ImGui
	if (m_wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Clear buffers from previous frame
	glClearColor(0.0f, 0.0f, 0.0f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Compute viewport and projection matrix
	GLint viewportSize[4];
	glGetIntegerv(GL_VIEWPORT, viewportSize);
	const float aspect_ratio = viewportSize[2] / (float)viewportSize[3];
	glm::mat4 projection_xform = glm::perspective(glm::radians(45.0f), aspect_ratio, 1.0f, 7500.0f);

	// Compute camera view matrix and combine with projection matrix for passing to shader
	glm::mat4 view_xform = glm::lookAt(camera.GetPosition(), camera.GetPosition() + camera.GetLookVector(), camera.GetUpVector());
	

	// Use our program. Doing this enables the shaders we attached previously.
	glUseProgram(skyboxProgram);

	// Send the combined matrix to the shader in a uniform
	//GLuint combined_xform_id = glGetUniformLocation(m_program, "combined_xform");
	//glUniformMatrix4fv(combined_xform_id, 1, GL_FALSE, glm::value_ptr(combined_xform));

	glm::mat4 model_xform = glm::mat4(1);

	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);

	//Skybox Render
	glm::mat4 view_xform2 = glm::mat4(glm::mat3(view_xform));
	glm::mat4 combined_xform2 = projection_xform * view_xform2;
	GLuint combined_xform_id2 = glGetUniformLocation(skyboxProgram, "combined_xform_sky");
	glUniformMatrix4fv(combined_xform_id2, 1, GL_FALSE, glm::value_ptr(combined_xform2));

	glm::mat4 skybox_xform = glm::mat4(1);
	GLuint skybox_xform_id = glGetUniformLocation(skyboxProgram, "model_xform");
	glUniformMatrix4fv(skybox_xform_id, 1, GL_FALSE, glm::value_ptr(skybox_xform));

	GLuint skyboxUniformID = glGetUniformLocation(skyboxProgram, "skybox");

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxMap);

	glBindVertexArray(s_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glBindVertexArray(0);

	//Jeep renderer
	glUseProgram(jeepProgram);
	glm::mat4 combined_xform = projection_xform * view_xform;
	GLuint combined_xform_id = glGetUniformLocation(jeepProgram, "combined_xform");
	glUniformMatrix4fv(combined_xform_id, 1, GL_FALSE, glm::value_ptr(combined_xform));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);
	glUniform1i(glGetUniformLocation(jeepProgram, "sampler_tex"), 0);
	// Send the model matrix to the shader in a uniform
	GLuint model_xform_id = glGetUniformLocation(jeepProgram, "model_xform");
	glUniformMatrix4fv(model_xform_id, 1, GL_FALSE, glm::value_ptr(model_xform));
	//// Bind our VAO and render
	glBindVertexArray(j_VAO);
	glDrawElements(GL_TRIANGLES, j_numElements, GL_UNSIGNED_INT, (void*)0);
	glBindVertexArray(0);

	//Terrain renderer
	glUseProgram(terrainProgram);
	glm::mat4 terrain_combined_xform = projection_xform * view_xform;
	GLuint terrain_combined_xform_id = glGetUniformLocation(terrainProgram, "combined_xform");
	glUniformMatrix4fv(terrain_combined_xform_id, 1, GL_FALSE, glm::value_ptr(terrain_combined_xform));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, t_tex);
	glUniform1i(glGetUniformLocation(terrainProgram, "sampler_tex"), 0);
	model_xform = glm::mat4(1);
	GLuint terrain_model_xform_id = glGetUniformLocation(terrainProgram, "model_xform");
	glUniformMatrix4fv(terrain_model_xform_id, 1, GL_FALSE, glm::value_ptr(model_xform));
	glBindVertexArray(t_VAO);
	glDrawElements(GL_TRIANGLES, m_numElements, GL_UNSIGNED_INT, (void*)0);
	glBindVertexArray(0);
	
	//Cube renderer
	glUseProgram(cubeProgram);
	combined_xform = projection_xform * view_xform;

	combined_xform_id = glGetUniformLocation(cubeProgram, "combined_xform");
	glUniformMatrix4fv(combined_xform_id, 1, GL_FALSE, glm::value_ptr(combined_xform));

	model_xform = glm::mat4(1);
	model_xform = glm::translate(model_xform, glm::vec3{ 1000.0f, 500.0f, 500.0f });
	model_xform = glm::scale(model_xform, glm::vec3{ 10.0f, 10.0f, 10.0f });

	//// Uncomment all the lines below to rotate cube first round y then round x
	static float angle = 0;
	static bool rotateY = true;

	if (rotateY) // Rotate around y axis		
		model_xform = glm::rotate(model_xform, angle, glm::vec3{ 0 ,1,0 });
	else // Rotate around x axis		
		model_xform = glm::rotate(model_xform, angle, glm::vec3{ 1 ,0,0 });

	angle += 0.001f;
	if (angle > glm::two_pi<float>())
	{
		angle = 0;
		rotateY = !rotateY;
	}

	model_xform_id = glGetUniformLocation(cubeProgram, "model_xform");
	glUniformMatrix4fv(model_xform_id, 1, GL_FALSE, glm::value_ptr(model_xform));

	glBindVertexArray(c_VAO);
	glDrawElements(GL_TRIANGLES, c_numElements, GL_UNSIGNED_INT, (void*)0);
	glBindVertexArray(0);

}

