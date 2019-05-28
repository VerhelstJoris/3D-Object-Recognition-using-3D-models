#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm.hpp>

#include <vector>

class System
{
public:
	System();
	~System();

	bool Initialize();
	void Shutdown();
	void Run();

private:

	void ProcessUserInput();

private:

	GLFWwindow* m_window;
	GLuint m_vertexBuffer, m_uvBuffer;;
	GLuint m_vertexArrayID, m_programID, m_MatrixID;

	std::vector<glm::vec3> m_vertices;
	std::vector<glm::vec2> m_uvs;
	std::vector<glm::vec3> m_normals;	//not used rn
};

