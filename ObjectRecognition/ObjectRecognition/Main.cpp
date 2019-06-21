
#include <iostream>
#include <vector>
#include <opencv2/core.hpp>

#include "OpenGL/OGLRenderer.h"
#include "OpenCV/ContourMatcher.h"

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include "OpenCV/ImageOperations.h"

//assimp test
//#include <assimp/scene.h>
//#include <assimp/Importer.hpp>
//#include <assimp/postprocess.h>

int main(void)
{
	//OPENGL object renderer
	OGLRenderer* ContourRendererObject, *DisplayRendererObject;
	ContourMatcher* matchingObject;

	bool result;

	// Create the system object.
	ContourRendererObject = new OGLRenderer;
	if (!ContourRendererObject)
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
	result = ContourRendererObject->Initialize("../Resources/Test/Suzanne.obj", 800 , 600);
	if (result)
	{
		ContourRendererObject->Run();
	}

	//retrieve the vector of cv::Mat screenshots from the renderer
	matchingObject->Initialize(ContourRendererObject->GetScreenRenders());
	
	//OPENCV
	//==================================================================
	cv::Mat testImg = cv::imread("../Resources/Test/Capture2.jpg");
	imshow("TestImage", testImg);

	// Shutdown and release the OPENGL object.
	ContourRendererObject->Shutdown();
	delete ContourRendererObject;
	ContourRendererObject = 0;

	//CONTOUR MATCHING TEST
	int testResult = matchingObject->MatchImgAgainstContours(testImg);
	imshow("ContourFit", matchingObject->ContourToMat(testResult));
	//imshow("Contour", drawing);
	//imshow("TestImage", testImg);

	//TESTING
	//create new object instead of reusing because resizing the window at runtime isn't easy
	DisplayRendererObject = new OGLRenderer;
	if (!DisplayRendererObject)
	{
		return 0;
	}

	result = DisplayRendererObject->Initialize("../Resources/Test/Suzanne.obj", testImg.size().width, testImg.size().height);
	if (result)
	{
		DisplayRendererObject->SwitchToDisplayMode(testImg);
		DisplayRendererObject->Run();
	}

	//std::vector<std::vector<cv::Point>> contourTestImg;
	//std::vector<cv::Vec4i> contourHierarchy;
	//
	//std::cout << matchingObject->GetAverageAreaRenders() << std::endl;
	//
	//ImageOperations::ExtractContourFromImage(testImg, contourTestImg, contourHierarchy, matchingObject->GetAverageAreaRenders(), matchingObject->GetAverageSquarenessRenders());
	//
	////DRAW CONTOURS
	//auto size = testImg.size();
	//cv::Mat drawing = cv::Mat::zeros(size.height, size.width, CV_8UC3);	//create a mat the size of the screenshot (contour img has the same size)
	//
	//cv::Scalar color;
	//std::vector<cv::Vec4i> hierarchy;
	//
	//cv::RNG rng(12345);
	//
	//for (int i = 0; i < contourTestImg.size(); i++)
	//{
	//	cv::Scalar color;
	//	//if (contourHierarchy[i][2] != -1) 
	//	{
	//		// random colour
	//		color = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
	//		drawContours(drawing, contourTestImg, i, color, 0.5, 8, hierarchy, 0, cv::Point());
	//	}
	//
	//}
	//
	//imshow("Contour", drawing);
	//imshow("TestImage", testImg);
	//int id = matchingObject->MatchImgAgainstContours(testImg);
	//imshow("TestContours", matchingObject->ContourToMat(id));
	

	cv::waitKey();
	cv::destroyAllWindows();




	return 0;
}

