#include "OGLRenderer.h"

#include <stdio.h>
#include <iostream>

//OPENGL
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <gtc/matrix_transform.hpp>
#include <gtx/euler_angles.hpp>

#include "OGLHelperFunctions.h"
#include "../OpenCV/ImageOperations.h"

//OPENCV
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include "mesh.h"


OGLRenderer::OGLRenderer()
{
}


OGLRenderer::~OGLRenderer()
{
}

bool OGLRenderer::Initialize(const char* modelFilePath, int windowWidth, int windowHeight, RENDERER_MODE mode)
{
	m_mode = mode;

	std::cout << std::endl << "=======================================" << std::endl << "INITIALIZING RENDERER" << std::endl << std::endl;


	#pragma region WINDOW_SETUP
	m_WindowWidth= windowWidth;
	m_WindowHeight = windowHeight;

	/* Initialize the library */
	if (!glfwInit())
	{
		std::cout << "Failed to initialize GLFW" << std::endl;
		std::cin.get();
		return false;

	}

	glfwWindowHint(GLFW_SAMPLES, 1);					//amount of samples per pixel
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);		//set to version 3.3 of opengl
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	//only use core profile


	//create the window
	m_window = glfwCreateWindow(m_WindowWidth, m_WindowHeight, "ModelViewer", NULL, NULL);
	if (m_window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(m_window);


	//sticky keys
	glfwSetInputMode(m_window, GLFW_STICKY_KEYS, GLFW_TRUE);


	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return false;
	}

	
	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	//glDepthRange(-1.0f, 1.0f);

	// Cull triangles which normal is not towards the camera
	//glEnable(GL_CULL_FACE);

	#pragma endregion 

	#pragma region RENDERING

	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);

	// Create and compile our GLSL program from the shaders
	if (m_mode == RENDERER_MODE::GENERATERENDERS || m_mode == RENDERER_MODE::CAMERAMOVE)
	{
		m_programID = OGLHelperFunctions::LoadShaders("OpenGL/Shaders/VertexShader.glsl", "OpenGL/Shaders/FragmentShaderRender.glsl");
	}
	else
	{
		m_programID = OGLHelperFunctions::LoadShaders("OpenGL/Shaders/VertexShader.glsl", "OpenGL/Shaders/FragmentShaderDisplay.glsl");
	}

	// Get a handle for our "MVP" uniform
	m_matrixID = glGetUniformLocation(m_programID, "MVP");
	m_viewMatrixID = glGetUniformLocation(m_programID, "V");
	m_modelMatrixID = glGetUniformLocation(m_programID, "M");

	//model
	m_model = new Mesh();
	m_model->LoadMesh(modelFilePath);
	

#pragma endregion

	#pragma region POST-PROCESSING
	//============================================================================================================
	//FRAME BUFFER OBJECT == RENDER TO TEXTURE
	// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
	m_bufferName = 0;
	glGenFramebuffers(1, &m_bufferName);
	glBindFramebuffer(GL_FRAMEBUFFER, m_bufferName);

	//the render texture
	glGenTextures(1, &m_renderTex);

	// "Bind" the newly created texture 
	glBindTexture(GL_TEXTURE_2D, m_renderTex);

	// Give an empty image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_WindowWidth, m_WindowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	// Poor filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Set renderTex as our colour attachement #0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_renderTex, 0);
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	// Set the list of draw buffers.
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "Something went wrong with creating the framebuffer" << std::endl;
		return false;
	}
	else
	{
		std::cout << "frame buffer created succesfully" << std::endl;
	}

	// The fullscreen quad's FBO
	static const GLfloat quadVertexBufferData[] = {
		-1.0f, -1.0f, 1.0f,
		 1.0f, -1.0f, 1.0f,
		-1.0f,  1.0f, 1.0f,
		-1.0f,  1.0f, 1.0f,
		 1.0f, -1.0f, 1.0f,
		 1.0f,  1.0f, 1.0f,
	};

	
	//vertex buffer for the quad that we render our post-processed texture to
	glGenBuffers(1, &m_quadVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_quadVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertexBufferData), quadVertexBufferData, GL_STATIC_DRAW);

	//loading the post-processing shaders
	m_quadProgramID = OGLHelperFunctions::LoadShaders("OpenGL/Shaders/PassThroughVertexShader.glsl", "OpenGL/Shaders/TextureFragmentShader.glsl");
	m_texID = glGetUniformLocation(m_quadProgramID, "renderedTexture");

	#pragma endregion


	//matrices
	//m_projectionMatrix = glm::ortho(0.0f, (float)m_WindowWidth, (float)m_WindowHeight, 0.0f, 0.1f, 100.0f);
	m_projectionMatrix = glm::perspective(glm::radians(45.0f), (float)m_WindowWidth / (float)m_WindowHeight, 0.1f, 100.0f);
	m_viewMatrix = glm::lookAt(
		m_cameraPos, // Camera is here, in World Space
		glm::vec3(m_cameraPos.x, m_cameraPos.y, m_cameraPos.z - 10.0f), // and looks forward
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);


	if (m_mode == RENDERER_MODE::CAMERAMOVE)
	{
		std::cout << std::endl << std::endl;
		std::cout << "========================================================" << std::endl;
		std::cout << "move the camera around until the object is full in screen" << std::endl;
		std::cout << "WASD to move forward/back and left/right" << std::endl;
		std::cout << "SPACE and Left CTRL to move up/down" << std::endl;
		std::cout << "ENTER to confirm and start the rendering" << std::endl;
	}

	return true;
}

