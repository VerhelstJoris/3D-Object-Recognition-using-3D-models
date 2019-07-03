
#include <iostream>
#include <vector>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

//rotation test includes
#include <opencv2\opencv.hpp>
#include "opencv2/shape.hpp"
#include "opencv2\opencv_modules.hpp"


#include "OpenGL/OGLRenderer.h"
#include "OpenCV/ContourMatcher.h"

#include "OpenCV/ImageOperations.h"

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
	//result = ContourRendererObject->Initialize("../Resources/Test/Chair.obj", 800 , 600);
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
	cv::Mat testImg = cv::imread("../Resources/Test/test1_rotated.jpg");
	//cv::Mat testImg = cv::imread("../Resources/Test/chair1.jpg");

	
	// Shutdown and release the RENDERER object.
	ContourRendererObject->Shutdown();
	delete ContourRendererObject;
	ContourRendererObject = 0;


	//CONTOUR MATCHING TEST
	ContourMatchOut testResult2 = matchingObject->MatchImgAgainstContours(testImg);
	imshow("MATCHING CONTOUR", matchingObject->ContourToMat(testResult2.lowestRenderID));
	
#pragma region ROTATION
	
	//TEST FOR ROTATION
	std::cout << "=======================================" << std::endl << "ROTATION TEST" << std::endl << std::endl;

	//drwaing related
	auto size = testImg.size();
	cv::Mat drawing = cv::Mat::zeros(size.height * 2, size.width * 2, CV_8UC3);	//create a mat the size of the screenshot (contour img has the same size)
	cv::Scalar color;
	
	std::vector<std::vector<cv::Point>> drawContVec;

	//translate render match cont
	cv::Point2f massCentre;
	float diagonalLength;

	std::vector<cv::Point> renderContTrans(testResult2.lowestContourRender.size());
	ImageOperations::TranslateContourToPoint(testResult2.lowestContourRender, renderContTrans, cv::Point(size.width/2,size.height/2), massCentre,diagonalLength);

	//translate image cont
	cv::Point2f massCentre2;
	float diagonalLength2;

	std::vector<cv::Point> imageContTrans(testResult2.lowestContourImage.size());
	ImageOperations::TranslateContourToPoint(testResult2.lowestContourImage, imageContTrans, cv::Point(size.width / 2, size.height / 2), massCentre2, diagonalLength2);

	//just for drawing
	drawContVec.push_back(renderContTrans);
	drawContVec.push_back(imageContTrans);
	cv::drawContours(drawing, drawContVec, 0, cv::Scalar(0,255,0));
	cv::drawContours(drawing, drawContVec, 1, cv::Scalar(0,255,255),2);

	//DISTANCE TEST
	//NEED SCALING
	//cv::Ptr <cv::ShapeContextDistanceExtractor> mysc = cv::createShapeContextDistanceExtractor();

	//float lowestDistance = FLT_MAX;
	//int lowestID = 0;
	//for (size_t i = 0; i < 60; i++)
	//{
	//	std::vector<cv::Point> rotated;
	//	ImageOperations::RotateContour(renderContTrans, rotated, 6 * i, cv::Point(size.width / 2, size.height / 2));

	//	float dis = mysc->computeDistance(rotated, imageContTrans);
	//	std::cout << dis << " | ";

	//	if (dis <= lowestDistance)
	//	{
	//		lowestDistance = dis;
	//		lowestID = i;
	//	}
	//}
	//std::cout << std::endl;	

	//std::vector<cv::Point> resultRot;
	//ImageOperations::RotateContour(renderContTrans, resultRot, 6 * lowestID, cv::Point(size.width / 2, size.height / 2));
	//
	//drawContVec.push_back(resultRot);
	//cv::drawContours(drawing, drawContVec, 2, cv::Scalar(255, 255, 0));

	//std::cout << lowestDistance << " with rotation: " << lowestID * 6 << std::endl;

	cv::imshow("CONTOURS TRANSLATED", drawing);


#pragma endregion


#pragma region DISPLAY
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
	//	int id = testResult2.lowestRenderID;
	//	DisplayRendererObject->SwitchToDisplayMode(testImg);
	//	DisplayRendererObject->SetModelOrientation(glm::vec3{ renderInfoVec[id].rotationX ,renderInfoVec[id].rotationY ,renderInfoVec[id].rotationZ + finalRot });
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

#pragma endregion

	return 0;
}

