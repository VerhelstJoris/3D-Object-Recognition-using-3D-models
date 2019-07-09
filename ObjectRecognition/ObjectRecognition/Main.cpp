
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
	//cv::Mat testImg = cv::imread("../Resources/Test/test2_rotated.jpg");
	//cv::Mat testImg = cv::imread("../Resources/Test/test2_rotated2.jpg");
	//cv::Mat testImg = cv::imread("../Resources/Test/test2_rotated3.jpg");
	//cv::Mat testImg = cv::imread("../Resources/Test/test2_rotated4.jpg");
	cv::Mat testImg = cv::imread("../Resources/Test/test2_rotated5.jpg");
	//cv::Mat testImg = cv::imread("../Resources/Test/test3.jpg");

	
	// Shutdown and release the RENDERER object.
	ContourRendererObject->Shutdown();
	delete ContourRendererObject;
	ContourRendererObject = 0;


	//CONTOUR MATCHING TEST
	ContourMatchOut testResult2 = matchingObject->MatchImgAgainstContours(testImg);
	//imshow("MATCHING CONTOUR", matchingObject->ContourToMat(testResult2.lowestRenderID));
	
#pragma region ROTATION
	
	//TEST FOR ROTATION
	std::cout << std::endl << "=======================================" << std::endl << "ROTATION TEST" << std::endl << std::endl;

	auto size = testImg.size();

	//drawing related
	cv::Mat drawing = cv::Mat::zeros(size.height , size.width , CV_8UC3);	//create a mat the size of the screenshot (contour img has the same size)
	cv::Scalar color;
	
	std::vector<std::vector<cv::Point>> drawContVec;

#pragma region TRANSLATECONTOURS
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

#pragma endregion

	//just for drawing
	drawContVec.push_back(imageContTrans);
	cv::drawContours(drawing, drawContVec, 0, cv::Scalar(0,255,0));


#pragma region SCALE
	bool imageSideways = false;

	cv::RotatedRect minAreaImage, minAreaRender;
	minAreaImage = cv::minAreaRect(imageContTrans);
	minAreaRender = cv::minAreaRect(renderContTrans);

	float largestElemImg, largestElemRender;
	largestElemImg = std::max(minAreaImage.size.width, minAreaImage.size.height);
	largestElemRender = std::max(minAreaRender.size.width, minAreaRender.size.height);

	std::cout << "IMAGE WIDTH: " << minAreaImage.size.width << " HEIGHT: " << minAreaImage.size.height << std::endl;
	std::cout << "RENDER WIDTH: " << minAreaRender.size.width << " HEIGHT: " << minAreaRender.size.height << std::endl;

	float scaleAmount = largestElemImg / largestElemRender;
	std::cout << "SCALE AMOUNT: " << scaleAmount << std::endl;

	std::vector<cv::Point> scaledRenderContour, renderContShuffled, imageContShuffled;


	ImageOperations::ScaleContour(renderContTrans, scaledRenderContour, cv::Point(size.width / 2, size.height / 2), scaleAmount);


#pragma endregion

#pragma region DISTANCE

	std::cout << std::endl << "=======================================" << std::endl << "DISTANCE CHECKS" << std::endl << std::endl;

	

	int highestAmountOfPoints = std::max(scaledRenderContour.size(), imageContTrans.size());
	//shuffle both contours for uniform sampling
	ImageOperations::simpleContour(scaledRenderContour, renderContShuffled, highestAmountOfPoints);
	ImageOperations::simpleContour(imageContTrans, imageContShuffled, highestAmountOfPoints);

	//DISTANCE TEST
	cv::Ptr <cv::ShapeContextDistanceExtractor> mysc = cv::createShapeContextDistanceExtractor();

	//float lowestDistance = FLT_MAX;
	//int lowestID = 0;
	//double angle = 1.0;
	//for (size_t i = 0; i < 360.0/ angle; i++)
	//{
	//	std::vector<cv::Point> rotated;
	//	ImageOperations::RotateContour(renderContShuffled, rotated, angle * i, cv::Point(size.width / 2, size.height / 2));
	//	
	//	float dis = mysc->computeDistance(rotated, imageContShuffled);
	//	std::cout << dis << " | ";
	//	
	//	if (dis <= lowestDistance)
	//	{
	//		lowestDistance = dis;
	//		lowestID = i;
	//	}
	//
	//}
	//std::cout << std::endl;	
	//
	//
	//double resultingRot = lowestID * angle;
	//std::cout << lowestDistance << " with rotation: " << resultingRot << " at ID: " << lowestID << std::endl;

