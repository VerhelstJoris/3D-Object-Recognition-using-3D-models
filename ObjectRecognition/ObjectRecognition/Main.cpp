
#include <iostream>
#include <vector>


//THIS PROJECT REQUIRES THE OPENCV , OPENGL, GLEW, GLFW, GLM LIBRARIES TO FUNCTION PROPERLY

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
	result = ContourRendererObject->Initialize("Resources/Stopsign/stopsign.obj", 800 , 600, RENDERER_MODE::CAMERAMOVE);
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
	
	std::cout << std::endl << "=================================" << std::endl;
	std::vector<std::string> testImgVec;
	//CHOOSE A TESTIMAGE
	//RENDERS
	testImgVec.push_back("Resources/Test/test2.jpg");
	testImgVec.push_back("Resources/Test/test2_rotated.jpg");
	testImgVec.push_back("Resources/Test/test2_rotated3.jpg");
	testImgVec.push_back("Resources/Test/test2_rotated3_chunked.jpg");
	testImgVec.push_back("Resources/Test/test2_rotated3_chunked2.jpg");
	testImgVec.push_back("Resources/Test/test2_rotated3_scale.jpg");
	testImgVec.push_back("Resources/Test/test2_rotated4.jpg");
	testImgVec.push_back("Resources/Test/test2_rotated5.jpg");
	testImgVec.push_back("Resources/Test/test3.jpg");
	testImgVec.push_back("Resources/Test/test3_rotated.jpg");
	testImgVec.push_back("Resources/Test/test3_chunked.jpg");
	testImgVec.push_back("Resources/Test/test3_chunked2.jpg");
	//RENDER WITH TEX AND BASIC SHADING

	testImgVec.push_back("Resources/Test/Render1.png");
	testImgVec.push_back("Resources/Test/Render1_chunked.png");
	testImgVec.push_back("Resources/Test/Render1_chunked2.png");
	testImgVec.push_back("Resources/Test/Render2.png");
	testImgVec.push_back("Resources/Test/Render3.png");

	//SCENE IMAGES
	testImgVec.push_back("Resources/Test/stopsign1.jpg");
	testImgVec.push_back("Resources/Test/stopsign2.jpg");
	testImgVec.push_back("Resources/Test/Render1_bg.png");
	testImgVec.push_back("Resources/Test/stopsign4.jpg");

	int chosenID = 0;
	std::cout << std::endl << "========================" << std::endl 
		<< "ENTER TEST IMAGE ID " << std::endl;

	for (size_t i = 0; i < testImgVec.size(); i++)
	{
		std::cout << i + 1 << ": " << testImgVec[i] << std::endl;
	}
	std::cin >> chosenID;

	cv::Mat testImg = cv::imread(testImgVec[chosenID-1]);

	if (!testImg.data)
	{
		std::cout << "FAILED TO LOAD IN IMAGE" << std::endl;
	}
	
	cv::imshow("TEST IMAGE", testImg);
	cv::waitKey(10);

	//CHOOSE WHETHER OR NOT TO RESCALE THE CHOSEN IMAGE FOR DEMONSTRATION
	std::cout << "RESCALE THE IMAGE?? (y/n)" << std::endl;

	char answer;
	bool scaleTestImage=false;
	std::cin >> answer;
	if (answer == 'y')
	{
		scaleTestImage = true;
		std::cout << "test image will be scaled down during the contour extraction process" << std::endl;
	}
	else if (answer == 'n')
	{
		scaleTestImage = false;
		std::cout << "test image will NOT be scaled down during the contour extraction process" << std::endl;
	}

	// Shutdown and release the RENDERER object.
	ContourRendererObject->Shutdown();
	delete ContourRendererObject;
	ContourRendererObject = 0;


	//CONTOUR MATCHING TEST
	bool contoursFoundInTestImg;
	ContourMatchOut testResult2 = matchingObject->MatchImgAgainstContours(testImg, contoursFoundInTestImg, scaleTestImage);
	
	if (contoursFoundInTestImg)
	{
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

		std::cout << std::endl << "=======================================" << std::endl << "SHAPE CONTEXT DISTANCE" << std::endl << std::endl;

		//prepare both contours
		std::vector<cv::Point> renderContShuffled, imageContShuffled;

		int highestAmountOfPoints = std::max(scaledRenderContour.size(), imageContTrans.size());
		//shuffle both contours for uniform sampling
		ImageOperations::ShuffleContour(scaledRenderContour, renderContShuffled, highestAmountOfPoints);
		//ImageOperations::ShuffleContour(scaledRenderContour, renderContShuffled, scaledRenderContour.size());
		ImageOperations::ShuffleContour(imageContTrans, imageContShuffled, highestAmountOfPoints);
		//ImageOperations::ShuffleContour(imageContTrans, imageContShuffled, imageContTrans.size());

		cv::Ptr <cv::ShapeContextDistanceExtractor> mysc = cv::createShapeContextDistanceExtractor();
		mysc->setAngularBins(60);
		mysc->setRadialBins(5);

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
			ImageOperations::RotateContour(renderContShuffled, rotated, imageAngle + (i * 90), massCentreRenderCont);


			float dis = mysc->computeDistance(rotated, imageContShuffled);
			std::cout << "Dist: " << dis << " | ";

			if (dis <= lowestDistance)
			{
				lowestDistance = dis;
				lowestID = i;
			}
		}


		//calculations compensating for wrong moments calculatios
		double angleAdjustment = 0.0;
		double angleDiff = 1.0;
		std::cout << std::endl << "PRECISION SHAPE CONTEXT DISTANCE CHECKS" << std::endl;

		for (int i = -4; i <= 4; i++)
		{
			if (i != 0)
			{
				double angleNew = imageAngle + (lowestID * 90) + (i * angleDiff);
				std::vector<cv::Point> rotatedNew;
				ImageOperations::RotateContour(renderContShuffled, rotatedNew, angleNew, massCentreRenderCont);

				float dis = mysc->computeDistance(rotatedNew, imageContShuffled);
				std::cout << "Dist: " << dis << " | ";


				if (dis <= lowestDistance)
				{
					std::cout << std::endl << "Lowest Distance in rot checks with angle: " << i * angleDiff << std::endl;
					lowestDistance = dis;
					angleAdjustment = i * angleDiff;
				}


			}
		}


		double angle = imageAngle + (lowestID * 90.0) + angleAdjustment;

		std::cout << std::endl << "FINAL ANGLE: " << angle << std::endl;

		std::vector<cv::Point> finalRot;

		ImageOperations::RotateContour(scaledRenderContour, finalRot, angle, cv::Point(imgSize.width / 2, imgSize.height / 2));



