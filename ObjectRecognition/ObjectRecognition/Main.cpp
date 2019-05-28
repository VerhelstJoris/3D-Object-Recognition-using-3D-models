#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>


#include "OpenCV/DisplayWindow.h"
#include "OpenCV/ImageOperations.h"
#include "OpenCV/HelperFunctions.h"

using namespace cv;
using namespace std;

void ProcessUserInput(GLFWwindow *window);
void Draw(GLuint vBuffer);

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
		return -1;

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


	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = HelperFunctions::LoadShaders("OpenGL/Shaders/VertexShader.glsl", "OpenGL/Shaders/FragmentShader.glsl");


	static const GLfloat g_vertex_buffer_data[] = {
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 0.0f,  1.0f, 0.0f,
	};

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);



	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		//INPUT
		ProcessUserInput(window);

		//RENDERING
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);		//set color
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(programID);

		Draw(vertexbuffer);

		//SWAP BUFFERS
		glfwSwapBuffers(window);

	}

	// Cleanup VBO
	glDeleteBuffers(1, &vertexbuffer);
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

	/* Poll for and process events */
	glfwPollEvents();
}

void Draw(GLuint vBuffer)
{
	// 1st attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
	glDisableVertexAttribArray(0);
}