
#include <iostream>
#include <vector>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

//momentsAngle test includes
#include <opencv2\opencv.hpp>
#include "opencv2/shape.hpp"
#include "opencv2\opencv_modules.hpp"


#include "OpenGL/OGLRenderer.h"
#include "OpenCV/ContourMatcher.h"

#include "OpenCV/ImageOperations.h"
#include "OpenGL/OGLHelperFunctions.h"

const float DEG2RAD = 0.0174532925f;
const float RAD2DEG = 1/0.0174532925f;

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
	result = ContourRendererObject->Initialize("../Resources/Stopsign/stopsign.obj", 800 , 600, RENDERER_MODE::CAMERAMOVE);
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
	//cv::Mat testImg = cv::imread("../Resources/Test/test2_rotated3.jpg");
	//cv::Mat testImg = cv::imread("../Resources/Test/test2_rotated3_chunked.jpg");
	//cv::Mat testImg = cv::imread("../Resources/Test/test2_rotated3_chunked2.jpg");
	//cv::Mat testImg = cv::imread("../Resources/Test/test2_rotated3_scale.jpg");
	//cv::Mat testImg = cv::imread("../Resources/Test/test2_rotated4.jpg");
	//cv::Mat testImg = cv::imread("../Resources/Test/test2_rotated5.jpg");
	//cv::Mat testImg = cv::imread("../Resources/Test/test3.jpg");

	//RENDER WITH TEX AND BASIC SHADING
	//cv::Mat testImg = cv::imread("../Resources/Test/Render1.png");
	//cv::Mat testImg = cv::imread("../Resources/Test/Render1_chunked.png");
	//cv::Mat testImg = cv::imread("../Resources/Test/Render1_chunked2.png");
	//cv::Mat testImg = cv::imread("../Resources/Test/Render1_bg.png");
	//cv::Mat testImg = cv::imread("../Resources/Test/Render2.png");
	//cv::Mat testImg = cv::imread("../Resources/Test/Render3.png");

	//FINAL IMAGE
	cv::Mat testImg = cv::imread("../Resources/Test/stopsign1.jpg");
	//cv::Mat testImg = cv::imread("../Resources/Test/stopsign4.jpg");

	if (!testImg.data)
	{
		std::cout << "FAILED TO LOAD IN IMAGE" << std::endl;
	}
	
	// Shutdown and release the RENDERER object.
	ContourRendererObject->Shutdown();
	delete ContourRendererObject;
	ContourRendererObject = 0;


	//CONTOUR MATCHING TEST
	ContourMatchOut testResult2 = matchingObject->MatchImgAgainstContours(testImg);
	
#pragma region ROTATION
	
	//TEST FOR ROTATION
	std::cout << std::endl << "=======================================" << std::endl << "ROTATION TEST" << std::endl << std::endl;

	auto imgSize = testImg.size();

	//drawing related
	cv::Mat drawing = cv::Mat::zeros(imgSize.height, imgSize.width, CV_8UC3);	//create a mat the imgSize of the screenshot (contour img has the same imgSize)
	cv::Scalar color;
	
	std::vector<std::vector<cv::Point>> drawContVec;

#pragma region TRANSLATECONTOURS
	////translate render match cont
	cv::Point2f massCentreRenderCont;
	float diagonalLengthRenderCont;
	
	std::vector<cv::Point> renderContTrans = testResult2.lowestContourRender;
	ImageOperations::FindBlobs(testResult2.lowestContourRender, massCentreRenderCont, diagonalLengthRenderCont);
	
	////translate image cont
	cv::Point2f massCentreImageCont;
	float diagonalLengthImageCont;
	
	std::vector<cv::Point> imageContTrans = testResult2.lowestContourImage;
	ImageOperations::FindBlobs(testResult2.lowestContourImage, massCentreImageCont, diagonalLengthImageCont);

#pragma endregion


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

	std::vector<cv::Point> scaledRenderContour;


	ImageOperations::ScaleContour(renderContTrans, scaledRenderContour, cv::Point(imgSize.width / 2, imgSize.height / 2), scaleAmount);


#pragma endregion

