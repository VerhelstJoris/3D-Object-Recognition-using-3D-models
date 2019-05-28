#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdio.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include "OpenCV/DisplayWindow.h"
#include "OpenCV/ImageOperations.h"
#include "OpenCV/HelperFunctions.h"

using namespace cv;
using namespace std;

void ProcessUserInput(GLFWwindow *window);

int main(void)
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
		cin.get();
		return -1;

	}

	glfwWindowHint(GLFW_SAMPLES, 4);					//amount of samples per pixel
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);		//set to version 3.3 of opengl
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	//only use core profile


	GLFWwindow* window = glfwCreateWindow(800, 600, "ModelViewer", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);


	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);


	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = HelperFunctions::LoadShaders("OpenGL/Shaders/VertexShader.glsl", "OpenGL/Shaders/FragmentShader.glsl");

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	// Read our .obj file
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals; // Won't be used at the moment.
	bool res = HelperFunctions::loadOBJ("Cube.obj", vertices, uvs, normals);

	// Load it into a VBO

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		//INPUT
		ProcessUserInput(window);

		//RENDERING
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

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
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
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
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glVertexAttribPointer(
			1,                                // attribute
			2,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		// Swap buffers
		glfwSwapBuffers(window);

	}

	// Cleanup VBO
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteProgram(programID);

	glfwTerminate();
	return 0;
}

void ProcessUserInput(GLFWwindow *window)
{
	//close on pressing escape
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{

	}

	/* Poll for and process events */
	glfwPollEvents();
}

