
#include <iostream>
#include <vector>
#include <opencv2/core.hpp>

#include "OpenGL/OGLRenderer.h"
#include "OpenCV/ContourMatcher.h"

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "OpenCV/ImageOperations.h"

//assimp test
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

const float DEG2RAD = 0.0174532925f;



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
	//cv::Mat testImg = cv::imread("../Resources/Test/test1.jpg");
	cv::Mat testImg2 = cv::imread("../Resources/Test/test1_rotated.jpg");

	
	// Shutdown and release the RENDERER object.
	ContourRendererObject->Shutdown();
	delete ContourRendererObject;
	ContourRendererObject = 0;

#pragma region DRAWGENERATEDCONTOURS

	////TESTING
	////=======================
	//std::vector<std::vector<cv::Point>> contourTestImg;
	//std::vector<cv::Vec4i> contourHierarchy;
	//
	//ImageOperations::ExtractContourFromImage(testImg, contourTestImg, contourHierarchy, matchingObject->GetAverageAreaRenders(), matchingObject->GetAverageSquarenessRenders());

	//int idx = 0;
	//same hierarchy -> SAME COLOUR
	//for (; idx >= 0; idx = hierarchy[idx][0])
	//{
	//	Scalar color(rand() & 255, rand() & 255, rand() & 255);
	//	drawContours(dst, contours, idx, color, FILLED, 8, hierarchy);
	//}
	

#pragma endregion 

	//CONTOUR MATCHING TEST
	ContourMatchOut testResult2 = matchingObject->MatchImgAgainstContours(testImg2);
	imshow("MATCHING CONTOUR ROTATED", matchingObject->ContourToMat(testResult2.lowestRenderID));

	
	//TEST FOR ROTATION
	std::cout << "=======================================" << std::endl << "ROTATION TEST" << std::endl << std::endl;

	//drwaing related
	auto size = testImg2.size();
	cv::Mat drawing = cv::Mat::zeros(size.height *2, size.width *2 , CV_8UC3);	//create a mat the size of the screenshot (contour img has the same size)
	cv::Scalar color;
	std::vector<cv::Vec4i> hierarchy;
	std::vector<std::vector<cv::Point>> contourRotVec;

	cv::RNG rng(12345);

	//rotation related
	std::vector<cv::Point> contourRot;
	double lowestResult = 1000000.0;
	int lowestID = 0;
	float finalRot = 0.0f;

	for (size_t i = 0; i < 60; i++)
	{
		float rot = 6 * i * DEG2RAD;

		cv::Rect rect = cv::boundingRect(testResult2.lowestContourRender);
		cv::Point center = { rect.x + (rect.width / 2),rect.y + (rect.height / 2) };
		ImageOperations::RotateContour(testResult2.lowestContourRender, contourRot, 6 * i, center);
		contourRotVec.push_back(contourRot);
		double result = cv::matchShapes(contourRot, testResult2.lowestContourImage, cv::CONTOURS_MATCH_I1, 0.0);

		std::cout << result << " | ";
		if (result <= lowestResult)
		{
			lowestResult = result;
			lowestID = i;
			finalRot = rot;
		}
	}


	//for (size_t i = 0; i < contourRotVec.size(); i++)
	//{
	//	cv::Scalar color(rand() & 255, rand() & 255, rand() & 255);
	//	cv::drawContours(drawing, contourRotVec, i, color,1, 8, hierarchy);
	//}
	//
	//imshow("IMAGE CONTOUR", drawing);


	std::cout <<std::endl << "BEST MATCH WITH: " << lowestID * 6 << " degrees rotation with value: " << lowestResult << std::endl << std::endl;

	//create new OGLRenderer object to display the test image on the far clipping plane and display model overtop of it
	//create new object instead of reusing because resizing the window at runtime isn't easy
	DisplayRendererObject = new OGLRenderer;
	if (!DisplayRendererObject)
	{
		std::cout << "FAILED TO CREATE THE DISPLAY RENDERER OBJECT" << std::endl;
		return 0;
	}
	               
	result = DisplayRendererObject->Initialize("../Resources/Stopsign/stopsign.obj", testImg2.size().width, testImg2.size().height);
	if (result)
	{
		int id = testResult2.lowestRenderID;
		DisplayRendererObject->SwitchToDisplayMode(testImg2);
		DisplayRendererObject->SetModelOrientation(glm::vec3{ renderInfoVec[id].rotationX ,renderInfoVec[id].rotationY ,renderInfoVec[id].rotationZ + finalRot });
		DisplayRendererObject->Run();
	}
	else
	{
		std::cout << "FAILED TO INITIALIZE THE DISPLAY RENDERER OBJECT" << std::endl;
		return 0;
	}


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

