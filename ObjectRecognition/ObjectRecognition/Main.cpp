#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>

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
		return -1;
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

	glViewport(0, 0, 800, 600);		//specify the size of the rendering window

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);


	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		//INPUT
		ProcessUserInput(window);

		//RENDERING
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);		//set color
		glClear(GL_COLOR_BUFFER_BIT);				//clear using previously set color

		//SWAP BUFFERS
		glfwSwapBuffers(window);

	}

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