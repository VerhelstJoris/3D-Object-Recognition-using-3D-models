#pragma once


#include <glm.hpp>

#include <vector>


namespace cv{class Mat;}	//forward declaration
//namespace glm { struct vec2; struct vec3; }

enum RENDERER_MODE
{
	GENERATERENDERS,
	DISPLAY
};

class OGLRenderer
{
public:
	OGLRenderer();
	~OGLRenderer();

	bool Initialize(const char* modelFilePath);
	void Shutdown();
	void Run();

	void SwitchToDisplayMode(cv::Mat imageToConvert);

	std::vector<cv::Mat> GetScreenRenders() { return m_renders; };

private:

	void ProcessUserInput();
	cv::Mat ConvertOpenGLToMat(const unsigned int buffer);
	void ConvertMatToTexture(cv::Mat& image, unsigned int& imageTexture);
	
public:
	const int m_WindowWidth = 800, m_WindowHeight = 600;

private:

	//OPENGL
	struct GLFWwindow* m_window;
	unsigned int m_vertexBuffer, m_uvBuffer, m_vertexArrayID, m_programID, m_matrixID, m_viewMatrixID, m_modelMatrixID; // GLUINT
	unsigned int m_renderTex, m_bufferName, m_quadVertexBuffer, m_quadProgramID, m_texID;		//GLUINT
	std::vector<unsigned int> m_indexBuffer;

	std::vector<glm::vec3> m_vertices;
	std::vector<glm::vec2> m_uvs;
	std::vector<glm::vec3> m_normals;	//not used rn

	//orientation of the object
	glm::vec3 m_Orientation = glm::vec3(0.0f, 1.0f, 0.0f);;

	float m_angleDifferenceDegrees = 6.0f;
	int m_amountOfRenders = 0;

	RENDERER_MODE m_mode = RENDERER_MODE::GENERATERENDERS;

	//OPENCV
	std::vector<cv::Mat> m_renders;
	unsigned int m_MatTex;
};

