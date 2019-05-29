#pragma once


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
	bool ScreenShot(std::string fileName, int windowWidth, int windowHeight);
private:

	struct GLFWwindow* m_window;
	unsigned int m_vertexBuffer, m_uvBuffer, m_vertexArrayID, m_programID, m_MatrixID;
	std::vector<unsigned int> m_indexBuffer;;

\
	std::vector<glm::vec3> m_vertices;
	std::vector<glm::vec2> m_uvs;
	std::vector<glm::vec3> m_normals;	//not used rn
};