#pragma endregion

#pragma DRAWING ONLY

		//Get image contour data
		cv::Point2f massCentreImgCont;
		float diagonalLength2;
		ImageOperations::FindBlobs(testResult2.lowestContourImage, massCentreImageCont, diagonalLength2);

		float diagonalLength;
		ImageOperations::TranslateContourToPoint(finalRot, renderContTrans, massCentreImageCont, massCentreRenderCont, diagonalLength);

		//DRAWING
		drawContVec.push_back(renderContTrans);
		drawContVec.push_back(imageContTrans);
		cv::drawContours(drawing, drawContVec, 0, cv::Scalar(0, 0, 255));
		cv::drawContours(drawing, drawContVec, 1, cv::Scalar(0, 255, 0));

		cv::imshow("RESULTING MATCH", drawing);
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
		
		result = DisplayRendererObject->Initialize("Resources/Stopsign/stopsign.obj", imgSize.width, imgSize.height, RENDERER_MODE::DISPLAY);
		if (result)
		{
			int id = testResult2.lowestRenderID;
			//DisplayRendererObject->SwitchToDisplayMode(drawing);
			DisplayRendererObject->SwitchToDisplayMode(testImg);
			//massCentreRenderCont to massCentreImageCont SCREEN TO WORLD
			auto newPos = DisplayRendererObject->GetWorldCoordFromWindowCoord(glm::vec2(massCentreImageCont.x, massCentreImageCont.y), glm::vec2(imgSize.width, imgSize.height));
			auto originalPos = glm::vec4(0, 0, 0, 1.0);
		
		
			DisplayRendererObject->SetModelPosition(glm::vec3(newPos.x, newPos.y, 0));
			DisplayRendererObject->SetModelPivotDiff(glm::vec3(newPos.x - originalPos.x, newPos.y - originalPos.y, 0));
			if (scaleTestImage)
			{
				DisplayRendererObject->SetModelScale(glm::vec3{ 1 + (scaleAmount*2) , 1 + (scaleAmount*2), 1 + (scaleAmount*2) });
			}
			else
			{
				DisplayRendererObject->SetModelScale(glm::vec3{ 1 + scaleAmount , 1 + scaleAmount, 1 + scaleAmount });
			}
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
		//std::cout << "PRESS ENTER WITH ONE OF THE CONTOURS WINDOWS HIGHLIGHTED TO SHUT DOWN" << std::endl;
		//cv::waitKey();
		//std::cout << "SHUTTING DOWN" << std::endl;

		cv::destroyAllWindows();

		//Shutdown and release the DISPLAY object.
		DisplayRendererObject->Shutdown();
		delete DisplayRendererObject;
		DisplayRendererObject = 0;

#pragma endregion
	}
	else
	{
		std::cout << "PRESS ENTER WITH ONE OF THE CONTOUR WINDOWS HIGHLIGHTED TO SHUT DOWN" << std::endl;
		cv::waitKey();
		std::cout << "SHUTTING DOWN" << std::endl;
		cv::destroyAllWindows();

	}

	return 0;
}

