#include "System.h"

#include <stdio.h>

//OPENGL
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <gtc/matrix_transform.hpp>

//OPENCV
#include <opencv2/core.hpp>

//
#include "OpenCV/DisplayWindow.h"
#include "OpenCV/ImageOperations.h"
#include "OpenCV/HelperFunctions.h"


System::System()
{
}


System::~System()
{
}

bool System::Initialize()
{
	////OPENCV
	//Mat image1, image2, destination1, destination2;
	//image1 = imread("../Resources/Stopsign/render1.png"); // Read the file
	//image2 = imread("../Resources/Stopsign/render3.png"); // Read the file
	//destination1 = image1;
	//destination2 = image2;
	//
	//if (image1.empty()) // Check for invalid input
	//{
	//	cout << "Could not open or find the image" << std::endl;
	//	return -1;
	//}
	//if (image2.empty()) // Check for invalid input
	//{
	//	cout << "Could not open or find the image" << std::endl;
	//	return -1;
	//}
	//
	////MAIN WINDOW
	//DisplayWindow* m_MainWindow = new DisplayWindow("MAIN", WINDOW_NORMAL);
	//m_MainWindow->ChangeWindowSize(1920, 540);
	//m_MainWindow->ChangeWindowTitle("Main");
	//
	//cv::Mat doubleImage(image1.rows, image1.cols * 2, image1.type());
	//
	//image1.copyTo(doubleImage(cv::Rect(0, 0, image1.cols, image1.rows)));
	//image2.copyTo(doubleImage(cv::Rect(image1.cols, 0, image1.cols, image1.rows)));
	//
	//cv::imshow(m_MainWindow->GetName(), doubleImage);
	//
	////SILHOUETTE WINDOW
	//DisplayWindow* m_SecondWindow = new DisplayWindow("SILHOUETTE", WINDOW_NORMAL);
	//m_SecondWindow->ChangeWindowSize(1920, 540);
	//m_SecondWindow->ChangeWindowTitle("Silhouettes");
	//
	//ImageOperations::ExtractSilhouette(image1, destination1, 50, 15, false);
	//ImageOperations::ExtractSilhouette(image2, destination2, 50, 15, false);
	//
	//cv::Mat doubleSilhouette(destination1.rows, destination1.cols * 2, destination1.type());
	//
	//destination1.copyTo(doubleSilhouette(cv::Rect(0, 0, destination1.cols, destination1.rows)));
	//destination2.copyTo(doubleSilhouette(cv::Rect(destination1.cols, 0, destination1.cols, destination1.rows)));
	//
	//cv::imshow(m_SecondWindow->GetName(), doubleSilhouette);

	//OPENGL
	//============================================================

	/* Initialize the library */
	if (!glfwInit())
	{
		std::cout << "Failed to initialize GLFW" << std::endl;
		std::cin.get();
		return false;

	}

	glfwWindowHint(GLFW_SAMPLES, 4);					//amount of samples per pixel
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);		//set to version 3.3 of opengl
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	//only use core profile


	m_window = glfwCreateWindow(m_WindowWidth, m_WindowHeight, "ModelViewer", NULL, NULL);
	if (m_window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(m_window);

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
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);

	// Create and compile our GLSL program from the shaders
	m_programID = HelperFunctions::LoadShaders("OpenGL/Shaders/VertexShader.glsl", "OpenGL/Shaders/FragmentShader.glsl");

	// Get a handle for our "MVP" uniform
	m_matrixID = glGetUniformLocation(m_programID, "MVP");

	// Read .obj file
	//TO-DO: REPLACE WITH MORE ROBUST FILE LOADER
	bool res = HelperFunctions::loadOBJ("../Resources/Test/Cube.obj", m_vertices, m_uvs, m_normals);

	//============================================================================================================
	//FRAME BUFFER OBJECT == RENDER TO TEXTURE
	// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
	m_quadProgramID = HelperFunctions::LoadShaders("OpenGL/Shaders/PassThroughVertexShader.glsl", "OpenGL/Shaders/TextureFragmentShader.glsl");
	m_texID = glGetUniformLocation(m_quadProgramID, "renderedTexture");
	m_timeID = glGetUniformLocation(m_quadProgramID, "time");

	m_bufferName = 0;
	glGenFramebuffers(1, &m_bufferName);
	glBindFramebuffer(GL_FRAMEBUFFER, m_bufferName);

	//the render texture
	glGenTextures(1, &m_renderTex);

	// "Bind" the newly created texture 
	glBindTexture(GL_TEXTURE_2D, m_renderTex);

	// Give an empty image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_WindowWidth, m_WindowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);


	// Set renderTex as our colour attachement #0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_renderTex, 0);

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
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,
	};

	glGenBuffers(1, &m_quadVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_quadVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertexBufferData), quadVertexBufferData, GL_STATIC_DRAW);


	glGenBuffers(1, &m_vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(glm::vec3), &m_vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_uvs.size() * sizeof(glm::vec2), &m_uvs[0], GL_STATIC_DRAW);

	return true;
}

