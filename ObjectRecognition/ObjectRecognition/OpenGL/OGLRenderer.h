#pragma once


#include <glm.hpp>

#include <vector>


namespace cv{class Mat;}	//forward declaration

class Mesh;
struct RenderStruct;
enum RENDERER_MODE;

class OGLRenderer
{
public:
	OGLRenderer();
	~OGLRenderer();

	bool Initialize(const char* modelFilePath, int windowWidth, int windowHeight, RENDERER_MODE mode);
	void Shutdown();
	void Run();

	void SwitchToDisplayMode(cv::Mat imageToConvert);
	void SetModelOrientation(glm::vec3 rotDeg);
	void SetModelScale(glm::vec3 scaleVec);
	void SetModelPosition(glm::vec3 newPos);
	void SetModelPivotDiff(glm::vec3 pivotDiff){m_pivotDiff = pivotDiff;};
	

	glm::vec4 GetWorldCoordFromWindowCoord(glm::vec2 imageCoord, glm::vec2 imageDimensions);


	std::vector<RenderStruct> GetScreenRenders() { return m_renders; };

private:

	void ProcessUserInput();
	cv::Mat ConvertOpenGLToMat(const unsigned int buffer);
	void ConvertMatToTexture(cv::Mat& image, unsigned int& imageTexture);
	
public:
	int m_WindowWidth, m_WindowHeight;

private:

	//OPENGL
	struct GLFWwindow* m_window;

	Mesh* m_model;
	unsigned int m_vertexArrayID, m_programID, m_matrixID, m_viewMatrixID, m_modelMatrixID; // GLUINT
	unsigned int m_renderTex, m_bufferName, m_quadVertexBuffer, m_quadProgramID, m_texID;		//GLUINT POSTPROCESSING


	bool m_keepRunning = true;


	//matrices
	glm::mat4 m_projectionMatrix, m_viewMatrix;

	//camera position
	glm::vec3 m_cameraPos = {0,0,7};

	//orientation of the object
	glm::vec3 m_orientation = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 m_scale = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 m_pivot = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 m_pivotDiff = glm::vec3(0.0f, 0.0f, 0.0f);

	const float m_angleDifferenceDegrees = 6.0f;
	const int m_amountOfRowsToRender = 1;

	int m_currentRowsRendered = 0;
	int m_amountOfRenders = 0;

	unsigned int m_blackColor[4] = { 0, 0, 0, 1 };		//GLUINT
	RENDERER_MODE m_mode;

	//OPENCV
	std::vector<RenderStruct> m_renders;
	unsigned int m_MatTex;
};

