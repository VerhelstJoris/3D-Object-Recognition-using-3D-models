#pragma once


#include <glm.hpp>

#include <vector>

namespace cv{class Mat;}	//forward declaration
//namespace glm { struct vec2; struct vec3; }

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
	cv::Mat GetMatFromOpenGL();

private:

	struct GLFWwindow* m_window;
	unsigned int m_vertexBuffer, m_uvBuffer, m_vertexArrayID, m_programID, m_matrixID;	//GLUINT
	unsigned int m_renderTex, m_bufferName;			//GLUINT
	std::vector<unsigned int> m_indexBuffer;


	std::vector<glm::vec3> m_vertices;
	std::vector<glm::vec2> m_uvs;
	std::vector<glm::vec3> m_normals;	//not used rn
};

