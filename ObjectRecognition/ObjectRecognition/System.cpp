#include "System.h"

#include <stdio.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>


#include <gtc/matrix_transform.hpp>

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


	m_window = glfwCreateWindow(800, 600, "ModelViewer", NULL, NULL);
	if (m_window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(m_window);


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
	m_MatrixID = glGetUniformLocation(m_programID, "MVP");

	// Read our .obj file
	bool res = HelperFunctions::loadOBJ("../Resources/Test/Cube.obj", m_vertices, m_uvs, m_normals);

	//std::vector<glm::vec3> indexed_vertices;
	//std::vector<glm::vec2> indexed_uvs;
	//std::vector<glm::vec3> indexed_normals;
	//indexVBO(vertices, uvs, normals, indices, indexed_vertices, indexed_uvs, indexed_normals);


	// Load it into a VBO

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
		glUniformMatrix4fv(m_MatrixID, 1, GL_FALSE, &MVP[0][0]);

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
		glDrawArrays(GL_TRIANGLES, 0, m_vertices.size());

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

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


	/* Poll for and process events */
	glfwPollEvents();
}

