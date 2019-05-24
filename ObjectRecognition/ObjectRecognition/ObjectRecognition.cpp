#include <GLFW/glfw3.h>


#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>


#include "DisplayWindow.h"
#include "ImageOperations.h"

using namespace cv;
using namespace std;

int main(void)
{
	Mat image1, image2, destination;
	image1 = imread("../Resources/Stopsign/render1.png"); // Read the file
	image2 = imread("../Resources/Stopsign/render3.png"); // Read the file
	destination = image1;

	if (image1.empty()) // Check for invalid input
	{
		cout << "Could not open or find the image" << std::endl;
		return -1;
	}
	if (image2.empty()) // Check for invalid input
	{
		cout << "Could not open or find the image" << std::endl;
		return -1;
	}

	//MAIN WINDOW
	DisplayWindow* m_MainWindow = new DisplayWindow("MAIN", WINDOW_AUTOSIZE);
	//m_MainWindow->ChangeWindowSize(600, 600);
	m_MainWindow->ChangeWindowTitle("Main");

	ImageOperations::ExtractSilhouette(image1, destination, 30, 50, false);

	imshow(m_MainWindow->GetName(), destination); // Show our image inside it.

	//SECONDARY WINDOW
	DisplayWindow* m_SecondWindow = new DisplayWindow("SECONDARY", WINDOW_AUTOSIZE);
	//m_SecondWindow->ChangeWindowSize(250, 250);
	m_SecondWindow->ChangeWindowTitle("Normal");
	imshow(m_SecondWindow->GetName(), image1); // Show our image inside it.



	//OPENGL
	//============================================================
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}