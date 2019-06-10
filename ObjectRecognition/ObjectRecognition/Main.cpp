
#include <iostream>
#include <vector>
#include <opencv2/core.hpp>

#include "OpenGL/OGLRenderer.h"
#include "OpenCV/ContourMatcher.h"

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

int main(void)
{
	//OPENGL object renderer
	OGLRenderer* systemObject;
	ContourMatcher* matchingObject;

	bool result;

	// Create the system object.
	systemObject = new OGLRenderer;
	if (!systemObject)
	{
		return 0;
	}

	//create the contourmatcher
	matchingObject = new ContourMatcher();
	if (!matchingObject)
	{
		return 0;
	}

	//initialize the renderer
	result = systemObject->Initialize("../Resources/Test/Suzanne.obj");
	if (result)
	{
		systemObject->Run();
	}

	//retrieve the vector of cv::Mat screenshots from the renderer
	matchingObject->Initialize(systemObject->GetScreenRenders());
	
	// Shutdown and release the OPENGL object.
	systemObject->Shutdown();
	delete systemObject;
	systemObject = 0;


	//CONTOUR MATCHING TEST
	cv::Mat testImg = cv::imread("../Resources/Test/Capture3.jpg");
	
	int testResult =	matchingObject->MatchImgAgainstContours(testImg);

	//imshow("ContourFit", matchingObject->ContourToMat(testResult));
	imshow("ContourFit", matchingObject->ContourToMat(1));
	imshow("TestImage", testImg);
	
	cv::waitKey();
	std::cin.get();


	return 0;
}