void OGLRenderer::Shutdown()
{
	// Cleanup VBO
	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);

	glDeleteFramebuffers(1, &m_bufferName);
	glDeleteTextures(1, &m_renderTex);
	glDeleteBuffers(1, &m_quadVertexBuffer);
	glDeleteVertexArrays(1, &m_vertexArrayID);

	glDeleteTextures(1, &m_MatTex);

	delete m_model;

	glfwTerminate();

	std::cout << std::endl << "SHUTTING DOWN RENDERER" << std::endl << 
		"==========================" << std::endl << std::endl;
}

void OGLRenderer::Run()
{

	//BEST APPROACH???
	m_orientation.x = -(m_angleDifferenceDegrees * 0.0174532925f)* (int)(m_amountOfRowsToRender / 2);


	while (m_keepRunning ==true)
	{
		//INPUT
		
		ProcessUserInput();
		
	#pragma region RENDERING
	
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0, 0, 0, 1);
		
		glClearTexImage(m_renderTex, 0, GL_BGRA, GL_UNSIGNED_BYTE, &m_blackColor);
	
		//RENDERING
		if (m_mode == RENDERER_MODE::GENERATERENDERS)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, m_bufferName);
			//glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glUseProgram(m_programID);

		}
		else
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			glUseProgram(m_programID);
		}
	
		glViewport(0, 0, m_WindowWidth, m_WindowHeight);
		glMatrixMode(GL_PROJECTION);
		float aspect = (float)m_WindowWidth / (float)m_WindowHeight;
		glOrtho(-aspect, aspect, -1, 1, -1, 1);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		#pragma region MATRICES
	
		m_viewMatrix = glm::lookAt(
			m_cameraPos, // Camera is here, in World Space
			glm::vec3(m_cameraPos.x, m_cameraPos.y, m_cameraPos.z-10.0f), // and looks forward
			glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
		);
	
		//MODEL MATRICES
	//==============================================
	//m_orientation.y += 3.14159f / 2.0f * m_angleDifferenceDegrees;
		if (m_mode == RENDERER_MODE::GENERATERENDERS)
		{
			m_orientation.y += (m_angleDifferenceDegrees * 0.0174532925f);			//degree to radian			
		}
	
		glm::mat4 rotationMatrix = glm::eulerAngleZYX(m_orientation.z, m_orientation.y, m_orientation.x);
		glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0), m_position);	
		glm::mat4 scalingMatrix = glm::scale(glm::mat4(1.0), m_scale);			
	
		// Build the model matrix
		glm::vec4 pivotWorld;
		if (m_mode == RENDERER_MODE::DISPLAY)
		{
			glm::mat4 modelMatrix =  translationMatrix * rotationMatrix * scalingMatrix;
			glm::vec4 pivotObject = glm::vec4(0, 0, 0, 1);

			pivotWorld = pivotObject * modelMatrix;
			pivotWorld.x /= pivotWorld.w;
			pivotWorld.y /= pivotWorld.w;
			pivotWorld.z /= pivotWorld.w;
		}
		glm::mat4 minusPivot = glm::translate(glm::mat4(1.0), -m_pivot + m_pivotDiff);
		glm::mat4 pivotMatrix = glm::translate(glm::mat4(1.0), m_pivot + m_pivotDiff);

		glm::mat4 modelMatrixNew =  minusPivot * scalingMatrix * rotationMatrix * pivotMatrix * translationMatrix;
		//glm::mat4 modelMatrixNew =  translationMatrix * rotationMatrix * scalingMatrix;

		glm::mat4 MVP = m_projectionMatrix * m_viewMatrix * modelMatrixNew;
	


		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(m_matrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(m_modelMatrixID, 1, GL_FALSE, &modelMatrixNew[0][0]);
		glUniformMatrix4fv(m_viewMatrixID, 1, GL_FALSE, &m_viewMatrix[0][0]);
	
		#pragma endregion
	
		m_model->Render();

		
	#pragma endregion 
	
	#pragma region POST-PROCESSING
		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, m_WindowWidth, m_WindowHeight);
	
		// Use our shader
		glUseProgram(m_quadProgramID);
	
		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
	
		if (m_mode == RENDERER_MODE::GENERATERENDERS)
		{
			glBindTexture(GL_TEXTURE_2D, m_renderTex);
		}
		else if (m_mode == RENDERER_MODE::DISPLAY)
		{
			glBindTexture(GL_TEXTURE_2D, m_MatTex);
		}
		// Set our texture sampler to use Texture Unit 0
		glUniform1i(m_texID, 0);
	
		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, m_quadVertexBuffer);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
	
	
		// Draw the triangles !
		glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles
	
		glDisableVertexAttribArray(0);
	
	#pragma endregion


		glfwSwapBuffers(m_window);

		if (m_mode == RENDERER_MODE::GENERATERENDERS)
		{
			if (m_amountOfRenders < (int)(360.0f / m_angleDifferenceDegrees))
			{

				RenderStruct renderInfo;
				renderInfo.renderImage = ConvertOpenGLToMat(m_bufferName);
				renderInfo.rotationX = m_orientation.x;
				renderInfo.rotationY = m_orientation.y;
				renderInfo.rotationZ = m_orientation.z;

				m_renders.push_back(renderInfo);
				m_amountOfRenders++;
			}
			else
			{
				m_currentRowsRendered++;
				if (m_currentRowsRendered >= m_amountOfRowsToRender)
				{
					std::cout << "RENDERS GENERATED" << std::endl << std::endl;
					m_keepRunning = false;
				}
				else
				{
					m_orientation.x += (m_angleDifferenceDegrees * 0.0174532925f);
					m_orientation.y =0;			//degree to radian
					std::cout << "NEXT ROW" << std::endl;
					m_amountOfRenders = 0;
				}
			}
		}
	}
}