#pragma region DISTANCE

	std::cout << std::endl << "=======================================" << std::endl << "DISTANCE CHECKS" << std::endl << std::endl;

	std::vector<cv::Point> renderContShuffled, imageContShuffled;
	
	int highestAmountOfPoints = std::max(scaledRenderContour.size(), imageContTrans.size());
	//shuffle both contours for uniform sampling
	ImageOperations::simpleContour(scaledRenderContour, renderContShuffled, highestAmountOfPoints);
	//ImageOperations::simpleContour(scaledRenderContour, renderContShuffled, scaledRenderContour.size());
	ImageOperations::simpleContour(imageContTrans, imageContShuffled, highestAmountOfPoints);
	//ImageOperations::simpleContour(imageContTrans, imageContShuffled, imageContTrans.size());

	//DISTANCE TEST
	cv::Ptr <cv::ShapeContextDistanceExtractor> mysc = cv::createShapeContextDistanceExtractor();
	mysc->setAngularBins(60);

	double imageAngle, angleRect;
	
	ImageOperations::AngleContour(imageContTrans, imageAngle, angleRect);
	std::cout << "IMAGE CONTOUR ANGLES: " << imageAngle << ", " << angleRect << std::endl;

	auto moments = cv::moments(imageContTrans, false);

	float momentsAngle = 0.5f*(atan((2 * moments.mu11) / ((2 * moments.mu20) - (2 * moments.mu02))));
	std::cout << "MOMENTS ANGLES: " << momentsAngle * RAD2DEG << std::endl;


	float lowestDistance = FLT_MAX;
	int lowestID = 0;
	for (size_t i = 0; i < 4; i++)
	{
		std::vector<cv::Point> rotated;
		ImageOperations::RotateContour(renderContShuffled, rotated, imageAngle + (i*90), massCentreRenderCont);


		float dis = mysc->computeDistance(rotated, imageContShuffled);
		std::cout << "Dist: " << dis << " | ";

		if (dis <= lowestDistance)
		{
			lowestDistance = dis;
			lowestID = i;
		}
	}
	

	//calculations compensating for wrong moments calculatios
	double angleAdjustment =0.0;
	double angleDiff = 1.0;
	//int iteration = 0;	//drawing
	for (int i = -4; i <= 4; i++)
	{
		if (i != 0)
		{
			double angleNew = imageAngle + (lowestID * 90) + (i * angleDiff);
			std::vector<cv::Point> rotatedNew;
			ImageOperations::RotateContour(renderContShuffled, rotatedNew, angleNew, massCentreRenderCont);
	
			float dis = mysc->computeDistance(rotatedNew, imageContShuffled);
			std::cout << "Dist: " << dis << " | ";
	
			//drawing
			//std::vector<cv::Point> drawingTest;
			//ImageOperations::RotateContour(scaledRenderContour, drawingTest, angleNew, massCentreRenderCont);
			//drawContVec.push_back(drawingTest);
			//
			//cv::drawContours(drawing, drawContVec, iteration, cv::Scalar(255, 0, 0));
			//iteration++;

			if (dis <= lowestDistance)
			{
				std::cout << std::endl	<< "Lowest Distance in rot checks with angle: " << i * angleDiff << std::endl;
				lowestDistance = dis;
				angleAdjustment = i * angleDiff;
			}


		}
	}
	
	
	double angle = imageAngle + (lowestID * 90.0) + angleAdjustment;

	std::cout << std::endl <<  "FINAL ANGLE: " << angle << std::endl;

	std::vector<cv::Point> finalRot;

	ImageOperations::RotateContour(scaledRenderContour, finalRot, angle, cv::Point(imgSize.width / 2, imgSize.height / 2));



#pragma endregion

#pragma DRAWING ONLY

	//Get image cont data
	cv::Point2f massCentreImgCont;
	float diagonalLength2;
	ImageOperations::FindBlobs(testResult2.lowestContourImage, massCentreImageCont, diagonalLength2);

	
	float diagonalLength;
	ImageOperations::TranslateContourToPoint(finalRot, renderContTrans, massCentreImageCont, massCentreRenderCont, diagonalLength);

	
	//remove later
	drawContVec.push_back(renderContTrans);
	drawContVec.push_back(imageContTrans);
	cv::drawContours(drawing, drawContVec, 0, cv::Scalar(0, 0, 255));
	cv::drawContours(drawing, drawContVec, 1, cv::Scalar(0, 255, 0));

	cv::imshow("RESULT", drawing);
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
	               
	result = DisplayRendererObject->Initialize("../Resources/Stopsign/stopsign.obj", imgSize.width, imgSize.height, RENDERER_MODE::DISPLAY);
	if (result)
	{
		int id = testResult2.lowestRenderID;
		//DisplayRendererObject->SwitchToDisplayMode(drawing);
		DisplayRendererObject->SwitchToDisplayMode(testImg);
		//massCentreRenderCont to massCentreImageCont SCREEN TO WORLD
		auto newPos = DisplayRendererObject->GetWorldCoordFromWindowCoord(glm::vec2(massCentreImageCont.x, massCentreImageCont.y), glm::vec2(imgSize.width, imgSize.height));
		auto originalPos = DisplayRendererObject->GetWorldCoordFromWindowCoord(glm::vec2(massCentreRenderCont.x, massCentreRenderCont.y), glm::vec2(imgSize.width, imgSize.height));


		DisplayRendererObject->SetModelPosition( glm::vec3(newPos.x , newPos.y, 0));
		DisplayRendererObject->SetModelPivotDiff(glm::vec3(originalPos.x, originalPos.y, 0));
		//DisplayRendererObject->SetModelPosition( glm::vec3(newPos.x, -newPos.y , 0));
		DisplayRendererObject->SetModelScale(glm::vec3{ 1+ scaleAmount , 1+ scaleAmount, 1+scaleAmount});
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