#pragma endregion

#pragma region MINAREARECT
	std::cout << "=======================================" << std::endl << "ROTATED RECT ANGLE" << std::endl << std::endl;

	//fmod is % operator
	//double cappedRot = resultingRot - std::fmod(resultingRot, 90.0);

	//std::cout << resultingRot << "-> " << cappedRot << std::endl;

	//ROTATED RECT TEST
	double angle1, angleRect1, angle2, angleRect2;

	//std::vector<cv::Point> resultRot;
	//ImageOperations::RotateContour(scaledRenderContour, resultRot, resultingRot, cv::Point(size.width / 2, size.height / 2));

	//ImageOperations::AngleContour(resultRot, angle1, angleRect1);
	//std::cout << "RENDER CONTOUR ANGLES: " << angle1 << ", " << angleRect1 << std::endl;
	
	ImageOperations::AngleContour(imageContTrans, angle2, angleRect2);
	std::cout << "IMAGE CONTOUR ANGLES: " << angle2 << ", " << angleRect2 << std::endl;

	//image angle should depend on whether or not it's sideways or not
	//cappedRot += angle2;
	//cappedRot = resultingRot;

	float lowestDistance = FLT_MAX;
	int lowestID = 0;
	for (size_t i = 0; i < 4; i++)
	{
		std::vector<cv::Point> rotated;
		ImageOperations::RotateContour(renderContShuffled, rotated, angle2 + (i*90), cv::Point(size.width / 2, size.height / 2));

		float dis = mysc->computeDistance(rotated, imageContShuffled);
		std::cout << dis << " | ";

		if (dis <= lowestDistance)
		{
			lowestDistance = dis;
			lowestID = i;
		}
	}
	double angle = angle2 + (lowestID * 90);

	std::cout << "FINAL ANGLE: " << angle << std::endl;

	std::vector<cv::Point> finalRot;

	ImageOperations::RotateContour(scaledRenderContour, finalRot, angle, cv::Point(size.width / 2, size.height / 2));

	//remove later
	drawContVec.push_back(finalRot);
	cv::drawContours(drawing, drawContVec, 1, cv::Scalar(255, 255, 255));

#pragma endregion



#pragma endregion


#pragma region DISPLAY
	//create new OGLRenderer object to display the test image on the far clipping plane and display model overtop of it
	//create new object instead of reusing because resizing the window at runtime isn't easy
	DisplayRendererObject = new OGLRenderer;
	if (!DisplayRendererObject)
	{
		std::cout << "FAILED TO CREATE THE DISPLAY RENDERER OBJECT" << std::endl;
		return 0;
	}
	               
	result = DisplayRendererObject->Initialize("../Resources/Stopsign/stopsign.obj", testImg.size().width, testImg.size().height);
	if (result)
	{
		int id = testResult2.lowestRenderID;
		DisplayRendererObject->SwitchToDisplayMode(drawing);
		DisplayRendererObject->SetModelOrientation(glm::vec3{ renderInfoVec[id].rotationX ,renderInfoVec[id].rotationY ,renderInfoVec[id].rotationZ + (angle*DEG2RAD) });
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
	std::cout << "SHUTTING DOWN" << std::endl;

	cv::destroyAllWindows();


	//Shutdown and release the DISPLAY object.
	DisplayRendererObject->Shutdown();
	delete DisplayRendererObject;
	DisplayRendererObject = 0;

#pragma endregion

	return 0;
}