void OGLRenderer::ProcessUserInput()
{
	//close on pressing escape
	if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(m_window, true);
	}


	if (m_mode == RENDERER_MODE::CAMERAMOVE)
	{
		//move UP/DOWN
		if (glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS)
		{
			m_cameraPos.y += 0.1f;
		}
		else if (glfwGetKey(m_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		{
			m_cameraPos.y -= 0.1f;
		}

		//MOVE FORWARD/BACK
		if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
		{
			m_cameraPos.z -= 0.1f;
		}
		else if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
		{
			m_cameraPos.z += 0.1f;
		}

		//MOVE LEFT/RIGHT
		if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
		{
			m_cameraPos.x -= 0.1f;
		}
		else if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
		{
			m_cameraPos.x += 0.1f;
		}

		//CONFIRM AND START RENDERING
		if (glfwGetKey(m_window, GLFW_KEY_ENTER) == GLFW_PRESS)
		{
			std::cout << "CONFIRMED" << std::endl
				<< "GENERATING RENDERS" << std::endl;
			m_mode = RENDERER_MODE::GENERATERENDERS;
		}
	}

	if (m_mode == RENDERER_MODE::DISPLAY)
	{
		////TEST ROTATION
		//if (glfwGetKey(m_window, GLFW_KEY_P) == GLFW_PRESS)
		//{
		//	m_orientation.z += 6 * 0.0174532925f;
		//	std::cout << m_orientation.z << std::endl;
		//}

		if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			m_keepRunning = false;
			
		}

	}

	/* Poll for and process events */
	glfwPollEvents();
}

