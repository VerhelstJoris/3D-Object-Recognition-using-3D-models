
#include <iostream>
#include <vector>
#include <opencv2/core.hpp>

#include "OpenGL/OGLRenderer.h"
#include "OpenCV/ContourMatcher.h"

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include "OpenCV/ImageOperations.h"

//assimp test
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

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
		std::cout << "FAILED TO create the matchingObject" << std::endl;
		return 0;
	}

	//initialize the renderer
	result = ContourRendererObject->Initialize("../Resources/Stopsign/stopsign.obj", 800 , 600);
	if (result)
	{
		ContourRendererObject->Run();
	}
	else
	{
		std::cout << "FAILED TO INITIALIZE THE CONTOUR RENDERER OBJECT" << std::endl;
		return 0;
	}

	//retrieve the vector of cv::Mat screenshots from the renderer
	std::vector<RenderStruct> renderInfoVec = ContourRendererObject->GetScreenRenders();
	matchingObject->Initialize(renderInfoVec);
	
	//OPENCV
	//==================================================================
	cv::Mat testImg = cv::imread("../Resources/Test/test1.jpg");
	cv::Mat testImg2 = cv::imread("../Resources/Test/test1_rotated.jpg");
	//cv::Mat testImg = cv::imread("../Resources/Test/test1_rotated.jpg");

	// Shutdown and release the RENDERER object.
	ContourRendererObject->Shutdown();
	delete ContourRendererObject;
	ContourRendererObject = 0;

#pragma region DRAWGENERATEDCONTOURS

	//TESTING
	//=======================
	//std::vector<std::vector<cv::Point>> contourTestImg;
	//std::vector<cv::Vec4i> contourHierarchy;

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
	//imshow("IMAGE CONTOUR", drawing);
	//imshow("IMAGE", testImg);
#pragma endregion 

	//CONTOUR MATCHING TEST
	int testResult = matchingObject->MatchImgAgainstContours(testImg);
	imshow("MATCHING CONTOUR", matchingObject->ContourToMat(testResult));

	int testResult2 = matchingObject->MatchImgAgainstContours(testImg2);
	imshow("MATCHING CONTOUR ROTATED", matchingObject->ContourToMat(testResult2));

	//create new OGLRenderer object to display the test image on the far clipping plane and display model overtop of it
	//create new object instead of reusing because resizing the window at runtime isn't easy
	//DisplayRendererObject = new OGLRenderer;
	//if (!DisplayRendererObject)
	//{
	//	std::cout << "FAILED TO CREATE THE DISPLAY RENDERER OBJECT" << std::endl;
	//	return 0;
	//}
	//               
	//result = DisplayRendererObject->Initialize("../Resources/Stopsign/stopsign.obj", testImg.size().width, testImg.size().height);
	//if (result)
	//{
	//	DisplayRendererObject->SwitchToDisplayMode(testImg);
	//	DisplayRendererObject->SetModelOrientation(glm::vec3{ renderInfoVec[testResult].rotationX,renderInfoVec[testResult].rotationY,renderInfoVec[testResult].rotationZ });
	//	DisplayRendererObject->Run();
	//}
	//else
	//{
	//	std::cout << "FAILED TO INITIALIZE THE DISPLAY RENDERER OBJECT" << std::endl;
	//	return 0;
	//}


	//SHUTDOWN
	//====================
	cv::waitKey();
	cv::destroyAllWindows();

	// Shutdown and release the DISPLAY object.
	//DisplayRendererObject->Shutdown();
	//delete DisplayRendererObject;
	//DisplayRendererObject = 0;

	return 0;
}