void System::Shutdown()
{
	// Cleanup VBO
	glDeleteBuffers(1, &m_vertexBuffer);
	glDeleteBuffers(1, &m_uvBuffer);
	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);

	glfwTerminate();
}

void System::Run()
{
	while (!glfwWindowShouldClose(m_window))
	{
		//INPUT
		ProcessUserInput();

		//RENDERING
		glBindFramebuffer(GL_FRAMEBUFFER, m_bufferName);
		glViewport(0, 0, m_WindowWidth, m_WindowHeight); // Render on the whole framebuffer, complete from the lower left corner to the upper right


		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(m_programID);

		// Compute the MVP matrix from keyboard and mouse input
		//computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
		glm::mat4 ViewMatrix = glm::lookAt(
			glm::vec3(4, 3, 3), // Camera is at (4,3,3), in World Space
			glm::vec3(0, 0, 0), // and looks at the origin
			glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
		);

		//glm::mat4 ProjectionMatrix = getProjectionMatrix();
		//glm::mat4 ViewMatrix = getViewMatrix();
		glm::mat4 ModelMatrix = glm::mat4(1.0);
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(m_matrixID, 1, GL_FALSE, &MVP[0][0]);
		//glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		//glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

		// 1rst attribute buffer : m_vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
		glVertexAttribPointer(
			0,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// 2nd attribute buffer : UVs
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, m_uvBuffer);
		glVertexAttribPointer(
			1,                                // attribute
			2,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, (GLsizei)m_vertices.size());

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);


		// POST-PROCESSINGm RENDER TO THE SCREEN
		//===================================================================================
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// Render on the whole framebuffer, complete from the lower left corner to the upper right
		glViewport(0, 0, m_WindowWidth, m_WindowHeight);

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(m_quadProgramID);

		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_renderTex);
		// Set our "renderedTexture" sampler to use Texture Unit 0
		glUniform1i(m_texID, 0);

		glUniform1f(m_timeID, (float)(glfwGetTime()*10.0f));

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


		// Swap buffers
		glfwSwapBuffers(m_window);
	}
}


void System::ProcessUserInput()
{
	//close on pressing escape
	if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(m_window, true);
	}


	if (glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		//if (ScreenShot("test.tga", 800, 600) == false)
		//{
		//	std::cout << "Failed making a screenshot" << std::endl;
		//}

		GetMatFromOpenGL();
	}

	/* Poll for and process events */
	glfwPollEvents();
}

//SAVES THE OUTPUT IMAGE TO A TGA FILE
bool System::ScreenShot(std::string fileName, int windowWidth, int windowHeight)
{
	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	const int nSize = windowWidth * windowHeight * 3;
	// create buffer
	char* dataBuffer = (char*)malloc(nSize * sizeof(char));	//malloc to be able to create the buffer with a 'dynamic' size

	if (!dataBuffer) return false;

	//fetch the backbuffer
	glReadPixels((GLint)0, (GLint)0,
		(GLint)windowWidth, (GLint)windowHeight,
		GL_BGR, GL_UNSIGNED_BYTE, dataBuffer);

	//Now the file creation
	FILE *filePtr = fopen(fileName.c_str(), "wb");
	if (!filePtr) return false;
	
	
	unsigned char TGAheader[12] = { 0,0,2,0,0,0,0,0,0,0,0,0 };
	unsigned char header[6] = { windowWidth % 256,windowWidth / 256,
					windowHeight % 256,windowHeight / 256,
					24,0 };
	// We write the headers
	fwrite(TGAheader, sizeof(unsigned char), 12, filePtr);
	fwrite(header, sizeof(unsigned char), 6, filePtr);
	// And finally our image data
	fwrite(dataBuffer, sizeof(GLubyte), nSize, filePtr);
	fclose(filePtr);

	free(dataBuffer);	//release the buffer created with malloc

	return true;
}

cv::Mat System::GetMatFromOpenGL()
{
	glBindTexture(GL_TEXTURE_2D, m_bufferName);
	GLenum texWidth, texHeight;

	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, (GLint*)&texWidth);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, (GLint*)&texHeight);

	unsigned char* texBytes = (unsigned char*)malloc(sizeof(unsigned char)*texWidth*texHeight * 3);
	glGetTexImage(GL_TEXTURE_2D, 0 /* mipmap level */, GL_BGR, GL_UNSIGNED_BYTE, texBytes);

	cv::Mat flipped = cv::Mat(texHeight, texWidth, CV_8UC3, texBytes); // this is the flipped image
	cv::Mat dest = flipped;
	cv::flip(flipped, dest,0 );	
	ImageOperations::ExtractSilhouette(dest, flipped);	//extract the silhouette from non-flipped and put it back in 'flipped'

	cv::imshow("main", flipped);
	return flipped;
}