cv::Mat OGLRenderer::ConvertOpenGLToMat(const GLuint buffer)
{
	glBindTexture(GL_TEXTURE_2D, buffer);
	GLenum texWidth, texHeight;

	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, (GLint*)&texWidth);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, (GLint*)&texHeight);

	unsigned char* texBytes = (unsigned char*)malloc(sizeof(unsigned char)*texWidth*texHeight * 3);
	glGetTexImage(GL_TEXTURE_2D, 0 /* mipmap level */, GL_BGR, GL_UNSIGNED_BYTE, texBytes);


	//create a cv::mat with the specified dimensions, format and size
	cv::Mat flipped = cv::Mat(texHeight, texWidth, CV_8UC3, texBytes); // this is the flipped image
	cv::Mat dest = flipped;
	cv::flip(flipped, dest,0 );	
	//ImageOperations::ExtractSilhouette(dest, flipped);	//extract the silhouette from non-flipped and put it back in 'flipped'

	//cv::imshow("main", flipped);
	return flipped;
}

void OGLRenderer::ConvertMatToTexture(cv::Mat& image, GLuint& imageTexture)
{
	if (image.empty()) {
		std::cout << "image empty" << std::endl;
	}
	else {

		cv::cvtColor(image, image, cv::COLOR_RGB2BGR);

		//use fast 4-byte alignment (default anyway) if possible
		glPixelStorei(GL_UNPACK_ALIGNMENT, (image.step & 3) ? 1 : 4);

		//set length of one complete row in data (doesn't need to equal image.cols)
		glPixelStorei(GL_UNPACK_ROW_LENGTH, (GLint)(image.step / image.elemSize()));


		//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glGenTextures(1, &imageTexture);
		glBindTexture(GL_TEXTURE_2D, imageTexture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Set texture clamping method
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);


		glTexImage2D(GL_TEXTURE_2D,         // Type of texture
			0,								// Pyramid level (for mip-mapping) - 0 is the top level
			GL_RGB,							// Internal colour format to convert to
			image.cols,						// Image width 
			image.rows,						// Image height
			0,								// Border width in pixels (can either be 1 or 0)
			GL_RGB,							// Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
			GL_UNSIGNED_BYTE,				// Image data type
			image.ptr());					// The actual image data itself
	}
}

void OGLRenderer::SwitchToDisplayMode(cv::Mat imageToConvert)
{
	std::cout << "Switching Display Modes" << std::endl;
	m_mode = RENDERER_MODE::DISPLAY;

	cv::flip(imageToConvert, imageToConvert,0);

	ConvertMatToTexture(imageToConvert, m_MatTex);
}

void OGLRenderer::SetModelOrientation(glm::vec3 rotDeg)
{ 
	m_orientation = rotDeg;
	//std::cout << rot.x << " , " << rot.y << " ," << rot.z << std::endl;
};

void OGLRenderer::SetModelScale(glm::vec3 scaleVec)
{
	m_scale = scaleVec;
	//std::cout << rot.x << " , " << rot.y << " ," << rot.z << std::endl;
};

void OGLRenderer::SetModelPosition(glm::vec3 newPos)
{
	m_position = newPos;
}




glm::vec4 OGLRenderer::GetWorldCoordFromWindowCoord(glm::vec2 imageCoord, glm::vec2 imageDimensions)
{
	//https://stackoverflow.com/questions/7692988/opengl-math-projecting-screen-space-to-world-space-coords

	glm::mat4 inversed =  glm::inverse(m_projectionMatrix * m_viewMatrix);

	//map the imageCoord to a range [-1,1]
	float mappedX = 1.0f - (2.0f*((float)(imageCoord.x) / (imageDimensions.x)));
	float mappedY = 1.0f - (2.0f*((float)(imageCoord.y) / (imageDimensions.y)));
	glm::vec4 vec = glm::vec4(mappedX, mappedY, 1.0, 1.0);

	glm::vec4 pos = vec * inversed;
	pos.x /= pos.w;
	pos.y /= pos.w;
	pos.z /= pos.w;

	std::cout << "WORLD POS: " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;

	return pos;
}

